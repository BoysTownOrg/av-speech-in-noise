#include "MersenneTwisterRandomizer.h"
#include "AvFoundationPlayers.h"
#include "CocoaView.h"
#include "common-objc.h"
#include <presentation/Presenter.h>
#include <recognition-test/RecognitionTestModel.hpp>
#include <recognition-test/OutputFileImpl.hpp>
#include <recognition-test/OutputFilePathImpl.hpp>
#include <masker-player/MaskerPlayerImpl.hpp>
#include <target-list/RandomizedTargetList.hpp>
#include <target-list/FileFilterDecorator.hpp>
#include <target-player/TargetPlayerImpl.hpp>
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

class FileWriter : public recognition_test::Writer {
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

class UnixFileSystemPath : public recognition_test::FileSystemPath {
    std::string homeDirectory() override {
        return std::getenv("HOME");
    }
    
    void createDirectory(std::string s) override {
        mkdir(s.c_str(), ACCESSPERMS);
    }
};

class TimeStampImpl : public recognition_test::TimeStamp {
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
    target_list::FileFilterDecorator filter{&reader, ".mov"};
    MersenneTwisterRandomizer randomizer;
    target_list::RandomizedTargetList list{&filter, &randomizer};
    AvFoundationVideoPlayer videoPlayer;
    target_player::TargetPlayerImpl targetPlayer{&videoPlayer};
    AvFoundationAudioPlayer audioPlayer;
    masker_player::MaskerPlayerImpl maskerPlayer{&audioPlayer};
    maskerPlayer.setFadeInOutSeconds(0.5);
    FileWriter writer;
    TimeStampImpl timeStamp;
    UnixFileSystemPath systemPath;
    recognition_test::OutputFilePathImpl path{&timeStamp, &systemPath};
    path.setRelativeOutputDirectory("Documents/AVCoordinatedResponseMeasureResults");
    recognition_test::OutputFileImpl outputFile{&writer, &path};
    recognition_test::RecognitionTestModel model{
        &list,
        &targetPlayer,
        &maskerPlayer,
        &outputFile
    };
    CocoaView view;
    presentation::Presenter presenter{&model, &view};
    presenter.run();
}
