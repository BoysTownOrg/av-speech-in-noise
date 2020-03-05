#include "MersenneTwisterRandomizer.hpp"
#include "AvFoundationPlayers.h"
#include "CocoaView.h"
#include "common-objc.h"
#include <presentation/Presenter.hpp>
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
#include <stimulus-players/AudioReaderImpl.hpp>
#include <target-list/RandomizedTargetList.hpp>
#include <target-list/FileFilterDecorator.hpp>
#include <adaptive-track/AdaptiveTrack.hpp>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <utility>

namespace {
auto contents(NSString *parent) -> NSArray<NSString *> * {
    return [[NSFileManager defaultManager] contentsOfDirectoryAtPath:parent
                                                               error:nil];
}

auto combinePath(NSString *base, id toAppend) -> NSString * {
    return [base stringByAppendingPathComponent:toAppend];
}

auto toCStr(id item) -> const char *_Nullable { return [item UTF8String]; }

auto collectContentsIf(std::string directory, BOOL (*f)(NSString *))
    -> std::vector<std::string> {
    std::vector<std::string> items{};
    auto parent = asNsString(std::move(directory));
    for (id item in contents(parent)) {
        auto path = combinePath(parent, item);
        if ((*f)(path) != 0)
            items.emplace_back(toCStr(item));
    }
    return items;
}

auto isDirectory(NSString *path) -> BOOL {
    BOOL isDir = NO;
    [[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDir];
    return isDir;
}

auto notADirectory(NSString *path) -> BOOL {
    return static_cast<BOOL>(isDirectory(path) == 0);
}

class MacOsDirectoryReader : public target_list::DirectoryReader {
    auto filesIn(std::string directory) -> std::vector<std::string> override {
        return collectContentsIf(std::move(directory), notADirectory);
    }

    auto subDirectories(std::string directory)
        -> std::vector<std::string> override {
        return collectContentsIf(std::move(directory), isDirectory);
    }
};

class FileWriter : public av_speech_in_noise::Writer {
    std::ofstream file{};

  public:
    void write(std::string s) override { file << s; }

    void open(std::string s) override { file.open(s); }

    auto failed() -> bool override { return file.fail(); }

    void close() override { file.close(); }

    void save() override { file.flush(); }
};

class UnixFileSystemPath : public av_speech_in_noise::FileSystemPath {
    auto homeDirectory() -> std::string override { return std::getenv("HOME"); }

    void createDirectory(std::string s) override {
        mkdir(s.c_str(), ACCESSPERMS);
    }
};

class TimeStampImpl : public av_speech_in_noise::TimeStamp {
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
        auto now = std::time(nullptr);
        time = std::localtime(&now);
        // Add fail method?
        if (time == nullptr)
            time = &dummyTime;
    }
};
}
namespace av_speech_in_noise::impl {
class TimerImpl;
}

@interface CallbackScheduler : NSObject
@property av_speech_in_noise::impl::TimerImpl *controller;
- (void)scheduleCallbackAfterSeconds:(double)x;
- (void)timerCallback;
@end

namespace av_speech_in_noise::impl {
class TimerImpl : public stimulus_players::Timer {
    EventListener *listener{};
    CallbackScheduler *scheduler{[CallbackScheduler alloc]};

  public:
    TimerImpl() { scheduler.controller = this; }

    void subscribe(EventListener *e) override { listener = e; }

    void scheduleCallbackAfterSeconds(double x) override {
        [scheduler scheduleCallbackAfterSeconds:x];
    }

    void timerCallback() { listener->callback(); }
};
}

@implementation CallbackScheduler
@synthesize controller;

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

namespace {
class TextFileReaderImpl : public av_speech_in_noise::TextFileReader {
    auto read(const std::string &s) -> std::string override {
        std::ifstream file{s};
        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }
};
}

@interface WindowDelegate : NSObject <NSWindowDelegate>
- (void)windowWillClose:(NSNotification *)notification;
@end

@implementation WindowDelegate
- (void)windowWillClose:(NSNotification *)__unused notification {
    [NSApp terminate:self];
}
@end

namespace av_speech_in_noise::impl {
void main() {
    MacOsDirectoryReader reader;
    target_list::FileExtensionFilter fileExtensions_{{".mov", ".avi", ".wav"}};
    target_list::FileFilterDecorator fileExtensions{&reader, &fileExtensions_};
    MersenneTwisterRandomizer randomizer;
    target_list::RandomizedTargetListFactory targetListFactory{
        &fileExtensions, &randomizer};
    target_list::SubdirectoryTargetListReader targetListReader{
        &targetListFactory, &reader};
    auto subjectScreen = [[NSScreen screens] lastObject];
    auto subjectScreenFrame = subjectScreen.frame;
    auto subjectScreenOrigin = subjectScreenFrame.origin;
    stimulus_players::AvFoundationVideoPlayer videoPlayer{subjectScreen};
    stimulus_players::CoreAudioBufferedReader bufferedReader;
    stimulus_players::AudioReaderImpl audioReader{&bufferedReader};
    stimulus_players::TargetPlayerImpl targetPlayer{&videoPlayer, &audioReader};
    stimulus_players::AvFoundationAudioPlayer audioPlayer;
    TimerImpl timer;
    stimulus_players::MaskerPlayerImpl maskerPlayer{
        &audioPlayer, &audioReader, &timer};
    maskerPlayer.setFadeInOutSeconds(0.5);
    FileWriter writer;
    TimeStampImpl timeStamp;
    UnixFileSystemPath systemPath;
    OutputFilePathImpl path{&timeStamp, &systemPath};
    path.setRelativeOutputDirectory(
        "Documents/AVCoordinateResponseMeasureResults");
    OutputFileImpl outputFile{&writer, &path};
    adaptive_track::AdaptiveTrack::Factory snrTrackFactory;
    ResponseEvaluatorImpl responseEvaluator;
    TextFileReaderImpl textFileReader;
    AdaptiveMethodImpl adaptiveMethod{
        &targetListReader, &snrTrackFactory, &responseEvaluator, &randomizer};
    target_list::RandomizedTargetList infiniteTargetList{
        &fileExtensions, &randomizer};
    target_list::FileIdentifierExcluderFilter originalStimuli_{
        {"100", "200", "300", "400"}};
    target_list::FileIdentifierFilter oneHundredMsStimuli_{"100"};
    target_list::FileIdentifierFilter twoHundredMsStimuli_{"200"};
    target_list::FileIdentifierFilter threeHundredMsStimuli_{"300"};
    target_list::FileIdentifierFilter fourHundredMsStimuli_{"400"};
    target_list::FileFilterDecorator originalStimuli{
        &fileExtensions, &originalStimuli_};
    target_list::FileFilterDecorator oneHundredMsStimuli{
        &fileExtensions, &oneHundredMsStimuli_};
    target_list::FileFilterDecorator twoHundredMsStimuli{
        &fileExtensions, &twoHundredMsStimuli_};
    target_list::FileFilterDecorator threeHundredMsStimuli{
        &fileExtensions, &threeHundredMsStimuli_};
    target_list::FileFilterDecorator fourHundredMsStimuli{
        &fileExtensions, &fourHundredMsStimuli_};
    target_list::RandomSubsetFiles randomSubsetStimuli{&randomizer, 30};
    target_list::FileFilterDecorator randomSubsetOriginalStimuli{
        &originalStimuli, &randomSubsetStimuli};
    target_list::FileFilterDecorator randomSubsetOneHundredMsStimuli{
        &oneHundredMsStimuli, &randomSubsetStimuli};
    target_list::FileFilterDecorator randomSubsetTwoHundredMsStimuli{
        &twoHundredMsStimuli, &randomSubsetStimuli};
    target_list::FileFilterDecorator randomSubsetThreeHundredMsStimuli{
        &threeHundredMsStimuli, &randomSubsetStimuli};
    target_list::FileFilterDecorator randomSubsetFourHundredMsStimuli{
        &fourHundredMsStimuli, &randomSubsetStimuli};
    target_list::DirectoryReaderComposite composite{
        {&randomSubsetOriginalStimuli, &randomSubsetOneHundredMsStimuli,
            &randomSubsetTwoHundredMsStimuli,
            &randomSubsetThreeHundredMsStimuli,
            &randomSubsetFourHundredMsStimuli}};
    target_list::RandomizedFiniteTargetList silentIntervals{
        &composite, &randomizer};
    target_list::RandomizedFiniteTargetList allStimuli{
        &fileExtensions, &randomizer};
    EmptyTargetListTestConcluder completesWhenTargetsEmpty;
    FixedTrialTestConcluder fixedTrials;
    FixedLevelMethodImpl fixedLevelMethod{&responseEvaluator};
    RecognitionTestModelImpl model_internal{&targetPlayer, &maskerPlayer,
        &responseEvaluator, &outputFile, &randomizer};
    ModelImpl model{&adaptiveMethod, &fixedLevelMethod, &infiniteTargetList,
        &fixedTrials, &silentIntervals, &completesWhenTargetsEmpty, &allStimuli,
        &model_internal};
    CocoaView view{NSMakeRect(0, 0, 900, 180)};
    view.center();
    auto delegate = [WindowDelegate alloc];
    view.setDelegate(delegate);
    auto subjectScreenSize = subjectScreenFrame.size;
    auto subjectViewHeight = subjectScreenSize.height / 4;
    auto subjectScreenWidth = subjectScreenSize.width;
    auto subjectViewWidth = subjectScreenWidth / 3;
    auto subjectViewLeadingEdge =
        subjectScreenOrigin.x + (subjectScreenWidth - subjectViewWidth) / 2;
    CocoaSubjectView subjectView{NSMakeRect(subjectViewLeadingEdge,
        subjectScreenOrigin.y, subjectViewWidth, subjectViewHeight)};
    Presenter::CoordinateResponseMeasure subject{&subjectView};
    Presenter::TestSetup testSetup{&view.testSetup()};
    Presenter::Experimenter experimenter{&view.experimenter()};
    TestSettingsInterpreterImpl testSettingsInterpreter;
    Presenter presenter{model, view, testSetup, subject, experimenter,
        testSettingsInterpreter, textFileReader};
    presenter.run();
}
}

int main() { av_speech_in_noise::impl::main(); }
