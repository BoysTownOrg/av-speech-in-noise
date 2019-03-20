#include <recognition-test/RecognitionTestModel.hpp>

namespace stimulus_player {
    class VideoPlayer {
    public:
        virtual ~VideoPlayer() = default;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void loadFile(std::string) = 0;
        virtual void play() = 0;
        virtual void setDevice(int index) = 0;
    };
    
    class StimulusPlayerImpl : public recognition_test::StimulusPlayer {
        VideoPlayer *player;
    public:
        StimulusPlayerImpl(VideoPlayer *player) : player{player} {}
        
        void subscribe(EventListener *) override {
        
        }
        
        void setDevice(int index) override {
            player->setDevice(index);
        }
        
        void play() override {
            player->play();
        }
        
        void loadFile(std::string filePath) override {
            player->loadFile(filePath);
        }
        
        void hideVideo() override {
            player->hide();
        }
        
        void showVideo() override {
            player->show();
        }
        
        double rms() override {
            return 0;
        }
        
        void setLevel_dB(double) override {
        
        }
    };
}

#include "assert-utility.h"
#include <gtest/gtest.h>
namespace {
    class VideoPlayerStub : public stimulus_player::VideoPlayer {
        std::string filePath_{};
        int deviceIndex_{};
        bool shown_{};
        bool hidden_{};
        bool played_{};
    public:
        void setDevice(int index) override {
            deviceIndex_ = index;
        }
        
        auto deviceIndex() const {
            return deviceIndex_;
        }
        
        void play() override {
            played_ = true;
        }
        
        auto played() const {
            return played_;
        }
        
        void loadFile(std::string f) override {
            filePath_ = std::move(f);
        }
        
        auto filePath() const {
            return filePath_;
        }
        
        void hide() override {
            hidden_ = true;
        }
        
        auto hidden() const {
            return hidden_;
        }
        
        auto shown() const {
            return shown_;
        }
        
        void show() override {
            shown_ = true;
        }
    };

    class StimulusPlayerTests : public ::testing::Test {
    protected:
        VideoPlayerStub videoPlayer;
        stimulus_player::StimulusPlayerImpl player{&videoPlayer};
    };

    TEST_F(StimulusPlayerTests, playPlaysVideo) {
        player.play();
        EXPECT_TRUE(videoPlayer.played());
    }

    TEST_F(StimulusPlayerTests, showVideoShowsVideo) {
        player.showVideo();
        EXPECT_TRUE(videoPlayer.shown());
    }

    TEST_F(StimulusPlayerTests, hideVideoHidesVideo) {
        player.hideVideo();
        EXPECT_TRUE(videoPlayer.hidden());
    }

    TEST_F(StimulusPlayerTests, loadFileLoadsFile) {
        player.loadFile("a");
        assertEqual("a", videoPlayer.filePath());
    }

    TEST_F(StimulusPlayerTests, setDeviceSetsVideoPlayerDevice) {
        player.setDevice(1);
        EXPECT_EQ(1, videoPlayer.deviceIndex());
    }
}
