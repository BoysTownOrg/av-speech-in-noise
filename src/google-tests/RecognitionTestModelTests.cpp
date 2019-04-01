#include "assert-utility.h"
#include <recognition-test/RecognitionTestModel.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <cmath>

namespace {
    class MaskerPlayerStub : public recognition_test::MaskerPlayer {
        std::vector<std::string> outputAudioDeviceDescriptions_{};
        std::string filePath_{};
        std::string device_{};
        int deviceIndex_{};
        EventListener *listener_{};
        bool fadeInCalled_{};
        bool fadeOutCalled_{};
        bool playing_{};
        bool setDeviceCalled_{};
        bool throwInvalidAudioDeviceWhenDeviceSet_{};
    public:
        void fadeOutComplete() {
            listener_->fadeOutComplete();
        }
        
        void throwInvalidAudioDeviceWhenDeviceSet() {
            throwInvalidAudioDeviceWhenDeviceSet_ = true;
        }
        
        void setAudioDevice(std::string s) override {
            if (throwInvalidAudioDeviceWhenDeviceSet_)
                throw recognition_test::InvalidAudioDevice{};
            device_ = std::move(s);
        }
        
        auto device() const {
            return device_;
        }
        
        auto setDeviceCalled() const {
            return setDeviceCalled_;
        }
        
        bool playing() override {
            return playing_;
        }
        
        void setPlaying() {
            playing_ = true;
        }
        
        void setOutputAudioDeviceDescriptions(std::vector<std::string> v) {
            outputAudioDeviceDescriptions_ = std::move(v);
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
        
        std::vector<std::string> outputAudioDeviceDescriptions() override {
            return outputAudioDeviceDescriptions_;
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

    class StimulusPlayerStub : public recognition_test::TargetPlayer {
        std::string filePath_{};
        std::string device_{};
        double rms_{};
        double level_dB_{};
        int deviceIndex_{};
        EventListener *listener_{};
        bool played_{};
        bool videoHidden_{};
        bool videoShown_{};
        bool throwInvalidAudioDeviceWhenDeviceSet_{};
    public:
        void throwInvalidAudioDeviceWhenDeviceSet() {
            throwInvalidAudioDeviceWhenDeviceSet_ = true;
        }
        
        void setAudioDevice(std::string s) override {
            if (throwInvalidAudioDeviceWhenDeviceSet_)
                throw recognition_test::InvalidAudioDevice{};
            device_ = std::move(s);
        }
        
        auto device() const {
            return device_;
        }
        
        auto level_dB() const {
            return level_dB_;
        }
        
        void setRms(double x) {
            rms_ = x;
        }
        
        void showVideo() override {
            videoShown_ = true;
        }
        
        auto videoShown() const {
            return videoShown_;
        }
        
        void hideVideo() override {
            videoHidden_ = true;
        }
        
        auto videoHidden() const {
            return videoHidden_;
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
        
        double rms() override {
            return rms_;
        }
        
        void setLevel_dB(double x) override {
            level_dB_ = x;
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

    class StimulusListStub : public recognition_test::TargetList {
        std::string directory_{};
        std::string next_{};
        bool empty_{};
        bool nextCalled_{};
    public:
        auto nextCalled() const {
            return nextCalled_;
        }
        
        auto directory() const {
            return directory_;
        }
        
        void loadFromDirectory(std::string directory) override {
            directory_ = std::move(directory);
        }
        
        bool empty() override {
            return empty_;
        }
        
        void setEmpty() {
            empty_ = true;
        }
        
        std::string next() override {
            nextCalled_ = true;
            return next_;
        }
        
        void setNext(std::string s) {
            next_ = std::move(s);
        }
    };
    
    class OutputFileStub : public recognition_test::OutputFile {
        av_coordinated_response_measure::Trial trialWritten_{};
        av_coordinated_response_measure::Model::Test newFileParameters_{};
        bool throwOnOpen_{};
    public:
        auto &trialWritten() const {
            return trialWritten_;
        }
        
        void writeTrial(const av_coordinated_response_measure::Trial &trial) override {
            trialWritten_ = trial;
        }
        
        void openNewFile(const av_coordinated_response_measure::Model::Test &p) override {
            if (throwOnOpen_)
                throw OpenFailure{};
            newFileParameters_ = p;
        }
        
        auto &newFileParameters() const {
            return newFileParameters_;
        }
        
        void throwOnOpen() {
            throwOnOpen_ = true;
        }
    };
    
    class EventListenerStub : public av_coordinated_response_measure::Model::EventListener {
        bool notified_{};
    public:
        void trialComplete() override {
            notified_ = true;
        }
        
        auto notified() const {
            return notified_;
        }
    };

    class RecognitionTestModelTests : public ::testing::Test {
    protected:
        recognition_test::RecognitionTestModel::Test test;
        recognition_test::RecognitionTestModel::AudioSettings trial;
        recognition_test::RecognitionTestModel::SubjectResponse subjectResponse;
        StimulusPlayerStub stimulusPlayer{};
        MaskerPlayerStub maskerPlayer{};
        StimulusListStub list{};
        OutputFileStub outputFile{};
        recognition_test::RecognitionTestModel model{
            &list,
            &stimulusPlayer,
            &maskerPlayer,
            &outputFile
        };
        EventListenerStub listener{};
        
        RecognitionTestModelTests() {
            model.subscribe(&listener);
            trial.audioDevice = "valid";
        }
        
        void initializeTest() {
            model.initializeTest(test);
        }
        
        void playTrial() {
            model.playTrial(trial);
        }
        
        void submitResponse() {
            model.submitResponse(subjectResponse);
        }
        
        void setOutputAudioDeviceDescriptions(std::vector<std::string> v) {
            maskerPlayer.setOutputAudioDeviceDescriptions(std::move(v));
        }
        
        bool maskerPlayerFadedIn() {
            return maskerPlayer.fadeInCalled();
        }
        
        void setListToEmpty() {
            list.setEmpty();
        }
        
        void assertMaskerPlayerNotPlayed() {
            EXPECT_FALSE(maskerPlayerFadedIn());
        }
        
        void assertListNotAdvanced() {
            EXPECT_FALSE(list.nextCalled());
        }
        
        void setMaskerIsPlaying() {
            maskerPlayer.setPlaying();
        }
        
        void assertMaskerPlayerNotPlayedAfterPlayingTrial() {
            playTrial();
            assertMaskerPlayerNotPlayed();
        }
        
        void assertListNotAdvancedAfterPlayingTrial() {
            playTrial();
            assertListNotAdvanced();
        }
        
        void setAuditoryOnly() {
            test.condition =
                av_coordinated_response_measure::Condition::auditoryOnly;
        }
        
        void setAudioVisual() {
            test.condition =
                av_coordinated_response_measure::Condition::audioVisual;
        }
        
        void assertStimulusVideoOnlyHidden() {
            EXPECT_TRUE(stimulusPlayer.videoHidden());
            EXPECT_FALSE(stimulusPlayer.videoShown());
        }
        
        void assertStimulusVideoOnlyShown() {
            EXPECT_FALSE(stimulusPlayer.videoHidden());
            EXPECT_TRUE(stimulusPlayer.videoShown());
        }
        
        void assertPlayTrialThrowsRequestFailure(std::string what) {
            assertCallThrowsRequestFailure(&RecognitionTestModelTests::playTrial, std::move(what));
        }
        
        void assertInitializeTestThrowsRequestFailure(std::string what) {
            assertCallThrowsRequestFailure(&RecognitionTestModelTests::initializeTest, std::move(what));
        }
        
        void assertCallThrowsRequestFailure(void(RecognitionTestModelTests::*f)(), std::string what) {
            try {
                (this->*f)();
                FAIL() << "Expected recognition_test::RecognitionTestModel::RequestFailure";
            } catch (const recognition_test::RecognitionTestModel::RequestFailure &e) {
                assertEqual(std::move(what), e.what());
            }
        }
        
        void playTrialIgnoringFailure() {
            try {
                playTrial();
            } catch (const recognition_test::RecognitionTestModel::RequestFailure &) {
            }
        }
        
        void throwInvalidAudioDeviceWhenSet() {
            maskerPlayer.throwInvalidAudioDeviceWhenDeviceSet();
            stimulusPlayer.throwInvalidAudioDeviceWhenDeviceSet();
        }
    };

    TEST_F(RecognitionTestModelTests, subscribesToPlayerEvents) {
        EXPECT_EQ(&model, stimulusPlayer.listener());
        EXPECT_EQ(&model, maskerPlayer.listener());
    }

    TEST_F(RecognitionTestModelTests, playTrialPassesAudioDeviceToPlayers) {
        trial.audioDevice = "a";
        playTrial();
        assertEqual("a", maskerPlayer.device());
        assertEqual("a", stimulusPlayer.device());
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialWithInvalidMaskerAudioDeviceThrowsRequestFailure
    ) {
        throwInvalidAudioDeviceWhenSet();
        trial.audioDevice = "d";
        assertPlayTrialThrowsRequestFailure("'d' is not a valid audio device.");
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialWithInvalidMaskerAudioDeviceDoesNotAdvanceStimulus
    ) {
        throwInvalidAudioDeviceWhenSet();
        playTrialIgnoringFailure();
        EXPECT_FALSE(list.nextCalled());
    }

    TEST_F(RecognitionTestModelTests, audioDevicesReturnsOutputAudioDeviceDescriptions) {
        setOutputAudioDeviceDescriptions({"a", "b", "c"});
        assertEqual({"a", "b", "c"}, model.audioDevices());
    }

    TEST_F(RecognitionTestModelTests, playTrialDoesChangeAudioDeviceWhenMaskerPlaying) {
        setMaskerIsPlaying();
        playTrial();
        EXPECT_FALSE(maskerPlayer.setDeviceCalled());
    }

    TEST_F(RecognitionTestModelTests, playTrialDoesNotPlayIfMaskerAlreadyPlaying) {
        setMaskerIsPlaying();
        assertMaskerPlayerNotPlayedAfterPlayingTrial();
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialDoesNotPlayIfListEmpty
    ) {
        setListToEmpty();
        assertMaskerPlayerNotPlayedAfterPlayingTrial();
    }

    TEST_F(RecognitionTestModelTests, playTrialFadesInMasker) {
        playTrial();
        EXPECT_TRUE(maskerPlayerFadedIn());
    }

    TEST_F(RecognitionTestModelTests, fadeInCompletePlaysStimulus) {
        maskerPlayer.fadeInComplete();
        EXPECT_TRUE(stimulusPlayer.played());
    }

    TEST_F(RecognitionTestModelTests, stimulusPlaybackCompleteFadesOutMasker) {
        stimulusPlayer.playbackComplete();
        EXPECT_TRUE(maskerPlayer.fadeOutCalled());
    }

    TEST_F(RecognitionTestModelTests, fadeOutCompleteNotifiesTrialComplete) {
        maskerPlayer.fadeOutComplete();
        EXPECT_TRUE(listener.notified());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestPassesMaskerFilePathToMaskerPlayer
    ) {
        test.maskerFilePath = "a";
        initializeTest();
        assertEqual("a", maskerPlayer.filePath());
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialSetsStimulusPlayerLevel
    ) {
        test.signalLevel_dB_SPL = 5;
        test.fullScaleLevel_dB_SPL = 11;
        initializeTest();
        stimulusPlayer.setRms(7);
        playTrial();
        EXPECT_EQ(20 * std::log10(1.0/7) + 5 - 11, stimulusPlayer.level_dB());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestHidesStimulusVideoWhenAuditoryOnly
    ) {
        setAuditoryOnly();
        initializeTest();
        assertStimulusVideoOnlyHidden();
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestShowsStimulusVideoWhenAudioVisual
    ) {
        setAudioVisual();
        initializeTest();
        assertStimulusVideoOnlyShown();
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestOpensNewOutputFile
    ) {
        test.testerId = "a";
        initializeTest();
        assertEqual("a", outputFile.newFileParameters().testerId);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestPassesStimulusListDirectoryToStimulusList
    ) {
        test.stimulusListDirectory = "a";
        initializeTest();
        assertEqual("a", list.directory());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestThrowsRequestFailureIfFileFailsToOpen
    ) {
        outputFile.throwOnOpen();
        assertInitializeTestThrowsRequestFailure("Unable to open output file.");
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
        playTrialDoesNotAdvanceListIfEmpty
    ) {
        setListToEmpty();
        assertListNotAdvancedAfterPlayingTrial();
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialDoesNotAdvanceListIfMaskerIsPlaying
    ) {
        setMaskerIsPlaying();
        assertListNotAdvancedAfterPlayingTrial();
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponseWritesTrial
    ) {
        subjectResponse.color = av_coordinated_response_measure::Color::green;
        subjectResponse.number = 1;
        submitResponse();
        EXPECT_EQ(1, outputFile.trialWritten().subjectNumber);
        EXPECT_EQ(
            av_coordinated_response_measure::Color::green,
            outputFile.trialWritten().subjectColor
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        testCompleteWhenListEmpty
    ) {
        setListToEmpty();
        EXPECT_TRUE(model.testComplete());
    }
}

