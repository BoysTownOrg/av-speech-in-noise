#include "UpdatedMaximumLikelihood.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <memory>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace av_speech_in_noise {
template <class UnaryOperator>
static auto transform(std::vector<double> v, UnaryOperator f)
    -> std::vector<double> {
    std::transform(v.begin(), v.end(), v.begin(), f);
    return v;
}

auto linspace(double x1, double x2, std::size_t N) -> std::vector<double> {
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

auto logspace(double x1, double x2, std::size_t N) -> std::vector<double> {
    return transform(linspace(std::log10(x1), std::log10(x2), N),
        [](double x) { return std::pow(10, x); });
}

static auto normpdf(std::vector<double> x, double mu, double sigma)
    -> std::vector<double> {
    return transform(std::move(x), [mu, sigma](double x) {
        const auto pi = std::acos(-1);
        return sigma <= 0 ? std::numeric_limits<double>::quiet_NaN()
                          : std::exp(-0.5 * std::pow((x - mu) / sigma, 2)) /
                (std::sqrt(2 * pi) * sigma);
    });
}

static auto sortIndices(const std::vector<double> &v) -> std::vector<size_t> {
    std::vector<size_t> indices(v.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(),
        [&v](size_t i1, size_t i2) { return v[i1] < v[i2]; });
    return indices;
}

static auto maxEpsilon(double tolerance, double x) -> double {
    const auto absX = std::abs(x);
    return std::max(tolerance, 10 * (std::nextafter(absX, absX + 1) - absX));
}

static auto fminsearch(const std::function<double(std::vector<double>)> &f,
    std::vector<double> startingPoint) -> std::vector<double> {
    const auto n = startingPoint.size();
    std::vector<std::vector<double>> simplexBuffer(n);
    for (std::size_t i = 0; i < n; i++) {
        simplexBuffer[i].resize(n + 1);
        simplexBuffer[i][0] = startingPoint[i];
    }
    std::vector<double> simplexEvaluations(n + 1);
    simplexEvaluations[0] = f(startingPoint);
    for (std::size_t i = 0; i < n; ++i) {
        auto simplexGuess = startingPoint;
        if (simplexGuess[i] != 0)
            simplexGuess[i] *= 1.05;
        else
            simplexGuess[i] = 0.00025;
        for (std::size_t j = 0; j < n; j++)
            simplexBuffer[j][i + 1] = simplexGuess[j];
        simplexEvaluations[i + 1] = f(simplexGuess);
    }
    auto simplex = simplexBuffer;
    auto order = sortIndices(simplexEvaluations);
    for (std::size_t i = 0; i < simplexEvaluations.size(); i++)
        for (std::size_t j = 0; j < simplexBuffer.size(); j++)
            simplex[j][i] = simplexBuffer[j][order[i]];
    std::sort(simplexEvaluations.begin(), simplexEvaluations.end());
    auto iterationCount = 1UL;
    auto functionEvaluationCount = n + 1;
    const auto maxFunctionEvaluations = 200 * n;
    const auto maxIterations = 200 * n;
    const auto functionValueTolerance = 1e-4;
    const auto xTolerance = 1e-4;
    const auto rho = 1.0;
    const auto chi = 2;
    const auto psi = 0.5;
    const auto sigma = 0.5;
    while (functionEvaluationCount < maxFunctionEvaluations &&
        iterationCount < maxIterations) {
        const double evaluationRange =
            simplexEvaluations.back() - simplexEvaluations.front();
        double maxDeltaX = 0;
        for (std::size_t row = 0; row < n; ++row)
            for (std::size_t col = 1; col < n + 1; ++col)
                maxDeltaX = std::max(maxDeltaX,
                    std::abs(simplex[row][col] - simplex[row].front()));
        double maxX = 0;
        for (std::size_t row = 0; row < n; row++)
            maxX = std::max(maxX, simplex[row][0]);
        if (evaluationRange <= maxEpsilon(functionValueTolerance,
                                   simplexEvaluations.front()) &&
            maxDeltaX <= maxEpsilon(xTolerance, maxX))
            break;
        std::vector<double> xbar(n);
        for (std::size_t i = 0; i < n; i++) {
            for (std::size_t j = 0; j < n; j++)
                xbar[i] += simplex[i][j];
            xbar[i] /= n;
        }
        std::vector<double> reflection(n);
        for (std::size_t i = 0; i < n; ++i)
            reflection[i] = (1 + rho) * xbar[i] - rho * simplex[i].back();
        const auto reflectionEvaluation = f(reflection);
        ++functionEvaluationCount;
        if (reflectionEvaluation < simplexEvaluations[0]) {
            std::vector<double> expansion(n);
            for (std::size_t i = 0; i < n; ++i)
                expansion[i] =
                    (1 + rho * chi) * xbar[i] - rho * chi * simplex[i].back();
            const auto expansionEvaluation = f(expansion);
            ++functionEvaluationCount;
            if (expansionEvaluation < reflectionEvaluation) {
                for (std::size_t i = 0; i < n; ++i)
                    simplex[i].back() = expansion[i];
                simplexEvaluations.back() = expansionEvaluation;
            } else {
                for (std::size_t i = 0; i < n; ++i)
                    simplex[i].back() = reflection[i];
                simplexEvaluations.back() = reflectionEvaluation;
            }
        } else {
            if (reflectionEvaluation < simplexEvaluations[n - 1]) {
                for (std::size_t i = 0; i < n; ++i)
                    simplex[i].back() = reflection[i];
                simplexEvaluations.back() = reflectionEvaluation;
            } else {
                bool shrink = false;
                if (reflectionEvaluation < simplexEvaluations.back()) {
                    std::vector<double> outsideContraction(n);
                    for (std::size_t i = 0; i < n; ++i)
                        outsideContraction[i] = (1 + psi * rho) * xbar[i] -
                            psi * rho * simplex[i][n];
                    const auto outsideContractionEvaluation =
                        f(outsideContraction);
                    ++functionEvaluationCount;
                    if (outsideContractionEvaluation <= reflectionEvaluation) {
                        for (std::size_t i = 0; i < n; i++)
                            simplex[i].back() = outsideContraction[i];
                        simplexEvaluations.back() =
                            outsideContractionEvaluation;
                    } else
                        shrink = true;
                } else {
                    std::vector<double> insideContraction(n);
                    for (std::size_t i = 0; i < n; i++)
                        insideContraction[i] =
                            (1 - psi) * xbar[i] + psi * simplex[i][n];
                    const auto insideContractionEvaluation =
                        f(insideContraction);
                    functionEvaluationCount++;
                    if (insideContractionEvaluation < simplexEvaluations[n]) {
                        for (std::size_t i = 0; i < n; i++)
                            simplex[i].back() = insideContraction[i];
                        simplexEvaluations.back() = insideContractionEvaluation;
                    } else
                        shrink = true;
                }
                if (shrink) {
                    for (std::size_t col = 1; col < n + 1; ++col) {
                        std::vector<double> shrinkPoint(n);
                        for (std::size_t row = 0; row < n; ++row)
                            shrinkPoint[row] = simplex[row][col] =
                                simplex[row].front() +
                                sigma *
                                    (simplex[row][col] - simplex[row].front());
                        simplexEvaluations[col] = f(shrinkPoint);
                    }
                    functionEvaluationCount += n;
                }
            }
        }
        order = sortIndices(simplexEvaluations);
        for (std::size_t col = 0; col < simplexEvaluations.size(); ++col)
            for (std::size_t row = 0; row < simplex.size(); ++row)
                simplexBuffer[row][col] = simplex[row][order[col]];
        simplex = simplexBuffer;
        std::sort(simplexEvaluations.begin(), simplexEvaluations.end());
        ++iterationCount;
    }
    std::vector<double> result(n);
    for (std::size_t i = 0; i < n; ++i)
        result[i] = simplex[i].front();
    return result;
}

auto LogisticPsychometricFunction::operator()(Phi phi, double x) -> double {
    return phi.gamma +
        ((1 - phi.gamma - phi.lambda) /
            (1 + std::exp(-(x - phi.alpha) * phi.beta)));
}

static auto alphaEstimate(Phi phi, double x) -> double {
    return -std::exp(2 * phi.beta * (phi.alpha - x)) *
        std::pow(1 + std::exp(phi.beta * (x - phi.alpha)), 2) *
        (-phi.gamma + (phi.lambda - 1) * std::exp(phi.beta * (x - phi.alpha))) *
        (1 - phi.gamma + phi.lambda * std::exp(phi.beta * (x - phi.alpha))) /
        (std::pow(phi.beta, 2) * std::pow(phi.gamma + phi.lambda - 1, 2));
}

static auto betaEstimate(Phi phi, double x) -> double {
    return -std::exp(2 * phi.beta * (phi.alpha - x)) *
        std::pow(1 + std::exp(phi.beta * (x - phi.alpha)), 2) *
        (-phi.gamma + (phi.lambda - 1) * std::exp(phi.beta * (x - phi.alpha))) *
        (1 - phi.gamma + phi.lambda * std::exp(phi.beta * (x - phi.alpha))) /
        (std::pow(x - phi.alpha, 2) * std::pow(phi.gamma + phi.lambda - 1, 2));
}

auto LogisticPsychometricFunction::sweetPoints(Phi phi) -> std::vector<double> {
    std::vector<double> sweetPoints = {
        fminsearch(
            [&](std::vector<double> x) {
                return betaEstimate(phi, x[0]) + (x[0] >= phi.alpha ? 1e10 : 0);
            },
            {phi.alpha - 10})[0],
        fminsearch(
            [&](std::vector<double> x) {
                return betaEstimate(phi, x[0]) + (x[0] <= phi.alpha ? 1e10 : 0);
            },
            {phi.alpha + 10})[0],
        fminsearch(
            [&](std::vector<double> x) { return alphaEstimate(phi, x[0]); },
            {phi.alpha})[0]};
    std::sort(sweetPoints.begin(), sweetPoints.end());
    return sweetPoints;
}

static auto logNormalizedSum(std::vector<double> v) -> std::vector<double> {
    const auto sum = std::accumulate(v.begin(), v.end(), 0.0);
    return transform(v, [sum](double x) { return std::log(x / sum); });
}

UpdatedMaximumLikelihood::UpdatedMaximumLikelihood(
    const PosteriorDistributions &distributions,
    PsychometricFunction &psychometricFunction, PhiComputer &phiComputer,
    TrackSpecifications trackSpecifications)
    : posteriorDistributions{distributions},
      trackSpecifications{trackSpecifications}, _phi{},
      psychometricFunction(psychometricFunction), phiComputer(phiComputer),
      _x(trackSpecifications.startingX),
      lowerBound(trackSpecifications.lowerBound),
      upperBound(trackSpecifications.upperBound),
      down_(trackSpecifications.down), up_(trackSpecifications.up),
      consecutiveDown(0), consecutiveUp(0),
      trackDirection(TrackDirection::undefined), _reversals(0) {
    if (distributions.lambda.prior.empty() ||
        distributions.gamma.prior.empty() || distributions.beta.prior.empty() ||
        distributions.alpha.prior.empty())
        throw std::runtime_error("Empty parameter space passed.");
    if (posteriorDistributions.alpha.space.size() > 1)
        sweetPointIndeces.push_back(2);
    if (posteriorDistributions.beta.space.size() > 1) {
        sweetPointIndeces.push_back(1);
        sweetPointIndeces.push_back(3);
    }
    if (posteriorDistributions.gamma.space.size() > 1)
        sweetPointIndeces.push_back(0);
    if (posteriorDistributions.lambda.space.size() > 1)
        sweetPointIndeces.push_back(4);
    std::sort(sweetPointIndeces.begin(), sweetPointIndeces.end());
    reset();
}

void UpdatedMaximumLikelihood::reset() {
    trials = 0;
    consecutiveDown = 0;
    consecutiveUp = 0;
    trackDirection = TrackDirection::undefined;
    _x = trackSpecifications.startingX;
    _reversals = 0;
    _posterior.clear();
    for (const auto l : posteriorDistributions.lambda.prior)
        for (const auto g : posteriorDistributions.gamma.prior)
            for (const auto b : posteriorDistributions.beta.prior)
                for (const auto a : posteriorDistributions.alpha.prior)
                    _posterior.push_back(a * b * g * l);
    _posterior = logNormalizedSum(std::move(_posterior));
    xCandidateIndex = (_x < (lowerBound + upperBound) / 2)
        ? sweetPointIndeces.front()
        : sweetPointIndeces.back();
}

void UpdatedMaximumLikelihood::addToPosteriorAndShiftByMax(
    const std::vector<double> &result) {
    std::transform(_posterior.begin(), _posterior.end(), result.begin(),
        _posterior.begin(), std::plus<>());
    const auto max = *std::max_element(_posterior.begin(), _posterior.end());
    _posterior =
        transform(std::move(_posterior), [max](double x) { return x - max; });
}

auto UpdatedMaximumLikelihood::evaluatePsychometricFunction()
    -> std::vector<double> {
    std::vector<double> result;
    for (const auto lambda : posteriorDistributions.lambda.space)
        for (const auto gamma : posteriorDistributions.gamma.space)
            for (const auto beta : posteriorDistributions.beta.space)
                for (const auto alpha : posteriorDistributions.alpha.space)
                    result.push_back(
                        psychometricFunction({alpha, beta, gamma, lambda}, _x));
    return result;
}

auto UpdatedMaximumLikelihood::computeSweetPoint(Phi phi)
    -> std::vector<double> {
    auto sweetPoint = psychometricFunction.sweetPoints(phi);
    sweetPoint.insert(sweetPoint.end(), (2 * sweetPoint[2]) - sweetPoint[1]);
    sweetPoint.insert(sweetPoint.begin(), (2 * sweetPoint[0]) - sweetPoint[1]);
    return transform(std::move(sweetPoint), [&](double x) {
        return std::max(std::min(x, upperBound), lowerBound);
    });
}

void UpdatedMaximumLikelihood::down() {
    if (++consecutiveDown == down_) {
        xCandidateIndex = std::max(xCandidateIndex,
                              *std::min_element(sweetPointIndeces.begin(),
                                  sweetPointIndeces.end()) +
                                  1) -
            1;
        consecutiveDown = 0;
        if (trackDirection == TrackDirection::up) {
            ++_reversals;
            _reversalXs.push_back(_x);
        }
        trackDirection = TrackDirection::down;
    }
    consecutiveUp = 0;

    addToPosteriorAndShiftByMax(
        logNormalizedSum(evaluatePsychometricFunction()));
    _phi = phiComputer(*this);
    _sweetPoint = computeSweetPoint(_phi);
    _x = _sweetPoint[xCandidateIndex];
    ++trials;
}

void UpdatedMaximumLikelihood::up() {
    if (++consecutiveUp == up_) {
        xCandidateIndex = std::min(xCandidateIndex + 1,
            *std::max_element(
                sweetPointIndeces.begin(), sweetPointIndeces.end()));
        consecutiveUp = 0;
        if (trackDirection == TrackDirection::down) {
            ++_reversals;
            _reversalXs.push_back(_x);
        }
        trackDirection = TrackDirection::up;
    }
    consecutiveDown = 0;

    addToPosteriorAndShiftByMax(logNormalizedSum(transform(
        evaluatePsychometricFunction(), [](double x) { return 1 - x; })));
    _phi = phiComputer(*this);
    _sweetPoint = computeSweetPoint(_phi);
    _x = _sweetPoint[xCandidateIndex];
    ++trials;
}

auto UpdatedMaximumLikelihood::lambdaSpace(size_t index) const -> const
    double & {
    return posteriorDistributions.lambda
        .space[(index / posteriorDistributions.alpha.space.size() /
                   posteriorDistributions.beta.space.size() /
                   posteriorDistributions.gamma.space.size()) %
            posteriorDistributions.lambda.space.size()];
}

auto UpdatedMaximumLikelihood::gammaSpace(size_t index) const -> const
    double & {
    return posteriorDistributions.gamma
        .space[(index / posteriorDistributions.alpha.space.size() /
                   posteriorDistributions.beta.space.size()) %
            posteriorDistributions.gamma.space.size()];
}

auto UpdatedMaximumLikelihood::betaSpace(size_t index) const -> const double & {
    return posteriorDistributions.beta
        .space[(index / posteriorDistributions.alpha.space.size()) %
            posteriorDistributions.beta.space.size()];
}

auto UpdatedMaximumLikelihood::alphaSpace(size_t index) const -> const
    double & {
    return posteriorDistributions.alpha
        .space[index % posteriorDistributions.alpha.space.size()];
}

auto UpdatedMaximumLikelihood::sweetPoints() const -> std::vector<double> {
    return _sweetPoint;
}

auto UpdatedMaximumLikelihood::phi() const -> std::vector<double> {
    return {_phi.alpha, _phi.beta, _phi.gamma, _phi.lambda};
}

auto UpdatedMaximumLikelihood::x() -> double { return _x; }

auto UpdatedMaximumLikelihood::reversals() -> int { return _reversals; }

auto UpdatedMaximumLikelihood::complete() -> bool {
    return trials >= trackSpecifications.trials;
}

LinearNormPrior::LinearNormPrior(double mu, double sigma)
    : mu{mu}, sigma{sigma} {}

std::vector<double> LinearNormPrior::operator()(
    std::vector<double> space) const {
    return normpdf(std::move(space), mu, sigma);
}

LogNormPrior::LogNormPrior(double mu, double sigma) : mu(mu), sigma(sigma) {}

auto LogNormPrior::operator()(std::vector<double> space) const
    -> std::vector<double> {
    return normpdf(
        transform(std::move(space), [](double x) { return std::log10(x); }), mu,
        sigma);
}

auto FlatPrior::operator()(std::vector<double> space) const
    -> std::vector<double> {
    return std::vector<double>(space.size(), 1);
}

auto MeanPhi::operator()(const UpdatedMaximumLikelihood &uml) const -> Phi {
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

auto exampleLogisticConfiguration() -> PosteriorDistributions {
    return {{linspace(-30, 30, 61), LinearNormPrior(0, 10)},
        {logspace(0.1, 10, 41), LogNormPrior(-0.5, 0.4)},
        {linspace(0.02, 0.2, 11), FlatPrior()},
        {linspace(0.02, 0.2, 11), FlatPrior()}};
}

enum class PriorProbabilityKind : std::uint8_t { LinearNorm, LogNorm, Flat };

enum class ParameterSpace : std::uint8_t { Linear, Log };

struct PriorProbabiltyData {
    double mu;
    double sigma;
};

struct PriorProbabilitySetting {
    PriorProbabiltyData data;
    PriorProbabilityKind kind;
};

struct ParameterSpaceSetting {
    double lower;
    double upper;
    std::size_t N;
    ParameterSpace space;
};

static auto makePriorProbability(PriorProbabilitySetting s)
    -> std::unique_ptr<PriorProbability> {
    switch (s.kind) {
    case PriorProbabilityKind::LinearNorm:
        return std::make_unique<LinearNormPrior>(s.data.mu, s.data.sigma);
    case PriorProbabilityKind::LogNorm:
        return std::make_unique<LogNormPrior>(s.data.mu, s.data.sigma);
    case PriorProbabilityKind::Flat:
        return std::make_unique<FlatPrior>();
    }
}

static auto makeParameterSpace(ParameterSpaceSetting s) -> std::vector<double> {
    switch (s.space) {
    case ParameterSpace::Linear:
        return linspace(s.lower, s.upper, s.N);
    case ParameterSpace::Log:
        return logspace(s.lower, s.upper, s.N);
    }
}

auto UpdatedMaximumLikelihood::Factory::make(const Settings &s)
    -> std::shared_ptr<AdaptiveTrack> {
    TrackSpecifications specs{};
    specs.down = 2;
    specs.up = 1;
    specs.trials = s.trials;
    specs.startingX = s.startingX;
    specs.lowerBound = s.floor;
    specs.upperBound = s.ceiling;
    PriorProbabilitySetting alphaPriorProbability{};
    alphaPriorProbability.kind = PriorProbabilityKind::LinearNorm;
    alphaPriorProbability.data.mu = 0;
    alphaPriorProbability.data.sigma = 10;
    PriorProbabilitySetting betaPriorProbability{};
    betaPriorProbability.kind = PriorProbabilityKind::LogNorm;
    betaPriorProbability.data.mu = -0.5;
    betaPriorProbability.data.sigma = 0.4;
    PriorProbabilitySetting gammaPriorProbability{};
    gammaPriorProbability.kind = PriorProbabilityKind::Flat;
    PriorProbabilitySetting lambdaPriorProbability{};
    lambdaPriorProbability.kind = PriorProbabilityKind::Flat;
    ParameterSpaceSetting alphaSpace{};
    alphaSpace.lower = -30;
    alphaSpace.upper = -30;
    alphaSpace.N = 61;
    alphaSpace.space = ParameterSpace::Linear;
    ParameterSpaceSetting betaSpace{};
    betaSpace.lower = 0.1;
    betaSpace.upper = 10;
    betaSpace.N = 41;
    betaSpace.space = ParameterSpace::Log;
    ParameterSpaceSetting gammaSpace{};
    gammaSpace.lower = 0.02;
    gammaSpace.upper = 0.2;
    gammaSpace.N = 11;
    gammaSpace.space = ParameterSpace::Linear;
    ParameterSpaceSetting lambdaSpace{};
    lambdaSpace.lower = 0.02;
    lambdaSpace.upper = 0.2;
    lambdaSpace.N = 11;
    lambdaSpace.space = ParameterSpace::Linear;
    PosteriorDistributions posteriorDistributions{
        {makeParameterSpace(alphaSpace),
            *makePriorProbability(alphaPriorProbability)},
        {makeParameterSpace(betaSpace),
            *makePriorProbability(betaPriorProbability)},
        {makeParameterSpace(gammaSpace),
            *makePriorProbability(gammaPriorProbability)},
        {makeParameterSpace(lambdaSpace),
            *makePriorProbability(lambdaPriorProbability)}};
    return std::make_shared<UpdatedMaximumLikelihood>(
        posteriorDistributions, pf, pc, specs);
}
}
