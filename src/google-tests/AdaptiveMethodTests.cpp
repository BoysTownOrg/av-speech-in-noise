#include "TargetListStub.h"
#include "TargetListSetReaderStub.h"
#include "TrackStub.h"
#include "RandomizerStub.h"
#include "ResponseEvaluatorStub.h"
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
        AdaptiveMethod method{
            &targetListSetReader,
            &snrTrackFactory,
            &evaluator,
            &randomizer
        };
        AdaptiveTest test;
        TrackingRule targetLevelRule_;
        std::vector<std::shared_ptr<TargetList>> lists;
        std::vector<std::shared_ptr<Track>> tracks;
        
        AdaptiveMethodTests() {
            test.targetLevelRule = &targetLevelRule_;
            for (int i = 0; i < 3; ++i) {
                lists.push_back(std::make_shared<TargetListStub>());
                tracks.push_back(std::make_shared<TrackStub>());
            }
            targetListSetReader.setTargetLists(lists);
            snrTrackFactory.setTracks(tracks);
        }
        
        auto snrTrackFactoryParameters() const {
            return snrTrackFactory.parameters();
        }
        
        void assertSettingsContainTargetLevelRule(const Track::Settings &s) {
            const auto *rule = &targetLevelRule_;
            assertEqual(rule, s.rule);
        }
        
        void initialize() {
            method.initialize(test);
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
        for (int i = 0; i < 3; ++i)
            assertSettingsContainTargetLevelRule(
                snrTrackFactoryParameters().at(i)
            );
    }

    TEST_F(
        AdaptiveMethodTests,
        initializeCreatesEachSnrTrackWithSnr
    ) {
        test.startingSnr_dB = 1;
        initialize();
        for (int i = 0; i < 3; ++i)
            assertEqual(1, snrTrackFactoryParameters().at(i).startingX);
    }
}
