#include "run.h"
#include "MersenneTwisterRandomizer.hpp"
#include "AvFoundationPlayers.h"
#include "AppKitView.h"
#include "Foundation-utility.h"
#include "AppKit-utility.h"
#include <presentation/SessionController.hpp>
#include <presentation/TestSettingsInterpreter.hpp>
#include <presentation/TestImpl.hpp>
#include <recognition-test/Model.hpp>
#include <recognition-test/RecognitionTestModel.hpp>
#include <recognition-test/AdaptiveMethod.hpp>
#include <recognition-test/FixedLevelMethod.hpp>
#include <recognition-test/OutputFile.hpp>
#include <recognition-test/OutputFilePath.hpp>
#include <recognition-test/ResponseEvaluator.hpp>
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <stimulus-players/TargetPlayerImpl.hpp>
#include <stimulus-players/AudioReaderSimplified.hpp>
#include <target-playlists/RandomizedTargetPlaylists.hpp>
#include <target-playlists/FileFilterDecorator.hpp>
#include <adaptive-track/AdaptiveTrack.hpp>
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
    return [[NSFileManager defaultManager] contentsOfDirectoryAtPath:parent
                                                               error:nil];
}

static auto collectContentsIf(const LocalUrl &directory,
    const std::function<bool(NSString *)> &predicate) -> std::vector<LocalUrl> {
    std::vector<LocalUrl> items{};
    const auto parent{nsString(directory.path).stringByExpandingTildeInPath};
    for (NSString *item in contents(parent)) {
        const auto path{[parent stringByAppendingPathComponent:item]};
        if (predicate(path))
            items.push_back({[item UTF8String]});
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
};

class UnixFileSystemPath : public FileSystemPath {
    auto homeDirectory() -> std::filesystem::path override {
        return [NSURL fileURLWithPath:@"~".stringByExpandingTildeInPath]
            .fileSystemRepresentation;
    }

    void createDirectory(const std::filesystem::path &p) override {
        create_directory(p);
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

    auto month() -> int override { return time->tm_mon; }

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

static void addChild(NSTabViewController *parent, NSTabViewController *child) {
    [parent.view addSubview:child.view];
}

void initializeAppAndRunEventLoop(EyeTracker &eyeTracker,
    AppKitTestSetupUIFactory &testSetupUIFactory,
    OutputFileNameFactory &outputFileNameFactory, NSViewController *,
    SessionController::Observer *sessionControllerObserver,
    std::filesystem::path relativeOutputDirectory, SessionUI *sessionUIMaybe,
    TestUI *testUIMaybe, FreeResponseUI *freeResponseUIMaybe,
    SyllablesUI_ *syllablesUIMaybe, ChooseKeywordsUI_ *chooseKeywordsUIMaybe,
    CorrectKeywordsUI_ *correctKeywordsUIMaybe, PassFailUI_ *passFailUIMaybe) {
    const auto subjectScreen{[[NSScreen screens] lastObject]};
    static AvFoundationVideoPlayer videoPlayer{subjectScreen};
    static AvFoundationBufferedAudioReaderFactory bufferedReaderFactory;
    static AudioReaderSimplified audioReader{bufferedReaderFactory};
    static TargetPlayerImpl targetPlayer{&videoPlayer, &audioReader};
    static AvFoundationAudioPlayer audioPlayer;
    static TimerImpl timer;
    static MaskerPlayerImpl maskerPlayer{&audioPlayer, &audioReader, &timer};
    maskerPlayer.setRampFor(Duration{0.02});
    static FileWriter fileWriter;
    static TimeStampImpl timeStamp;
    static UnixFileSystemPath systemPath;
    static auto outputFileName{outputFileNameFactory.make(timeStamp)};
    static OutputFilePathImpl outputFilePath{*outputFileName, systemPath};
    outputFilePath.setRelativeOutputDirectory(
        std::move(relativeOutputDirectory));
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
    static RecognitionTestModelImpl recognitionTestModel{targetPlayer,
        maskerPlayer, responseEvaluator, outputFile, randomizer, eyeTracker};
    static RandomizedTargetPlaylistWithReplacement::Factory
        targetsWithReplacementFactory{
            &onlyIncludesTargetFileExtensions, &randomizer};
    static SubdirectoryTargetPlaylistReader targetsWithReplacementReader{
        &targetsWithReplacementFactory, &directoryReader};
    static CyclicRandomizedTargetPlaylist::Factory cyclicTargetsFactory{
        &onlyIncludesTargetFileExtensions, &randomizer};
    static SubdirectoryTargetPlaylistReader cyclicTargetsReader{
        &cyclicTargetsFactory, &directoryReader};
    static ModelImpl model{adaptiveMethod, fixedLevelMethod,
        targetsWithReplacementReader, cyclicTargetsReader,
        targetsWithReplacement, silentIntervalTargets, everyTargetOnce,
        allTargetsNTimes, recognitionTestModel, outputFile};
    const auto viewController{nsTabViewControllerWithoutTabControl()};
    const auto app{[NSApplication sharedApplication]};
    const auto audioDeviceMenu{
        [[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)
                                   pullsDown:NO]};
    static auto testSetupUI{testSetupUIFactory.make(nil)};
    const auto subjectScreenFrame{subjectScreen.frame};
    const auto subjectScreenOrigin{subjectScreenFrame.origin};
    const auto subjectScreenSize{subjectScreenFrame.size};
    const auto subjectViewHeight{subjectScreenSize.height / 4};
    const auto subjectScreenWidth{subjectScreenSize.width};
    const auto subjectViewWidth{subjectScreenWidth / 3};
    auto subjectViewLeadingEdge =
        subjectScreenOrigin.x + (subjectScreenWidth - subjectViewWidth) / 2;
    static AppKitConsonantUI consonantView{
        NSMakeRect(subjectScreenOrigin.x + subjectScreenWidth / 4,
            subjectScreenOrigin.y + subjectScreenSize.height / 12,
            subjectScreenWidth / 2, subjectScreenSize.height / 2)};
    static AppKitCoordinateResponseMeasureUI coordinateResponseMeasureView{
        NSMakeRect(subjectViewLeadingEdge, subjectScreenOrigin.y,
            subjectViewWidth, subjectViewHeight)};
    static ConsonantTaskPresenterImpl consonantPresenter{consonantView};
    static FreeResponsePresenter freeResponsePresenter{
        *testUIMaybe, *freeResponseUIMaybe};
    static ChooseKeywordsPresenterImpl chooseKeywordsPresenter{model,
        *testUIMaybe, *chooseKeywordsUIMaybe,
        sentencesWithThreeKeywords(
            read_file(resourceUrl("mlst-c", "txt").path))};
    static SyllablesPresenterImpl syllablesPresenter{
        *syllablesUIMaybe, *testUIMaybe};
    static CorrectKeywordsPresenter correctKeywordsPresenter{
        *testUIMaybe, *correctKeywordsUIMaybe};
    static PassFailPresenter passFailPresenter{*testUIMaybe, *passFailUIMaybe};
    static CoordinateResponseMeasurePresenter
        coordinateResponseMeasurePresenter{coordinateResponseMeasureView};
    static TestSetupPresenterImpl testSetupPresenter{
        *(testSetupUI.get()), *sessionUIMaybe};
    static UninitializedTaskPresenterImpl taskPresenter;
    static TestPresenterImpl testPresenter{model, *testUIMaybe, &taskPresenter};
    static SessionControllerImpl sessionController{
        model, *sessionUIMaybe, testSetupPresenter, testPresenter};
    static TestControllerImpl testController{
        sessionController, model, *sessionUIMaybe, *testUIMaybe, testPresenter};
    static ChooseKeywordsController chooseKeywordsController{
        testController, model, *chooseKeywordsUIMaybe, chooseKeywordsPresenter};
    static SyllablesController syllablesController{*syllablesUIMaybe,
        testController, model,
        {{"B", Syllable::bi}, {"D", Syllable::di}, {"G", Syllable::dji},
            {"F", Syllable::fi}, {"Ghee", Syllable::gi}, {"H", Syllable::hi},
            {"Yee", Syllable::ji}, {"K", Syllable::ki}, {"L", Syllable::li},
            {"M", Syllable::mi}, {"N", Syllable::ni}, {"P", Syllable::pi},
            {"R", Syllable::ri}, {"Sh", Syllable::shi}, {"S", Syllable::si},
            {"Th", Syllable::thi}, {"T", Syllable::ti}, {"Ch", Syllable::tsi},
            {"V", Syllable::vi}, {"W", Syllable::wi}, {"Z", Syllable::zi}}};
    static CorrectKeywordsController correctKeywordsController{
        testController, model, *sessionUIMaybe, *correctKeywordsUIMaybe};
    static FreeResponseController freeResponseController{
        testController, model, *freeResponseUIMaybe};
    static PassFailController passFailController{
        testController, model, *passFailUIMaybe};
    static ConsonantTaskController consonantTaskController{
        testController, model, consonantView, consonantPresenter};
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
    static TestSetupController testSetupController{*(testSetupUI.get()),
        sessionController, *sessionUIMaybe, testSetupPresenter, model,
        testSettingsInterpreter, textFileReader};
    sessionController.attach(sessionControllerObserver);
}
}
