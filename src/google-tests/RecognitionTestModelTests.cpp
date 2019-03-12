#include "assert-utility.h"
#include <recognition-test/Model.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>

namespace {
    class MaskerPlayerStub : public recognition_test::MaskerPlayer {
        std::vector<std::string> audioDeviceDescriptions_{};
        std::string filePath_{};
        int deviceIndex_{};
        EventListener *listener_{};
        bool fadeInCalled_{};
        bool fadeOutCalled_{};
        bool playing_{};
    public:
        bool playing() override {
            return playing_;
        }
        
        void setPlaying() {
            playing_ = true;
        }
        
        void setAudioDeviceDescriptions(std::vector<std::string> v) {
            audioDeviceDescriptions_ = std::move(v);
        }
        
        auto deviceIndex() {
            return deviceIndex_;
        }
        
        auto fadeInCalled() const {
            return fadeInCalled_;
        }
        
        void fadeIn() override {
            fadeInCalled_ = true;
        }
        
        auto fadeOutCalled() const {
            return fadeOutCalled_;
        }
        
        void subscribe(EventListener *listener) override {
            listener_ = listener;
        }
        
        void fadeOut() override {
            fadeOutCalled_ = true;
        }
        
        void loadFile(std::string filePath) override {
            filePath_ = filePath;
        }
        
        int deviceCount() override {
            return gsl::narrow<int>(audioDeviceDescriptions_.size());
        }
        
        std::string deviceDescription(int index) override {
            return audioDeviceDescriptions_.at(index);
        }
        
        void setDevice(int index) override {
            deviceIndex_ = index;
        }
        
        auto listener() const {
            return listener_;
        }
        
        void fadeInComplete() {
            listener_->fadeInComplete();
        }
        
        auto filePath() const {
            return filePath_;
        }
    };

    class StimulusPlayerStub : public recognition_test::StimulusPlayer {
        std::string filePath_{};
        int deviceIndex_{};
        EventListener *listener_{};
        bool played_{};
    public:
        auto deviceIndex() {
            return deviceIndex_;
        }
        
        auto played() const {
            return played_;
        }
        
        void play() override {
            played_ = true;
        }
        
        void subscribe(EventListener *listener) override {
            listener_ = listener;
        }
        
        void loadFile(std::string filePath) override {
            filePath_ = filePath;
        }
        
        void setDevice(int index) override {
            deviceIndex_ = index;
        }
        
        void playbackComplete() {
            listener_->playbackComplete();
        }
        
        auto listener() const {
            return listener_;
        }
        
        auto filePath() const {
            return filePath_;
        }
    };

    class StimulusListStub : public recognition_test::StimulusList {
        std::string directory_{};
        std::string next_{};
        bool empty_{};
    public:
        auto directory() const {
            return directory_;
        }
        
        void initialize(std::string directory) override {
            directory_ = std::move(directory);
        }
        
        bool empty() override {
            return empty_;
        }
        
        void setEmpty() {
            empty_ = true;
        }
        
        std::string next() override {
            return next_;
        }
        
        void setNext(std::string s) {
            next_ = std::move(s);
        }
    };

    class RecognitionTestModelTests : public ::testing::Test {
    protected:
        recognition_test::Model::TestParameters testParameters;
        recognition_test::Model::TrialParameters trialParameters;
        StimulusPlayerStub stimulusPlayer{};
        MaskerPlayerStub maskerPlayer{};
        StimulusListStub list{};
        recognition_test::Model model{&maskerPlayer, &list, &stimulusPlayer};
        
        void initializeTest() {
            model.initializeTest(testParameters);
        }
        
        void playTrial() {
            model.playTrial(trialParameters);
        }
    };

    TEST_F(RecognitionTestModelTests, subscribesToPlayerEvents) {
        EXPECT_EQ(&model, stimulusPlayer.listener());
        EXPECT_EQ(&model, maskerPlayer.listener());
    }

    TEST_F(RecognitionTestModelTests, playTrialPassesAudioDeviceIndexToPlayers) {
        maskerPlayer.setAudioDeviceDescriptions({"zeroth", "first", "second", "third"});
        trialParameters.audioDevice = "second";
        playTrial();
        EXPECT_EQ(2, maskerPlayer.deviceIndex());
        EXPECT_EQ(2, stimulusPlayer.deviceIndex());
    }

    TEST_F(RecognitionTestModelTests, audioDevicesReturnsDescriptions) {
        maskerPlayer.setAudioDeviceDescriptions({"a", "b", "c"});
        assertEqual({"a", "b", "c"}, model.audioDevices());
    }

    TEST_F(RecognitionTestModelTests, playTrialDoesNotFadeInMaskerWhenMaskerPlaying) {
        maskerPlayer.setPlaying();
        playTrial();
        EXPECT_FALSE(maskerPlayer.fadeInCalled());
    }

    TEST_F(RecognitionTestModelTests, playTrialFadesInMasker) {
        playTrial();
        EXPECT_TRUE(maskerPlayer.fadeInCalled());
    }

    TEST_F(RecognitionTestModelTests, fadeInCompletePlaysStimulus) {
        maskerPlayer.fadeInComplete();
        EXPECT_TRUE(stimulusPlayer.played());
    }

    TEST_F(RecognitionTestModelTests, stimulusPlaybackCompleteFadesOutMasker) {
        stimulusPlayer.playbackComplete();
        EXPECT_TRUE(maskerPlayer.fadeOutCalled());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestPassesMaskerFilePathToMaskerPlayer
    ) {
        testParameters.maskerFilePath = "a";
        initializeTest();
        assertEqual("a", maskerPlayer.filePath());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestPassesStimulusListDirectoryToStimulusList
    ) {
        testParameters.stimulusListDirectory = "a";
        initializeTest();
        assertEqual("a", list.directory());
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialPassesNextStimulusToStimulusPlayer
    ) {
        list.setNext("a");
        playTrial();
        assertEqual("a", stimulusPlayer.filePath());
    }

    TEST_F(
        RecognitionTestModelTests,
        testCompleteWhenListEmpty
    ) {
        list.setEmpty();
        EXPECT_TRUE(model.testComplete());
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialDoesNotPlayIfListEmpty
    ) {
        list.setEmpty();
        playTrial();
        EXPECT_FALSE(maskerPlayer.fadeInCalled());
    }
}

