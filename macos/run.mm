#include "run.h"
#include "MersenneTwisterRandomizer.hpp"
#include "AvFoundationPlayers.h"
#include "AppKitView.h"
#include "Foundation-utility.h"
#include "AppKit-utility.h"
#include <presentation/SessionController.hpp>
#include <presentation/TestSettingsInterpreter.hpp>
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

@interface ResizesToContentsViewController : NSTabViewController
@end

@implementation ResizesToContentsViewController
- (instancetype)init {
    if ((self = [super init]) != nullptr) {
        [self setTabStyle:NSTabViewControllerTabStyleUnspecified];
    }
    return self;
}
- (void)viewWillAppear {
    [super viewWillAppear];
    self.preferredContentSize = self.view.fittingSize;
}
@end

// https://developer.apple.com/library/archive/documentation/FileManagement/Conceptual/FileSystemProgrammingGuide/ManagingFIlesandDirectories/ManagingFIlesandDirectories.html#//apple_ref/doc/uid/TP40010672-CH6-SW2
// Listing 6-1
static auto applicationDataDirectory() -> NSURL * {
    NSString *bundleID = [[NSBundle mainBundle] bundleIdentifier];
    NSFileManager *fm = [NSFileManager defaultManager];
    NSURL *dirPath = nil;
    // Find the application support directory in the home directory.
    NSArray *appSupportDir = [fm URLsForDirectory:NSApplicationSupportDirectory
                                        inDomains:NSUserDomainMask];
    if ([appSupportDir count] > 0) {
        // Append the bundle ID to the URL for the
        // Application Support directory
        dirPath = [[appSupportDir objectAtIndex:0]
            URLByAppendingPathComponent:bundleID];
        // If the directory does not exist, this method creates it.
        // This method is only available in macOS 10.7 and iOS 5.0 or later.
        NSError *theError = nil;
        if ([fm createDirectoryAtURL:dirPath
                withIntermediateDirectories:YES
                                 attributes:nil
                                      error:&theError] == 0) {
            // Handle the error.
            return nil;
        }
    }
    return dirPath;
}

@interface ApplicationDelegate : NSObject <NSApplicationDelegate>
@end

@implementation ApplicationDelegate {
  @public
    NSPopUpButton *audioDeviceMenu;
}
- (void)applicationWillTerminate:(NSNotification *)notification {
    if (audioDeviceMenu.titleOfSelectedItem != nil) {
        std::ofstream defaultAudioDeviceFile{[applicationDataDirectory()
            URLByAppendingPathComponent:@"default-audio-device.txt"]
                                                 .fileSystemRepresentation};
        defaultAudioDeviceFile
            << audioDeviceMenu.titleOfSelectedItem.UTF8String;
    }
}
@end

@interface WindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation WindowDelegate
- (void)windowWillClose:(NSNotification *)__unused notification {
    [NSApp terminate:self];
}
@end

@interface MenuActions : NSObject
@end

@implementation MenuActions {
  @public
    NSWindow *preferencesWindow;
}

- (void)notifyThatPreferencesHasBeenClicked {
    [preferencesWindow makeKeyAndOrderFront:nil];
}
@end

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
    auto homeDirectory() -> std::string override {
        return [NSURL fileURLWithPath:@"~".stringByExpandingTildeInPath]
            .fileSystemRepresentation;
    }

    void createDirectory(std::string s) override {
        [[NSFileManager defaultManager]
                   createDirectoryAtURL:[NSURL fileURLWithPath:nsString(s)]
            withIntermediateDirectories:YES
                             attributes:nil
                                  error:nil];
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
    OutputFileNameFactory &outputFileNameFactory,
    SessionController::Observer *sessionControllerObserver,
    const std::string &relativeOutputDirectory) {
    const auto subjectScreen{[[NSScreen screens] lastObject]};
    AvFoundationVideoPlayer videoPlayer{subjectScreen};
    AvFoundationBufferedAudioReaderFactory bufferedReaderFactory;
    AudioReaderSimplified audioReader{bufferedReaderFactory};
    TargetPlayerImpl targetPlayer{&videoPlayer, &audioReader};
    AvFoundationAudioPlayer audioPlayer;
    TimerImpl timer;
    MaskerPlayerImpl maskerPlayer{&audioPlayer, &audioReader, &timer};
    maskerPlayer.setRampFor(Duration{0.02});
    FileWriter fileWriter;
    TimeStampImpl timeStamp;
    UnixFileSystemPath systemPath;
    const auto outputFileName{outputFileNameFactory.make(timeStamp)};
    OutputFilePathImpl outputFilePath{*outputFileName, systemPath};
    outputFilePath.setRelativeOutputDirectory(relativeOutputDirectory);
    OutputFileImpl outputFile{fileWriter, outputFilePath};
    adaptive_track::AdaptiveTrack::Factory snrTrackFactory;
    ResponseEvaluatorImpl responseEvaluator;
    TextFileReaderImpl textFileReader;
    MersenneTwisterRandomizer randomizer;
    AdaptiveMethodImpl adaptiveMethod{
        snrTrackFactory, responseEvaluator, randomizer};
    MacOsDirectoryReader directoryReader;
    FileExtensionFilter targetFileExtensionFilter{
        {".mov", ".avi", ".wav", ".mp4"}};
    FileFilterDecorator onlyIncludesTargetFileExtensions{
        &directoryReader, &targetFileExtensionFilter};
    RandomizedTargetPlaylistWithReplacement targetsWithReplacement{
        &onlyIncludesTargetFileExtensions, &randomizer};
    FileIdentifierExcluderFilter
        excludesTargetsThatHave100_200_300Or400InTheirName{
            {"100", "200", "300", "400"}};
    FileIdentifierFilter targetsThatHave100InTheirName{"100"};
    FileIdentifierFilter targetsThatHave200InTheirName{"200"};
    FileIdentifierFilter targetsThatHave300InTheirName{"300"};
    FileIdentifierFilter targetsThatHave400InTheirName{"400"};
    FileFilterDecorator allButSilentIntervalTargets{
        &onlyIncludesTargetFileExtensions,
        &excludesTargetsThatHave100_200_300Or400InTheirName};
    FileFilterDecorator oneHundred_ms_SilentIntervalTargets{
        &onlyIncludesTargetFileExtensions, &targetsThatHave100InTheirName};
    FileFilterDecorator twoHundred_ms_SilentIntervalTargets{
        &onlyIncludesTargetFileExtensions, &targetsThatHave200InTheirName};
    FileFilterDecorator threeHundred_ms_SilentIntervalTargets{
        &onlyIncludesTargetFileExtensions, &targetsThatHave300InTheirName};
    FileFilterDecorator fourHundred_ms_SilentIntervalTargets{
        &onlyIncludesTargetFileExtensions, &targetsThatHave400InTheirName};
    RandomSubsetFiles passesThirtyRandomFiles{&randomizer, 30};
    FileFilterDecorator thirtyRandomAllButSilentIntervalTargets{
        &allButSilentIntervalTargets, &passesThirtyRandomFiles};
    FileFilterDecorator thirtyRandomOneHundred_ms_SilentIntervalTargets{
        &oneHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    FileFilterDecorator thirtyRandomTwoHundred_ms_SilentIntervalTargets{
        &twoHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    FileFilterDecorator thirtyRandomThreeHundred_ms_SilentIntervalTargets{
        &threeHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    FileFilterDecorator thirtyRandomFourHundred_ms_SilentIntervalTargets{
        &fourHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    DirectoryReaderComposite silentIntervalTargetsDirectoryReader{
        {&thirtyRandomAllButSilentIntervalTargets,
            &thirtyRandomOneHundred_ms_SilentIntervalTargets,
            &thirtyRandomTwoHundred_ms_SilentIntervalTargets,
            &thirtyRandomThreeHundred_ms_SilentIntervalTargets,
            &thirtyRandomFourHundred_ms_SilentIntervalTargets}};
    RandomizedTargetPlaylistWithoutReplacement silentIntervalTargets{
        &silentIntervalTargetsDirectoryReader, &randomizer};
    RandomizedTargetPlaylistWithoutReplacement everyTargetOnce{
        &onlyIncludesTargetFileExtensions, &randomizer};
    EachTargetPlayedOnceThenShuffleAndRepeat allTargetsNTimes{
        &onlyIncludesTargetFileExtensions, &randomizer};
    FixedLevelMethodImpl fixedLevelMethod{responseEvaluator};
    RecognitionTestModelImpl recognitionTestModel{targetPlayer, maskerPlayer,
        responseEvaluator, outputFile, randomizer, eyeTracker};
    RandomizedTargetPlaylistWithReplacement::Factory
        targetsWithReplacementFactory{
            &onlyIncludesTargetFileExtensions, &randomizer};
    SubdirectoryTargetPlaylistReader targetsWithReplacementReader{
        &targetsWithReplacementFactory, &directoryReader};
    CyclicRandomizedTargetPlaylist::Factory cyclicTargetsFactory{
        &onlyIncludesTargetFileExtensions, &randomizer};
    SubdirectoryTargetPlaylistReader cyclicTargetsReader{
        &cyclicTargetsFactory, &directoryReader};
    ModelImpl model{adaptiveMethod, fixedLevelMethod,
        targetsWithReplacementReader, cyclicTargetsReader,
        targetsWithReplacement, silentIntervalTargets, everyTargetOnce,
        allTargetsNTimes, recognitionTestModel, outputFile};
    const auto viewController{nsTabViewControllerWithoutTabControl()};
    const auto window{
        [NSWindow windowWithContentViewController:viewController]};
    window.title = @"AV Speech in Noise";
    [window makeKeyAndOrderFront:nil];
    const auto app{[NSApplication sharedApplication]};
    app.mainMenu = [[NSMenu alloc] init];
    auto appMenu{[[NSMenuItem alloc] init]};
    auto appSubMenu{[[NSMenu alloc] init]};
    auto preferencesMenuItem {
        [appSubMenu
            addItemWithTitle:@"Preferences..."
                      action:@selector(notifyThatPreferencesHasBeenClicked)
               keyEquivalent:@","]
    };
    auto menuActions{[[MenuActions alloc] init]};
    const auto preferencesViewController{
        [[ResizesToContentsViewController alloc] init]};
    const auto preferencesWindow{
        [NSWindow windowWithContentViewController:preferencesViewController]};
    preferencesWindow.styleMask =
        NSWindowStyleMaskClosable | NSWindowStyleMaskTitled;
    menuActions->preferencesWindow = preferencesWindow;
    preferencesMenuItem.target = menuActions;
    [appSubMenu addItemWithTitle:@"Quit"
                          action:@selector(stop:)
                   keyEquivalent:@"q"];
    [appMenu setSubmenu:appSubMenu];
    [app.mainMenu addItem:appMenu];
    const auto audioDeviceMenu{
        [[NSPopUpButton alloc] initWithFrame:NSMakeRect(0, 0, 0, 0)
                                   pullsDown:NO]};
    const auto applicationDelegate{[[ApplicationDelegate alloc] init]};
    applicationDelegate->audioDeviceMenu = audioDeviceMenu;
    [app setDelegate:applicationDelegate];
    AppKitSessionUI sessionUI{app, preferencesViewController, audioDeviceMenu};
    const auto testSetupViewController{nsTabViewControllerWithoutTabControl()};
    addChild(viewController, testSetupViewController);
    testSetupViewController.view.translatesAutoresizingMaskIntoConstraints = NO;
    [NSLayoutConstraint activateConstraints:@[
        [testSetupViewController.view.topAnchor
            constraintEqualToAnchor:viewController.view.topAnchor
                           constant:8],
        [testSetupViewController.view.bottomAnchor
            constraintEqualToAnchor:viewController.view.bottomAnchor
                           constant:-8],
        [testSetupViewController.view.leadingAnchor
            constraintEqualToAnchor:viewController.view.leadingAnchor
                           constant:8],
        [testSetupViewController.view.trailingAnchor
            constraintEqualToAnchor:viewController.view.trailingAnchor
                           constant:-8]
    ]];
    const auto testSetupUI{testSetupUIFactory.make(testSetupViewController)};
    const auto testViewController{nsTabViewControllerWithoutTabControl()};
    addChild(viewController, testViewController);
    const auto chooseKeywordsUIController{
        nsTabViewControllerWithoutTabControl()};
    const auto freeResponseUIController{nsTabViewControllerWithoutTabControl()};
    const auto correctKeywordsUIController{
        nsTabViewControllerWithoutTabControl()};
    const auto passFailUIController{nsTabViewControllerWithoutTabControl()};
    const auto syllablesUIController{nsTabViewControllerWithoutTabControl()};
    addChild(testViewController, chooseKeywordsUIController);
    addChild(testViewController, freeResponseUIController);
    addChild(testViewController, correctKeywordsUIController);
    addChild(testViewController, passFailUIController);
    addChild(testViewController, syllablesUIController);
    AppKitTestUI testUI{testViewController};
    [window center];
    [window setDelegate:[[WindowDelegate alloc] init]];
    const auto subjectScreenFrame{subjectScreen.frame};
    const auto subjectScreenOrigin{subjectScreenFrame.origin};
    const auto subjectScreenSize{subjectScreenFrame.size};
    const auto subjectViewHeight{subjectScreenSize.height / 4};
    const auto subjectScreenWidth{subjectScreenSize.width};
    const auto subjectViewWidth{subjectScreenWidth / 3};
    auto subjectViewLeadingEdge =
        subjectScreenOrigin.x + (subjectScreenWidth - subjectViewWidth) / 2;
    AppKitConsonantUI consonantView{
        NSMakeRect(subjectScreenOrigin.x + subjectScreenWidth / 4,
            subjectScreenOrigin.y + subjectScreenSize.height / 12,
            subjectScreenWidth / 2, subjectScreenSize.height / 2)};
    AppKitCoordinateResponseMeasureUI coordinateResponseMeasureView{
        NSMakeRect(subjectViewLeadingEdge, subjectScreenOrigin.y,
            subjectViewWidth, subjectViewHeight)};
    ConsonantTaskPresenterImpl consonantPresenter{consonantView};
    FreeResponseUI freeResponseUI{freeResponseUIController};
    FreeResponsePresenter freeResponsePresenter{testUI, freeResponseUI};
    ChooseKeywordsUI chooseKeywordsUI{chooseKeywordsUIController};
    ChooseKeywordsPresenterImpl chooseKeywordsPresenter{model, testUI,
        chooseKeywordsUI,
        sentencesWithThreeKeywords(
            read_file(resourceUrl("mlst-c", "txt").path))};
    SyllablesUI syllablesUI{syllablesUIController};
    SyllablesPresenterImpl syllablesPresenter{syllablesUI, testUI};
    CorrectKeywordsUI correctKeywordsUI{correctKeywordsUIController};
    CorrectKeywordsPresenter correctKeywordsPresenter{
        testUI, correctKeywordsUI};
    PassFailUI passFailUI{passFailUIController};
    PassFailPresenter passFailPresenter{testUI, passFailUI};
    CoordinateResponseMeasurePresenter coordinateResponseMeasurePresenter{
        coordinateResponseMeasureView};
    TestSetupPresenterImpl testSetupPresenter{*(testSetupUI.get()), sessionUI};
    UninitializedTaskPresenterImpl taskPresenter;
    TestPresenterImpl testPresenter{model, testUI, &taskPresenter};
    SessionControllerImpl sessionController{
        model, sessionUI, testSetupPresenter, testPresenter};
    std::ifstream defaultAudioDeviceFile{[applicationDataDirectory()
        URLByAppendingPathComponent:@"default-audio-device.txt"]
                                             .fileSystemRepresentation};
    if (defaultAudioDeviceFile) {
        std::string defaultAudioDevice;
        std::getline(defaultAudioDeviceFile, defaultAudioDevice);
        [audioDeviceMenu selectItemWithTitle:nsString(defaultAudioDevice)];
    }
    TestControllerImpl testController{
        sessionController, model, sessionUI, testUI, testPresenter};
    ChooseKeywordsController chooseKeywordsController{
        testController, model, chooseKeywordsUI, chooseKeywordsPresenter};
    SyllablesController syllablesController{syllablesUI, testController, model,
        {{"B", Syllable::bi}, {"D", Syllable::di}, {"G", Syllable::dji},
            {"F", Syllable::fi}, {"Ghee", Syllable::gi}, {"H", Syllable::hi},
            {"Yee", Syllable::ji}, {"K", Syllable::ki}, {"L", Syllable::li},
            {"M", Syllable::mi}, {"N", Syllable::ni}, {"P", Syllable::pi},
            {"R", Syllable::ri}, {"Sh", Syllable::shi}, {"S", Syllable::si},
            {"Th", Syllable::thi}, {"T", Syllable::ti}, {"Ch", Syllable::tsi},
            {"V", Syllable::vi}, {"W", Syllable::wi}, {"Z", Syllable::zi}}};
    CorrectKeywordsController correctKeywordsController{
        testController, model, sessionUI, correctKeywordsUI};
    FreeResponseController freeResponseController{
        testController, model, freeResponseUI};
    PassFailController passFailController{testController, model, passFailUI};
    ConsonantTaskController consonantTaskController{
        testController, model, consonantView, consonantPresenter};
    CoordinateResponseMeasureController coordinateResponseMeasureController{
        testController, model, coordinateResponseMeasureView};
    coordinateResponseMeasureController.attach(
        &coordinateResponseMeasurePresenter);
    TestSettingsInterpreterImpl testSettingsInterpreter{
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
    TestSetupController testSetupController{*(testSetupUI.get()),
        sessionController, sessionUI, testSetupPresenter, model,
        testSettingsInterpreter, textFileReader};
    sessionController.attach(sessionControllerObserver);
    [app run];
}
}
