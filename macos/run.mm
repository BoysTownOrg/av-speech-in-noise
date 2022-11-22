#include "run.h"
#include "MersenneTwisterRandomizer.hpp"
#include "AvFoundationPlayers.h"
#include "AppKitView.h"
#include "Foundation-utility.h"
#include "AppKit-utility.h"
#include "av-speech-in-noise/core/AudioRecording.hpp"
#include "av-speech-in-noise/core/EyeTracking.hpp"

#include <av-speech-in-noise/playlist/PredeterminedTargetPlaylist.hpp>
#include <av-speech-in-noise/core/SubmittingConsonant.hpp>
#include <av-speech-in-noise/ui/PassFail.hpp>
#include <av-speech-in-noise/ui/SessionController.hpp>
#include <av-speech-in-noise/ui/TestSettingsInterpreter.hpp>
#include <av-speech-in-noise/ui/TestImpl.hpp>
#include <av-speech-in-noise/ui/SubjectImpl.hpp>
#include <av-speech-in-noise/core/Model.hpp>
#include <av-speech-in-noise/core/RecognitionTestModel.hpp>
#include <av-speech-in-noise/core/AdaptiveMethod.hpp>
#include <av-speech-in-noise/core/FixedLevelMethod.hpp>
#include <av-speech-in-noise/core/OutputFile.hpp>
#include <av-speech-in-noise/core/OutputFilePath.hpp>
#include <av-speech-in-noise/core/ResponseEvaluator.hpp>
#include <av-speech-in-noise/core/AdaptiveTrack.hpp>
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

@interface CallbackScheduler : NSObject
@end

// https://stackoverflow.com/a/116220
static auto read_file(std::string_view path) -> std::string {
    constexpr auto read_size = std::size_t{4096};
    auto stream = std::ifstream{path};
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
namespace {
class TimerImpl : public Timer {
  public:
    TimerImpl();
    void attach(Observer *e) override;
    void scheduleCallbackAfterSeconds(double x) override;
    void timerCallback();

  private:
    Observer *listener{};
    CallbackScheduler *scheduler{[[CallbackScheduler alloc] init]};
};
}
}

@implementation CallbackScheduler {
  @public
    av_speech_in_noise::TimerImpl *controller;
}

- (void)scheduleCallbackAfterSeconds:(double)x {
    [NSTimer scheduledTimerWithTimeInterval:x
                                     target:self
                                   selector:@selector(timerCallback)
                                   userInfo:nil
                                    repeats:NO];
}

- (void)timerCallback {
    controller->timerCallback();
}
@end

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

class TextFileReaderImpl : public TextFileReader {
  public:
    auto read(const LocalUrl &s) -> std::string override {
        return read_file(s.path);
    }
};

TimerImpl::TimerImpl() { scheduler->controller = this; }

void TimerImpl::attach(Observer *e) { listener = e; }

void TimerImpl::scheduleCallbackAfterSeconds(double x) {
    [scheduler scheduleCallbackAfterSeconds:x];
}

void TimerImpl::timerCallback() { listener->callback(); }
}

void initializeAppAndRunEventLoop(EyeTracker &eyeTracker,
    OutputFileNameFactory &outputFileNameFactory,
    AppKitTestSetupUIFactory &testSetupUIFactory, SessionUI &sessionUI,
    TestUI &testUI, submitting_free_response::UI &freeResponseUI,
    submitting_syllable::UI &syllablesUI,
    submitting_keywords::UI &chooseKeywordsUI,
    submitting_number_keywords::UI &correctKeywordsUI,
    submitting_pass_fail::UI &passFailUI, SubjectPresenter &subjectPresenter,
    NSWindow *subjectNSWindow,
    SessionController::Observer *sessionControllerObserver,
    std::filesystem::path relativeOutputDirectory) {
    const auto videoNSView{
        [[NSView alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)]};
    addAutolayoutEnabledSubview(subjectNSWindow.contentView, videoNSView);
    static AvFoundationVideoPlayer videoPlayer{videoNSView};
    static AvFoundationBufferedAudioReaderFactory audioReaderFactory;
    static AudioReaderSimplified audioReader{audioReaderFactory};
    static TargetPlayerImpl targetPlayer{&videoPlayer, &audioReader};
    static AvFoundationAudioPlayer audioPlayer;
    static TimerImpl timer;
    static MaskerPlayerImpl maskerPlayer{&audioPlayer, &audioReader, &timer};
    maskerPlayer.setRampFor(Duration{0.02});
    static FileWriter fileWriter;
    static TimeStampImpl timeStamp;
    static UnixFileSystemPath systemPath;
    static const auto outputFileName{outputFileNameFactory.make(timeStamp)};
    static OutputFilePathImpl outputFilePath{*outputFileName, systemPath};
    static OutputFileImpl outputFile{fileWriter, outputFilePath};
    static adaptive_track::AdaptiveTrack::Factory snrTrackFactory;
    static ResponseEvaluatorImpl responseEvaluator;
    static TextFileReaderImpl textFileReader;
    static MersenneTwisterRandomizer randomizer;
    static AdaptiveMethodImpl adaptiveMethod{
        snrTrackFactory, responseEvaluator, randomizer};
    static MacOsDirectoryReader directoryReader;
    static FileExtensionFilter targetFileExtensionFilter{
        {".mov", ".avi", ".wav", ".mp4"}};
    static FileFilterDecorator onlyIncludesTargetFileExtensions{
        &directoryReader, &targetFileExtensionFilter};
    static RandomizedTargetPlaylistWithReplacement targetsWithReplacement{
        &onlyIncludesTargetFileExtensions, &randomizer};
    static FileIdentifierExcluderFilter
        excludesTargetsThatHave100_200_300Or400InTheirName{
            {"100", "200", "300", "400"}};
    static FileIdentifierFilter targetsThatHave100InTheirName{"100"};
    static FileIdentifierFilter targetsThatHave200InTheirName{"200"};
    static FileIdentifierFilter targetsThatHave300InTheirName{"300"};
    static FileIdentifierFilter targetsThatHave400InTheirName{"400"};
    static FileFilterDecorator allButSilentIntervalTargets{
        &onlyIncludesTargetFileExtensions,
        &excludesTargetsThatHave100_200_300Or400InTheirName};
    static FileFilterDecorator oneHundred_ms_SilentIntervalTargets{
        &onlyIncludesTargetFileExtensions, &targetsThatHave100InTheirName};
    static FileFilterDecorator twoHundred_ms_SilentIntervalTargets{
        &onlyIncludesTargetFileExtensions, &targetsThatHave200InTheirName};
    static FileFilterDecorator threeHundred_ms_SilentIntervalTargets{
        &onlyIncludesTargetFileExtensions, &targetsThatHave300InTheirName};
    static FileFilterDecorator fourHundred_ms_SilentIntervalTargets{
        &onlyIncludesTargetFileExtensions, &targetsThatHave400InTheirName};
    static RandomSubsetFiles passesThirtyRandomFiles{&randomizer, 30};
    static FileFilterDecorator thirtyRandomAllButSilentIntervalTargets{
        &allButSilentIntervalTargets, &passesThirtyRandomFiles};
    static FileFilterDecorator thirtyRandomOneHundred_ms_SilentIntervalTargets{
        &oneHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    static FileFilterDecorator thirtyRandomTwoHundred_ms_SilentIntervalTargets{
        &twoHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    static FileFilterDecorator
        thirtyRandomThreeHundred_ms_SilentIntervalTargets{
            &threeHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    static FileFilterDecorator thirtyRandomFourHundred_ms_SilentIntervalTargets{
        &fourHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    static DirectoryReaderComposite silentIntervalTargetsDirectoryReader{
        {&thirtyRandomAllButSilentIntervalTargets,
            &thirtyRandomOneHundred_ms_SilentIntervalTargets,
            &thirtyRandomTwoHundred_ms_SilentIntervalTargets,
            &thirtyRandomThreeHundred_ms_SilentIntervalTargets,
            &thirtyRandomFourHundred_ms_SilentIntervalTargets}};
    static RandomizedTargetPlaylistWithoutReplacement silentIntervalTargets{
        &silentIntervalTargetsDirectoryReader, &randomizer};
    static RandomizedTargetPlaylistWithoutReplacement everyTargetOnce{
        &onlyIncludesTargetFileExtensions, &randomizer};
    static EachTargetPlayedOnceThenShuffleAndRepeat allTargetsNTimes{
        &onlyIncludesTargetFileExtensions, &randomizer};
    static FixedLevelMethodImpl fixedLevelMethod{responseEvaluator};
    static LocalTimeClock localTimeClock;
    static AvFoundationAudioRecorder audioRecorder;
    static RunningATestImpl recognitionTestModel{targetPlayer, maskerPlayer,
        responseEvaluator, outputFile, randomizer, localTimeClock};
    static RandomizedTargetPlaylistWithReplacement::Factory
        targetsWithReplacementFactory{
            &onlyIncludesTargetFileExtensions, &randomizer};
    static SubdirectoryTargetPlaylistReader targetsWithReplacementReader{
        &targetsWithReplacementFactory, &directoryReader};
    static CyclicRandomizedTargetPlaylist::Factory cyclicTargetsFactory{
        &onlyIncludesTargetFileExtensions, &randomizer};
    static SubdirectoryTargetPlaylistReader cyclicTargetsReader{
        &cyclicTargetsFactory, &directoryReader};
    static PredeterminedTargetPlaylist predeterminedTargetPlaylist{
        textFileReader};
    static AudioRecording audioRecording{audioRecorder, outputFile, timeStamp};
    static EyeTracking eyeTracking{
        eyeTracker, maskerPlayer, targetPlayer, outputFile};
    static RunningATestFacadeImpl model{adaptiveMethod, fixedLevelMethod,
        targetsWithReplacementReader, cyclicTargetsReader,
        targetsWithReplacement, silentIntervalTargets, everyTargetOnce,
        allTargetsNTimes, predeterminedTargetPlaylist, recognitionTestModel,
        outputFile, audioRecording, eyeTracking};
    static const auto testSetupUI{testSetupUIFactory.make(nil)};
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
    static submitting_consonant::AppKitUI consonantUI{consonantNSView};
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
    static submitting_consonant::PresenterImpl consonantPresenter{consonantUI};
    static submitting_free_response::Presenter freeResponsePresenter{
        testUI, freeResponseUI};
    static submitting_keywords::PresenterImpl chooseKeywordsPresenter{model,
        testUI, chooseKeywordsUI,
        submitting_keywords::sentencesWithThreeKeywords(
            read_file(resourceUrl("mlst-c", "txt").path))};
    static submitting_syllable::PresenterImpl syllablesPresenter{
        syllablesUI, testUI};
    static submitting_number_keywords::Presenter correctKeywordsPresenter{
        testUI, correctKeywordsUI};
    static submitting_pass_fail::Presenter passFailPresenter{
        testUI, passFailUI};
    static CoordinateResponseMeasurePresenter
        coordinateResponseMeasurePresenter{coordinateResponseMeasureView};
    static TestSetupPresenterImpl testSetupPresenter{*testSetupUI, sessionUI};
    static UninitializedTaskPresenterImpl taskPresenter;
    static TestPresenterImpl testPresenter{model, testUI, &taskPresenter};
    static SessionPresenterImpl sessionPresenter{sessionUI, model};
    static SessionControllerImpl sessionController{
        testSetupPresenter, testPresenter, subjectPresenter};
    static TestControllerImpl testController{
        sessionController, model, sessionUI, testUI, testPresenter};
    static submitting_keywords::InteractorImpl submittingKeywordsInteractor{
        fixedLevelMethod, recognitionTestModel, outputFile};
    static submitting_keywords::Controller chooseKeywordsController{
        testController, submittingKeywordsInteractor, chooseKeywordsUI,
        chooseKeywordsPresenter};
    static submitting_syllable::InteractorImpl submittingSyllableInteractor{
        fixedLevelMethod, recognitionTestModel, outputFile};
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
            adaptiveMethod, recognitionTestModel, outputFile};
    static submitting_number_keywords::Controller correctKeywordsController{
        testController, submittingNumberKeywordsInteractor, sessionUI,
        correctKeywordsUI};
    static submitting_free_response::InteractorImpl
        submittingFreeResponseInteractor{
            fixedLevelMethod, recognitionTestModel, outputFile};
    static submitting_free_response::Controller freeResponseController{
        testController, submittingFreeResponseInteractor, freeResponseUI};
    static submitting_pass_fail::InteractorImpl submittingPassFailInteractor{
        adaptiveMethod, recognitionTestModel, outputFile};
    static submitting_pass_fail::Controller passFailController{
        testController, submittingPassFailInteractor, passFailUI};
    static submitting_consonant::InteractorImpl submittingConsonantInteractor{
        fixedLevelMethod, recognitionTestModel, outputFile};
    static submitting_consonant::Controller consonantTaskController{
        testController, submittingConsonantInteractor, consonantUI,
        consonantPresenter};
    static CoordinateResponseMeasureController
        coordinateResponseMeasureController{
            testController, model, coordinateResponseMeasureView};
    coordinateResponseMeasureController.attach(
        &coordinateResponseMeasurePresenter);
    static TestSettingsInterpreterImpl testSettingsInterpreter{
        {{Method::adaptiveCoordinateResponseMeasure,
             coordinateResponseMeasurePresenter},
            {Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker,
                coordinateResponseMeasurePresenter},
            {Method::adaptiveCoordinateResponseMeasureWithDelayedMasker,
                coordinateResponseMeasurePresenter},
            {Method::adaptiveCoordinateResponseMeasureWithEyeTracking,
                coordinateResponseMeasurePresenter},
            {Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement,
                coordinateResponseMeasurePresenter},
            {Method::
                    fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking,
                coordinateResponseMeasurePresenter},
            {Method::
                    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets,
                coordinateResponseMeasurePresenter},
            {Method::fixedLevelFreeResponseWithAllTargets,
                freeResponsePresenter},
            {Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking,
                freeResponsePresenter},
            {Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording,
                freeResponsePresenter},
            {Method::
                    fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording,
                freeResponsePresenter},
            {Method::fixedLevelFreeResponseWithSilentIntervalTargets,
                freeResponsePresenter},
            {Method::fixedLevelFreeResponseWithTargetReplacement,
                freeResponsePresenter},
            {Method::fixedLevelChooseKeywordsWithAllTargets,
                chooseKeywordsPresenter},
            {Method::fixedLevelSyllablesWithAllTargets, syllablesPresenter},
            {Method::adaptiveCorrectKeywords, correctKeywordsPresenter},
            {Method::adaptiveCorrectKeywordsWithEyeTracking,
                correctKeywordsPresenter},
            {Method::fixedLevelConsonants, consonantPresenter},
            {Method::adaptivePassFail, passFailPresenter},
            {Method::adaptivePassFailWithEyeTracking, passFailPresenter}}};
    static TestSetupController testSetupController{*testSetupUI,
        sessionController, sessionUI, testSetupPresenter, model,
        testSettingsInterpreter, textFileReader};
    sessionController.attach(sessionControllerObserver);
}
}
