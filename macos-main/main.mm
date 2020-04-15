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

@interface WindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation WindowDelegate
- (void)windowWillClose:(NSNotification *)__unused notification {
    [NSApp terminate:self];
}
@end

@class CallbackScheduler;

namespace {
class TimerImpl : public stimulus_players::Timer {
  public:
    TimerImpl();
    void subscribe(EventListener *e) override;
    void scheduleCallbackAfterSeconds(double x) override;
    void timerCallback();

  private:
    EventListener *listener{};
    CallbackScheduler *scheduler;
};
}

@interface CallbackScheduler : NSObject
@end

@implementation CallbackScheduler {
  @public
    TimerImpl *controller;
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

static auto contents(NSString *parent) -> NSArray<NSString *> * {
    return [[NSFileManager defaultManager] contentsOfDirectoryAtPath:parent
                                                               error:nil];
}

static auto collectContentsIf(const std::string &directory,
    BOOL (*f)(NSString *)) -> std::vector<std::string> {
    std::vector<std::string> items{};
    auto parent{asNsString(directory)};
    for (NSString *item in contents(parent)) {
        auto path{[parent stringByAppendingPathComponent:item]};
        if ((*f)(path) != 0)
            items.emplace_back([item UTF8String]);
    }
    return items;
}

static auto isDirectory(NSString *path) -> BOOL {
    BOOL isDir{NO};
    [[NSFileManager defaultManager] fileExistsAtPath:path isDirectory:&isDir];
    return isDir;
}

static auto notADirectory(NSString *path) -> BOOL {
    return static_cast<BOOL>(isDirectory(path) == 0);
}

namespace {
class MacOsDirectoryReader : public target_list::DirectoryReader {
    auto filesIn(std::string directory) -> std::vector<std::string> override {
        return collectContentsIf(directory, notADirectory);
    }

    auto subDirectories(std::string directory)
        -> std::vector<std::string> override {
        return collectContentsIf(directory, isDirectory);
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
        auto now{std::time(nullptr)};
        time = std::localtime(&now);
        // Add fail method?
        if (time == nullptr)
            time = &dummyTime;
    }
};

class TextFileReaderImpl : public av_speech_in_noise::TextFileReader {
    auto read(const std::string &s) -> std::string override {
        std::ifstream file{s};
        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }
};

TimerImpl::TimerImpl() : scheduler{[[CallbackScheduler alloc] init]} {
    scheduler->controller = this;
}

void TimerImpl::subscribe(EventListener *e) { listener = e; }

void TimerImpl::scheduleCallbackAfterSeconds(double x) {
    [scheduler scheduleCallbackAfterSeconds:x];
}

void TimerImpl::timerCallback() { listener->callback(); }
}

namespace av_speech_in_noise {
static void main() {
    MacOsDirectoryReader reader;
    target_list::FileExtensionFilter fileExtensions_{{".mov", ".avi", ".wav"}};
    target_list::FileFilterDecorator fileExtensions{&reader, &fileExtensions_};
    MersenneTwisterRandomizer randomizer;
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
    path.setRelativeOutputDirectory("Documents/AvSpeechInNoise Data");
    OutputFileImpl outputFile{writer, path};
    adaptive_track::AdaptiveTrack::Factory snrTrackFactory;
    ResponseEvaluatorImpl responseEvaluator;
    TextFileReaderImpl textFileReader;
    AdaptiveMethodImpl adaptiveMethod{
        snrTrackFactory, responseEvaluator, randomizer};
    target_list::RandomizedTargetListWithReplacement targetsWithReplacement{
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
    target_list::RandomizedTargetListWithoutReplacement silentIntervals{
        &composite, &randomizer};
    target_list::RandomizedTargetListWithoutReplacement allStimuli{
        &fileExtensions, &randomizer};
    FixedLevelMethodImpl fixedLevelMethod{responseEvaluator};
    TobiiEyeTracker eyeTracker;
    RecognitionTestModelImpl recognitionTestModel{targetPlayer, maskerPlayer,
        responseEvaluator, outputFile, randomizer, eyeTracker};
    target_list::RandomizedTargetListWithReplacement::Factory
        targetsWithReplacementFactory{&fileExtensions, &randomizer};
    target_list::SubdirectoryTargetListReader targetsWithReplacementReader{
        &targetsWithReplacementFactory, &reader};
    target_list::CyclicRandomizedTargetList::Factory cyclicTargetsFactory{
        &fileExtensions, &randomizer};
    target_list::SubdirectoryTargetListReader cyclicTargetsReader{
        &cyclicTargetsFactory, &reader};
    ModelImpl model{adaptiveMethod, fixedLevelMethod,
        targetsWithReplacementReader, cyclicTargetsReader,
        targetsWithReplacement, silentIntervals, allStimuli,
        recognitionTestModel};
    CocoaView view{NSMakeRect(0, 0, 900, 240)};
    view.center();
    auto delegate{[WindowDelegate alloc]};
    view.setDelegate(delegate);
    auto subjectScreenSize{subjectScreenFrame.size};
    auto subjectViewHeight{subjectScreenSize.height / 4};
    auto subjectScreenWidth{subjectScreenSize.width};
    auto subjectViewWidth{subjectScreenWidth / 3};
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

int main() { av_speech_in_noise::main(); }
