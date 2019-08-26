#include "MersenneTwisterRandomizer.h"
#include "AvFoundationPlayers.h"
#include "CocoaView.h"
#include "common-objc.h"
#include <presentation/Presenter.h>
#include <recognition-test/RecognitionTestModel.hpp>
#include <recognition-test/RecognitionTestModel_Internal.hpp>
#include <recognition-test/AdaptiveMethod.hpp>
#include <recognition-test/TrackSettingsReader.hpp>
#include <recognition-test/TrackSettingsInterpreter.hpp>
#include <recognition-test/FixedLevelMethod.hpp>
#include <recognition-test/OutputFileImpl.hpp>
#include <recognition-test/OutputFilePathImpl.hpp>
#include <recognition-test/ResponseEvaluatorImpl.hpp>
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <stimulus-players/TargetPlayerImpl.hpp>
#include <stimulus-players/AudioReaderImpl.hpp>
#include <target-list/RandomizedTargetList.hpp>
#include <target-list/FileExtensionFilterDecorator.hpp>
#include <adaptive-track/AdaptiveTrack.hpp>
#include <sys/stat.h>
#include <fstream>
#include <sstream>

class MacOsDirectoryReader : public target_list::DirectoryReader
{
    std::vector<std::string> filesIn(std::string directory) override {
        return collectAllPredicateAbidingContents(
            std::move(directory),
            &MacOsDirectoryReader::notADirectory
        );
    }
    
    std::vector<std::string> subDirectories(std::string directory) override {
        return collectAllPredicateAbidingContents(
            std::move(directory),
            &MacOsDirectoryReader::isDirectory
        );
    }
    
    std::vector<std::string> collectAllPredicateAbidingContents(
        std::string directory,
        BOOL(MacOsDirectoryReader::*f)(NSString *)
    ) {
        std::vector<std::string> items{};
        auto parent = asNsString(directory);
        for (id item in contents(parent)) {
            auto path = combinePath(parent, item);
            if ((this->*f)(path))
                items.push_back(toCStr(item));
        }
        return items;
    }
    
    NSArray<NSString *> *contents(NSString *parent) {
        return [[NSFileManager defaultManager]
            contentsOfDirectoryAtPath: parent
            error: nil
        ];
    }
    
    NSString *combinePath(NSString *base, id toAppend) {
        return [base stringByAppendingPathComponent:toAppend];
    }
    
    BOOL isDirectory(NSString *path) {
        BOOL isDir = NO;
        [[NSFileManager defaultManager]
            fileExistsAtPath:path
            isDirectory:&isDir];
        return isDir;
    }
    
    BOOL notADirectory(NSString *path) {
        return !isDirectory(path);
    }
    
    const char * _Nullable toCStr(id item) {
        return [item UTF8String];
    }
};

class FileWriter : public av_speech_in_noise::Writer {
    std::ofstream file{};
public:
    void write(std::string s) override {
        file << std::move(s);
    }
    
    void open(std::string s) override {
        file.open(s);
    }
    
    bool failed() override {
        return file.fail();
    }
    
    void close() override {
        file.close();
    }
    
    void save() override {
        file.flush();
    }
};

class UnixFileSystemPath : public av_speech_in_noise::FileSystemPath {
    std::string homeDirectory() override {
        return std::getenv("HOME");
    }
    
    void createDirectory(std::string s) override {
        mkdir(s.c_str(), ACCESSPERMS);
    }
};

class TimeStampImpl : public av_speech_in_noise::TimeStamp {
    tm dummyTime{};
    tm *time{&dummyTime};
public:
    int year() override {
        // https://en.cppreference.com/w/c/chrono/tm
        return time->tm_year + 1900;
    }
    
    int month() override {
        return time->tm_mon;
    }
    
    int dayOfMonth() override {
        return time->tm_mday;
    }
    
    int hour() override {
        return time->tm_hour;
    }
    
    int minute() override {
        return time->tm_min;
    }
    
    int second() override {
        return time->tm_sec;
    }
    
    void capture() override {
        auto now = std::time(nullptr);
        time = std::localtime(&now);
        // Add fail method?
        if (time == nullptr)
            time = &dummyTime;
    }
};

class TimerImpl;

@interface CallbackScheduler : NSObject
@property TimerImpl *controller;
- (void) scheduleCallbackAfterSeconds: (double) x;
- (void) timerCallback;
@end

class TimerImpl : public stimulus_players::Timer {
    EventListener *listener{};
    CallbackScheduler *scheduler{[CallbackScheduler alloc]};
public:
    TimerImpl() {
        scheduler.controller = this;
    }
    
    void subscribe(EventListener *e) override {
        listener = e;
    }
    
    void scheduleCallbackAfterSeconds(double x) override{
        [scheduler scheduleCallbackAfterSeconds:x];
    }
    
    void timerCallback() {
        listener->callback();
    }
};

@implementation CallbackScheduler
@synthesize controller;

- (void)scheduleCallbackAfterSeconds:(double)x {
    [NSTimer
        scheduledTimerWithTimeInterval:x
        target:self
        selector: @selector(timerCallback)
        userInfo:nil
        repeats:NO
    ];
}

- (void)timerCallback {
    controller->timerCallback();
}
@end

class TextFileReaderImpl : public av_speech_in_noise::TextFileReader {
    std::string read(std::string s) override {
        std::ifstream file{std::move(s)};
        std::stringstream stream;
        stream << file.rdbuf();
        return stream.str();
    }
};

int main() {
    using namespace av_speech_in_noise;
    MacOsDirectoryReader reader;
    target_list::FileExtensionFilterDecorator fileExtensions{
        &reader,
        {".mov", ".avi", ".wav"}
    };
    MersenneTwisterRandomizer randomizer;
    target_list::RandomizedTargetListFactory targetListFactory{
        &fileExtensions,
        &randomizer
    };
    target_list::SubdirectoryTargetListReader targetListReader{
        &targetListFactory,
        &reader
    };
    auto subjectScreen = [[NSScreen screens] lastObject];
    auto subjectScreenFrame = subjectScreen.frame;
    auto subjectScreenOrigin = subjectScreenFrame.origin;
    AvFoundationVideoPlayer videoPlayer{subjectScreen};
    CoreAudioBufferedReader bufferedReader;
    stimulus_players::AudioReaderImpl audioReader{&bufferedReader};
    stimulus_players::TargetPlayerImpl targetPlayer{
        &videoPlayer,
        &audioReader
    };
    AvFoundationAudioPlayer audioPlayer;
    TimerImpl timer;
    stimulus_players::MaskerPlayerImpl maskerPlayer{
        &audioPlayer,
        &audioReader,
        &timer
    };
    maskerPlayer.setFadeInOutSeconds(0.5);
    FileWriter writer;
    TimeStampImpl timeStamp;
    UnixFileSystemPath systemPath;
    OutputFilePathImpl path{&timeStamp, &systemPath};
    path.setRelativeOutputDirectory(
        "Documents/AVCoordinateResponseMeasureResults"
    );
    OutputFileImpl outputFile{&writer, &path};
    adaptive_track::AdaptiveTrackFactory snrTrackFactory;
    ResponseEvaluatorImpl responseEvaluator;
    TrackSettingsInterpreter trackSettingsInterpreter;
    TextFileReaderImpl textFileReader;
    TrackSettingsReader trackSettingsReader{
        &textFileReader,
        &trackSettingsInterpreter
    };
    AdaptiveMethod adaptiveMethod{
        &targetListReader,
        &trackSettingsReader,
        &snrTrackFactory,
        &responseEvaluator,
        &randomizer
    };
    target_list::RandomizedTargetList infiniteTargetList{
        &fileExtensions,
        &randomizer
    };
    target_list::FileIdentifierFilterDecorator 
        oneHundredMsStimuli{&fileExtensions, "100ms"};
    target_list::FileIdentifierFilterDecorator 
        twoHundredMsStimuli{&fileExtensions, "200ms"};
    target_list::FileIdentifierFilterDecorator 
        threeHundredMsStimuli{&fileExtensions, "300ms"};
    target_list::FileIdentifierFilterDecorator 
        fourHundredMsStimuli{&fileExtensions, "400ms"};
    target_list::RandomSubsetFilesDecorator 
        randomSubsetOneHundredMsStimuli{&oneHundredMsStimuli, &randomizer, 30};
    target_list::RandomSubsetFilesDecorator 
        randomSubsetTwoHundredMsStimuli{&twoHundredMsStimuli, &randomizer, 30};
    target_list::RandomSubsetFilesDecorator 
        randomSubsetThreeHundredMsStimuli{&threeHundredMsStimuli, &randomizer, 30};
    target_list::RandomSubsetFilesDecorator 
        randomSubsetFourHundredMsStimuli{&fourHundredMsStimuli, &randomizer, 30};
    target_list::DirectoryReaderComposite composite{{
        &randomSubsetOneHundredMsStimuli, 
        &randomSubsetTwoHundredMsStimuli, 
        &randomSubsetThreeHundredMsStimuli, 
        &randomSubsetFourHundredMsStimuli
    }};
    target_list::RandomizedFiniteTargetList finiteTargetList{
        &composite,
        &randomizer
    };
    EmptyTargetListTestConcluder completesWhenTargetsEmpty;
    FixedTrialTestConcluder fixedTrials;
    FixedLevelMethod fixedLevelMethod{
        &responseEvaluator
    };
    RecognitionTestModel_Internal model_internal{
        &targetPlayer,
        &maskerPlayer,
        &responseEvaluator,
        &outputFile,
        &randomizer
    };
    RecognitionTestModel model{
        &adaptiveMethod,
        &fixedLevelMethod,
        &infiniteTargetList,
        &fixedTrials,
        &finiteTargetList,
        &completesWhenTargetsEmpty,
        &model_internal
    };
    auto testerWindowFrame = NSMakeRect(15, 15, 900, 400);
    auto testerWindowViewMargin = 15;
    auto testerContentFrame = NSMakeRect(
        testerWindowViewMargin,
        testerWindowViewMargin,
        testerWindowFrame.size.width - testerWindowViewMargin * 2,
        testerWindowFrame.size.height - testerWindowViewMargin * 2
    );
    CocoaTestSetupView testSetupView{testerContentFrame};
    testSetupView.setMaskerLevel_dB_SPL("65");
    testSetupView.setCalibrationLevel_dB_SPL("65");
    testSetupView.setStartingSnr_dB("5");
    testSetupView.setMasker(
        "/Users/basset/Documents/maxdetection/Stimuli/Masker/L1L2_EngEng.wav"
    );
    testSetupView.setCalibration(
        "/Users/basset/Documents/maxdetection/Stimuli/Video/List_1/List1E01AVLNST.mov"
    );
    testSetupView.setTargetListDirectory(
        "/Users/basset/Documents/Lalonde/Lalonde-coordinate-response/Seth Mars Attack"
    );
    testSetupView.setTrackSettingsFile(
        "/Users/basset/Desktop/track-settings.txt"
    );
    CocoaExperimenterView experimenterView{testerContentFrame};
    CocoaView view{testerWindowFrame};
    view.addSubview(testSetupView.view());
    view.addSubview(experimenterView.view());
    view.center();
    auto subjectScreenSize = subjectScreenFrame.size;
    auto subjectViewHeight = subjectScreenSize.height / 4;
    auto subjectScreenWidth = subjectScreenSize.width;
    auto subjectViewWidth = subjectScreenWidth / 3;
    auto subjectViewLeadingEdge =
        subjectScreenOrigin.x +
        (subjectScreenWidth - subjectViewWidth) / 2;
    CocoaSubjectView subjectView{NSMakeRect(
        subjectViewLeadingEdge,
        subjectScreenOrigin.y,
        subjectViewWidth,
        subjectViewHeight
    )};
    Presenter::Subject subject{&subjectView};
    Presenter::TestSetup testSetup{&testSetupView};
    Presenter::Experimenter experimenter{&experimenterView};
    Presenter presenter{
        &model,
        &view,
        &testSetup,
        &subject,
        &experimenter
    };
    presenter.run();
}
