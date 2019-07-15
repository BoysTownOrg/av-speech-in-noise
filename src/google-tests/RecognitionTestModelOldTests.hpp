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
#include <recognition-test/FixedLevelMethod.hpp>
#include <recognition-test/AdaptiveMethod.hpp>
#include <recognition-test/RecognitionTestModel.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <cmath>

namespace av_speech_in_noise::tests::recognition_test {
    class UseCase {
    public:
        virtual ~UseCase() = default;
        virtual void run(RecognitionTestModel &) = 0;
        virtual void run(RecognitionTestModel_Internal &) {}
    };

    class SubmittingResponse : public virtual UseCase {
    };

    class SubmittingCoordinateResponse : public SubmittingResponse {
        coordinate_response_measure::SubjectResponse response_{};
    public:
        void run(RecognitionTestModel &m) override {
            m.submitResponse(response_);
        }
        
        void run(RecognitionTestModel_Internal &m) override {
            m.submitResponse(response_);
        }
        
        void setNumber(int n) {
            response_.number = n;
        }
        
        void setColor(coordinate_response_measure::Color c) {
            response_.color = c;
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
        void run(RecognitionTestModel_Internal &m) override {
            m.submitCorrectResponse();
        }
    };

    class SubmittingIncorrectResponse : public UseCase {
    public:
        void run(RecognitionTestModel &m) override {
            m.submitIncorrectResponse();
        }
        void run(RecognitionTestModel_Internal &m) override {
            m.submitIncorrectResponse();
        }
    };

    class SubmittingFreeResponse : public SubmittingResponse {
        FreeResponse response_;
    public:
        void run(RecognitionTestModel &m) override {
            m.submitResponse(response_);
        }
        void run(RecognitionTestModel_Internal &m) override {
            m.submitResponse(response_);
        }
        
        void setResponse(std::string s) {
            response_.response = std::move(s);
        }
    };

    class ConditionUseCase : public virtual UseCase {
    public:
        virtual void setAuditoryOnly() = 0;
        virtual void setAudioVisual() = 0;
    };

    class InitializingTestUseCase : public virtual ConditionUseCase {
    public:
        virtual const TestInformation &testInformation() = 0;
        virtual const coordinate_response_measure::Trial &
            writtenCoordinateResponseTrial(OutputFileStub &) = 0;
        virtual void setTargetListDirectory(std::string) = 0;
        virtual void setMaskerFilePath(std::string) = 0;
        virtual std::string receivedTargetListDirectory() = 0;
        virtual void setSnr_dB(int x) = 0;
        virtual void setCurrentTarget(std::string) = 0;
        virtual void setNextTarget(std::string) = 0;
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
            selectList(0);
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
            snrTrack(2)->setX(x);
            selectList(2);
        }
        
        void setNextTarget(std::string s) override {
            setTargetListNext(1, std::move(s));
            selectList(1);
        }
        
        void setCurrentTarget(std::string s) override {
            setTargetListCurrent(0, std::move(s));
        }
        
        void setTargetListNext(int n, std::string s) {
            targetList(n)->setNext(std::move(s));
        }
        
        void setSnrTrackComplete(int n) {
            snrTrack(n)->setComplete();
        }
        
        void setComplete() {
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

    class InitializingFixedLevelTest : public InitializingTestUseCase
    {
        FixedLevelTest test_;
        TargetListStub *targetList;
    public:
        explicit InitializingFixedLevelTest(
            TargetListStub *targetList
        ) :
            targetList{targetList}
        {
            test_.trials = 3;
        }
        
        void run(RecognitionTestModel &m) override {
            m.initializeTest(test_);
        }
        
        auto &common() {
            return test_.common;
        }
        
        void setSnr_dB(int x) override {
            test_.snr_dB = x;
        }
        
        void setTrials(int n) {
            test_.trials = n;
        }
        
        void setNextTarget(std::string s) override {
            targetList->setNext(std::move(s));
        }
        
        void setCurrentTarget(std::string s) override {
            targetList->setCurrent(std::move(s));
        }
        
        void setCurrentTargetWhenNext(std::string s) {
            targetList->setCurrentTargetWhenNext(std::move(s));
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
        
        const coordinate_response_measure::Trial &
            writtenCoordinateResponseTrial(OutputFileStub &file
        ) override {
            return file.writtenFixedLevelTrial2();
        }
        
        std::string receivedTargetListDirectory() override {
            return targetList->directory();
        }
    };

    class RecognitionTestModelOldTests : public ::testing::Test {
    protected:
        TargetListSetReaderStub targetListSetReader{};
        TargetListStub targetList{};
        TargetPlayerStub targetPlayer{};
        MaskerPlayerStub maskerPlayer{};
        OutputFileStub outputFile{};
        TrackFactoryStub snrTrackFactory{};
        ResponseEvaluatorStub evaluator{};
        RandomizerStub randomizer{};
        AdaptiveMethod adaptiveMethod{
            &targetListSetReader,
            &snrTrackFactory,
            &evaluator,
            &randomizer
        };
        FixedLevelMethod fixedLevelMethod{&targetList, &evaluator};
        RecognitionTestModel_Internal internalModel{
            &targetPlayer,
            &maskerPlayer,
            &evaluator,
            &outputFile,
            &randomizer
        };
        RecognitionTestModel model{
            &adaptiveMethod,
            &fixedLevelMethod,
            &internalModel
        };
        InitializingAdaptiveTest initializingAdaptiveTest{
            &targetListSetReader,
            &snrTrackFactory,
            &randomizer
        };
        InitializingFixedLevelTest initializingFixedLevelTest{&targetList};
        SubmittingCoordinateResponse submittingCoordinateResponse{};
        SubmittingCorrectResponse submittingCorrectResponse{};
        SubmittingIncorrectResponse submittingIncorrectResponse{};
        SubmittingFreeResponse submittingFreeResponse{};
        
        void initializeAdaptiveTest() {
            run(initializingAdaptiveTest);
        }
        
        void run(UseCase &useCase) {
            useCase.run(model);
        }
        
        void submitCoordinateResponse() {
            run(submittingCoordinateResponse);
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
        
        void assertTargetFilePathEquals(std::string what) {
            assertEqual(std::move(what), targetFilePath());
        }
        
        auto &outputFileLog() {
            return outputFile.log();
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
        
        void assertPushesSnrTrackDown(UseCase &useCase) {
            initializingAdaptiveTest.selectList(1);
            run(initializingAdaptiveTest);
            initializingAdaptiveTest.selectList(2);
            run(useCase);
            assertTrue(initializingAdaptiveTest.snrTrackPushedDown(1));
            assertFalse(initializingAdaptiveTest.snrTrackPushedUp(1));
        }
        
        void assertPushesSnrTrackUp(UseCase &useCase) {
            initializingAdaptiveTest.selectList(1);
            run(initializingAdaptiveTest);
            initializingAdaptiveTest.selectList(2);
            run(useCase);
            assertTrue(initializingAdaptiveTest.snrTrackPushedUp(1));
            assertFalse(initializingAdaptiveTest.snrTrackPushedDown(1));
        }
        
        void assertSelectsListAmongThoseWithIncompleteTracks(UseCase &useCase) {
            run(initializingAdaptiveTest);
            initializingAdaptiveTest.setTargetListNext(2, "a");
            initializingAdaptiveTest.setSnrTrackComplete(1);
            
            initializingAdaptiveTest.selectList(1);
            run(useCase);
            assertTargetFilePathEquals("a");
        }
    };
}
#endif
