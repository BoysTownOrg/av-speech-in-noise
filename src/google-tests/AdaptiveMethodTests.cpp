#include "TargetListStub.h"
#include "TargetListSetReaderStub.h"
#include "TrackStub.h"
#include "RandomizerStub.h"
#include "ResponseEvaluatorStub.h"
#include "OutputFileStub.h"
#include "assert-utility.h"
#include <recognition-test/AdaptiveMethod.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
    class AdaptiveMethodTests : public ::testing::Test {
    protected:
        TargetListSetReaderStub targetListSetReader;
        TrackFactoryStub snrTrackFactory;
        ResponseEvaluatorStub evaluator;
        RandomizerStub randomizer;
        OutputFileStub outputFile;
        AdaptiveMethod method{
            &targetListSetReader,
            &snrTrackFactory,
            &evaluator,
            &randomizer
        };
        AdaptiveTest test;
        coordinate_response_measure::SubjectResponse coordinateResponse;
        TrackingRule targetLevelRule_;
        std::vector<std::shared_ptr<TargetListStub>> lists;
        std::vector<std::shared_ptr<TrackStub>> tracks;
        
        AdaptiveMethodTests() {
            test.targetLevelRule = &targetLevelRule_;
            for (int i = 0; i < 3; ++i) {
                lists.push_back(std::make_shared<TargetListStub>());
                tracks.push_back(std::make_shared<TrackStub>());
            }
            targetListSetReader.setTargetLists({lists.begin(), lists.end()});
            snrTrackFactory.setTracks({tracks.begin(), tracks.end()});
        }
    public:
        auto snrTrackFactoryParameters() const {
            return snrTrackFactory.parameters();
        }
        
        auto snrTrackFactoryParameters(int x) const {
            return snrTrackFactoryParameters().at(x);
        }
        
        void initialize() {
            method.initialize(test);
        }
        
        void assertPassedTargetLevelRule(const Track::Settings &s) {
            assertEqual(&std::as_const(targetLevelRule_), s.rule);
        }
        
        void assertStartingXEqualsOne(const Track::Settings &s) {
            assertEqual(1, s.startingX);
        }
        
        void assertCeilingEqualsOne(const Track::Settings &s) {
            assertEqual(1, s.ceiling);
        }
        
        void assertFloorEqualsOne(const Track::Settings &s) {
            assertEqual(1, s.floor);
        }
        
        void applyToSnrTrackFactoryParameters(
            int n,
            void(AdaptiveMethodTests::*f)(const Track::Settings &)
        ) {
            for (int i = 0; i < n; ++i)
                (this->*f)(snrTrackFactoryParameters(i));
        }
        
        void selectList(int n) {
            randomizer.setRandomInt(n);
        }
        
        std::string next() {
            return method.next();
        }
        
        void assertNextEquals(std::string s) {
            assertEqual(std::move(s), next());
        }
        
        void setNextForList(int n, std::string s) {
            lists.at(n)->setNext(std::move(s));
        }
        
        void assertRandomizerPassedIntegerBounds(int a, int b) {
            assertEqual(a, randomizer.lowerIntBound());
            assertEqual(b, randomizer.upperIntBound());
        }
        
        void submitCoordinateResponse() {
            method.submitResponse(coordinateResponse);
        }
        
        auto track(int n) {
            return tracks.at(n);
        }
        
        void setCurrentForTarget(int n, std::string s) {
            lists.at(n)->setCurrent(std::move(s));
        }
        
        void writeLastCoordinateResponse() {
            method.writeLastCoordinateResponse(&outputFile);
        }
        
        auto writtenCoordinateResponseTrial() const {
            return outputFile.writtenAdaptiveCoordinateResponseTrial2();
        }
        
        void writeCoordinateResponse() {
            submitCoordinateResponse();
            writeLastCoordinateResponse();
        }
    };
    
    TEST_F(
        AdaptiveMethodTests,
        initializeCreatesSnrTrackForEachList
    ) {
        initialize();
        assertEqual(3UL, snrTrackFactoryParameters().size());
    }

    TEST_F(
        AdaptiveMethodTests,
        initializeCreatesEachSnrTrackWithTargetLevelRule
    ) {
        initialize();
        applyToSnrTrackFactoryParameters(
            3,
            &AdaptiveMethodTests::assertPassedTargetLevelRule
        );
    }

    TEST_F(
        AdaptiveMethodTests,
        initializeCreatesEachSnrTrackWithSnr
    ) {
        test.startingSnr_dB = 1;
        initialize();
        applyToSnrTrackFactoryParameters(
            3,
            &AdaptiveMethodTests::assertStartingXEqualsOne
        );
    }

    TEST_F(
        AdaptiveMethodTests,
        initializeCreatesEachSnrTrackWithCeiling
    ) {
        test.ceilingSnr_dB = 1;
        initialize();
        applyToSnrTrackFactoryParameters(
            3,
            &AdaptiveMethodTests::assertCeilingEqualsOne
        );
    }

    TEST_F(
        AdaptiveMethodTests,
        initializeCreatesEachSnrTrackWithFloor
    ) {
        test.floorSnr_dB = 1;
        initialize();
        applyToSnrTrackFactoryParameters(
            3,
            &AdaptiveMethodTests::assertFloorEqualsOne
        );
    }

    TEST_F(
        AdaptiveMethodTests,
        writeTestParametersPassesToOutputFile
    ) {
        initialize();
        method.writeTestingParameters(&outputFile);
        assertEqual(&std::as_const(test), outputFile.adaptiveTest());
    }

    TEST_F(
        AdaptiveMethodTests,
        initializePassesTargetListDirectory
    ) {
        test.common.targetListDirectory = "a";
        initialize();
        assertEqual("a", targetListSetReader.directory());
    }

    TEST_F(
        AdaptiveMethodTests,
        nextReturnsNextFilePathAfterInitialize
    ) {
        setNextForList(0, "a");
        selectList(0);
        initialize();
        assertNextEquals("a");
    }

    TEST_F(
        AdaptiveMethodTests,
        nextReturnsNextFilePathAfterCoordinateResponse
    ) {
        initialize();
        setNextForList(1, "a");
        selectList(1);
        submitCoordinateResponse();
        assertNextEquals("a");
    }

    TEST_F(
        AdaptiveMethodTests,
        randomizerPassedIntegerBoundsOfLists
    ) {
        initialize();
        assertRandomizerPassedIntegerBounds(0, 2);
    }

    TEST_F(
        AdaptiveMethodTests,
        submitCoordinateResponseSelectsListInRangeAfterRemovingCompleteTracks
    ) {
        initialize();
        track(2)->setComplete();
        submitCoordinateResponse();
        assertRandomizerPassedIntegerBounds(0, 1);
    }

    TEST_F(
        AdaptiveMethodTests,
        snrReturnsThatOfCurrentTrack
    ) {
        track(0)->setX(1);
        selectList(0);
        initialize();
        assertEqual(1, method.snr_dB());
    }

    TEST_F(
        AdaptiveMethodTests,
        submitCoordinateResponsePassesCurrentToEvaluator
    ) {
        selectList(1);
        initialize();
        setCurrentForTarget(1, "a");
        submitCoordinateResponse();
        assertEqual("a", evaluator.correctColorFilePath());
        assertEqual("a", evaluator.correctNumberFilePath());
    }

    TEST_F(
        AdaptiveMethodTests,
        submitCoordinateResponsePassesCorrectFilePathToEvaluator
    ) {
        selectList(1);
        initialize();
        setCurrentForTarget(1, "a");
        submitCoordinateResponse();
        assertEqual("a", evaluator.correctFilePath());
    }

    TEST_F(
        AdaptiveMethodTests,
        submitCoordinateResponsePassesResponseToEvaluator
    ) {
        initialize();
        submitCoordinateResponse();
        assertEqual(&std::as_const(coordinateResponse), evaluator.response());
    }

    TEST_F(
        AdaptiveMethodTests,
        writeCoordinateResponsePassesSubjectColor
    ) {
        using coordinate_response_measure::Color;
        
        initialize();
        coordinateResponse.color = Color::blue;
        writeCoordinateResponse();
        assertEqual(
            Color::blue,
            writtenCoordinateResponseTrial().subjectColor
        );
    }

    TEST_F(
        AdaptiveMethodTests,
        writeCoordinateResponsePassesSubjectNumber
    ) {
        initialize();
        coordinateResponse.number = 1;
        writeCoordinateResponse();
        assertEqual(1, writtenCoordinateResponseTrial().subjectNumber);
    }

    TEST_F(
        AdaptiveMethodTests,
        writeCoordinateResponsePassesReversalsAfterUpdatingTrack
    ) {
        selectList(1);
        initialize();
        track(1)->setReversalsWhenUpdated(3);
        selectList(2);
        writeCoordinateResponse();
        assertEqual(3, outputFile.writtenAdaptiveCoordinateResponseTrial().reversals);
    }

    TEST_F(
        AdaptiveMethodTests,
        writeCoordinateResponsePassesSnrBeforeUpdatingTrack
    ) {
        selectList(1);
        initialize();
        track(1)->setX(4);
        track(1)->setXWhenUpdated(4);
        selectList(2);
        writeCoordinateResponse();
        assertEqual(4, outputFile.writtenAdaptiveCoordinateResponseTrial().SNR_dB);
    }
}
