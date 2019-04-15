#include "AudioReaderStub.h"
#include "assert-utility.h"
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <gtest/gtest.h>
#include <cmath>

namespace {
    class AudioPlayerStub : public stimulus_players::AudioPlayer {
        std::vector<std::vector<float>> audioRead_{};
        std::vector<std::string> audioDeviceDescriptions_{10};
        std::string filePath_{};
        std::string deviceDescription_{};
        std::string audioFilePath_{};
        std::map<int, bool> outputDevices{};
        double sampleRateHz_{};
        int deviceIndex_{};
        int deviceCount_{};
        int deviceDescriptionDeviceIndex_{};
        EventListener *listener_{};
        bool playing_{};
        bool played_{};
        bool stopped_{};
        bool callbackScheduled_{};
    public:
        bool outputDevice(int index) override {
            return outputDevices[index];
        }
        
        void clearCallbackCount() {
            callbackScheduled_ = false;
        }
        
        auto callbackScheduled() const {
            return callbackScheduled_;
        }
        
        void timerCallback() {
            listener_->timerCallback();
        }
        
        void setAudioDeviceDescriptions(std::vector<std::string> v) {
            audioDeviceDescriptions_ = std::move(v);
        }
        
        void setAsOutputDevice(int index) {
            outputDevices[index] = true;
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
        
        void scheduleCallbackAfterSeconds(double) override {
            callbackScheduled_ = true;
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
        public av_coordinate_response_measure::MaskerPlayer::EventListener
    {
        int fadeInCompletions_{};
        int fadeOutCompletions_{};
        bool fadeInCompleted_{};
        bool fadeOutCompleted_{};
    public:
        void fadeInComplete() override {
            fadeInCompleted_ = true;
            ++fadeInCompletions_;
        }
        
        void fadeOutComplete() override {
            ++fadeOutCompletions_;
            fadeOutCompleted_ = true;
        }
        
        auto fadeInCompleted() const {
            return fadeInCompleted_;
        }
        
        auto fadeOutCompleted() const {
            return fadeOutCompleted_;
        }
        
        auto fadeInCompletions() const {
            return fadeInCompletions_;
        }
        
        auto fadeOutCompletions() const {
            return fadeOutCompletions_;
        }
    };

    class MaskerPlayerTests : public ::testing::Test {
    protected:
        std::vector<float> leftChannel{};
        std::vector<float> rightChannel{};
        AudioPlayerStub audioPlayer;
        MaskerPlayerListenerStub listener;
        AudioReaderStub audioReader;
        stimulus_players::MaskerPlayerImpl player{&audioPlayer, &audioReader};
        
        MaskerPlayerTests() {
            player.subscribe(&listener);
        }
        
        void fillAudioBufferMono() {
            audioPlayer.fillAudioBuffer({ leftChannel });
        }
        
        void fillAudioBufferStereo() {
            audioPlayer.fillAudioBuffer({ leftChannel, rightChannel });
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
        
        void setAudioDeviceDescriptions(std::vector<std::string> v) {
            audioPlayer.setAudioDeviceDescriptions(std::move(v));
        }
        
        void setAsOutputDevice(int i) {
            audioPlayer.setAsOutputDevice(i);
        }
        
        void fadeInToFullLevel() {
            completeOneFadeCycle(&MaskerPlayerTests::fadeIn);
        }
        
        void fadeOutToSilence() {
            completeOneFadeCycle(&MaskerPlayerTests::fadeOut);
        }
        
        void completeOneFadeCycle(void (MaskerPlayerTests::*fade)()) {
            player.setFadeInOutSeconds(2);
            audioPlayer.setSampleRateHz(3);
            (this->*fade)();
            resizeChannels(2 * 3 + 1);
            fillAudioBufferMono();
        }
        
        void fadeIn() {
            player.fadeIn();
        }
        
        void fadeOut() {
            player.fadeOut();
        }
        
        void resizeChannels(int n) {
            leftChannel.resize(n);
            rightChannel.resize(n);
        }
        
        void timerCallback() {
            audioPlayer.timerCallback();
        }
        
        void assertCallbackScheduled() {
            EXPECT_TRUE(audioPlayer.callbackScheduled());
        }
        
        void assertCallbackNotScheduled() {
            EXPECT_FALSE(audioPlayer.callbackScheduled());
        }
    };

    TEST_F(MaskerPlayerTests, playingWhenVideoPlayerPlaying) {
        audioPlayer.setPlaying();
        EXPECT_TRUE(player.playing());
    }

    TEST_F(MaskerPlayerTests, loadFileLoadsVideoFile) {
        player.loadFile("a");
        assertEqual("a", audioPlayer.filePath());
    }

    TEST_F(MaskerPlayerTests, fadeInPlaysVideoPlayer) {
        fadeIn();
        EXPECT_TRUE(audioPlayer.played());
    }

    TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunction) {
        player.setFadeInOutSeconds(5);
        audioPlayer.setSampleRateHz(6);
        auto window = halfHannWindow(2 * 5 * 6 + 1);
    
        fadeIn();
        leftChannel = { 7, 8, 9 };
        fillAudioBufferMono();
        EXPECT_NEAR(window.at(0) * 7, leftChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(1) * 8, leftChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(2) * 9, leftChannel.at(2), 1e-6);
        leftChannel = { 7, 8, 9 };
        fillAudioBufferMono();
        EXPECT_NEAR(window.at(3) * 7, leftChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(4) * 8, leftChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(5) * 9, leftChannel.at(2), 1e-6);
    }

    TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunctionStereo) {
        player.setFadeInOutSeconds(5);
        audioPlayer.setSampleRateHz(6);
        auto window = halfHannWindow(2 * 5 * 6 + 1);
    
        fadeIn();
        leftChannel = { 1, 2, 3 };
        rightChannel = { 7, 8, 9 };
        fillAudioBufferStereo();
        EXPECT_NEAR(window.at(0) * 1, leftChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(1) * 2, leftChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(2) * 3, leftChannel.at(2), 1e-6);
        EXPECT_NEAR(window.at(0) * 7, rightChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(1) * 8, rightChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(2) * 9, rightChannel.at(2), 1e-6);
        leftChannel = { 1, 2, 3 };
        rightChannel = { 7, 8, 9 };
        fillAudioBufferStereo();
        EXPECT_NEAR(window.at(3) * 1, leftChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(4) * 2, leftChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(5) * 3, leftChannel.at(2), 1e-6);
        EXPECT_NEAR(window.at(3) * 7, rightChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(4) * 8, rightChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(5) * 9, rightChannel.at(2), 1e-6);
    }

    TEST_F(MaskerPlayerTests, steadyLevelFollowingFadeIn) {
        fadeInToFullLevel();
        
        leftChannel = { 1, 2, 3 };
        fillAudioBufferMono();
        assertEqual({ 1, 2, 3 }, leftChannel);
    }

    TEST_F(MaskerPlayerTests, fadesOutAccordingToHannFunction) {
        fadeInToFullLevel();
        player.setFadeInOutSeconds(5);
        audioPlayer.setSampleRateHz(6);
        auto window = backHalfHannWindow(2 * 5 * 6 + 1);
        
        fadeOut();
        leftChannel = { 7, 8, 9 };
        fillAudioBufferMono();
        EXPECT_NEAR(window.at(0) * 7, leftChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(1) * 8, leftChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(2) * 9, leftChannel.at(2), 1e-6);
        leftChannel = { 7, 8, 9 };
        fillAudioBufferMono();
        EXPECT_NEAR(window.at(3) * 7, leftChannel.at(0), 1e-6);
        EXPECT_NEAR(window.at(4) * 8, leftChannel.at(1), 1e-6);
        EXPECT_NEAR(window.at(5) * 9, leftChannel.at(2), 1e-6);
    }

    TEST_F(MaskerPlayerTests, steadyLevelFollowingFadeOut) {
        fadeInToFullLevel();
        fadeOutToSilence();
        
        leftChannel = { 1, 2, 3 };
        fillAudioBufferMono();
        assertEqual({ 0, 0, 0 }, leftChannel, 1e-15f);
    }

    TEST_F(MaskerPlayerTests, fadeInCompleteOnlyAfterFadeTime) {
        player.setFadeInOutSeconds(3);
        audioPlayer.setSampleRateHz(4);
        
        fadeIn();
        resizeChannels(1);
        for (int i = 0; i < 3 * 4; ++i) {
            fillAudioBufferMono();
            timerCallback();
            EXPECT_FALSE(listener.fadeInCompleted());
        }
        fillAudioBufferMono();
        timerCallback();
        EXPECT_TRUE(listener.fadeInCompleted());
    }

    TEST_F(MaskerPlayerTests, observerNotifiedOnceForFadeIn) {
        fadeInToFullLevel();
        timerCallback();
        EXPECT_EQ(1, listener.fadeInCompletions());
        fillAudioBufferMono();
        timerCallback();
        EXPECT_EQ(1, listener.fadeInCompletions());
    }

    TEST_F(MaskerPlayerTests, fadeOutCompleteOnlyAfterFadeTime) {
        fadeInToFullLevel();
        player.setFadeInOutSeconds(3);
        audioPlayer.setSampleRateHz(4);
        
        fadeOut();
        resizeChannels(1);
        for (int i = 0; i < 3 * 4; ++i) {
            fillAudioBufferMono();
            timerCallback();
            EXPECT_FALSE(listener.fadeOutCompleted());
        }
        fillAudioBufferMono();
        timerCallback();
        EXPECT_TRUE(listener.fadeOutCompleted());
    }

    TEST_F(MaskerPlayerTests, observerNotifiedOnceForFadeOut) {
        fadeInToFullLevel();
        audioPlayer.timerCallback();
        
        fadeOutToSilence();
        timerCallback();
        EXPECT_EQ(1, listener.fadeOutCompletions());
        fillAudioBufferMono();
        timerCallback();
        EXPECT_EQ(1, listener.fadeOutCompletions());
    }

    TEST_F(MaskerPlayerTests, audioPlayerStoppedOnlyAtEndOfFadeOutTime) {
        fadeInToFullLevel();
        player.setFadeInOutSeconds(3);
        audioPlayer.setSampleRateHz(4);
        
        fadeOut();
        resizeChannels(1);
        for (int i = 0; i < 3 * 4; ++i) {
            fillAudioBufferMono();
            timerCallback();
            EXPECT_FALSE(audioPlayer.stopped());
        }
        fillAudioBufferMono();
        timerCallback();
        EXPECT_TRUE(audioPlayer.stopped());
    }

    TEST_F(MaskerPlayerTests, fadeInSchedulesCallback) {
        fadeIn();
        assertCallbackScheduled();
    }

    TEST_F(MaskerPlayerTests, fadeOutSchedulesCallback) {
        fadeOut();
        assertCallbackScheduled();
    }

    TEST_F(MaskerPlayerTests, callbackSchedulesAdditionalCallback) {
        timerCallback();
        assertCallbackScheduled();
    }

    TEST_F(MaskerPlayerTests, callbackDoesNotScheduleAdditionalCallbackWhenFadeInComplete) {
        fadeInToFullLevel();
        audioPlayer.clearCallbackCount();
        
        timerCallback();
        assertCallbackNotScheduled();
    }

    TEST_F(MaskerPlayerTests, callbackDoesNotScheduleAdditionalCallbackWhenFadeOutComplete) {
        fadeInToFullLevel();
        timerCallback();
        fadeOutToSilence();
        audioPlayer.clearCallbackCount();
        
        timerCallback();
        assertCallbackNotScheduled();
    }

    TEST_F(MaskerPlayerTests, setAudioDeviceFindsIndex) {
        setAudioDeviceDescriptions({"zeroth", "first", "second", "third"});
        player.setAudioDevice("second");
        EXPECT_EQ(2, audioPlayer.deviceIndex());
    }

    TEST_F(MaskerPlayerTests, setAudioDeviceThrowsInvalidAudioDeviceIfDoesntExist) {
        setAudioDeviceDescriptions({"zeroth", "first", "second"});
        try {
            player.setAudioDevice("third");
            FAIL() << "Expected recognition_test::InvalidAudioDevice";
        } catch(const av_coordinate_response_measure::InvalidAudioDevice &) {
        
        }
    }

    TEST_F(MaskerPlayerTests, outputAudioDevicesReturnsDescriptions) {
        setAudioDeviceDescriptions({"a", "b", "c"});
        setAsOutputDevice(0);
        setAsOutputDevice(2);
        assertEqual({"a", "c"}, player.outputAudioDeviceDescriptions());
    }

    TEST_F(MaskerPlayerTests, rmsComputesFirstChannel) {
        audioReader.set({
            { 1, 2, 3 },
            { 4, 5, 6 },
            { 7, 8, 9 }
        });
        EXPECT_EQ(std::sqrt((1*1 + 2*2 + 3*3)/3.f), player.rms());
    }

    TEST_F(MaskerPlayerTests, rmsPassesLoadedFileToVideoPlayer) {
        player.loadFile("a");
        player.rms();
        assertEqual("a", audioReader.filePath());
    }
}
