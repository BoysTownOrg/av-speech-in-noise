#include "MersenneTwisterRandomizer.h"
#include "AvFoundationPlayers.h"
#include "CocoaView.h"
#include "common-objc.h"
#include <presentation/Presenter.h>
#include <recognition-test/RecognitionTestModel.hpp>
#include <recognition-test/OutputFileImpl.hpp>
#include <recognition-test/OutputFilePathImpl.hpp>
#include <recognition-test/AdaptiveTrack.hpp>
#include <recognition-test/ResponseEvaluatorImpl.hpp>
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <stimulus-players/TargetPlayerImpl.hpp>
#include <stimulus-players/AudioReaderImpl.hpp>
#include <target-list/RandomizedTargetList.hpp>
#include <target-list/FileExtensionFilterDecorator.hpp>
#include <sys/stat.h>
#include <fstream>

class MacOsDirectoryReader :
    public av_coordinate_response_measure::DirectoryReader,
    public target_list::DirectoryReader
{
    std::vector<std::string> filesIn(std::string directory) override {
        std::vector<std::string> files{};
        const auto contents = [[NSFileManager defaultManager]
            contentsOfDirectoryAtPath: asNsString(directory)
            error: nil
        ];
        for (id thing in contents)
            files.push_back([thing UTF8String]);
        return files;
    }
    
    std::vector<std::string> subDirectories(std::string directory) override {
        auto parent = asNsString(directory);
        const auto contents = [[NSFileManager defaultManager]
            contentsOfDirectoryAtPath: parent
            error: nil
        ];
        std::vector<std::string> subDirectories_{};
        for (id thing in contents) {
            BOOL isDir = NO;
            auto path = [parent stringByAppendingPathComponent:thing];
            if ([[NSFileManager defaultManager]
                    fileExistsAtPath:path
                    isDirectory:&isDir] &&
                isDir
            )
                subDirectories_.push_back([thing UTF8String]);
        }
        return subDirectories_;
    }
};

class FileWriter : public av_coordinate_response_measure::Writer {
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
};

class UnixFileSystemPath : public av_coordinate_response_measure::FileSystemPath {
    std::string homeDirectory() override {
        return std::getenv("HOME");
    }
    
    void createDirectory(std::string s) override {
        mkdir(s.c_str(), ACCESSPERMS);
    }
};

class TimeStampImpl : public av_coordinate_response_measure::TimeStamp {
    tm dummyTime{};
    tm *time{&dummyTime};
public:
    int year() override {
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

int main() {
    MacOsDirectoryReader reader;
    target_list::FileExtensionFilterDecorator fileExtensions{
        &reader,
        {".mov", ".avi", ".wav"}
    };
    MersenneTwisterRandomizer randomizer;
    target_list::RandomizedTargetListFactory targetListFactory{&fileExtensions, &randomizer};
    av_coordinate_response_measure::SubdirectoryTargetListReader targetListReader{&targetListFactory, &reader};
    auto subjectScreen = [[NSScreen screens] lastObject];
    auto subjectScreenFrame = subjectScreen.frame;
    auto subjectScreenOrigin = subjectScreenFrame.origin;
    AvFoundationVideoPlayer videoPlayer{subjectScreen};
    CoreAudioBufferedReader bufferedReader;
    stimulus_players::AudioReaderImpl audioReader{&bufferedReader};
    stimulus_players::TargetPlayerImpl targetPlayer{&videoPlayer, &audioReader};
    AvFoundationAudioPlayer audioPlayer;
    TimerImpl timer;
    stimulus_players::MaskerPlayerImpl maskerPlayer{&audioPlayer, &audioReader, &timer};
    maskerPlayer.setFadeInOutSeconds(0.5);
    FileWriter writer;
    TimeStampImpl timeStamp;
    UnixFileSystemPath systemPath;
    av_coordinate_response_measure::OutputFilePathImpl path{&timeStamp, &systemPath};
    path.setRelativeOutputDirectory(
        "Documents/AVCoordinatedResponseMeasureResults"
    );
    av_coordinate_response_measure::OutputFileImpl outputFile{&writer, &path};
    av_coordinate_response_measure::AdaptiveTrackFactory snrTrackFactory{};
    av_coordinate_response_measure::ResponseEvaluatorImpl responseEvaluator{};
    av_coordinate_response_measure::RecognitionTestModel model{
        &targetListReader,
        &targetPlayer,
        &maskerPlayer,
        &snrTrackFactory,
        &responseEvaluator,
        &outputFile,
        &randomizer
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
    testSetupView.setStartingSnr_dB("10");
    testSetupView.setMasker(
        "/Users/basset/Documents/maxdetection/Stimuli/Masker/L1L2_EngEng.wav"
    );
    testSetupView.setCalibration(
        "/Users/basset/Documents/maxdetection/Stimuli/Video/List_1/List1E01AVLNST.mov"
    );
    testSetupView.setTargetListDirectory(
        "/Users/basset/Documents/Lalonde/Lalonde-coordinate-response/Seth Mars Attack"
    );
    CocoaView view{testerWindowFrame};
    view.addSubview(testSetupView.view());
    view.center();
    auto subjectScreenSize = subjectScreenFrame.size;
    auto subjectViewHeight = subjectScreenSize.height / 4;
    auto subjectViewWidth = subjectScreenSize.width / 3;
    auto subjectViewLeadingEdge =
        subjectScreenOrigin.x +
        (subjectScreenSize.width - subjectViewWidth) / 2;
    CocoaSubjectView subjectView{NSMakeRect(
        subjectViewLeadingEdge,
        subjectScreenOrigin.y,
        subjectViewWidth,
        subjectViewHeight
    )};
    av_coordinate_response_measure::Presenter::Subject subject{&subjectView};
    av_coordinate_response_measure::Presenter::TestSetup testSetup{&testSetupView};
    av_coordinate_response_measure::Presenter presenter{
        &model,
        &view,
        &testSetup,
        &subject
    };
    presenter.run();
}
