#include "av-speech-in-noise/core/AdaptiveTrackFactory.hpp"
#include "ConfigurationRegistryStub.hpp"
#include "assert-utility.hpp"
#include <gtest/gtest.h>
#include <sstream>

namespace av_speech_in_noise {
namespace {
class AdaptiveTrackFactoryTests : public ::testing::Test {};

TEST_F(AdaptiveTrackFactoryTests, tbd) {
    ConfigurationRegistryStub registry;
    AdaptiveTrackFactory factory{registry};
    factory.configure("uml", "true");
    factory.configure("alpha space", "log -29 31 63");
    factory.configure("alpha prior", "flat");
    factory.configure("beta space", "linear 0.2 3.4 11");
    factory.configure("beta prior", "linearnorm 2.3 4.5");
    factory.configure("gamma prior", "lognorm 1.2 3.4");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        ParameterSpace::Log, factory.umlSettings.alpha.space.space);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        -29., factory.umlSettings.alpha.space.lower);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(31., factory.umlSettings.alpha.space.upper);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(63, factory.umlSettings.alpha.space.N);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(PriorProbabilityKind::Flat,
        factory.umlSettings.alpha.priorProbability.kind);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        ParameterSpace::Linear, factory.umlSettings.beta.space.space);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(0.2, factory.umlSettings.beta.space.lower);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(3.4, factory.umlSettings.beta.space.upper);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(11, factory.umlSettings.beta.space.N);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(PriorProbabilityKind::LinearNorm,
        factory.umlSettings.beta.priorProbability.kind);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        2.3, factory.umlSettings.beta.priorProbability.mu);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        4.5, factory.umlSettings.beta.priorProbability.sigma);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(PriorProbabilityKind::LogNorm,
        factory.umlSettings.gamma.priorProbability.kind);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        1.2, factory.umlSettings.gamma.priorProbability.mu);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        3.4, factory.umlSettings.gamma.priorProbability.sigma);
}

TEST_F(AdaptiveTrackFactoryTests, oneSequence) {
    ConfigurationRegistryStub registry;
    AdaptiveTrackFactory factory{registry};
    TrackingSequence sequence{};
    sequence.up = 1;
    sequence.down = 2;
    sequence.runCount = 3;
    sequence.stepSize = 4;
    factory.configure(up, "1");
    factory.configure(down, "2");
    factory.configure(reversalsPerStepSize, "3");
    factory.configure(stepSizes, "4");
    assertEqual({sequence},
        std::get<LevittSettings>(adaptiveMethod.test.trackSettings)
            .trackingRule);
}

TEST_F(AdaptiveTrackFactoryTests, twoSequences) {
    TrackingSequence first{};
    first.up = 1;
    first.down = 3;
    first.runCount = 5;
    first.stepSize = 7;
    TrackingSequence second{};
    second.up = 2;
    second.down = 4;
    second.runCount = 6;
    second.stepSize = 8;
    factory.configure("up", "1 2");
    factory.configure("down", "3 4");
    factory.configure("reversalsPerStepSize", "5 6");
    factory.configure("stepSizes", "7 8");
    assertEqual({first, second},
        std::get<LevittSettings>(adaptiveMethod.test.trackSettings)
            .trackingRule);
}

TEST_F(AdaptiveTrackFactoryTests, writesUmlTrackSettings) {
    ConfigurationRegistryStub registry;
    AdaptiveTrackFactory factory{registry};
    factory.umlSettings.alpha.space.space = ParameterSpace::Linear;
    factory.umlSettings.alpha.space.lower = -12.3;
    factory.umlSettings.alpha.space.upper = 45.6;
    factory.umlSettings.alpha.space.N = 7;
    factory.umlSettings.beta.space.space = ParameterSpace::Log;
    factory.umlSettings.beta.space.lower = -2.3;
    factory.umlSettings.beta.space.upper = 5.6;
    factory.umlSettings.beta.space.N = 70;
    factory.umlSettings.alpha.priorProbability.kind =
        PriorProbabilityKind::LinearNorm;
    factory.umlSettings.alpha.priorProbability.mu = 1.2;
    factory.umlSettings.alpha.priorProbability.sigma = 3.4;
    factory.umlSettings.beta.priorProbability.kind =
        PriorProbabilityKind::LogNorm;
    factory.umlSettings.beta.priorProbability.mu = 1.01;
    factory.umlSettings.beta.priorProbability.sigma = 2.02;
    factory.umlSettings.gamma.priorProbability.kind =
        PriorProbabilityKind::Flat;
    factory.umlSettings.up = 42;
    factory.umlSettings.down = 64;
    factory.umlSettings.trials = 123;
    factory.uml = true;
    std::stringstream stream;
    factory.write(stream);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(R"(alpha space: linear -12.3 45.6 7
beta space: log -2.3 5.6 70
alpha prior: linearnorm 1.2 3.4
beta prior: lognorm 1.01 2.02
gamma prior: flat
up: 42
down: 64
trials: 123)",
        stream.str());
}
}
}
