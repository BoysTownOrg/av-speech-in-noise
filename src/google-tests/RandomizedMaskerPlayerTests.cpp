#include "assert-utility.h"
#include <masker-player/RandomizedMaskerPlayer.hpp>
#include <gtest/gtest.h>
#include <cmath>

namespace {
    class AudioPlayerStub : public masker_player::AudioPlayer {
        std::vector<std::string> audioDeviceDescriptions_{10};
        std::string filePath_{};
        std::string deviceDescription_{};
        double sampleRateHz_{};
        int deviceIndex_{};
        int deviceCount_{};
        int deviceDescriptionDeviceIndex_{};
        EventListener *listener_{};
        bool playing_{};
        bool played_{};
        bool stopped_{};
    public:
        void setAudioDeviceDescriptions(std::vector<std::string> v) {
            audioDeviceDescriptions_ = std::move(v);
        }
        
        void stop() override {
            stopped_ = true;
        }
        
        auto stopped() const {
            return stopped_;
        }
        
        double sampleRateHz() override {
            return sampleRateHz_;
        }
        
        void setSampleRateHz(double x) {
            sampleRateHz_ = x;
        }
        
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
            return gsl::narrow<int>(audioDeviceDescriptions_.size());
        }
        
        std::string deviceDescription(int index) override {
            deviceDescriptionDeviceIndex_ = index;
            return audioDeviceDescriptions_.at(index);
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
        
        auto played() const {
            return played_;
        }
        
        void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) {
            listener_->fillAudioBuffer(audio);
        }
    };
    
    class MaskerPlayerListenerStub :
        public recognition_test::MaskerPlayer::EventListener
    {
        int notifications_{};
        bool notified_{};
    public:
        void fadeInComplete() override {
            notified_ = true;
            ++notifications_;
        }
        
        auto notified() const {
            return notified_;
        }
        
        auto notifications() const {
            return notifications_;
        }
    };

    class RandomizedMaskerPlayerTests : public ::testing::Test {
    protected:
        std::vector<float> leftChannel{};
        AudioPlayerStub audioPlayer;
        MaskerPlayerListenerStub listener;
        masker_player::RandomizedMaskerPlayer player{&audioPlayer};
        
        RandomizedMaskerPlayerTests() {
            player.subscribe(&listener);
        }
        
        void fillAudioBuffer() {
            audioPlayer.fillAudioBuffer({ leftChannel });
        }
        
        std::vector<float> halfHannWindow(int N) {
            const auto pi = std::acos(-1);
            std::vector<float> window;
            for (int n = 0; n < (N + 1) / 2; ++n)
                window.push_back((1 - std::cos((2*pi*n)/(N - 1))) / 2);
            return window;
        }
        
        std::vector<float> backHalfHannWindow(int N) {
            auto frontHalf = halfHannWindow(N);
            std::reverse(frontHalf.begin(), frontHalf.end());
            return frontHalf;
        }
        
        std::vector<float> product(std::vector<float> x, std::vector<float> y) {
            std::vector<float> result;
            std::transform(
                x.begin(),
                x.end(),
                y.begin(),
                std::back_inserter(result),
                std::multiplies<>()
            );
            return result;
        }
        
        void setAudioDeviceDescriptions(std::vector<std::string> v) {
            audioPlayer.setAudioDeviceDescriptions(std::move(v));
        }
    };

    TEST_F(RandomizedMaskerPlayerTests, playingWhenVideoPlayerPlaying) {
        audioPlayer.setPlaying();
        EXPECT_TRUE(player.playing());
    }

    TEST_F(RandomizedMaskerPlayerTests, loadFileLoadsVideoFile) {
        player.loadFile("a");
        assertEqual("a", audioPlayer.filePath());
    }

    TEST_F(RandomizedMaskerPlayerTests, setDeviceSetsDevice) {
        player.setDevice(1);
        EXPECT_EQ(1, audioPlayer.deviceIndex());
    }

    TEST_F(RandomizedMaskerPlayerTests, fadeInPlaysVideoPlayer) {
        player.fadeIn();
        EXPECT_TRUE(audioPlayer.played());
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
        assertEqual({ 0.1, 0.2, 0.3 }, leftChannel);
    }

    TEST_F(RandomizedMaskerPlayerTests, fadesInAccordingToHannFunction) {
        player.setFadeInOutSeconds(0.5);
        player.fadeIn();
        auto N = 6/0.5 + 1;
        audioPlayer.setSampleRateHz(N - 1);
        leftChannel = { 0, 1, 2, 3, 4, 5, 6 };
        fillAudioBuffer();
        assertEqual(
            product(halfHannWindow(N), { 0, 1, 2, 3, 4, 5, 6 }),
            leftChannel,
            1e-6f
        );
    }

    TEST_F(RandomizedMaskerPlayerTests, steadyLevelFollowingFadeIn) {
        player.setFadeInOutSeconds(0.5);
        player.fadeIn();
        auto N = 6/0.5 + 1;
        audioPlayer.setSampleRateHz(N - 1);
        leftChannel = { 0, 1, 2, 3, 4, 5, 6, 7 };
        fillAudioBuffer();
        EXPECT_EQ(7, leftChannel.at(7));
    }

    TEST_F(RandomizedMaskerPlayerTests, fadeInTwice) {
        player.setFadeInOutSeconds(0.5);
        player.fadeIn();
        auto N = 6/0.5 + 1;
        audioPlayer.setSampleRateHz(N - 1);
        leftChannel.resize(7);
        fillAudioBuffer();
        player.fadeIn();
        leftChannel = { 0, 1, 2, 3, 4, 5, 6 };
        fillAudioBuffer();
        assertEqual(
            product(halfHannWindow(N), { 0, 1, 2, 3, 4, 5, 6 }),
            leftChannel,
            1e-6f
        );
    }

    TEST_F(RandomizedMaskerPlayerTests, fadesOutAccordingToHannFunction) {
        player.setFadeInOutSeconds(0.5);
        auto N = 6/0.5 + 1;
        audioPlayer.setSampleRateHz(N - 1);
        player.fadeOut();
        leftChannel = { 0, 1, 2, 3, 4, 5, 6 };
        fillAudioBuffer();
        assertEqual(
            product(backHalfHannWindow(N), { 0, 1, 2, 3, 4, 5, 6 }),
            leftChannel,
            1e-6f
        );
    }

    TEST_F(RandomizedMaskerPlayerTests, steadyLevelFollowingFadeOut) {
        player.setFadeInOutSeconds(0.5);
        auto N = 6/0.5 + 1;
        audioPlayer.setSampleRateHz(N - 1);
        player.fadeOut();
        leftChannel = { 0, 1, 2, 3, 4, 5, 6, 7 };
        fillAudioBuffer();
        EXPECT_NEAR(0.f, leftChannel.at(7), 1e-15);
    }

    TEST_F(RandomizedMaskerPlayerTests, fadeInCompleteOnlyAfterFadeTime) {
        player.setFadeInOutSeconds(0.5);
        player.fadeIn();
        auto N = 6/0.5 + 1;
        audioPlayer.setSampleRateHz(N - 1);
        leftChannel = { 0, 1, 2 };
        fillAudioBuffer();
        EXPECT_FALSE(listener.notified());
        leftChannel = { 3, 4, 5 };
        fillAudioBuffer();
        EXPECT_FALSE(listener.notified());
        leftChannel = { 6 };
        fillAudioBuffer();
        EXPECT_TRUE(listener.notified());
    }

    TEST_F(RandomizedMaskerPlayerTests, observerNotifiedOnce) {
        player.setFadeInOutSeconds(0.5);
        player.fadeIn();
        auto N = 6/0.5 + 1;
        audioPlayer.setSampleRateHz(N - 1);
        leftChannel = { 0, 1, 2, 3, 4, 5, 6 };
        fillAudioBuffer();
        fillAudioBuffer();
        EXPECT_EQ(1, listener.notifications());
    }

    TEST_F(RandomizedMaskerPlayerTests, audioPlayerStoppedOnlyAtEndOfFadeOutTime) {
        player.setFadeInOutSeconds(0.5);
        auto N = 6/0.5 + 1;
        audioPlayer.setSampleRateHz(N - 1);
        player.fadeOut();
        leftChannel = { 0, 1, 2 };
        fillAudioBuffer();
        EXPECT_FALSE(audioPlayer.stopped());
        leftChannel = { 3, 4, 5 };
        fillAudioBuffer();
        EXPECT_FALSE(audioPlayer.stopped());
        leftChannel = { 6 };
        fillAudioBuffer();
        EXPECT_TRUE(audioPlayer.stopped());
    }

    TEST_F(RandomizedMaskerPlayerTests, setAudioDeviceFindsIndex) {
        setAudioDeviceDescriptions({"zeroth", "first", "second", "third"});
        player.setAudioDevice("second");
        EXPECT_EQ(2, audioPlayer.deviceIndex());
    }

    TEST_F(RandomizedMaskerPlayerTests, audioDevicesReturnsDescriptions) {
        setAudioDeviceDescriptions({"a", "b", "c"});
        assertEqual({"a", "b", "c"}, player.audioDeviceDescriptions());
    }
}
