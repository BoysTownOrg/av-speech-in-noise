#include "assert-utility.h"
#include <masker-player/RandomizedMaskerPlayer.hpp>
#include <gtest/gtest.h>

namespace {
    class VideoPlayerStub : public VideoPlayer {
        std::string filePath_{};
        std::string deviceDescription_{};
        int deviceIndex_{};
        int deviceCount_{};
        int deviceDescriptionDeviceIndex_{};
        EventListener *listener_{};
        bool playing_{};
        bool played_{};
    public:
        void setPlaying() {
            playing_ = true;
        }
        
        bool playing() override {
            return playing_;
        }
        
        void loadFile(std::string s) override {
            filePath_ = std::move(s);
        }
        
        void setDevice(int index) override {
            deviceIndex_ = index;
        }
        
        int deviceCount() override {
            return deviceCount_;
        }
        
        std::string deviceDescription(int index) override {
            deviceDescriptionDeviceIndex_ = index;
            return deviceDescription_;
        }
        
        void play() override {
            played_ = true;
        }
        
        void subscribe(EventListener * listener) override {
            listener_ = listener;
        }
        
        auto filePath() const {
            return filePath_;
        }
        
        auto deviceIndex() const {
            return deviceIndex_;
        }
        
        void setDeviceCount(int x) {
            deviceCount_ = x;
        }
        
        void setDeviceDescription(std::string s) {
            deviceDescription_ = std::move(s);
        }
        
        auto deviceDescriptionDeviceIndex() const {
            return deviceDescriptionDeviceIndex_;
        }
        
        auto played() const {
            return played_;
        }
        
        void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) {
            listener_->fillAudioBuffer(audio);
        }
    };

    class RandomizedMaskerPlayerTests : public ::testing::Test {
    protected:
        std::vector<float> leftChannel{};
        VideoPlayerStub videoPlayer;
        RandomizedMaskerPlayer player{&videoPlayer};
        
        void fillAudioBuffer() {
            videoPlayer.fillAudioBuffer({ leftChannel });
        }
    };

    TEST_F(RandomizedMaskerPlayerTests, playingWhenVideoPlayerPlaying) {
        videoPlayer.setPlaying();
        EXPECT_TRUE(player.playing());
    }

    TEST_F(RandomizedMaskerPlayerTests, loadFileLoadsVideoFile) {
        player.loadFile("a");
        assertEqual("a", videoPlayer.filePath());
    }

    TEST_F(RandomizedMaskerPlayerTests, setDeviceSetsDevice) {
        player.setDevice(1);
        EXPECT_EQ(1, videoPlayer.deviceIndex());
    }

    TEST_F(RandomizedMaskerPlayerTests, returnsVideoPlayerDeviceCount) {
        videoPlayer.setDeviceCount(1);
        EXPECT_EQ(1, player.deviceCount());
    }

    TEST_F(RandomizedMaskerPlayerTests, returnsVideoPlayerDeviceDescription) {
        videoPlayer.setDeviceDescription("a");
        assertEqual("a", player.deviceDescription({}));
    }

    TEST_F(RandomizedMaskerPlayerTests, passesDeviceIndexToDeviceDescription) {
        player.deviceDescription(1);
        EXPECT_EQ(1, videoPlayer.deviceDescriptionDeviceIndex());
    }

    TEST_F(RandomizedMaskerPlayerTests, fadeInPlaysVideoPlayer) {
        player.fadeIn();
        EXPECT_TRUE(videoPlayer.played());
    }

    TEST_F(RandomizedMaskerPlayerTests, twentydBMultipliesSignalByTen) {
        player.setLevel_dB(20);
        leftChannel = { 1, 2, 3 };
        fillAudioBuffer();
        assertEqual({ 10, 20, 30 }, leftChannel);
    }

    TEST_F(RandomizedMaskerPlayerTests, minusTwentydBDividesSignalByTen) {
        player.setLevel_dB(-20);
        leftChannel = { 1, 2, 3 };
        fillAudioBuffer();
        assertEqual({ 1/10., 2/10., 3/10. }, leftChannel);
    }
}
