#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_UPDATEDMAXIMUMLIKELIHOODHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_UPDATEDMAXIMUMLIKELIHOODHPP_

#include <av-speech-in-noise/Interface.hpp>
#include <vector>

namespace av_speech_in_noise {
struct Phi {
    double alpha;
    double beta;
    double gamma;
    double lambda;
};

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
    double startingX;
    double upperBound;
    double lowerBound;
};

enum class TrackDirection { up, down, undefined };

class UpdatedMaximumLikelihood {
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
    const int down;
    const int up;
    int consecutiveDown;
    int consecutiveUp;
    std::size_t xCandidateIndex;
    TrackDirection trackDirection;
    int _reversals;

  public:
    UpdatedMaximumLikelihood(const PosteriorDistributions &distributions,
        PsychometricFunction &psychometricFunction, PhiComputer &phiComputer,
        TrackSpecifications trackSpecifications);
    void pushDown();
    void pushUp();
    void reset();
    double x() const;
    int reversals() const;
    std::vector<double> sweetPoints() const;
    std::vector<double> phi() const;
    const std::vector<double> &posterior() const { return _posterior; };
    const double &alphaSpace(size_t index) const;
    const double &betaSpace(size_t index) const;
    const double &gammaSpace(size_t index) const;
    const double &lambdaSpace(size_t index) const;
    std::vector<double> reversalXs() const;

  private:
    void addToPosteriorAndShiftByMax(const std::vector<double> &result);
    std::vector<double> computeSweetPoint(Phi phi);
    std::vector<double> evaluatePsychometricFunction();
};
}

#endif
