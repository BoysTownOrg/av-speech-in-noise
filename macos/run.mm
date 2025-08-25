#include "run.h"
#include "MersenneTwisterRandomizer.hpp"
#include "AvFoundationPlayers.h"
#include "AppKitView.h"
#include "Timer.h"
#include "Foundation-utility.h"
#include "AppKit-utility.h"
#include "av-speech-in-noise/core/AdaptiveTrackFactory.hpp"
#include "masking-images.h"

#include <av-speech-in-noise/core/SubmittingFixedPassFail.hpp>
#include <av-speech-in-noise/ui/FixedPassFail.hpp>
#include <av-speech-in-noise/core/SubmittingEmotion.hpp>
#include <av-speech-in-noise/ui/Emotion.hpp>
#include <av-speech-in-noise/core/SubmittingKeyPress.hpp>
#include <av-speech-in-noise/ui/KeyPress.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/core/AudioRecording.hpp>
#include <av-speech-in-noise/core/EyeTracking.hpp>
#include <av-speech-in-noise/playlist/PredeterminedTargetPlaylist.hpp>
#include <av-speech-in-noise/core/SubmittingConsonant.hpp>
#include <av-speech-in-noise/ui/PassFail.hpp>
#include <av-speech-in-noise/ui/SessionController.hpp>
#include <av-speech-in-noise/ui/TestSettingsInterpreter.hpp>
#include <av-speech-in-noise/ui/TestImpl.hpp>
#include <av-speech-in-noise/ui/RevealImage.hpp>
#include <av-speech-in-noise/ui/SubjectImpl.hpp>
#include <av-speech-in-noise/core/RunningATest.hpp>
#include <av-speech-in-noise/core/AdaptiveMethod.hpp>
#include <av-speech-in-noise/core/FixedLevelMethod.hpp>
#include <av-speech-in-noise/core/OutputFile.hpp>
#include <av-speech-in-noise/core/OutputFilePath.hpp>
#include <av-speech-in-noise/core/ResponseEvaluator.hpp>
#include <av-speech-in-noise/core/LevittTrack.hpp>
#include <av-speech-in-noise/core/UpdatedMaximumLikelihood.hpp>
#include <av-speech-in-noise/core/SubmittingFreeResponse.hpp>
#include <av-speech-in-noise/core/SubmittingPassFail.hpp>
#include <av-speech-in-noise/core/SubmittingKeywords.hpp>
#include <av-speech-in-noise/core/SubmittingNumberKeywords.hpp>
#include <av-speech-in-noise/core/SubmittingSyllable.hpp>
#include <av-speech-in-noise/player/MaskerPlayerImpl.hpp>
#include <av-speech-in-noise/player/TargetPlayerImpl.hpp>
#include <av-speech-in-noise/player/AudioReaderSimplified.hpp>
#include <av-speech-in-noise/playlist/RandomizedTargetPlaylists.hpp>
#include <av-speech-in-noise/playlist/FileFilterDecorator.hpp>

#include <fstream>
#include <sstream>
#include <utility>
#include <string_view>
#include <functional>
#include <filesystem>

#include <Foundation/Foundation.h>
#include <variant>

// https://stackoverflow.com/a/116220
namespace {
class FileCannotBeOpened {};
}
static auto read_file(std::string_view path) -> std::string {
    constexpr auto read_size = std::size_t{4096};
    auto stream = std::ifstream{path};
    if (!stream.is_open()) {
        throw FileCannotBeOpened{};
    }
    stream.exceptions(std::ios_base::badbit);

    auto out = std::string{};
    auto buf = std::string(read_size, '\0');
    while (stream.read(&buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}

namespace av_speech_in_noise {
static auto contents(NSString *parent) -> NSArray<NSString *> * {
    NSError *error{nil};
    const auto contents{
        [[NSFileManager defaultManager] contentsOfDirectoryAtPath:parent
                                                            error:&error]};
    if (contents == nil)
        throw DirectoryReader::CannotRead{string([error localizedDescription])};
    return contents;
}

static auto collectContentsIf(const LocalUrl &directory,
    const std::function<bool(NSString *)> &predicate) -> std::vector<LocalUrl> {
    std::vector<LocalUrl> items{};
    const auto parent{nsString(directory.path).stringByExpandingTildeInPath};
    for (NSString *item in contents(parent)) {
        const auto path{[parent stringByAppendingPathComponent:item]};
        if (predicate(path))
            items.push_back({string(item)});
    }
    return items;
}

static auto isDirectory(NSString *path) -> bool {
    BOOL isDir{NO};
    [[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDir];
    return isDir == YES;
}

static auto notADirectory(NSString *path) -> bool { return !isDirectory(path); }

namespace {
class MacOsDirectoryReader : public DirectoryReader {
    auto filesIn(const LocalUrl &directory) -> std::vector<LocalUrl> override {
        return collectContentsIf(directory, notADirectory);
    }

    auto subDirectories(const LocalUrl &directory)
        -> std::vector<LocalUrl> override {
        return collectContentsIf(directory, isDirectory);
    }
};

class MacOsTargetValidator : public TargetValidator {
  public:
    auto isValid(const LocalUrl &url) -> bool override {
        return [[NSFileManager defaultManager]
                   fileExistsAtPath:nsString(url.path)] == YES;
    }
};

class FileWriter : public Writer {
    std::ofstream file{};

  public:
    void write(const std::string &s) override { file << s; }

    void open(const std::string &s) override { file.open(s); }

    auto failed() -> bool override { return file.fail(); }

    void close() override { file.close(); }

    void save() override { file.flush(); }

    void write(Writable &writable) override { writable.write(file); }
};

class UnixFileSystemPath : public FileSystemPath {
    auto homeDirectory() -> std::filesystem::path override {
        return [NSURL fileURLWithPath:@"~".stringByExpandingTildeInPath]
            .fileSystemRepresentation;
    }

    void createDirectory(const std::filesystem::path &p) override {
        create_directories(p);
    }
};

class TimeStampImpl : public TimeStamp {
    tm dummyTime{};
    tm *time{&dummyTime};

  public:
    auto year() -> int override {
        // https://en.cppreference.com/w/c/chrono/tm
        return time->tm_year + 1900;
    }

    auto month() -> int override { return time->tm_mon + 1; }

    auto dayOfMonth() -> int override { return time->tm_mday; }

    auto hour() -> int override { return time->tm_hour; }

    auto minute() -> int override { return time->tm_min; }

    auto second() -> int override { return time->tm_sec; }

    void capture() override {
        const auto now{std::time(nullptr)};
        time = std::localtime(&now);
        if (time == nullptr)
            time = &dummyTime;
    }
};

class LocalTimeClock : public Clock {
  public:
    auto time() -> std::string override {
        const auto now{std::chrono::system_clock::to_time_t(
            std::chrono::system_clock::now())};
        std::stringstream stream;
        stream << std::put_time(std::localtime(&now), "%F %T");
        return stream.str();
    }
};

class MacosSystemTime : public submitting_emotion::SystemTime {
  public:
    auto nowSeconds() -> double override {
        return [NSProcessInfo processInfo].systemUptime;
    };
};

class TextFileReaderImpl : public TextFileReader {
  public:
    auto read(const LocalUrl &s) -> std::string override {
        try {
            return read_file(s.path);
        } catch (const ::FileCannotBeOpened &) {
            throw FileDoesNotExist{};
        }
    }
};

class KeyPressUI : public submitting_keypress::Control,
                   public KeyPressListener {
  public:
    explicit KeyPressUI(KeyableSubjectWindow *window) : window{window} {
        window->listener = this;
    }

    void attach(Observer *ob) override { observer = ob; }

    auto keyPressed() -> std::string override {
        return string(lastPressEvent.characters);
    }

    auto keyPressedSeconds() -> double override {
        return lastPressEvent.timestamp;
    }

    void giveKeyFocus() override { [window makeKeyWindow]; }

    void onPress(NSEvent *event) override {
        lastPressEvent = event;
        observer->notifyThatKeyHasBeenPressed();
    }

  private:
    KeyableSubjectWindow *window;
    Observer *observer{};
    NSEvent *lastPressEvent;
};
}

void initializeAppAndRunEventLoop(EyeTracker &eyeTracker,
    OutputFileNameFactory &outputFileNameFactory,
    AppKitTestSetupUIFactory &testSetupUIFactory, SessionUI &sessionUI,
    TestUI &testUI, submitting_free_response::UI &freeResponseUI,
    submitting_syllable::UI &syllablesUI,
    submitting_keywords::UI &chooseKeywordsUI,
    submitting_number_keywords::UI &correctKeywordsUI,
    submitting_pass_fail::UI &passFailUI, SubjectPresenter &subjectPresenter,
    KeyableSubjectWindow *subjectNSWindow,
    SessionController::Observer *sessionControllerObserver) {
    static TestSettingsInterpreterImpl testSettingsInterpreter{};
    const auto videoNSView{
        [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)]};
    addAutolayoutEnabledSubview(subjectNSWindow.contentView, videoNSView);
    const auto audioDevices{loadAudioDevices()};
    NSLog(@"Initializing video player...");
    static AvFoundationVideoPlayer videoPlayer{videoNSView, audioDevices};
    NSLog(@"Initializing audio reader...");
    static AvFoundationBufferedAudioReaderFactory audioReaderFactory;
    static AudioReaderSimplified audioReader{audioReaderFactory};
    NSLog(@"Initializing target player...");
    static TargetPlayerImpl targetPlayer{&videoPlayer, &audioReader};
    NSLog(@"Initializing audio player...");
    static AvFoundationAudioPlayer audioPlayer{audioDevices};
    static TimerImpl timer;
    NSLog(@"Initializing masker player...");
    static MaskerPlayerImpl maskerPlayer{audioPlayer, audioReader, timer};
    maskerPlayer.setRampFor(Duration{0.02});
    NSLog(@"Initializing output file...");
    static FileWriter fileWriter;
    static TimeStampImpl timeStamp;
    static UnixFileSystemPath systemPath;
    static const auto outputFileName{outputFileNameFactory.make(timeStamp)};
    static OutputFilePathImpl outputFilePath{
        *outputFileName, systemPath, testSettingsInterpreter};
    static OutputFileImpl outputFile{fileWriter, outputFilePath};
    NSLog(@"Initializing adaptive method...");
    static AdaptiveTrackFactory adaptiveTrackFactory{testSettingsInterpreter};
    static ResponseEvaluatorImpl responseEvaluator;
    static TextFileReaderImpl textFileReader;
    static MersenneTwisterRandomizer randomizer;
    NSLog(@"Initializing target playlists...");
    static MacOsDirectoryReader directoryReader;
    static FileExtensionFilter targetFileExtensionFilter{
        {".mov", ".avi", ".wav", ".mp4"}};
    static FileFilterDecorator onlyIncludesTargetFileExtensions{
        &directoryReader, &targetFileExtensionFilter};
    static SubdirectoryTargetPlaylistReader subdirectoryTargetPlaylistReader{
        &directoryReader};
    static RandomSubsetFiles passesThirtyRandomFiles{&randomizer, 30};
    NSLog(@"Initializing fixed level method...");
    static LocalTimeClock localTimeClock;
    NSLog(@"Initializing audio recorder...");
    static AvFoundationAudioRecorder audioRecorder;
    static RunningATestImpl runningATest{targetPlayer, maskerPlayer,
        responseEvaluator, outputFile, randomizer, localTimeClock,
        testSettingsInterpreter};
    static FixedLevelMethodImpl fixedLevelMethod{
        testSettingsInterpreter, responseEvaluator, runningATest};
    static RandomizedTargetPlaylistWithReplacement targetsWithReplacement{
        &testSettingsInterpreter, &onlyIncludesTargetFileExtensions,
        &randomizer, &fixedLevelMethod};
    static EachTargetPlayedOnceThenShuffleAndRepeat allTargetsNTimes{
        testSettingsInterpreter, &onlyIncludesTargetFileExtensions, &randomizer,
        fixedLevelMethod};
    static RandomizedTargetPlaylistWithoutReplacement everyTargetOnce{
        testSettingsInterpreter, &onlyIncludesTargetFileExtensions, &randomizer,
        fixedLevelMethod};
    static AdaptiveMethodImpl adaptiveMethod{testSettingsInterpreter,
        responseEvaluator, randomizer, adaptiveTrackFactory, runningATest,
        subdirectoryTargetPlaylistReader};
    static RandomizedTargetPlaylistWithReplacement::Factory
        targetsWithReplacementFactory{testSettingsInterpreter,
            &onlyIncludesTargetFileExtensions, &randomizer,
            subdirectoryTargetPlaylistReader};
    static CyclicRandomizedTargetPlaylist::Factory cyclicTargetsFactory{
        testSettingsInterpreter, &onlyIncludesTargetFileExtensions, &randomizer,
        subdirectoryTargetPlaylistReader};
    static SubdirectoryTargetPlaylistReader cyclicTargetsReader{
        &directoryReader};
    static MacOsTargetValidator targetValidator;
    static PredeterminedTargetPlaylist predeterminedTargetPlaylist{
        testSettingsInterpreter, textFileReader, targetValidator,
        fixedLevelMethod};
    NSLog(@"Initializing audio recording...");
    static AudioRecording audioRecording{testSettingsInterpreter, audioRecorder,
        outputFile, timeStamp, runningATest};
    NSLog(@"Initializing eye tracking...");
    static EyeTracking eyeTracking{testSettingsInterpreter, eyeTracker,
        maskerPlayer, targetPlayer, outputFile, runningATest};
    NSLog(@"Initializing test setup UI...");
    static const auto testSetupUI{testSetupUIFactory.make(nil)};
    NSLog(@"Initializing consonant UI...");
    const auto consonantNSView{
        [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)]};
    addAutolayoutEnabledSubview(subjectNSWindow.contentView, consonantNSView);
    [NSLayoutConstraint activateConstraints:@[
        [consonantNSView.centerXAnchor
            constraintEqualToAnchor:subjectNSWindow.contentView.centerXAnchor],
        [consonantNSView.widthAnchor
            constraintEqualToAnchor:subjectNSWindow.contentView.widthAnchor
                         multiplier:0.5],
        [consonantNSView.heightAnchor
            constraintEqualToAnchor:subjectNSWindow.contentView.heightAnchor
                         multiplier:0.5],
        [consonantNSView.bottomAnchor
            constraintEqualToAnchor:subjectNSWindow.contentView.bottomAnchor
                           constant:-80]
    ]];
    const auto emotionNSView{
        [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)]};
    addAutolayoutEnabledSubview(subjectNSWindow.contentView, emotionNSView);
    [NSLayoutConstraint activateConstraints:@[
        [emotionNSView.centerXAnchor
            constraintEqualToAnchor:subjectNSWindow.contentView.centerXAnchor],
        [emotionNSView.widthAnchor
            constraintEqualToAnchor:subjectNSWindow.contentView.widthAnchor],
        [emotionNSView.heightAnchor
            constraintEqualToAnchor:subjectNSWindow.contentView.heightAnchor],
        [emotionNSView.centerYAnchor
            constraintEqualToAnchor:subjectNSWindow.contentView.centerYAnchor]
    ]];

    static submitting_consonant::AppKitUI consonantUI{consonantNSView};
    NSLog(@"Initializing CRM UI...");
    const auto coordinateResponseMeasureNSView{
        [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)]};
    addAutolayoutEnabledSubview(
        subjectNSWindow.contentView, coordinateResponseMeasureNSView);
    [NSLayoutConstraint activateConstraints:@[
        [coordinateResponseMeasureNSView.centerXAnchor
            constraintEqualToAnchor:subjectNSWindow.contentView.centerXAnchor],
        [coordinateResponseMeasureNSView.widthAnchor
            constraintEqualToAnchor:subjectNSWindow.contentView.widthAnchor
                         multiplier:0.5],
        [coordinateResponseMeasureNSView.heightAnchor
            constraintEqualToAnchor:subjectNSWindow.contentView.heightAnchor
                         multiplier:0.5],
        [coordinateResponseMeasureNSView.bottomAnchor
            constraintEqualToAnchor:subjectNSWindow.contentView.bottomAnchor
                           constant:-80]
    ]];
    static AppKitCoordinateResponseMeasureUI coordinateResponseMeasureView{
        coordinateResponseMeasureNSView};
    NSLog(@"Initializing presenters...");
    static MaskedCoreGraphicsImage maskedImage{subjectNSWindow};
    static RevealImage revealImagePuzzle{maskedImage, randomizer, 6, 7};
    static TestSetupPresenterImpl testSetupPresenter{*testSetupUI, sessionUI};
    static TestPresenterImpl testPresenter{
        runningATest, adaptiveMethod, testUI};
    static CoordinateResponseMeasurePresenter
        coordinateResponseMeasurePresenter{testSettingsInterpreter,
            coordinateResponseMeasureView, testPresenter};
    static submitting_number_keywords::Presenter correctKeywordsPresenter{
        testSettingsInterpreter, testUI, correctKeywordsUI, testPresenter};
    static submitting_syllable::PresenterImpl syllablesPresenter{
        testSettingsInterpreter, syllablesUI, testUI, testPresenter};
    static submitting_keywords::PresenterImpl chooseKeywordsPresenter{
        testSettingsInterpreter, runningATest, fixedLevelMethod, testUI,
        chooseKeywordsUI, testPresenter,
        submitting_keywords::sentencesWithThreeKeywords(
            read_file(resourceUrl("mlst-c", "txt").path))};
    static submitting_free_response::Presenter freeResponsePresenter{
        testSettingsInterpreter, testUI, freeResponseUI, testPresenter,
        revealImagePuzzle};
    static submitting_consonant::PresenterImpl consonantPresenter{
        testSettingsInterpreter, consonantUI, testPresenter};
    static SessionPresenterImpl sessionPresenter{sessionUI, runningATest};
    static SessionControllerImpl sessionController{
        testSetupPresenter, testPresenter, subjectPresenter};
    static TestControllerImpl testController{sessionController, runningATest,
        adaptiveMethod, sessionUI, testUI, testPresenter};
    static submitting_keywords::InteractorImpl submittingKeywordsInteractor{
        fixedLevelMethod, runningATest, outputFile};
    static submitting_keywords::Controller chooseKeywordsController{
        testController, submittingKeywordsInteractor, chooseKeywordsUI,
        chooseKeywordsPresenter};
    static submitting_syllable::InteractorImpl submittingSyllableInteractor{
        fixedLevelMethod, runningATest, outputFile};
    static submitting_syllable::Controller syllablesController{syllablesUI,
        testController, submittingSyllableInteractor,
        {{"B", Syllable::bi}, {"D", Syllable::di}, {"G", Syllable::dji},
            {"F", Syllable::fi}, {"Ghee", Syllable::gi}, {"H", Syllable::hi},
            {"Yee", Syllable::ji}, {"K", Syllable::ki}, {"L", Syllable::li},
            {"M", Syllable::mi}, {"N", Syllable::ni}, {"P", Syllable::pi},
            {"R", Syllable::ri}, {"Sh", Syllable::shi}, {"S", Syllable::si},
            {"Th", Syllable::thi}, {"T", Syllable::ti}, {"Ch", Syllable::tsi},
            {"V", Syllable::vi}, {"W", Syllable::wi}, {"Z", Syllable::zi}}};
    static submitting_number_keywords::InteractorImpl
        submittingNumberKeywordsInteractor{
            adaptiveMethod, runningATest, outputFile};
    static submitting_number_keywords::Controller correctKeywordsController{
        testController, submittingNumberKeywordsInteractor, sessionUI,
        correctKeywordsUI};
    static submitting_free_response::InteractorImpl
        submittingFreeResponseInteractor{
            fixedLevelMethod, runningATest, outputFile};
    static TimerImpl puzzleTimer;
    static submitting_free_response::Controller freeResponseController{
        testSettingsInterpreter, testController,
        submittingFreeResponseInteractor, freeResponseUI, revealImagePuzzle,
        puzzleTimer};
    freeResponseController.setNTrialsPerNewPuzzlePiece(5);
    static submitting_pass_fail::InteractorImpl submittingPassFailInteractor{
        adaptiveMethod, runningATest, outputFile};
    static submitting_pass_fail::Presenter passFailPresenter{
        testSettingsInterpreter, runningATest, testController, testUI,
        submittingPassFailInteractor, passFailUI, testPresenter};
    static submitting_consonant::InteractorImpl submittingConsonantInteractor{
        testSettingsInterpreter, fixedLevelMethod, runningATest, outputFile,
        maskerPlayer};
    static submitting_consonant::Controller consonantTaskController{
        testController, submittingConsonantInteractor, consonantUI,
        consonantPresenter};
    static submitting_keypress::InteractorImpl submittingKeyPressInteractor{
        testSettingsInterpreter, fixedLevelMethod, runningATest, outputFile,
        maskerPlayer, randomizer};
    static KeyPressUI keyPressControl{subjectNSWindow};
    static TimerImpl keyPressTimer;
    static submitting_keypress::Presenter keypressPresenter{
        testSettingsInterpreter, testUI, testController,
        submittingKeyPressInteractor, keyPressControl, keyPressTimer,
        testPresenter};
    static submitting_emotion::AppKitUI emotionUI{emotionNSView};
    static submitting_emotion::InteractorImpl emotionInteractor{
        fixedLevelMethod, runningATest, outputFile};
    static MacosSystemTime systemTime;
    static submitting_emotion::Presenter emotionPresenter{
        testSettingsInterpreter, emotionUI, testController, emotionInteractor,
        systemTime, testPresenter};
    static submitting_fixed_pass_fail::InteractorImpl fixedPassFailInteractor(
        fixedLevelMethod, runningATest, outputFile);
    static submitting_fixed_pass_fail::Presenter fixedPassFailPresenter{
        testSettingsInterpreter, testController, testUI,
        fixedPassFailInteractor, passFailUI, testPresenter, keypressPresenter};
    static CoordinateResponseMeasureController
        coordinateResponseMeasureController{
            testController, runningATest, coordinateResponseMeasureView};
    coordinateResponseMeasureController.attach(
        &coordinateResponseMeasurePresenter);
    static TestSetupController testSetupController{*testSetupUI, sessionUI,
        testSetupPresenter, runningATest, testSettingsInterpreter,
        textFileReader, sessionController};
    sessionController.attach(sessionControllerObserver);
    testPresenter.subscribe(testSettingsInterpreter);
    NSLog(@"Finished main initialization.");
}
}
