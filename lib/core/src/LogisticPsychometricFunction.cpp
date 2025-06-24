#include "LogisticPsychometricFunction.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <stdexcept>

namespace av_speech_in_noise {
using std::plus;

template <class UnaryOperator>
static auto transform(std::vector<double> v, UnaryOperator f)
    -> std::vector<double> {
    std::transform(v.begin(), v.end(), v.begin(), f);
    return v;
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

auto LogisticSweetPoints::operator()(Phi phi) -> std::vector<double> {
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
    PosteriorDistributions distributions,
    PsychometricFunction &psychometricFunction, SweetPoints &sweetPointComputer,
    PhiComputer &phiComputer, TrackSpecifications trackSpecifications)
    : _alphaSpace(distributions.alpha.space),
      _betaSpace(distributions.beta.space),
      _gammaSpace(distributions.gamma.space),
      _lambdaSpace(distributions.lambda.space), _phi({}),
      psychometricFunction(psychometricFunction),
      sweetPointComputer(sweetPointComputer), phiComputer(phiComputer),
      _x(trackSpecifications.startingX),
      lowerBound(trackSpecifications.lowerBound),
      upperBound(trackSpecifications.upperBound),
      down(trackSpecifications.down), up(trackSpecifications.up),
      consecutiveDown(0), consecutiveUp(0),
      trackDirection(TrackDirection::undefined), _reversals(0), _trials(0) {
    for (const auto l : distributions.lambda.prior)
        for (const auto g : distributions.gamma.prior)
            for (const auto b : distributions.beta.prior)
                for (const auto a : distributions.alpha.prior)
                    _posterior.push_back(a * b * g * l);
    if (_posterior.size() == 0)
        throw std::runtime_error("Empty parameter space passed.");
    _posterior = logNormalizedSum(std::move(_posterior));
    if (_alphaSpace.size() > 1)
        sweetPointIndeces.push_back(2);
    if (_betaSpace.size() > 1) {
        sweetPointIndeces.push_back(1);
        sweetPointIndeces.push_back(3);
    }
    if (_gammaSpace.size() > 1)
        sweetPointIndeces.push_back(0);
    if (_lambdaSpace.size() > 1)
        sweetPointIndeces.push_back(4);
    std::sort(sweetPointIndeces.begin(), sweetPointIndeces.end());
    xCandidateIndex = (_x < (lowerBound + upperBound) / 2)
        ? sweetPointIndeces.front()
        : sweetPointIndeces.back();
}

void UpdatedMaximumLikelihood::addToPosteriorAndShiftByMax(
    const std::vector<double> &result) {
    std::transform(_posterior.begin(), _posterior.end(), result.begin(),
        _posterior.begin(), plus<>());
    const auto max = *std::max_element(_posterior.begin(), _posterior.end());
    _posterior =
        transform(std::move(_posterior), [max](double x) { return x - max; });
}

auto UpdatedMaximumLikelihood::evaluatePsychometricFunction()
    -> std::vector<double> {
    std::vector<double> result;
    for (const auto lambda : _lambdaSpace)
        for (const auto gamma : _gammaSpace)
            for (const auto beta : _betaSpace)
                for (const auto alpha : _alphaSpace)
                    result.push_back(
                        psychometricFunction({alpha, beta, gamma, lambda}, _x));
    return result;
}

auto UpdatedMaximumLikelihood::computeSweetPoint(Phi phi)
    -> std::vector<double> {
    auto sweetPoint = sweetPointComputer(phi);
    sweetPoint.insert(sweetPoint.end(), (2 * sweetPoint[2]) - sweetPoint[1]);
    sweetPoint.insert(sweetPoint.begin(), (2 * sweetPoint[0]) - sweetPoint[1]);
    return transform(std::move(sweetPoint), [&](double x) {
        return std::max(std::min(x, upperBound), lowerBound);
    });
}

void UpdatedMaximumLikelihood::pushDown() {
    if (++consecutiveDown == down) {
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
    ++_trials;
}

void UpdatedMaximumLikelihood::pushUp() {
    if (++consecutiveUp == up) {
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
    ++_trials;
}

auto UpdatedMaximumLikelihood::lambdaSpace(size_t index) const -> const
    double & {
    return _lambdaSpace[(index / _alphaSpace.size() / _betaSpace.size() /
                            _gammaSpace.size()) %
        _lambdaSpace.size()];
}

auto UpdatedMaximumLikelihood::gammaSpace(size_t index) const -> const
    double & {
    return _gammaSpace[(index / _alphaSpace.size() / _betaSpace.size()) %
        _gammaSpace.size()];
}

auto UpdatedMaximumLikelihood::betaSpace(size_t index) const -> const double & {
    return _betaSpace[(index / _alphaSpace.size()) % _betaSpace.size()];
}

auto UpdatedMaximumLikelihood::alphaSpace(size_t index) const -> const
    double & {
    return _alphaSpace[index % _alphaSpace.size()];
}

auto UpdatedMaximumLikelihood::x() const -> double { return _x; }

auto UpdatedMaximumLikelihood::reversals() const -> int { return _reversals; }

auto UpdatedMaximumLikelihood::sweetPoints() const -> std::vector<double> {
    return _sweetPoint;
}

auto UpdatedMaximumLikelihood::phi() const -> std::vector<double> {
    return {_phi.alpha, _phi.beta, _phi.gamma, _phi.lambda};
}

auto UpdatedMaximumLikelihood::reversalXs() const -> std::vector<double> {
    return _reversalXs;
}

auto UpdatedMaximumLikelihood::trials() const -> long { return _trials; }
}
