#include "TargetListStub.h"
#include "TargetListSetReaderStub.h"
#include "TrackStub.h"
#include "RandomizerStub.h"
#include "ResponseEvaluatorStub.h"
#include "assert-utility.h"
#include <recognition-test/RecognitionTestModel.hpp>
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
    };
    
    TEST_F(
        AdaptiveMethodTests,
        loadFromDirectoryCreatesSnrTrackForEachList
    ) {
        std::vector<std::shared_ptr<TargetList>> lists;
        std::vector<std::shared_ptr<Track>> tracks;
        for (int i = 0; i < 3; ++i) {
            lists.push_back(std::make_shared<TargetListStub>());
            tracks.push_back(std::make_shared<TrackStub>());
        }
        targetListSetReader.setTargetLists(lists);
        snrTrackFactory.setTracks(tracks);
        method.loadTargets({});
        assertEqual(3UL, snrTrackFactory.parameters().size());
    }
}
