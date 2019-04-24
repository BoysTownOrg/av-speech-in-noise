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
        double durationSeconds_{};
        double secondsSeeked_{};
        int deviceIndex_{};
        int deviceCount_{};
        int deviceDescriptionDeviceIndex_{};
        EventListener *listener_{};
        bool playing_{};
        bool played_{};
        bool stopped_{};
        bool callbackScheduled_{};
    public:
        void seekSeconds(double x) override {
            secondsSeeked_ = x;
        }
        
        double durationSeconds() override {
            return durationSeconds_;
        }
        
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
        
        void setDurationSeconds(double x) {
            durationSeconds_ = x;
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
        
        auto secondsSeeked() const {
            return secondsSeeked_;
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
    
    template<typename T>
    class VectorFacade {
        std::vector<T> v;
    public:
        VectorFacade(std::vector<T> v) : v{std::move(v)} {}
        
        std::vector<T> elementWiseProduct(std::vector<T> y) {
            std::vector<T> product;
            std::transform(
                v.begin(),
                v.end(),
                y.begin(),
                std::back_inserter(product),
                std::multiplies<T>()
            );
            return product;
        }
        
        VectorFacade<T> subvector(int b, int e) {
            return std::vector<T>{v.begin() + b, v.begin() + e};
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
        
        std::vector<float> halfHannWindow(int length) {
            auto N = 2 * length - 1;
            const auto pi = std::acos(-1);
            std::vector<float> window;
            for (int n = 0; n < length; ++n)
                window.push_back((1 - std::cos((2*pi*n)/(N - 1))) / 2);
            return window;
        }
        
        std::vector<float> backHalfHannWindow(int length) {
            auto frontHalf = halfHannWindow(length);
            std::reverse(frontHalf.begin(), frontHalf.end());
            return frontHalf;
        }
        
        std::vector<float> oneToN(int N) {
            std::vector<float> result;
            result.resize(N);
            std::iota(result.begin(), result.end(), 1);
            return result;
        }
        
        std::vector<float> NtoOne(int N) {
            auto result = oneToN(N);
            std::reverse(result.begin(), result.end());
            return result;
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
        
        void assertFillingLeftChannelMultipliesBy_Buffered(
            VectorFacade<float> multiplicand,
            int buffers,
            int framesPerBuffer
        ) {
            for (int i = 0; i < buffers; ++i) {
                auto offset = i * framesPerBuffer;
                assertFillingLeftChannelMultipliesBy(
                    multiplicand.subvector(offset, offset + framesPerBuffer),
                    framesPerBuffer
                );
            }
        }
        
        void assertFillingStereoChannelsMultipliesBy_Buffered(
            VectorFacade<float> multiplicand,
            int buffers,
            int framesPerBuffer
        ) {
            for (int i = 0; i < buffers; ++i) {
                auto offset = i * framesPerBuffer;
                assertFillingStereoChannelsMultipliesBy(
                    multiplicand.subvector(offset, offset + framesPerBuffer),
                    framesPerBuffer
                );
            }
        }
        
        void assertFillingLeftChannelMultipliesBy(
            VectorFacade<float> multiplicand,
            int framesPerBuffer
        ) {
            leftChannel = oneToN(framesPerBuffer);
            fillAudioBufferMono();
            assertLeftChannelEquals(
                multiplicand.elementWiseProduct(oneToN(framesPerBuffer))
            );
        }
        
        void assertLeftChannelEquals(std::vector<float> x) {
            assertChannelEqual(leftChannel, x);
        }
        
        void assertChannelEqual(
            const std::vector<float> &channel,
            std::vector<float> x
        ) {
            assertEqual(x, channel, 1e-6f);
        }
        
        void assertFillingStereoChannelsMultipliesBy(
            VectorFacade<float> multiplicand,
            int framesPerBuffer
        ) {
            leftChannel = oneToN(framesPerBuffer);
            rightChannel = NtoOne(framesPerBuffer);
            fillAudioBufferStereo();
            assertLeftChannelEquals(
                multiplicand.elementWiseProduct(oneToN(framesPerBuffer))
            );
            assertRightChannelEquals(
                multiplicand.elementWiseProduct(NtoOne(framesPerBuffer))
            );
        }
        
        void assertRightChannelEquals(std::vector<float> x) {
            assertChannelEqual(rightChannel, x);
        }
        
        void assertFadeInNotCompletedAfterMonoFill() {
            callbackAfterMonoFill();
            EXPECT_FALSE(listener.fadeInCompleted());
        }
        
        void callbackAfterMonoFill() {
            fillAudioBufferMono();
            timerCallback();
        }
        
        void assertFadeInCompletedAfterMonoFill() {
            callbackAfterMonoFill();
            EXPECT_TRUE(listener.fadeInCompleted());
        }
        
        void assertFadeOutNotCompletedAfterMonoFill() {
            callbackAfterMonoFill();
            EXPECT_FALSE(listener.fadeOutCompleted());
        }
        
        void assertFadeOutCompletedAfterMonoFill() {
            callbackAfterMonoFill();
            EXPECT_TRUE(listener.fadeOutCompleted());
        }
    };

    TEST_F(MaskerPlayerTests, playingWhenVideoPlayerPlaying) {
        audioPlayer.setPlaying();
        EXPECT_TRUE(player.playing());
    }

    TEST_F(MaskerPlayerTests, durationReturnsDuration) {
        audioPlayer.setDurationSeconds(1);
        EXPECT_EQ(1, player.durationSeconds());
    }

    TEST_F(MaskerPlayerTests, seekSeeksAudioPlayer) {
        player.seekSeconds(1);
        EXPECT_EQ(1, audioPlayer.secondsSeeked());
    }

    TEST_F(MaskerPlayerTests, fadeTimeReturnsFadeTime) {
        player.setFadeInOutSeconds(1);
        EXPECT_EQ(1, player.fadeTimeSeconds());
    }

    TEST_F(MaskerPlayerTests, loadFileLoadsVideoFile) {
        player.loadFile("a");
        assertEqual("a", audioPlayer.filePath());
    }

    TEST_F(MaskerPlayerTests, fadeInPlaysVideoPlayer) {
        fadeIn();
        EXPECT_TRUE(audioPlayer.played());
    }

    TEST_F(MaskerPlayerTests, twentydBMultipliesSignalByTen) {
        player.setLevel_dB(20);
        leftChannel = { 1, 2, 3 };
        fillAudioBufferMono();
        assertEqual({ 10, 20, 30 }, leftChannel);
    }

    TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunctionMultipleFills) {
        // For this test:
        // halfWindowLength is determined by fade time and sample rate...
        // but must be divisible by framesPerBuffer.
        player.setFadeInOutSeconds(3);
        audioPlayer.setSampleRateHz(5);
        auto halfWindowLength = 3 * 5 + 1;
        auto framesPerBuffer = 4;
        
        fadeIn();
        assertFillingLeftChannelMultipliesBy_Buffered(
            halfHannWindow(halfWindowLength),
            halfWindowLength/framesPerBuffer,
            framesPerBuffer
        );
    }

    TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunctionOneFill) {
        player.setFadeInOutSeconds(2);
        audioPlayer.setSampleRateHz(3);
        auto halfWindowLength = 2 * 3 + 1;
    
        fadeIn();
        assertFillingLeftChannelMultipliesBy(
            halfHannWindow(halfWindowLength),
            halfWindowLength
        );
    }

    TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunctionStereoMultipleFills) {
        // For this test:
        // halfWindowLength is determined by fade time and sample rate...
        // but must be divisible by framesPerBuffer.
        player.setFadeInOutSeconds(3);
        audioPlayer.setSampleRateHz(5);
        auto halfWindowLength = 3 * 5 + 1;
        auto framesPerBuffer = 4;
        
        fadeIn();
        assertFillingStereoChannelsMultipliesBy_Buffered(
            halfHannWindow(halfWindowLength),
            halfWindowLength/framesPerBuffer,
            framesPerBuffer
        );
    }

    TEST_F(MaskerPlayerTests, fadesInAccordingToHannFunctionStereoOneFill) {
        player.setFadeInOutSeconds(2);
        audioPlayer.setSampleRateHz(3);
        auto halfWindowLength = 2 * 3 + 1;
    
        fadeIn();
        assertFillingStereoChannelsMultipliesBy(
            halfHannWindow(halfWindowLength),
            halfWindowLength
        );
    }

    TEST_F(MaskerPlayerTests, steadyLevelFollowingFadeIn) {
        fadeInToFullLevel();
        
        leftChannel = { 1, 2, 3 };
        fillAudioBufferMono();
        assertEqual({ 1, 2, 3 }, leftChannel);
    }

    TEST_F(MaskerPlayerTests, fadesOutAccordingToHannFunctionMultipleFills) {
        // For this test:
        // halfWindowLength is determined by fade time and sample rate...
        // but must be divisible by framesPerBuffer.
        fadeInToFullLevel();
        player.setFadeInOutSeconds(3);
        audioPlayer.setSampleRateHz(5);
        auto halfWindowLength = 3 * 5 + 1;
        auto framesPerBuffer = 4;
        
        fadeOut();
        assertFillingLeftChannelMultipliesBy_Buffered(
            backHalfHannWindow(halfWindowLength),
            halfWindowLength/framesPerBuffer,
            framesPerBuffer
        );
    }

    TEST_F(MaskerPlayerTests, fadesOutAccordingToHannFunctionOneFill) {
        fadeInToFullLevel();
        player.setFadeInOutSeconds(2);
        audioPlayer.setSampleRateHz(3);
        auto halfWindowLength = 2 * 3 + 1;
    
        fadeOut();
        assertFillingLeftChannelMultipliesBy(
            backHalfHannWindow(halfWindowLength),
            halfWindowLength
        );
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
        for (int i = 0; i < 3 * 4; ++i)
            assertFadeInNotCompletedAfterMonoFill();
        assertFadeInCompletedAfterMonoFill();
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
        for (int i = 0; i < 3 * 4; ++i)
            assertFadeOutNotCompletedAfterMonoFill();
        assertFadeOutCompletedAfterMonoFill();
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

    TEST_F(MaskerPlayerTests, fadeInTwiceDoesNotScheduleAdditionalCallback) {
        fadeIn();
        audioPlayer.clearCallbackCount();
        fadeIn();
        assertCallbackNotScheduled();
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

    TEST_F(MaskerPlayerTests, rmsThrowsInvalidAudioFileWhenAudioReaderThrows) {
        audioReader.throwOnRead();
        try {
            player.rms();
            FAIL() << "Expected av_coordinate_response_measure::InvalidAudioFile";
        } catch(const av_coordinate_response_measure::InvalidAudioFile &) {
        
        }
    }
}
