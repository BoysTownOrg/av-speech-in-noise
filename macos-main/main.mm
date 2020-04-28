#include "MersenneTwisterRandomizer.hpp"
#include "TobiiEyeTracker.hpp"
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
#include <functional>

@interface WindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation WindowDelegate
- (void)windowWillClose:(NSNotification *)__unused notification {
    [NSApp terminate:self];
}
@end

@interface CallbackScheduler : NSObject
@end

namespace av_speech_in_noise {
namespace {
class TimerImpl : public Timer {
  public:
    TimerImpl();
    void subscribe(EventListener *e) override;
    void scheduleCallbackAfterSeconds(double x) override;
    void timerCallback();

  private:
    EventListener *listener{};
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
namespace {
auto contents(NSString *parent) -> NSArray<NSString *> * {
    return [[NSFileManager defaultManager] contentsOfDirectoryAtPath:parent
                                                               error:nil];
}

auto collectContentsIf(const LocalUrl &directory,
    const std::function<bool(NSString *)> &predicate) -> std::vector<LocalUrl> {
    std::vector<LocalUrl> items{};
    const auto parent{asNsString(directory.path)};
    for (NSString *item in contents(parent)) {
        const auto path{[parent stringByAppendingPathComponent:item]};
        if (predicate(path))
            items.push_back({[item UTF8String]});
    }
    return items;
}

auto isDirectory(NSString *path) -> bool {
    BOOL isDir{NO};
    [[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDir];
    return isDir == YES;
}

auto notADirectory(NSString *path) -> bool { return !isDirectory(path); }

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
    auto homeDirectory() -> std::string override { return std::getenv("HOME"); }

    void createDirectory(std::string s) override {
        mkdir(s.c_str(), ACCESSPERMS);
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
    auto read(const LocalUrl &s) -> std::string override {
        std::ifstream file{s.path};
        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }
};

TimerImpl::TimerImpl() { scheduler->controller = this; }

void TimerImpl::subscribe(EventListener *e) { listener = e; }

void TimerImpl::scheduleCallbackAfterSeconds(double x) {
    [scheduler scheduleCallbackAfterSeconds:x];
}

void TimerImpl::timerCallback() { listener->callback(); }

void main() {
    const auto subjectScreen{[[NSScreen screens] lastObject]};
    AvFoundationVideoPlayer videoPlayer{subjectScreen};
    CoreAudioBufferedReader bufferedReader;
    AudioReaderImpl audioReader{&bufferedReader};
    TargetPlayerImpl targetPlayer{&videoPlayer, &audioReader};
    AvFoundationAudioPlayer audioPlayer;
    TimerImpl timer;
    MaskerPlayerImpl maskerPlayer{&audioPlayer, &audioReader, &timer};
    maskerPlayer.setFadeInOutSeconds(0.5);
    FileWriter writer;
    TimeStampImpl timeStamp;
    UnixFileSystemPath systemPath;
    OutputFilePathImpl path{&timeStamp, &systemPath};
    path.setRelativeOutputDirectory("Documents/AvSpeechInNoise Data");
    OutputFileImpl outputFile{writer, path};
    adaptive_track::AdaptiveTrack::Factory snrTrackFactory;
    ResponseEvaluatorImpl responseEvaluator;
    TextFileReaderImpl textFileReader;
    MersenneTwisterRandomizer randomizer;
    AdaptiveMethodImpl adaptiveMethod{
        snrTrackFactory, responseEvaluator, randomizer};
    MacOsDirectoryReader reader;
    FileExtensionFilter targetFileExtensionFilter{{".mov", ".avi", ".wav"}};
    FileFilterDecorator passesTargetFileExtensions{
        &reader, &targetFileExtensionFilter};
    RandomizedTargetListWithReplacement targetsWithReplacement{
        &passesTargetFileExtensions, &randomizer};
    FileIdentifierExcluderFilter
        filtersOutTargetsThatHave100_200_300Or400InTheirName_{
            {"100", "200", "300", "400"}};
    FileIdentifierFilter targetsThatHave100InTheirName_{"100"};
    FileIdentifierFilter targetsThatHave200InTheirName_{"200"};
    FileIdentifierFilter targetsThatHave300InTheirName_{"300"};
    FileIdentifierFilter targetsThatHave400InTheirName_{"400"};
    FileFilterDecorator noSilentIntervalTargets{&passesTargetFileExtensions,
        &filtersOutTargetsThatHave100_200_300Or400InTheirName_};
    FileFilterDecorator oneHundred_ms_SilentIntervalTargets{
        &passesTargetFileExtensions, &targetsThatHave100InTheirName_};
    FileFilterDecorator twoHundred_ms_SilentIntervalTargets{
        &passesTargetFileExtensions, &targetsThatHave200InTheirName_};
    FileFilterDecorator threeHundred_ms_SilentIntervalTargets{
        &passesTargetFileExtensions, &targetsThatHave300InTheirName_};
    FileFilterDecorator fourHundred_ms_SilentIntervalTargets{
        &passesTargetFileExtensions, &targetsThatHave400InTheirName_};
    RandomSubsetFiles passesThirtyRandomFiles{&randomizer, 30};
    FileFilterDecorator thirtyRandomNoSilentIntervalTargets{
        &noSilentIntervalTargets, &passesThirtyRandomFiles};
    FileFilterDecorator thirtyRandomOneHundred_ms_SilentIntervalTargets{
        &oneHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    FileFilterDecorator thirtyRandomTwoHundred_ms_SilentIntervalTargets{
        &twoHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    FileFilterDecorator thirtyRandomThreeHundred_ms_SilentIntervalTargets{
        &threeHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    FileFilterDecorator thirtyRandomFourHundred_ms_SilentIntervalTargets{
        &fourHundred_ms_SilentIntervalTargets, &passesThirtyRandomFiles};
    DirectoryReaderComposite silentIntervalTargetsDirectoryReader{
        {&thirtyRandomNoSilentIntervalTargets,
            &thirtyRandomOneHundred_ms_SilentIntervalTargets,
            &thirtyRandomTwoHundred_ms_SilentIntervalTargets,
            &thirtyRandomThreeHundred_ms_SilentIntervalTargets,
            &thirtyRandomFourHundred_ms_SilentIntervalTargets}};
    RandomizedTargetListWithoutReplacement silentIntervalTargets{
        &silentIntervalTargetsDirectoryReader, &randomizer};
    RandomizedTargetListWithoutReplacement everyTargetOnce{
        &passesTargetFileExtensions, &randomizer};
    FixedLevelMethodImpl fixedLevelMethod{responseEvaluator};
    TobiiEyeTracker eyeTracker;
    RecognitionTestModelImpl recognitionTestModel{targetPlayer, maskerPlayer,
        responseEvaluator, outputFile, randomizer, eyeTracker};
    RandomizedTargetListWithReplacement::Factory targetsWithReplacementFactory{
        &passesTargetFileExtensions, &randomizer};
    SubdirectoryTargetListReader targetsWithReplacementReader{
        &targetsWithReplacementFactory, &reader};
    CyclicRandomizedTargetList::Factory cyclicTargetsFactory{
        &passesTargetFileExtensions, &randomizer};
    SubdirectoryTargetListReader cyclicTargetsReader{
        &cyclicTargetsFactory, &reader};
    ModelImpl model{adaptiveMethod, fixedLevelMethod,
        targetsWithReplacementReader, cyclicTargetsReader,
        targetsWithReplacement, silentIntervalTargets, everyTargetOnce,
        recognitionTestModel};
    CocoaView view{NSMakeRect(0, 0, 900, 270)};
    view.center();
    const auto delegate{[WindowDelegate alloc]};
    view.setDelegate(delegate);
    const auto subjectScreenFrame{subjectScreen.frame};
    const auto subjectScreenOrigin{subjectScreenFrame.origin};
    const auto subjectScreenSize{subjectScreenFrame.size};
    const auto subjectViewHeight{subjectScreenSize.height / 4};
    const auto subjectScreenWidth{subjectScreenSize.width};
    const auto subjectViewWidth{subjectScreenWidth / 3};
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
}

int main() { av_speech_in_noise::main(); }
