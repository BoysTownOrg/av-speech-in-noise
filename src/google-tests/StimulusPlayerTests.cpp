#include "assert-utility.h"
#include <stimulus-player/StimulusPlayerImpl.hpp>
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
