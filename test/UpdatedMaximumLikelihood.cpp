#include "assert-utility.hpp"

#include <av-speech-in-noise/core/UpdatedMaximumLikelihood.hpp>

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
    assertEqual(LogisticPsychometricFunction{}.sweetPoints(
                    {alpha, beta, gamma, lambda}),
        {-6.338253001141149e29, 0, 6.338253001141149e29}, 1e15);
}

TEST_F(LogisticSweetPointTester, testSimpleParameters) {
    const auto alpha = 1;
    const auto beta = 2;
    const auto gamma = 0.1;
    const auto lambda = 0.1;
    assertEqual(LogisticPsychometricFunction{}.sweetPoints(
                    {alpha, beta, gamma, lambda}),
        {0.095141601562522, 1, 1.904858398437488}, 1e-15);
}

class UpdatedMaximumLikelihoodTester : public ::testing::Test {};

auto exampleLogisticConfiguration() -> PosteriorDistributions {
    return {{linspace(-30, 30, 61), LinearNormPrior(0, 10)},
        {logspace(0.1, 10, 41), LogNormPrior(-0.5, 0.4)},
        {linspace(0.02, 0.2, 11), FlatPrior()},
        {linspace(0.02, 0.2, 11), FlatPrior()}};
}

TEST_F(UpdatedMaximumLikelihoodTester, testExampleLogisticMeanPhiDownOnly) {
    TrackSpecifications track{};
    track.down = 3;
    track.up = 1;
    track.startingX = 30.0;
    track.lowerBound = -30.0;
    track.upperBound = 30.0;
    LogisticPsychometricFunction pf;
    MeanPhi pc;
    UpdatedMaximumLikelihood uml(exampleLogisticConfiguration(), pf, pc, track);
    assertEqual(uml.x(), 30.0);
    assertEqual(uml.reversals(), 0);
    uml.down();
    assertEqual(uml.x(), 6.652636329123384, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.down();
    assertEqual(uml.x(), 3.301128275552773, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.down();
    assertEqual(uml.x(), -2.377301836485305, 1e-13);
    assertEqual(uml.reversals(), 0);
}

TEST_F(UpdatedMaximumLikelihoodTester, testExampleLogisticMeanPhiUpOnly) {
    TrackSpecifications track{};
    track.down = 3;
    track.up = 1;
    track.startingX = 30.0;
    track.lowerBound = -30.0;
    track.upperBound = 30.0;
    LogisticPsychometricFunction pf;
    MeanPhi pc;
    UpdatedMaximumLikelihood uml(exampleLogisticConfiguration(), pf, pc, track);
    assertEqual(uml.x(), 30.0);
    assertEqual(uml.reversals(), 0);
    uml.up();
    assertEqual(uml.x(), 7.937229385563851, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.up();
    assertEqual(uml.x(), 14.914421286430443, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.up();
    assertEqual(uml.x(), 20.816765327034403, 1e-13);
    assertEqual(uml.reversals(), 0);
}

TEST_F(UpdatedMaximumLikelihoodTester, testExampleLogisticMeanPhiAlternating) {
    TrackSpecifications track{};
    track.down = 3;
    track.up = 1;
    track.startingX = 30.0;
    track.lowerBound = -30.0;
    track.upperBound = 30.0;
    LogisticPsychometricFunction pf;
    MeanPhi pc;
    UpdatedMaximumLikelihood uml(exampleLogisticConfiguration(), pf, pc, track);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(uml.sweetPoints().empty());
    uml.up();
    const auto phi{*uml.phi()};
    assertEqual({phi.alpha, phi.beta, phi.gamma, phi.lambda},
        {0.935279300498662, 0.501739472594823, 0.109708801753186,
            0.137098486912143},
        1e-13);
    assertEqual(uml.sweetPoints(),
        {-6.129234036864910, -2.625748166977760, 0.877737702909389,
            4.407483544236620, 7.937229385563851},
        1e-13);
    uml.down();
    assertEqual(uml.x(), 4.242855160473956, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.up();
    assertEqual(uml.x(), 9.861193730461308, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.down();
    assertEqual(uml.x(), 7.537607961883664, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.up();
    assertEqual(uml.x(), 11.828823619069409, 1e-13);
    assertEqual(uml.reversals(), 0);
}

TEST_F(UpdatedMaximumLikelihoodTester,
    testExampleLogisticMeanPhiAlternatingWithReset) {
    TrackSpecifications track{};
    track.down = 3;
    track.up = 1;
    track.startingX = 30.0;
    track.lowerBound = -30.0;
    track.upperBound = 30.0;
    LogisticPsychometricFunction pf;
    MeanPhi pc;
    UpdatedMaximumLikelihood uml(exampleLogisticConfiguration(), pf, pc, track);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(uml.sweetPoints().empty());
    uml.up();
    uml.up();
    uml.down();
    uml.reset();
    uml.up();
    const auto phi{*uml.phi()};
    assertEqual({phi.alpha, phi.beta, phi.gamma, phi.lambda},
        {0.935279300498662, 0.501739472594823, 0.109708801753186,
            0.137098486912143},
        1e-13);
    assertEqual(uml.sweetPoints(),
        {-6.129234036864910, -2.625748166977760, 0.877737702909389,
            4.407483544236620, 7.937229385563851},
        1e-13);
    uml.down();
    assertEqual(uml.x(), 4.242855160473956, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.up();
    assertEqual(uml.x(), 9.861193730461308, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.down();
    assertEqual(uml.x(), 7.537607961883664, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.up();
    assertEqual(uml.x(), 11.828823619069409, 1e-13);
    assertEqual(uml.reversals(), 0);
}

TEST_F(UpdatedMaximumLikelihoodTester, testExampleLogisticCompletion) {
    TrackSpecifications track{};
    track.down = 3;
    track.up = 1;
    track.trials = 10;
    track.startingX = 30.0;
    track.lowerBound = -30.0;
    track.upperBound = 30.0;
    LogisticPsychometricFunction pf;
    MeanPhi pc;
    UpdatedMaximumLikelihood uml(exampleLogisticConfiguration(), pf, pc, track);
    uml.up();
    uml.up();
    uml.down();
    uml.up();
    uml.up();
    uml.down();
    uml.down();
    uml.up();
    uml.up();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(uml.complete());
    uml.up();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(uml.complete());
}
}
