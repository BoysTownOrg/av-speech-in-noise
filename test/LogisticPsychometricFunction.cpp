#include "assert-utility.hpp"
#include <av-speech-in-noise/core/LogisticPsychometricFunction.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
class LogisticPsychometricFunctionTester : public ::testing::Test {};

TEST_F(LogisticPsychometricFunctionTester, testZeroedParameters) {
    const auto alpha = 0;
    const auto beta = 0;
    const auto gamma = 0;
    const auto lambda = 0;
    auto f(LogisticPsychometricFunction{});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(f({alpha, beta, gamma, lambda}, 0), 0.5);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(f({alpha, beta, gamma, lambda}, 1), 0.5);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(f({alpha, beta, gamma, lambda}, -1), 0.5);
}

TEST_F(LogisticPsychometricFunctionTester, testSimpleValues) {
    const auto alpha = 1;
    const auto beta = 2;
    const auto gamma = 3;
    const auto lambda = 4;
    auto f(LogisticPsychometricFunction{});
    assertEqual(f({alpha, beta, gamma, lambda}, 0), 2.284782467867295, 1e-15);
    assertEqual(f({alpha, beta, gamma, lambda}, 1), 0.0);
    assertEqual(f({alpha, beta, gamma, lambda}, -1), 2.892082740227451, 1e-15);
}

class LogisticSweetPointTester : public ::testing::Test {};

TEST_F(LogisticSweetPointTester, testZeroedParameters) {
    const auto alpha = 0;
    const auto beta = 0;
    const auto gamma = 0;
    const auto lambda = 0;
    assertEqual(LogisticSweetPoints()({alpha, beta, gamma, lambda}),
        {-6.338253001141149e29, 0, 6.338253001141149e29}, 1e15);
}

TEST_F(LogisticSweetPointTester, testSimpleParameters) {
    const auto alpha = 1;
    const auto beta = 2;
    const auto gamma = 0.1;
    const auto lambda = 0.1;
    assertEqual(LogisticSweetPoints()({alpha, beta, gamma, lambda}),
        {0.095141601562522, 1, 1.904858398437488}, 1e-15);
}

}
