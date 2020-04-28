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

auto collectContentsIf(const LocalUrl &directory, bool (*f)(NSString *))
    -> std::vector<LocalUrl> {
    std::vector<LocalUrl> items{};
    auto parent{asNsString(directory.path)};
    for (NSString *item in contents(parent)) {
        auto path{[parent stringByAppendingPathComponent:item]};
        if ((*f)(path))
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
        auto now{std::time(nullptr)};
        time = std::localtime(&now);
        // Add fail method?
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
    MacOsDirectoryReader reader;
    FileExtensionFilter fileExtensions_{{".mov", ".avi", ".wav"}};
    FileFilterDecorator fileExtensions{&reader, &fileExtensions_};
    MersenneTwisterRandomizer randomizer;
    auto subjectScreen = [[NSScreen screens] lastObject];
    auto subjectScreenFrame = subjectScreen.frame;
    auto subjectScreenOrigin = subjectScreenFrame.origin;
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
    AdaptiveMethodImpl adaptiveMethod{
        snrTrackFactory, responseEvaluator, randomizer};
    RandomizedTargetListWithReplacement targetsWithReplacement{
        &fileExtensions, &randomizer};
    FileIdentifierExcluderFilter originalStimuli_{{"100", "200", "300", "400"}};
    FileIdentifierFilter oneHundredMsStimuli_{"100"};
    FileIdentifierFilter twoHundredMsStimuli_{"200"};
    FileIdentifierFilter threeHundredMsStimuli_{"300"};
    FileIdentifierFilter fourHundredMsStimuli_{"400"};
    FileFilterDecorator originalStimuli{&fileExtensions, &originalStimuli_};
    FileFilterDecorator oneHundredMsStimuli{
        &fileExtensions, &oneHundredMsStimuli_};
    FileFilterDecorator twoHundredMsStimuli{
        &fileExtensions, &twoHundredMsStimuli_};
    FileFilterDecorator threeHundredMsStimuli{
        &fileExtensions, &threeHundredMsStimuli_};
    FileFilterDecorator fourHundredMsStimuli{
        &fileExtensions, &fourHundredMsStimuli_};
    RandomSubsetFiles randomSubsetStimuli{&randomizer, 30};
    FileFilterDecorator randomSubsetOriginalStimuli{
        &originalStimuli, &randomSubsetStimuli};
    FileFilterDecorator randomSubsetOneHundredMsStimuli{
        &oneHundredMsStimuli, &randomSubsetStimuli};
    FileFilterDecorator randomSubsetTwoHundredMsStimuli{
        &twoHundredMsStimuli, &randomSubsetStimuli};
    FileFilterDecorator randomSubsetThreeHundredMsStimuli{
        &threeHundredMsStimuli, &randomSubsetStimuli};
    FileFilterDecorator randomSubsetFourHundredMsStimuli{
        &fourHundredMsStimuli, &randomSubsetStimuli};
    DirectoryReaderComposite composite{{&randomSubsetOriginalStimuli,
        &randomSubsetOneHundredMsStimuli, &randomSubsetTwoHundredMsStimuli,
        &randomSubsetThreeHundredMsStimuli, &randomSubsetFourHundredMsStimuli}};
    RandomizedTargetListWithoutReplacement silentIntervals{
        &composite, &randomizer};
    RandomizedTargetListWithoutReplacement allStimuli{
        &fileExtensions, &randomizer};
    FixedLevelMethodImpl fixedLevelMethod{responseEvaluator};
    TobiiEyeTracker eyeTracker;
    RecognitionTestModelImpl recognitionTestModel{targetPlayer, maskerPlayer,
        responseEvaluator, outputFile, randomizer, eyeTracker};
    RandomizedTargetListWithReplacement::Factory targetsWithReplacementFactory{
        &fileExtensions, &randomizer};
    SubdirectoryTargetListReader targetsWithReplacementReader{
        &targetsWithReplacementFactory, &reader};
    CyclicRandomizedTargetList::Factory cyclicTargetsFactory{
        &fileExtensions, &randomizer};
    SubdirectoryTargetListReader cyclicTargetsReader{
        &cyclicTargetsFactory, &reader};
    ModelImpl model{adaptiveMethod, fixedLevelMethod,
        targetsWithReplacementReader, cyclicTargetsReader,
        targetsWithReplacement, silentIntervals, allStimuli,
        recognitionTestModel};
    CocoaView view{NSMakeRect(0, 0, 900, 270)};
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
}

int main() { av_speech_in_noise::main(); }
