#include "assert-utility.hpp"
#include <av-speech-in-noise/core/LogisticPsychometricFunction.hpp>
#include <cmath>
#include <gtest/gtest.h>
#include <algorithm>
#include <numeric>
#include <utility>

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
    assertEqual(LogisticSweetPoints{}({alpha, beta, gamma, lambda}),
        {-6.338253001141149e29, 0, 6.338253001141149e29}, 1e15);
}

TEST_F(LogisticSweetPointTester, testSimpleParameters) {
    const auto alpha = 1;
    const auto beta = 2;
    const auto gamma = 0.1;
    const auto lambda = 0.1;
    assertEqual(LogisticSweetPoints{}({alpha, beta, gamma, lambda}),
        {0.095141601562522, 1, 1.904858398437488}, 1e-15);
}

class ParameterSpacer {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(ParameterSpacer);
    virtual std::vector<double> operator()(
        double lower, double upper, std::size_t N) = 0;
};

class LinearSpacer : public ParameterSpacer {
  public:
    std::vector<double> operator()(
        double lower, double upper, std::size_t N) override;
};

class LogSpacer : public ParameterSpacer {
  public:
    std::vector<double> operator()(
        double lower, double upper, std::size_t N) override;
};

static std::vector<double> linspace(double x1, double x2, std::size_t N) {
    if (N <= 0)
        return {};
    if (N == 1)
        return {x2};
    if (x1 == x2)
        return std::vector<double>(N, x1);
    std::vector<double> x(N);
    x.back() = x2;
    const auto step = (x2 - x1) / (N - 1);
    for (size_t i = N - 1; i > 1; i--)
        x[i - 1] = x2 - step * (N - i);
    x.front() = x1;
    return x;
}

template <class UnaryOperator>
static std::vector<double> transform(std::vector<double> v, UnaryOperator f) {
    std::transform(v.begin(), v.end(), v.begin(), f);
    return v;
}

static std::vector<double> logspace(double x1, double x2, std::size_t N) {
    return transform(
        linspace(x1, x2, N), [](double x) { return std::pow(10, x); });
}

static std::vector<double> normpdf(
    std::vector<double> x, double mu, double sigma) {
    return transform(std::move(x), [mu, sigma](double x) {
        const auto pi = std::acos(-1);
        return sigma <= 0 ? std::numeric_limits<double>::quiet_NaN()
                          : std::exp(-0.5 * std::pow((x - mu) / sigma, 2)) /
                (std::sqrt(2 * pi) * sigma);
    });
}

std::vector<double> LinearSpacer::operator()(
    double lower, double upper, std::size_t N) {
    if (N == 1)
        return {lower};
    return linspace(lower, upper, N);
}

std::vector<double> LogSpacer::operator()(
    double lower, double upper, std::size_t N) {
    if (N == 1)
        return {lower};
    return logspace(std::log10(lower), std::log10(upper), N);
}

class LinearNormPrior : public PriorProbability {
    const double mu;
    const double sigma;

  public:
    LinearNormPrior(double mu, double sigma);
    std::vector<double> operator()(std::vector<double> space) const override;
};

LinearNormPrior::LinearNormPrior(double mu, double sigma)
    : mu{mu}, sigma{sigma} {}

std::vector<double> LinearNormPrior::operator()(
    std::vector<double> space) const {
    return normpdf(std::move(space), mu, sigma);
}

class LogNormPrior : public PriorProbability {
    const double mu;
    const double sigma;

  public:
    LogNormPrior(double mu, double sigma);
    std::vector<double> operator()(std::vector<double> space) const override;
};

LogNormPrior::LogNormPrior(double mu, double sigma) : mu(mu), sigma(sigma) {}

std::vector<double> LogNormPrior::operator()(std::vector<double> space) const {
    return normpdf(
        transform(std::move(space), [](double x) { return std::log10(x); }), mu,
        sigma);
}

class FlatPrior : public PriorProbability {
  public:
    std::vector<double> operator()(std::vector<double> space) const override;
};

std::vector<double> FlatPrior::operator()(std::vector<double> space) const {
    return std::vector<double>(space.size(), 1);
}

static PosteriorDistributions exampleLogisticConfiguration() {
    return {{LinearSpacer()(-30, 30, 61), LinearNormPrior(0, 10)},
        {LogSpacer()(0.1, 10, 41), LogNormPrior(-0.5, 0.4)},
        {LinearSpacer()(0.02, 0.2, 11), FlatPrior()},
        {LinearSpacer()(0.02, 0.2, 11), FlatPrior()}};
}

class MeanPhi : public PhiComputer {
  public:
    Phi operator()(const UpdatedMaximumLikelihood &) const override;
};

Phi MeanPhi::operator()(const UpdatedMaximumLikelihood &uml) const {
    auto normalizedPosterior =
        transform(uml.posterior(), [](double x) { return std::exp(x); });
    const auto sum = std::accumulate(
        normalizedPosterior.begin(), normalizedPosterior.end(), 0.0);
    normalizedPosterior = transform(
        std::move(normalizedPosterior), [sum](double x) { return x / sum; });
    double alphaEstimate = 0;
    double betaEstimate = 0;
    double gammaEstimate = 0;
    double lambdaEstimate = 0;
    for (size_t i = 0; i < normalizedPosterior.size(); i++) {
        alphaEstimate += normalizedPosterior[i] * uml.alphaSpace(i);
        betaEstimate += normalizedPosterior[i] * uml.betaSpace(i);
        gammaEstimate += normalizedPosterior[i] * uml.gammaSpace(i);
        lambdaEstimate += normalizedPosterior[i] * uml.lambdaSpace(i);
    }
    return {alphaEstimate, betaEstimate, gammaEstimate, lambdaEstimate};
}

class UpdatedMaximumLikelihoodTester : public ::testing::Test {};

TEST_F(UpdatedMaximumLikelihoodTester, testExampleLogisticMeanPhiDownOnly) {
    TrackSpecifications track{};
    track.down = 3;
    track.up = 1;
    track.startingX = 30.0;
    track.lowerBound = -30.0;
    track.upperBound = 30.0;
    LogisticPsychometricFunction pf;
    LogisticSweetPoints sp;
    MeanPhi pc;
    UpdatedMaximumLikelihood uml(
        exampleLogisticConfiguration(), pf, sp, pc, track);
    assertEqual(uml.x(), 30.0);
    assertEqual(uml.reversals(), 0);
    uml.pushDown();
    assertEqual(uml.x(), 6.652636329123384, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.pushDown();
    assertEqual(uml.x(), 3.301128275552773, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.pushDown();
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
    LogisticSweetPoints sp;
    MeanPhi pc;
    UpdatedMaximumLikelihood uml(
        exampleLogisticConfiguration(), pf, sp, pc, track);
    assertEqual(uml.x(), 30.0);
    assertEqual(uml.reversals(), 0);
    uml.pushUp();
    assertEqual(uml.x(), 7.937229385563851, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.pushUp();
    assertEqual(uml.x(), 14.914421286430443, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.pushUp();
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
    LogisticSweetPoints sp;
    MeanPhi pc;
    UpdatedMaximumLikelihood uml(
        exampleLogisticConfiguration(), pf, sp, pc, track);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(uml.sweetPoints().empty());
    uml.pushUp();
    assertEqual(uml.phi(),
        {0.935279300498662, 0.501739472594823, 0.109708801753186,
            0.137098486912143},
        1e-13);
    assertEqual(uml.sweetPoints(),
        {-6.129234036864910, -2.625748166977760, 0.877737702909389,
            4.407483544236620, 7.937229385563851},
        1e-13);
    uml.pushDown();
    assertEqual(uml.x(), 4.242855160473956, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.pushUp();
    assertEqual(uml.x(), 9.861193730461308, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.pushDown();
    assertEqual(uml.x(), 7.537607961883664, 1e-13);
    assertEqual(uml.reversals(), 0);
    uml.pushUp();
    assertEqual(uml.x(), 11.828823619069409, 1e-13);
    assertEqual(uml.reversals(), 0);
}
}
