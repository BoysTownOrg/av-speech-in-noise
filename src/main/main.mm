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
#include <target-list/FileFilterDecorator.hpp>
#include <sys/stat.h>
#include <fstream>

class MacOsDirectoryReader : public target_list::DirectoryReader {
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

int main() {
    MacOsDirectoryReader reader;
    target_list::FileFilterDecorator filter{&reader, {".mov", ".avi", ".wav"}};
    MersenneTwisterRandomizer randomizer;
    target_list::RandomizedTargetList list{&filter, &randomizer};
    AvFoundationVideoPlayer videoPlayer;
    CoreAudioBufferedReader bufferedReader;
    stimulus_players::AudioReaderImpl audioReader{&bufferedReader};
    stimulus_players::TargetPlayerImpl targetPlayer{&videoPlayer, &audioReader};
    AvFoundationAudioPlayer audioPlayer;
    stimulus_players::MaskerPlayerImpl maskerPlayer{&audioPlayer, &audioReader};
    maskerPlayer.setFadeInOutSeconds(0.5);
    FileWriter writer;
    TimeStampImpl timeStamp;
    UnixFileSystemPath systemPath;
    av_coordinate_response_measure::OutputFilePathImpl path{&timeStamp, &systemPath};
    path.setRelativeOutputDirectory("Documents/AVCoordinatedResponseMeasureResults");
    av_coordinate_response_measure::OutputFileImpl outputFile{&writer, &path};
    av_coordinate_response_measure::AdaptiveTrack snrTrack{};
    av_coordinate_response_measure::ResponseEvaluatorImpl responseEvaluator{};
    av_coordinate_response_measure::RecognitionTestModel model{
        &list,
        &targetPlayer,
        &maskerPlayer,
        &snrTrack,
        &responseEvaluator,
        &outputFile
    };
    auto rect = NSMakeRect(15, 15, 900 - 15 * 2, 400 - 15 * 2);
    CocoaTestSetupView testSetupView{rect};
    testSetupView.setMasker("/Users/basset/Documents/maxdetection/Stimuli/Masker/L1L2_EngEng.wav");
    testSetupView.setTargetListDirectory("/Users/basset/Documents/maxdetection/Stimuli/Video/List_Detection");
    CocoaTesterView testerView{rect};
    CocoaView view{NSMakeRect(15, 15, 900, 400)};
    view.addSubview(testSetupView.view());
    view.addSubview(testerView.view());
    CocoaSubjectView subjectView{};
    av_coordinate_response_measure::Presenter::Tester tester{&testerView};
    av_coordinate_response_measure::Presenter::Subject subject{&subjectView};
    av_coordinate_response_measure::Presenter::TestSetup testSetup{&testSetupView};
    av_coordinate_response_measure::Presenter presenter{&model, &view, &testSetup, &tester, &subject};
    presenter.run();
}
