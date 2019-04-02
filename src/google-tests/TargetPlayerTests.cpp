#include "assert-utility.h"
#include <target-player/TargetPlayerImpl.hpp>
#include <gtest/gtest.h>
#include <cmath>

namespace {
    class VideoPlayerStub : public target_player::VideoPlayer {
        std::vector<std::string> audioDeviceDescriptions_{};
        std::vector<std::vector<float>> audioRead_{};
        std::string filePath_{};
        int deviceIndex_{};
        EventListener *listener_{};
        bool shown_{};
        bool hidden_{};
        bool played_{};
    public:
        void setAudioRead(std::vector<std::vector<float>> x) {
            audioRead_ = std::move(x);
        }
        
        int deviceCount() override {
            return gsl::narrow<int>(audioDeviceDescriptions_.size());
        }
        
        std::string deviceDescription(int index) override {
            return audioDeviceDescriptions_.at(index);
        }
        
        void setAudioDeviceDescriptions(std::vector<std::string> v) {
            audioDeviceDescriptions_ = std::move(v);
        }
        
        void playbackComplete() {
            listener_->playbackComplete();
        }
        
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
        
        void subscribe(EventListener *e) override {
            listener_ = e;
        }
        
        std::vector<std::vector<float>> readAudio(std::string filePath) override {
            return audioRead_;
        }
        
        
        void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) {
            listener_->fillAudioBuffer(audio);
        }
    };
    
    class TargetPlayerListenerStub :
        public recognition_test::TargetPlayer::EventListener
    {
        bool notified_{};
    public:
        void playbackComplete() override {
            notified_ = true;
        }
        
        auto notified() const {
            return notified_;
        }
    };

    class TargetPlayerTests : public ::testing::Test {
    protected:
        std::vector<float> leftChannel{};
        VideoPlayerStub videoPlayer;
        TargetPlayerListenerStub listener;
        target_player::TargetPlayerImpl player{&videoPlayer};
        
        TargetPlayerTests() {
            player.subscribe(&listener);
        }
        
        void fillAudioBuffer() {
            videoPlayer.fillAudioBuffer({ leftChannel });
        }
        
        void setAudioDeviceDescriptions(std::vector<std::string> v) {
            videoPlayer.setAudioDeviceDescriptions(std::move(v));
        }
    };

    TEST_F(TargetPlayerTests, playPlaysVideo) {
        player.play();
        EXPECT_TRUE(videoPlayer.played());
    }

    TEST_F(TargetPlayerTests, showVideoShowsVideo) {
        player.showVideo();
        EXPECT_TRUE(videoPlayer.shown());
    }

    TEST_F(TargetPlayerTests, hideVideoHidesVideo) {
        player.hideVideo();
        EXPECT_TRUE(videoPlayer.hidden());
    }

    TEST_F(TargetPlayerTests, loadFileLoadsFile) {
        player.loadFile("a");
        assertEqual("a", videoPlayer.filePath());
    }

    TEST_F(TargetPlayerTests, videoPlaybackCompleteNotifiesSubscriber) {
        videoPlayer.playbackComplete();
        EXPECT_TRUE(listener.notified());
    }

    TEST_F(TargetPlayerTests, twentydBMultipliesSignalByTen) {
        player.setLevel_dB(20);
        leftChannel = { 1, 2, 3 };
        fillAudioBuffer();
        assertEqual({ 10, 20, 30 }, leftChannel);
    }

    TEST_F(TargetPlayerTests, setAudioDeviceFindsIndex) {
        setAudioDeviceDescriptions({"zeroth", "first", "second", "third"});
        player.setAudioDevice("second");
        EXPECT_EQ(2, videoPlayer.deviceIndex());
    }

    TEST_F(TargetPlayerTests, setAudioDeviceThrowsInvalidAudioDeviceIfDoesntExist) {
        setAudioDeviceDescriptions({"zeroth", "first", "second"});
        try {
            player.setAudioDevice("third");
            FAIL() << "Expected recognition_test::InvalidAudioDevice";
        } catch(const recognition_test::InvalidAudioDevice &) {
        
        }
    }

    TEST_F(TargetPlayerTests, audioDevicesReturnsDescriptions) {
        setAudioDeviceDescriptions({"a", "b", "c"});
        assertEqual({"a", "b", "c"}, player.audioDevices());
    }

    TEST_F(TargetPlayerTests, rmsComputesFirstChannel) {
        videoPlayer.setAudioRead({
            { 1, 2, 3 },
            { 4, 5, 6 },
            { 7, 8, 9 }
        });
        EXPECT_EQ(std::sqrt((1*1 + 2*2 + 3*3)/3.), player.rms());
    }
}
