#include "assert-utility.hpp"
#include <av-speech-in-noise/core/LogisticPsychometricFunction.hpp>
#include <gtest/gtest.h>

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
