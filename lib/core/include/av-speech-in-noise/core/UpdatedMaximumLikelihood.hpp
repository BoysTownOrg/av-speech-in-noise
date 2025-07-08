#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_UPDATEDMAXIMUMLIKELIHOODHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_UPDATEDMAXIMUMLIKELIHOODHPP_

#include "IAdaptiveMethod.hpp"
#include <av-speech-in-noise/Interface.hpp>
#include <memory>
#include <vector>

namespace av_speech_in_noise {
class PsychometricFunction {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(PsychometricFunction);
    virtual auto operator()(Phi phi, double x) -> double = 0;
    virtual auto sweetPoints(Phi phi) -> std::vector<double> = 0;
};

class LogisticPsychometricFunction : public PsychometricFunction {
  public:
    auto operator()(Phi phi, double x) -> double override;
    auto sweetPoints(Phi phi) -> std::vector<double> override;
};

class PriorProbability {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(PriorProbability);
    virtual auto operator()(std::vector<double> space) const
        -> std::vector<double> = 0;
};

class UpdatedMaximumLikelihood;

class PhiComputer {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(PhiComputer);
    virtual auto operator()(const UpdatedMaximumLikelihood &uml) const
        -> Phi = 0;
};

struct ParameterDistribution {
    // Order important for construction
    std::vector<double> space;
    std::vector<double> prior;
    ParameterDistribution(
        std::vector<double> parameterSpace, const PriorProbability &probability)
        : space(std::move(parameterSpace)), prior(probability(space)) {}
};

struct PosteriorDistributions {
    ParameterDistribution alpha;
    ParameterDistribution beta;
    ParameterDistribution gamma;
    ParameterDistribution lambda;
};

struct TrackSpecifications {
    int down;
    int up;
    int trials;
    double startingX;
    double upperBound;
    double lowerBound;
};

enum class TrackDirection { up, down, undefined };

class ParameterSpacer {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(ParameterSpacer);
    virtual auto operator()(double lower, double upper, std::size_t N)
        -> std::vector<double> = 0;
};

class LinearSpacer : public ParameterSpacer {
  public:
    auto operator()(double lower, double upper, std::size_t N)
        -> std::vector<double> override;
};

class LogSpacer : public ParameterSpacer {
  public:
    auto operator()(double lower, double upper, std::size_t N)
        -> std::vector<double> override;
};

class LinearNormPrior : public PriorProbability {
    const double mu;
    const double sigma;

  public:
    LinearNormPrior(double mu, double sigma);
    auto operator()(std::vector<double> space) const
        -> std::vector<double> override;
};

class LogNormPrior : public PriorProbability {
    const double mu;
    const double sigma;

  public:
    LogNormPrior(double mu, double sigma);
    auto operator()(std::vector<double> space) const
        -> std::vector<double> override;
};

class FlatPrior : public PriorProbability {
  public:
    auto operator()(std::vector<double> space) const
        -> std::vector<double> override;
};

auto exampleLogisticConfiguration() -> PosteriorDistributions;

class MeanPhi : public PhiComputer {
  public:
    auto operator()(const UpdatedMaximumLikelihood &) const -> Phi override;
};

class UpdatedMaximumLikelihood : public Track {
    const PosteriorDistributions posteriorDistributions;
    TrackSpecifications trackSpecifications;
    std::vector<double> _posterior;
    std::vector<std::size_t> sweetPointIndeces;
    std::vector<double> _sweetPoint;
    std::vector<double> _reversalXs;
    Phi _phi;
    // Order important for construction
    PsychometricFunction &psychometricFunction;
    PhiComputer &phiComputer;
    double _x;
    const double lowerBound;
    const double upperBound;
    const int down_;
    const int up_;
    int consecutiveDown;
    int consecutiveUp;
    std::size_t xCandidateIndex;
    TrackDirection trackDirection;
    int trials;
    int _reversals;

  public:
    UpdatedMaximumLikelihood(const PosteriorDistributions &distributions,
        PsychometricFunction &psychometricFunction, PhiComputer &phiComputer,
        TrackSpecifications trackSpecifications);
    void down() override;
    void up() override;
    void reset() override;
    auto x() -> double override;
    auto reversals() -> int override;
    auto sweetPoints() const -> std::vector<double>;
    auto phi() const -> std::vector<double>;
    auto posterior() const -> const std::vector<double> & {
        return _posterior;
    };
    auto alphaSpace(size_t index) const -> const double &;
    auto betaSpace(size_t index) const -> const double &;
    auto gammaSpace(size_t index) const -> const double &;
    auto lambdaSpace(size_t index) const -> const double &;
    auto reversalXs() const -> std::vector<double>;
    auto complete() -> bool override;
    auto result() -> std::variant<Threshold, Phi> override { return _phi; }

    class Factory : public Track::Factory {
      public:
        auto make(const Settings &s) -> std::shared_ptr<Track> override {
            TrackSpecifications specs{};
            specs.down = 2;
            specs.up = 1;
            specs.trials = s.trials;
            specs.startingX = s.startingX;
            specs.lowerBound = s.floor;
            specs.upperBound = s.ceiling;
            return std::make_shared<UpdatedMaximumLikelihood>(
                exampleLogisticConfiguration(), pf, pc, specs);
        }

      private:
        LogisticPsychometricFunction pf;
        MeanPhi pc;
    };

  private:
    void addToPosteriorAndShiftByMax(const std::vector<double> &result);
    auto computeSweetPoint(Phi phi) -> std::vector<double>;
    auto evaluatePsychometricFunction() -> std::vector<double>;
};
}

#endif
