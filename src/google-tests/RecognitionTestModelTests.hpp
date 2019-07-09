#ifndef RecognitionTestModelTests_h
#define RecognitionTestModelTests_h

#include "TargetListStub.h"
#include "TargetListSetReaderStub.h"
#include "TrackStub.h"
#include "MaskerPlayerStub.h"
#include "TargetPlayerStub.h"
#include "OutputFileStub.h"
#include "ResponseEvaluatorStub.h"
#include "RandomizerStub.h"
#include "LogString.h"
#include "assert-utility.h"
#include <recognition-test/RecognitionTestModel.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <cmath>

namespace av_speech_in_noise::tests {
    class ModelEventListenerStub : public Model::EventListener {
        bool notified_{};
    public:
        void trialComplete() override {
            notified_ = true;
        }
        
        auto notified() const {
            return notified_;
        }
    };

    class FiniteTargetListStub : public FiniteTargetList {
        LogString log_;
        std::string directory_;
        std::string next_;
        std::string current_;
        std::string currentWhenNext_{};
        bool empty_{};
        bool nextCalled_{};
    public:
        std::string current() override {
            return current_;
        }
        
        void setCurrentTargetWhenNext(std::string s) {
            currentWhenNext_ = std::move(s);
        }
        
        void setCurrent(std::string s) {
            current_ = std::move(s);
        }
        
        void loadFromDirectory(std::string directory) override {
            log_.insert("loadFromDirectory ");
            directory_ = std::move(directory);
        }
        
        std::string next() override {
            log_.insert("next ");
            nextCalled_ = true;
            current_ = currentWhenNext_;
            return next_;
        }
        
        void setNext(std::string s) {
            next_ = std::move(s);
        }
        
        auto directory() const {
            return directory_;
        }
        
        void setEmpty() {
            empty_ = true;
        }
        
        bool empty() override {
            return empty_;
        }
        
        auto &log() const {
            return log_;
        }
    };

    class UseCase {
    public:
        virtual ~UseCase() = default;
        virtual void run(RecognitionTestModel &) = 0;
    };

    class SubmittingResponse : public virtual UseCase {
    };

    class SubmittingCoordinateResponse : public SubmittingResponse {
        coordinate_response_measure::SubjectResponse response_;
    public:
        void run(RecognitionTestModel &m) override {
            m.submitResponse(response_);
        }
        
        auto &response() const {
            return response_;
        }
    };

    class SubmittingCorrectResponse : public UseCase {
    public:
        void run(RecognitionTestModel &m) override {
            m.submitCorrectResponse();
        }
    };

    class SubmittingIncorrectResponse : public UseCase {
    public:
        void run(RecognitionTestModel &m) override {
            m.submitIncorrectResponse();
        }
    };

    class SubmittingFreeResponse : public SubmittingResponse {
        FreeResponse response_;
    public:
        void run(RecognitionTestModel &m) override {
            m.submitResponse(response_);
        }
        
        void setResponse(std::string s) {
            response_.response = std::move(s);
        }
    };

    class MaskerUseCase : public virtual UseCase {
    public:
        virtual void setMaskerFilePath(std::string) = 0;
    };

    class ConditionUseCase : public virtual UseCase {
    public:
        virtual void setAuditoryOnly() = 0;
        virtual void setAudioVisual() = 0;
    };
    
    class TargetLoaderUseCase : public virtual UseCase {
    public:
        virtual void setNextTarget(std::string) = 0;
    };

    class InitializingTestUseCase :
        public virtual MaskerUseCase,
        public virtual ConditionUseCase,
        public virtual TargetLoaderUseCase
    {
    public:
        virtual const TestInformation &testInformation() = 0;
        virtual const coordinate_response_measure::Trial &
            writtenCoordinateResponseTrial(OutputFileStub &) = 0;
        virtual void setTargetListDirectory(std::string) = 0;
        virtual std::string receivedTargetListDirectory() = 0;
        virtual void setSnr_dB(int x) = 0;
        virtual void setCurrentTarget(std::string) = 0;
        virtual void setComplete() = 0;
    };

    class InitializingAdaptiveTest : public InitializingTestUseCase {
        std::vector<std::shared_ptr<TargetListStub>> targetLists;
        std::vector<std::shared_ptr<TrackStub>> snrTracks;
        AdaptiveTest test_;
        TrackingRule targetLevelRule_;
        TargetListSetReaderStub *targetListSetReader;
        TrackFactoryStub *snrTrackFactory;
        RandomizerStub *randomizer;
    public:
        InitializingAdaptiveTest(
            TargetListSetReaderStub *targetListSetReader,
            TrackFactoryStub *snrTrackFactory,
            RandomizerStub *randomizer
        ) :
            targetListSetReader{targetListSetReader},
            snrTrackFactory{snrTrackFactory},
            randomizer{randomizer}
        {
            test_.targetLevelRule = &targetLevelRule_;
            setTargetListCount(3);
        }
        
        auto &common() {
            return test_.common;
        }
        
        auto targetList(int n) {
            return targetLists.at(n);
        }
        
        auto snrTrack(int n) {
            return snrTracks.at(n);
        }
        
        void setSnr_dB(int x) override {
            snrTrack(1)->setX(x);
            selectList(1);
        }
        
        void setNextTarget(std::string s) override {
            setTargetListNext(1, std::move(s));
            selectList(1);
        }
        
        void setCurrentTarget(std::string s) override {
            setTargetListCurrent(1, std::move(s));
            selectList(1);
        }
        
        void setTargetListNext(int n, std::string s) {
            targetList(n)->setNext(std::move(s));
        }
        
        void setSnrTrackComplete(int n) {
            snrTrack(n)->setComplete();
        }
        
        void setComplete() override {
            for (auto track : snrTracks)
                track->setComplete();
        }
        
        void setTargetListCount(int n) {
            targetLists.clear();
            snrTracks.clear();
            for (int i = 0; i < n; ++i) {
                targetLists.push_back(std::make_shared<TargetListStub>());
                snrTracks.push_back(std::make_shared<TrackStub>());
            }
            targetListSetReader->setTargetLists({targetLists.begin(), targetLists.end()});
            snrTrackFactory->setTracks({snrTracks.begin(), snrTracks.end()});
        }
        
        bool snrTrackPushedDown(int n) {
            return snrTrack(n)->pushedDown();
        }
        
        bool snrTrackPushedUp(int n) {
            return snrTrack(n)->pushedUp();
        }
        
        void setTargetListCurrent(int n, std::string s) {
            targetList(n)->setCurrent(std::move(s));;
        }
        
        void selectList(int n) {
            randomizer->setRandomInt(n);
        }
        
        void setTargetListDirectory(std::string s) override {
            common().targetListDirectory = std::move(s);
        }
        
        void setMaskerFilePath(std::string s) override {
            common().maskerFilePath = std::move(s);
        }
        
        void run(RecognitionTestModel &m) override {
            m.initializeTest(test_);
        }
        
        void setAudioVisual() override {
            common().condition = Condition::audioVisual;
        }
        
        void setAuditoryOnly() override {
            common().condition = Condition::auditoryOnly;
        }
        
        void setMaskerLevel_dB_SPL(int x) {
            common().maskerLevel_dB_SPL = x;
        }
        
        void setStartingSnr_dB(int x) {
            test_.startingSnr_dB = x;
        }
        
        void setCeilingSnr_dB(int x) {
            test_.ceilingSnr_dB = x;
        }
        
        void setFloorSnr_dB(int x) {
            test_.floorSnr_dB = x;
        }
        
        void setFullScaleLevel_dB_SPL(int x) {
            common().fullScaleLevel_dB_SPL = x;
        }
        
        auto targetLevelRule() const {
            return &targetLevelRule_;
        }
        
        auto &test() const {
            return test_;
        }
        
        const TestInformation &testInformation() override {
            return test_.information;
        }
        
        const coordinate_response_measure::Trial &writtenCoordinateResponseTrial(
            OutputFileStub &file
        ) override {
            return file.writtenAdaptiveCoordinateResponseTrial2();
        }
        
        std::string receivedTargetListDirectory() override {
            return targetListSetReader->directory();
        }
    };

    class SnrUseCase {
    public:
        virtual ~SnrUseCase() = default;
        virtual void setSnr(InitializingAdaptiveTest &, int) = 0;
        virtual int value(const Track::Settings &) = 0;
    };

    class SettingStartingSnr : public SnrUseCase {
    public:
        void setSnr(InitializingAdaptiveTest &test, int x) override {
            test.setStartingSnr_dB(x);
        }
        
        int value(const Track::Settings & s) override {
            return s.startingX;
        }
    };

    class SettingCeilingSnr : public SnrUseCase {
    public:
        void setSnr(InitializingAdaptiveTest &test, int x) override {
            test.setCeilingSnr_dB(x);
        }
        
        int value(const Track::Settings & s) override {
            return s.ceiling;
        }
    };

    class SettingFloorSnr : public SnrUseCase {
    public:
        void setSnr(InitializingAdaptiveTest &test, int x) override {
            test.setFloorSnr_dB(x);
        }
        
        int value(const Track::Settings & s) override {
            return s.floor;
        }
    };

    class InitializingFixedLevelTest : public InitializingTestUseCase
    {
        FixedLevelTest test_;
        FiniteTargetListStub *finiteTargetList;
    public:
        InitializingFixedLevelTest(FiniteTargetListStub *finiteTargetList) :
            finiteTargetList{finiteTargetList} {}
        
        void run(RecognitionTestModel &m) override {
            m.initializeTest(test_);
        }
        
        void setComplete() override {
            finiteTargetList->setEmpty();
        }
        
        auto &common() {
            return test_.common;
        }
        
        void setSnr_dB(int x) override {
            test_.snr_dB = x;
        }
        
        void setNextTarget(std::string s) override {
            finiteTargetList->setNext(std::move(s));
        }
        
        void setCurrentTarget(std::string s) override {
            finiteTargetList->setCurrent(std::move(s));
        }
        
        void setCurrentTargetWhenNext(std::string s) {
            finiteTargetList->setCurrentTargetWhenNext(std::move(s));
        }
        
        void setMaskerLevel_dB_SPL(int x) {
            common().maskerLevel_dB_SPL = x;
        }
        
        void setFullScaleLevel_dB_SPL(int x) {
            common().fullScaleLevel_dB_SPL = x;
        }
        
        void setAudioVisual() override {
            common().condition = Condition::audioVisual;
        }
        
        void setAuditoryOnly() override {
            common().condition = Condition::auditoryOnly;
        }
        
        void setMaskerFilePath(std::string s) override {
            common().maskerFilePath = std::move(s);
        }
        
        void setTargetListDirectory(std::string s) override {
            common().targetListDirectory = std::move(s);
        }
        
        auto &test() const {
            return test_;
        }
        
        const TestInformation &testInformation() override {
            return test_.information;
        }
        
        const coordinate_response_measure::Trial &writtenCoordinateResponseTrial(
            OutputFileStub &file
        ) override {
            return file.writtenFixedLevelTrial2();
        }
        
        std::string receivedTargetListDirectory() override {
            return finiteTargetList->directory();
        }
    };

    class AudioDeviceUseCase : public virtual UseCase {
    public:
        virtual void setAudioDevice(std::string) = 0;
    };

    class PlayingTrial : public AudioDeviceUseCase {
        AudioSettings trial;
    public:
        void setAudioDevice(std::string s) override {
            trial.audioDevice = std::move(s);
        }
        
        void run(RecognitionTestModel &m) override {
            m.playTrial(trial);
        }
    };

    class PlayingCalibration :
        public AudioDeviceUseCase,
        public ConditionUseCase
    {
        Calibration calibration;
    public:
        void setAudioDevice(std::string s) override {
            calibration.audioSettings.audioDevice = std::move(s);
        }
        
        void run(RecognitionTestModel &m) override {
            m.playCalibration(calibration);
        }
        
        void setFilePath(std::string s) {
            calibration.filePath = std::move(s);
        }
        
        void setLevel_dB_SPL(int x) {
            calibration.level_dB_SPL = x;
        }
        
        void setFullScaleLevel_dB_SPL(int x) {
            calibration.fullScaleLevel_dB_SPL = x;
        }
        
        void setAudioVisual() override {
            calibration.condition = Condition::audioVisual;
        }
        
        void setAuditoryOnly() override {
            calibration.condition = Condition::auditoryOnly;
        }
    };

    class RecognitionTestModelTests : public ::testing::Test {
    protected:
        Calibration calibration;
        coordinate_response_measure::SubjectResponse coordinateResponse;
        TargetListSetReaderStub targetListSetReader;
        FiniteTargetListStub finiteTargetList;
        TargetPlayerStub targetPlayer;
        MaskerPlayerStub maskerPlayer;
        OutputFileStub outputFile;
        TrackFactoryStub snrTrackFactory;
        ResponseEvaluatorStub evaluator;
        RandomizerStub randomizer;
        AdaptiveMethod adaptiveMethod{
            &targetListSetReader,
            &snrTrackFactory,
            &evaluator,
            &randomizer
        };
        FixedLevelMethod fixedLevelMethod{&finiteTargetList, &evaluator};
        RecognitionTestModel model{
            &adaptiveMethod,
            &fixedLevelMethod,
            &targetPlayer,
            &maskerPlayer,
            &evaluator,
            &outputFile,
            &randomizer
        };
        ModelEventListenerStub listener;
        InitializingAdaptiveTest initializingAdaptiveTest{
            &targetListSetReader,
            &snrTrackFactory,
            &randomizer
        };
        InitializingFixedLevelTest initializingFixedLevelTest{&finiteTargetList};
        PlayingTrial playingTrial;
        PlayingCalibration playingCalibration;
        SubmittingCoordinateResponse submittingCoordinateResponse;
        SubmittingCorrectResponse submittingCorrectResponse;
        SubmittingIncorrectResponse submittingIncorrectResponse;
        SubmittingFreeResponse submittingFreeResponse;
        SettingStartingSnr settingStartingSnr;
        SettingCeilingSnr settingCeilingSnr;
        SettingFloorSnr settingFloorSnr;
        
        RecognitionTestModelTests() {
            model.subscribe(&listener);
        }
        
        void initializeAdaptiveTest() {
            run(initializingAdaptiveTest);
        }
        
        void run(UseCase &useCase) {
            useCase.run(model);
        }
        
        void playTrial() {
            run(playingTrial);
        }
        
        void playCalibration() {
            run(playingCalibration);
        }
        
        void submitCoordinateResponse() {
            model.submitResponse(coordinateResponse);
        }
        
        void setOutputAudioDeviceDescriptions(std::vector<std::string> v) {
            maskerPlayer.setOutputAudioDeviceDescriptions(std::move(v));
        }
        
        void assertMaskerPlayerNotPlayed() {
            assertFalse(maskerPlayerFadedIn());
        }
        
        bool maskerPlayerFadedIn() {
            return maskerPlayer.fadeInCalled();
        }
        
        void assertTargetPlayerPlayed() {
            assertTrue(targetPlayerPlayed());
        }
        
        bool targetPlayerPlayed() {
            return targetPlayer.played();
        }
        
        void assertTargetPlayerNotPlayed() {
            assertFalse(targetPlayerPlayed());
        }
        
        void setTrialInProgress() {
            maskerPlayer.setPlaying();
        }
        
        void assertTargetVideoOnlyHidden() {
            assertTrue(targetPlayerVideoHidden());
            assertFalse(targetPlayerVideoShown());
        }
        
        bool targetPlayerVideoHidden() {
            return targetPlayer.videoHidden();
        }
        
        bool targetPlayerVideoShown() {
            return targetPlayer.videoShown();
        }
        
        void assertTargetVideoOnlyShown() {
            assertTargetVideoNotHidden();
            assertTrue(targetPlayerVideoShown());
        }
        
        void assertTargetVideoNotHidden() {
            assertFalse(targetPlayerVideoHidden());
        }
        
        void assertThrowsRequestFailureWhenTrialInProgress(UseCase &useCase) {
            setTrialInProgress();
            assertCallThrowsRequestFailure(useCase, "Trial in progress.");
        }
        
        void assertCallThrowsRequestFailure(
            UseCase &useCase,
            std::string what
        ) {
            try {
                run(useCase);
                FAIL() <<
                    "Expected recognition_test::"
                    "RecognitionTestModel::"
                    "RequestFailure";
            } catch (const RecognitionTestModel::RequestFailure &e) {
                assertEqual(std::move(what), e.what());
            }
        }
        
        void runIgnoringFailure(UseCase &useCase) {
            try {
                run(useCase);
            } catch (const RecognitionTestModel::RequestFailure &) {
            }
        }
        
        template<typename T>
        void assertDevicePassedToPlayer(
            const T &player,
            AudioDeviceUseCase &useCase
        ) {
            useCase.setAudioDevice("a");
            run(useCase);
            assertEqual("a", player.device());
        }
        
        void assertDevicePassedToTargetPlayer(AudioDeviceUseCase &useCase) {
            assertDevicePassedToPlayer(targetPlayer, useCase);
        }
        
        void assertDevicePassedToMaskerPlayer(AudioDeviceUseCase &useCase) {
            assertDevicePassedToPlayer(maskerPlayer, useCase);
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
        
        void throwInvalidAudioDeviceWhenSet() {
            maskerPlayer.throwInvalidAudioDeviceWhenDeviceSet();
            targetPlayer.throwInvalidAudioDeviceWhenDeviceSet();
        }
        
        void assertTargetFileLoadedPriorToRmsQuery(UseCase &useCase) {
            run(useCase);
            assertEqual("loadFile rms ", targetPlayer.log());
        }
    
        double dB(double x) {
            return 20 * std::log10(x);
        }
        
        auto targetPlayerLevel_dB() {
            return targetPlayer.level_dB();
        }
        
        template<typename T>
        void setTargetPlayerRms(T &&x) {
            targetPlayer.setRms(std::forward<T>(x));
        }
        
        auto targetFilePath() {
            return targetPlayer.filePath();
        }
        
        auto blueColor() {
            return coordinate_response_measure::Color::blue;
        }
        
        void playTrialWhenTrialAlreadyInProgressIgnoringFailure() {
            runIgnoringFailureWithTrialInProgress(playingTrial);
        }
        
        void runIgnoringFailureWithTrialInProgress(UseCase &useCase) {
            setTrialInProgress();
            runIgnoringFailure(useCase);
        }
        
        void playCalibrationWhenTrialAlreadyInProgressIgnoringFailure() {
            runIgnoringFailureWithTrialInProgress(playingCalibration);
        }
        
        void assertMaskerFilePathNotPassedToPlayerWhenTrialInProgress(InitializingTestUseCase &useCase) {
            useCase.setMaskerFilePath("a");
            runIgnoringFailureWithTrialInProgress(useCase);
            assertEqual("", maskerPlayer.filePath());
        }
        
        void assertTargetVideoNotHiddenWhenAuditoryOnlyButTrialInProgress(InitializingTestUseCase &useCase) {
            useCase.setAuditoryOnly();
            runIgnoringFailureWithTrialInProgress(useCase);
            assertTargetVideoNotHidden();
        }
        
        void assertTargetFilePathEquals(std::string what) {
            assertEqual(std::move(what), targetFilePath());
        }
        
        auto &outputFileLog() {
            return outputFile.log();
        }
        
        auto writtenAdaptiveCoordinateResponseTrial() {
            return outputFile.writtenAdaptiveCoordinateResponseTrial();
        }
        
        auto writtenFreeResponseTrial() {
            return outputFile.writtenFreeResponseTrial();
        }
        
        void initializeFixedLevelTest() {
            run(initializingFixedLevelTest);
        }
        
        void selectList(int n) {
            randomizer.setRandomInt(n);
        }
        
        void assertTargetVideoHiddenWhenAuditoryOnly(ConditionUseCase &useCase) {
            useCase.setAuditoryOnly();
            run(useCase);
            assertTargetVideoOnlyHidden();
        }
        
        void assertTargetVideoShownWhenAudioVisual(ConditionUseCase &useCase) {
            useCase.setAudioVisual();
            run(useCase);
            assertTargetVideoOnlyShown();
        }
        
        auto targetLevelRule() {
            return initializingAdaptiveTest.targetLevelRule();
        }
        
        void assertSettingsContainTargetLevelRule(const Track::Settings &s) {
            assertEqual(targetLevelRule(), s.rule);
        }
        
        auto &adaptiveTestSettings() const {
            return initializingAdaptiveTest.test();
        }
        
        auto &fixedLevelTestSettings() const {
            return initializingFixedLevelTest.test();
        }
        
        void assertRandomizerPassedIntegerBounds(int a, int b) {
            assertEqual(a, randomizer.lowerIntBound());
            assertEqual(b, randomizer.upperIntBound());
        }
        
        void setMaskerLevel_dB_SPL(int x) {
            initializingAdaptiveTest.setMaskerLevel_dB_SPL(x);
            initializingFixedLevelTest.setMaskerLevel_dB_SPL(x);
        }
        
        void setSnr_dB(int x) {
            initializingFixedLevelTest.setSnr_dB(x);
        }
        
        void setTestingFullScaleLevel_dB_SPL(int x) {
            initializingAdaptiveTest.setFullScaleLevel_dB_SPL(x);
            initializingFixedLevelTest.setFullScaleLevel_dB_SPL(x);
        }
        
        auto snrTrackFactoryParameters() {
            return snrTrackFactory.parameters();
        }
        
        bool targetPlayerPlaybackCompletionSubscribed() {
            return targetPlayer.playbackCompletionSubscribedTo();
        }
        
        auto maskerPlayerSecondsSeeked() {
            return maskerPlayer.secondsSeeked();
        }
        
        void setCorrectResponse() {
            evaluator.setCorrect();
        }
        
        void setIncorrectResponse() {
            evaluator.setIncorrect();
        }
        
        void assertTestIncomplete() {
            assertFalse(testComplete());
        }
        
        bool testComplete() {
            return model.testComplete();
        }
        
        void assertTestComplete() {
            assertTrue(testComplete());
        }
        
        void assertPushesSnrTrackDown(UseCase &useCase) {
            initializingAdaptiveTest.selectList(1);
            run(initializingAdaptiveTest);
            run(useCase);
            assertTrue(initializingAdaptiveTest.snrTrackPushedDown(1));
            assertFalse(initializingAdaptiveTest.snrTrackPushedUp(1));
        }
        
        void assertSelectsRandomListInRangeAfterRemovingCompleteTracks(UseCase &useCase) {
            run(initializingAdaptiveTest);
            initializingAdaptiveTest.setSnrTrackComplete(2);
            run(useCase);
            assertRandomizerPassedIntegerBounds(0, 1);
        }
        
        void assertTargetPlayerPlaybackCompletionSubscribed(UseCase &useCase) {
            run(useCase);
            assertTrue(targetPlayerPlaybackCompletionSubscribed());
        }
        
        void assertMaskerPlayerSeekedToRandomTime(UseCase &useCase) {
            randomizer.setRandomFloat(1);
            run(useCase);
            assertEqual(1., maskerPlayerSecondsSeeked());
        }
        
        void assertSelectsListAmongThoseWithIncompleteTracks(UseCase &useCase) {
            run(initializingAdaptiveTest);
            initializingAdaptiveTest.setTargetListNext(2, "a");
            initializingAdaptiveTest.setSnrTrackComplete(1);
            
            initializingAdaptiveTest.selectList(1);
            run(useCase);
            assertTargetFilePathEquals("a");
        }
        
        void assertPushesSnrTrackUp(UseCase &useCase) {
            initializingAdaptiveTest.selectList(1);
            run(initializingAdaptiveTest);
            run(useCase);
            assertTrue(initializingAdaptiveTest.snrTrackPushedUp(1));
            assertFalse(initializingAdaptiveTest.snrTrackPushedDown(1));
        }
        
        void assertMaskerFilePathPassedToPlayer(InitializingTestUseCase &useCase) {
            useCase.setMaskerFilePath("a");
            run(useCase);
            assertEqual("a", maskerPlayer.filePath());
        }
        
        void assertNextTargetPassedToPlayer(TargetLoaderUseCase &useCase) {
            useCase.setNextTarget("a");
            run(useCase);
            assertTargetFilePathEquals("a");
        }
        
        void assertNextTargetPassedToPlayer(InitializingTestUseCase &initializeTest, UseCase &useCase) {
            run(initializeTest);
            initializeTest.setNextTarget("a");
            run(useCase);
            assertTargetFilePathEquals("a");
        }
        
        void assertSeeksToRandomMaskerPositionWithinTrialDuration(UseCase &useCase) {
            targetPlayer.setDurationSeconds(1);
            maskerPlayer.setFadeTimeSeconds(2);
            maskerPlayer.setDurationSeconds(3);
            run(useCase);
            assertEqual(0., randomizer.lowerFloatBound());
            assertEqual(3. - 2 - 1 - 2, randomizer.upperFloatBound());
        }
        
        void assertMaskerPlayerLevelSet(UseCase &useCase) {
            setMaskerLevel_dB_SPL(1);
            setTestingFullScaleLevel_dB_SPL(2);
            maskerPlayer.setRms(3);
            run(useCase);
            assertEqual(1 - 2 - dB(3), maskerPlayer.level_dB());
        }
        
        void assertOutputFilePassedTestInformation(InitializingTestUseCase &useCase) {
            run(useCase);
            assertEqual(outputFile.openNewFileParameters(), &useCase.testInformation());
        }
        
        void assertThrowsRequestFailureWhenOutputFileThrows(UseCase &useCase) {
            outputFile.throwOnOpen();
            assertCallThrowsRequestFailure(useCase, "Unable to open output file.");
        }
        
        void assertOutputFileLog(std::string s) {
            assertEqual(std::move(s), outputFileLog());
        }
        
        void assertSavesOutputFileAfterWritingTrial(UseCase &useCase) {
            run(useCase);
            assertTrue(outputFileLog().endsWith("save "));
        }
        
        void assertSnrPassedToTrackFactory(SnrUseCase &useCase) {
            useCase.setSnr(initializingAdaptiveTest, 1);
            run(initializingAdaptiveTest);
            for (int i = 0; i < 3; ++i)
                assertEqual(1, useCase.value(snrTrackFactoryParameters().at(i)));
        }
        
        void assertClosesOutputFileOpensAndWritesTestInOrder(UseCase &useCase) {
            run(useCase);
            assertOutputFileLog("close openNewFile writeTest ");
        }
        
        void assertSetsTargetLevel(InitializingTestUseCase &useCase) {
            useCase.setSnr_dB(2);
            setMaskerLevel_dB_SPL(3);
            setTestingFullScaleLevel_dB_SPL(4);
            setTargetPlayerRms(5);
            run(useCase);
            assertEqual(2 + 3 - 4 - dB(5), targetPlayerLevel_dB());
        }
        
        void assertSetsTargetLevel(
            InitializingTestUseCase &initializingTest,
            UseCase &useCase
        ) {
            setMaskerLevel_dB_SPL(3);
            setTestingFullScaleLevel_dB_SPL(4);
            run(initializingTest);
            setTargetPlayerRms(5);
            initializingTest.setSnr_dB(2);
            run(useCase);
            assertEqual(2 + 3 - 4 - dB(5), targetPlayerLevel_dB());
        }
        
        auto writtenCoordinateResponseTrial(InitializingTestUseCase &useCase) {
            return useCase.writtenCoordinateResponseTrial(outputFile);
        }
        
        void assertWritesSubjectColor(InitializingTestUseCase &useCase) {
            run(useCase);
            coordinateResponse.color = blueColor();
            submitCoordinateResponse();
            assertEqual(blueColor(), writtenCoordinateResponseTrial(useCase).subjectColor);
        }
        
        void assertWritesSubjectNumber(InitializingTestUseCase &useCase) {
            run(useCase);
            coordinateResponse.number = 1;
            submitCoordinateResponse();
            assertEqual(1, writtenCoordinateResponseTrial(useCase).subjectNumber);
        }
        
        void assertWritesCorrectColor(InitializingTestUseCase &useCase) {
            run(useCase);
            evaluator.setCorrectColor(blueColor());
            submitCoordinateResponse();
            assertEqual(blueColor(), writtenCoordinateResponseTrial(useCase).correctColor);
        }
        
        void assertWritesCorrectNumber(InitializingTestUseCase &useCase) {
            run(useCase);
            evaluator.setCorrectNumber(1);
            submitCoordinateResponse();
            assertEqual(1, writtenCoordinateResponseTrial(useCase).correctNumber);
        }
        
        void assertWritesCorrectEvaluation(InitializingTestUseCase &useCase) {
            run(useCase);
            setCorrectResponse();
            submitCoordinateResponse();
            assertTrue(writtenCoordinateResponseTrial(useCase).correct);
        }
        
        void assertWritesIncorrectEvaluation(InitializingTestUseCase &useCase) {
            run(useCase);
            setIncorrectResponse();
            submitCoordinateResponse();
            assertFalse(writtenCoordinateResponseTrial(useCase).correct);
        }
        
        void assertCoordinateResponsePassedToEvaluator(UseCase &useCase) {
            run(useCase);
            run(submittingCoordinateResponse);
            assertEqual(&submittingCoordinateResponse.response(), evaluator.response());
        }
        
        void assertCoordinateResponsePassesCurrentTargetToEvaluator(
            InitializingTestUseCase &initializingTest
        ) {
            initializingTest.setCurrentTarget("a");
            run(initializingTest);
            submitCoordinateResponse();
            assertEqual("a", evaluator.correctColorFilePath());
            assertEqual("a", evaluator.correctNumberFilePath());
        }
        
        void assertCoordinateResponsePassesCurrentTargetToEvaluatorForDeterminingResponseCorrectness(
            InitializingTestUseCase &initializingTest
        ) {
            initializingTest.setCurrentTarget("a");
            run(initializingTest);
            submitCoordinateResponse();
            assertEqual("a", evaluator.correctFilePath());
        }
        
        void assertTargetListPassed(InitializingTestUseCase &useCase) {
            useCase.setTargetListDirectory("a");
            run(useCase);
            assertEqual("a", useCase.receivedTargetListDirectory());
        }
        
        void assertCurrentTargetPassedToEvaluator(
            InitializingTestUseCase &initializingTest,
            UseCase &useCase
        ) {
            initializingTest.setCurrentTarget("a");
            run(initializingTest);
            run(useCase);
            assertEqual("a", evaluator.filePathForFileName());
        }
        void assertCoordinateResponseDoesNotLoadNextTargetWhenTestComplete(
            InitializingTestUseCase &initializingTest
        ) {
            initializingTest.setNextTarget("a");
            run(initializingTest);
            initializingTest.setComplete();
            initializingTest.setNextTarget("b");
            submitCoordinateResponse();
            assertTargetFilePathEquals("a");
        }
    };
}
#endif
