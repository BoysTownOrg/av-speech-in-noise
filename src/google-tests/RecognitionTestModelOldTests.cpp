#include "RecognitionTestModelOldTests.hpp"

namespace av_speech_in_noise::tests::recognition_test {
    TEST_F(
        RecognitionTestModelOldTests,
        submitCorrectResponsePushesSnrDownForAdaptiveTest
    ) {
        assertPushesSnrTrackDown(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitIncorrectResponsePushesSnrUpForAdaptiveTest
    ) {
        assertPushesSnrTrackUp(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitCorrectResponseSelectsNextListAmongThoseWithIncompleteTracksForAdaptiveTest
    ) {
        assertSelectsListAmongThoseWithIncompleteTracks(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelOldTests,
        submitIncorrectResponseSelectsNextListAmongThoseWithIncompleteTracksForAdaptiveTest
    ) {
        assertSelectsListAmongThoseWithIncompleteTracks(submittingIncorrectResponse);
    }
}

