#include "LogString.h"
#include "assert-utility.h"
#include <recognition-test/RecognitionTestModel.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <cmath>

namespace {
    class MaskerPlayerStub : public recognition_test::MaskerPlayer {
        std::vector<std::string> outputAudioDeviceDescriptions_{};
        LogString log_{};
        std::string filePath_{};
        std::string device_{};
        double rms_{};
        double level_dB_{};
        int deviceIndex_{};
        EventListener *listener_{};
        bool fadeInCalled_{};
        bool fadeOutCalled_{};
        bool playing_{};
        bool setDeviceCalled_{};
        bool throwInvalidAudioDeviceWhenDeviceSet_{};
    public:
        auto &log() const {
            return log_;
        }
        
        void fadeOutComplete() {
            listener_->fadeOutComplete();
        }
        
        void throwInvalidAudioDeviceWhenDeviceSet() {
            throwInvalidAudioDeviceWhenDeviceSet_ = true;
        }
        
        void setAudioDevice(std::string s) override {
            device_ = std::move(s);
            if (throwInvalidAudioDeviceWhenDeviceSet_)
                throw recognition_test::InvalidAudioDevice{};
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
            log_.insert("loadFile ");
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
        
        void setRms(double x) {
            rms_ = x;
        }
        
        auto level_dB() const {
            return level_dB_;
        }
        
        double rms() override {
            log_.insert("rms ");
            return rms_;
        }
        
        void setLevel_dB(double x) override {
            level_dB_ = x;
        }
    };

    class TargetPlayerStub : public recognition_test::TargetPlayer {
        LogString log_{};
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
        bool setDeviceCalled_{};
        bool playing_{};
    public:
        bool playing() override {
            return playing_;
        }
        
        auto &log() const {
            return log_;
        }
        
        void setPlaying() {
            playing_ = true;
        }
        
        auto setDeviceCalled() const {
            return setDeviceCalled_;
        }
        
        void throwInvalidAudioDeviceWhenDeviceSet() {
            throwInvalidAudioDeviceWhenDeviceSet_ = true;
        }
        
        void setAudioDevice(std::string s) override {
            setDeviceCalled_ = true;
            device_ = std::move(s);
            if (throwInvalidAudioDeviceWhenDeviceSet_)
                throw recognition_test::InvalidAudioDevice{};
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
            log_.insert("loadFile ");
            filePath_ = filePath;
        }
        
        double rms() override {
            log_.insert("rms ");
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

    class TargetListStub : public recognition_test::TargetList {
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
        av_coordinated_response_measure::Model::Test testWritten_{};
        av_coordinated_response_measure::Model::Test newFileParameters_{};
        LogString log_{};
        bool throwOnOpen_{};
        bool headingWritten_{};
    public:
        auto &log() const {
            return log_;
        }
        
        auto headingWritten() const {
            return headingWritten_;
        }
        
        auto &testWritten() const {
            return testWritten_;
        }
        
        auto &trialWritten() const {
            return trialWritten_;
        }
        
        void writeTrial(
            const av_coordinated_response_measure::Trial &trial
        ) override {
            log_.insert("writeTrial ");
            trialWritten_ = trial;
        }
        
        void openNewFile(
            const av_coordinated_response_measure::Model::Test &p
        ) override {
            log_.insert("openNewFile ");
            newFileParameters_ = p;
            if (throwOnOpen_)
                throw OpenFailure{};
        }
        
        void writeTrialHeading() override {
            log_.insert("writeTrialHeading ");
            headingWritten_ = true;
        }
        
        void writeTest(
            const av_coordinated_response_measure::Model::Test &test
        ) override {
            log_.insert("writeTest ");
            testWritten_ = test;
        }
        
        void close() override {
            log_.insert("close ");
        }
        
        auto &newFileParameters() const {
            return newFileParameters_;
        }
        
        void throwOnOpen() {
            throwOnOpen_ = true;
        }
    };
    
    class ModelEventListenerStub :
        public av_coordinated_response_measure::Model::EventListener
    {
        bool notified_{};
    public:
        void trialComplete() override {
            notified_ = true;
        }
        
        auto notified() const {
            return notified_;
        }
    };
    
    class UseCase {
    public:
        virtual ~UseCase() = default;
        virtual void run(recognition_test::RecognitionTestModel &) = 0;
    };
    
    class AudioDeviceUseCase : public UseCase {
    public:
        virtual void setAudioDevice(std::string) = 0;
    };
    
    class PlayingTrial : public AudioDeviceUseCase {
        recognition_test::RecognitionTestModel::AudioSettings trial;
    public:
        void setAudioDevice(std::string s) override {
            trial.audioDevice = std::move(s);
        }
        
        void run(recognition_test::RecognitionTestModel &m) override {
            m.playTrial(trial);
        }
    };
    
    class PlayingCalibration : public AudioDeviceUseCase {
        recognition_test::RecognitionTestModel::Calibration calibration;
    public:
        void setAudioDevice(std::string s) override {
            calibration.audioDevice = std::move(s);
        }
        
        void run(recognition_test::RecognitionTestModel &m) override {
            m.playCalibration(calibration);
        }
    };

    class RecognitionTestModelTests : public ::testing::Test {
    protected:
        recognition_test::RecognitionTestModel::Test test;
        recognition_test::RecognitionTestModel::Calibration calibration;
        recognition_test::RecognitionTestModel::AudioSettings trial;
        recognition_test::RecognitionTestModel::SubjectResponse subjectResponse;
        TargetListStub targetList{};
        TargetPlayerStub targetPlayer{};
        MaskerPlayerStub maskerPlayer{};
        OutputFileStub outputFile{};
        recognition_test::RecognitionTestModel model{
            &targetList,
            &targetPlayer,
            &maskerPlayer,
            &outputFile
        };
        ModelEventListenerStub listener{};
        PlayingTrial playingTrial{};
        PlayingCalibration playingCalibration{};
        
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
            targetList.setEmpty();
        }
        
        void assertMaskerPlayerNotPlayed() {
            EXPECT_FALSE(maskerPlayerFadedIn());
        }
        
        void assertListNotAdvanced() {
            EXPECT_FALSE(targetList.nextCalled());
        }
        
        void setMaskerIsPlaying() {
            maskerPlayer.setPlaying();
        }
        
        void setTargetIsPlaying() {
            targetPlayer.setPlaying();
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
        
        void assertTargetVideoOnlyHidden() {
            EXPECT_TRUE(targetPlayer.videoHidden());
            EXPECT_FALSE(targetPlayer.videoShown());
        }
        
        void assertTargetVideoOnlyShown() {
            EXPECT_FALSE(targetPlayer.videoHidden());
            EXPECT_TRUE(targetPlayer.videoShown());
        }
        
        void assertInitializeTestThrowsRequestFailure(std::string what) {
            assertCallThrowsRequestFailure(
                &RecognitionTestModelTests::initializeTest,
                std::move(what)
            );
        }
        
        void assertCallThrowsRequestFailure(
            void(RecognitionTestModelTests::*f)(),
            std::string what
        ) {
            try {
                (this->*f)();
                FAIL() << "Expected recognition_test::RecognitionTestModel::RequestFailure";
            } catch (const recognition_test::RecognitionTestModel::RequestFailure &e) {
                assertEqual(std::move(what), e.what());
            }
        }
        
        void assertCallThrowsRequestFailure(
            UseCase &useCase,
            std::string what
        ) {
            try {
                useCase.run(model);
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
            targetPlayer.throwInvalidAudioDeviceWhenDeviceSet();
        }
        
        void assertDevicePassedToTargetPlayer(AudioDeviceUseCase &useCase) {
            useCase.setAudioDevice("a");
            useCase.run(model);
            assertEqual("a", targetPlayer.device());
        }
        
        void assertThrowsRequestFailureWhenInvalidAudioDevice(
            AudioDeviceUseCase &useCase
        ) {
            throwInvalidAudioDeviceWhenSet();
            useCase.setAudioDevice("a");
            assertCallThrowsRequestFailure(
                useCase,
                "'a' is not a valid audio device."
            );
        }
    };

    TEST_F(RecognitionTestModelTests, subscribesToPlayerEvents) {
        EXPECT_EQ(&model, targetPlayer.listener());
        EXPECT_EQ(&model, maskerPlayer.listener());
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialPassesAudioDeviceToTargetPlayer
    ) {
        assertDevicePassedToTargetPlayer(playingTrial);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationPassesAudioDeviceToTargetPlayer
    ) {
        assertDevicePassedToTargetPlayer(playingCalibration);
    }

    TEST_F(RecognitionTestModelTests, playTrialPassesAudioDeviceToMaskerPlayer) {
        trial.audioDevice = "a";
        playTrial();
        assertEqual("a", maskerPlayer.device());
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialWithInvalidAudioDeviceThrowsRequestFailure
    ) {
        assertThrowsRequestFailureWhenInvalidAudioDevice(playingTrial);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationWithInvalidAudioDeviceThrowsRequestFailure
    ) {
        assertThrowsRequestFailureWhenInvalidAudioDevice(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialWithInvalidMaskerAudioDeviceDoesNotAdvanceTarget
    ) {
        throwInvalidAudioDeviceWhenSet();
        playTrialIgnoringFailure();
        assertListNotAdvanced();
    }

    TEST_F(
        RecognitionTestModelTests,
        audioDevicesReturnsOutputAudioDeviceDescriptions
    ) {
        setOutputAudioDeviceDescriptions({"a", "b", "c"});
        assertEqual({"a", "b", "c"}, model.audioDevices());
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialDoesNotChangeAudioDeviceWhenMaskerPlaying
    ) {
        setMaskerIsPlaying();
        playTrial();
        EXPECT_FALSE(maskerPlayer.setDeviceCalled());
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationDoesNotChangeAudioDeviceWhenTargetPlaying
    ) {
        setTargetIsPlaying();
        playingCalibration.run(model);
        EXPECT_FALSE(targetPlayer.setDeviceCalled());
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

    TEST_F(
        RecognitionTestModelTests,
        playTrialQueriesTargetRmsAfterLoadingFile
    ) {
        playTrial();
        assertEqual(
            "loadFile rms ",
            targetPlayer.log()
        );
    }

    TEST_F(RecognitionTestModelTests, fadeInCompletePlaysTarget) {
        maskerPlayer.fadeInComplete();
        EXPECT_TRUE(targetPlayer.played());
    }

    TEST_F(RecognitionTestModelTests, targetPlaybackCompleteFadesOutMasker) {
        targetPlayer.playbackComplete();
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
        playTrialSetsTargetPlayerLevel
    ) {
        test.signalLevel_dB_SPL = 5;
        test.fullScaleLevel_dB_SPL = 11;
        initializeTest();
        targetPlayer.setRms(7);
        playTrial();
        EXPECT_EQ(20 * std::log10(1.0/7) + 5 - 11, targetPlayer.level_dB());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestSetsMaskerPlayerLevel
    ) {
        test.startingSnr_dB = 5;
        test.signalLevel_dB_SPL = 13;
        test.fullScaleLevel_dB_SPL = 11;
        maskerPlayer.setRms(7);
        initializeTest();
        EXPECT_EQ(20 * std::log10(1.0/7) + 13 - 5 - 11, maskerPlayer.level_dB());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestHidesTargetVideoWhenAuditoryOnly
    ) {
        setAuditoryOnly();
        initializeTest();
        assertTargetVideoOnlyHidden();
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestShowsTargetVideoWhenAudioVisual
    ) {
        setAudioVisual();
        initializeTest();
        assertTargetVideoOnlyShown();
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
        initializeTestWritesTrialHeading
    ) {
        initializeTest();
        EXPECT_TRUE(outputFile.headingWritten());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestWritesTest
    ) {
        test.testerId = "a";
        initializeTest();
        assertEqual("a", outputFile.testWritten().testerId);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestClosesOutputFileOpensWritesTestAndWritesTrialHeadingInOrder
    ) {
        initializeTest();
        assertEqual(
            "close openNewFile writeTest writeTrialHeading ",
            outputFile.log()
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestQueriesMaskerRmsAfterLoadingFile
    ) {
        initializeTest();
        assertEqual(
            "loadFile rms ",
            maskerPlayer.log()
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestPassesTargetListDirectoryToTargetList
    ) {
        test.targetListDirectory = "a";
        initializeTest();
        assertEqual("a", targetList.directory());
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
        playTrialPassesNextTargetToTargetPlayer
    ) {
        targetList.setNext("a");
        playTrial();
        assertEqual("a", targetPlayer.filePath());
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

