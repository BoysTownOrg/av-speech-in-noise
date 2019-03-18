#include "assert-utility.h"
#include <masker-player/RandomizedMaskerPlayer.hpp>
#include <gtest/gtest.h>

namespace {
    class AudioPlayerStub : public masker_player::AudioPlayer {
        std::string filePath_{};
        std::string deviceDescription_{};
        double sampleRateHz_{};
        int deviceIndex_{};
        int deviceCount_{};
        int deviceDescriptionDeviceIndex_{};
        EventListener *listener_{};
        bool playing_{};
        bool played_{};
    public:
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
    
    class MaskerPlayerObserverStub : public recognition_test::MaskerPlayer::EventListener {
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
        MaskerPlayerObserverStub observer;
        masker_player::RandomizedMaskerPlayer player{&audioPlayer};
        
        RandomizedMaskerPlayerTests() {
            player.subscribe(&observer);
        }
        
        void fillAudioBuffer() {
            audioPlayer.fillAudioBuffer({ leftChannel });
        }
        
        std::vector<float> halfHannWindow(int N) {
            const auto pi = std::acos(-1);
            std::vector<float> window;
            for (int n = 0; n < (N + 1) / 2; ++n)
                window.push_back((1 - std::cos((2*pi*n)/(N - 1)))/2);
            return window;
        }
        
        std::vector<float> backHalfHannWindow(int N) {
            auto frontHalf = halfHannWindow(N);
            std::reverse(frontHalf.begin(), frontHalf.end());
            return frontHalf;
        }
        
        std::vector<float> product(std::vector<float> x, std::vector<float> y) {
            std::vector<float> result;
            std::transform(x.begin(), x.end(), y.begin(), std::back_inserter(result), std::multiplies<>());
            return result;
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

    TEST_F(RandomizedMaskerPlayerTests, returnsVideoPlayerDeviceCount) {
        audioPlayer.setDeviceCount(1);
        EXPECT_EQ(1, player.deviceCount());
    }

    TEST_F(RandomizedMaskerPlayerTests, returnsVideoPlayerDeviceDescription) {
        audioPlayer.setDeviceDescription("a");
        assertEqual("a", player.deviceDescription({}));
    }

    TEST_F(RandomizedMaskerPlayerTests, passesDeviceIndexToDeviceDescription) {
        player.deviceDescription(1);
        EXPECT_EQ(1, audioPlayer.deviceDescriptionDeviceIndex());
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
        assertEqual({ 1/10., 2/10., 3/10. }, leftChannel);
    }

    TEST_F(RandomizedMaskerPlayerTests, fadesInAccordingToHannFunction) {
        player.setFadeInOutSeconds(0.5);
        player.fadeIn();
        audioPlayer.setSampleRateHz(6 / 0.5);
        leftChannel = { 0, 1, 2, 3, 4, 5, 6 };
        fillAudioBuffer();
        assertEqual(product(halfHannWindow(6 / 0.5 + 1), { 0, 1, 2, 3, 4, 5, 6 }), leftChannel, 1e-6f);
    }

    TEST_F(RandomizedMaskerPlayerTests, fadesOutAccordingToHannFunction) {
        player.setFadeInOutSeconds(0.5);
        audioPlayer.setSampleRateHz(6 / 0.5);
        player.fadeOut();
        leftChannel = { 0, 1, 2, 3, 4, 5, 6 };
        fillAudioBuffer();
        assertEqual(product(backHalfHannWindow(6 / 0.5 + 1), { 0, 1, 2, 3, 4, 5, 6 }), leftChannel, 1e-6f);
    }

    TEST_F(RandomizedMaskerPlayerTests, fadeInCompleteOnlyAfterFadeTime) {
        player.setFadeInOutSeconds(0.5);
        player.fadeIn();
        audioPlayer.setSampleRateHz(6 / 0.5);
        leftChannel = { 0, 1, 2 };
        fillAudioBuffer();
        EXPECT_FALSE(observer.notified());
        leftChannel = { 3, 4, 5 };
        fillAudioBuffer();
        EXPECT_FALSE(observer.notified());
        leftChannel = { 6 };
        fillAudioBuffer();
        EXPECT_TRUE(observer.notified());
    }

    TEST_F(RandomizedMaskerPlayerTests, observerNotifiedOnce) {
        player.setFadeInOutSeconds(0.5);
        player.fadeIn();
        audioPlayer.setSampleRateHz(6 / 0.5);
        leftChannel = { 0, 1, 2, 3, 4, 5, 6 };
        fillAudioBuffer();
        fillAudioBuffer();
        EXPECT_EQ(1, observer.notifications());
    }
}
