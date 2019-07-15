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
        TrackingRule targetLevelRule_;
        std::vector<std::shared_ptr<TargetListStub>> lists;
        std::vector<std::shared_ptr<Track>> tracks;
        
        AdaptiveMethodTests() {
            test.targetLevelRule = &targetLevelRule_;
            for (int i = 0; i < 3; ++i) {
                lists.push_back(std::make_shared<TargetListStub>());
                tracks.push_back(std::make_shared<TrackStub>());
            }
            targetListSetReader.setTargetLists({lists.begin(), lists.end()});
            snrTrackFactory.setTracks(tracks);
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
        lists.at(0)->setNext("a");
        randomizer.setRandomInt(0);
        initialize();
        assertEqual("a", method.next());
    }

    TEST_F(
        AdaptiveMethodTests,
        nextReturnsNextFilePathAfterSubmitCoordinateResponse
    ) {
        initialize();
        lists.at(1)->setNext("a");
        randomizer.setRandomInt(1);
        method.submitResponse(coordinate_response_measure::SubjectResponse{});
        assertEqual("a", method.next());
    }
}
