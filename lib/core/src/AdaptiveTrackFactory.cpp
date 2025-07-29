#include "AdaptiveTrackFactory.hpp"

#include <sstream>

namespace av_speech_in_noise {
auto make(const UmlSettings &specific, const AdaptiveTrack::Settings &s)
    -> std::shared_ptr<AdaptiveTrack> {
    return std::make_shared<UpdatedMaximumLikelihood>(specific, s);
}

auto make(const LevittSettings &specific, const AdaptiveTrack::Settings &s)
    -> std::shared_ptr<AdaptiveTrack> {
    return std::make_shared<LevittTrack>(specific, s);
}

AdaptiveTrackFactory::AdaptiveTrackFactory(ConfigurationRegistry &registry) {
    registry.subscribe(*this, "alpha space");
    registry.subscribe(*this, "alpha prior");
    registry.subscribe(*this, "beta space");
    registry.subscribe(*this, "beta prior");
    registry.subscribe(*this, "gamma space");
    registry.subscribe(*this, "gamma prior");
    registry.subscribe(*this, "lambda space");
    registry.subscribe(*this, "lambda prior");
    registry.subscribe(*this, "trials");
}

static void initializeParameterSpace(
    PhiParameterSetting &s, const std::string &entry) {
    std::stringstream stream{entry};
    std::string kind;
    stream >> kind;
    if (kind == "linear")
        s.space.space = ParameterSpace::Linear;
    else if (kind == "log")
        s.space.space = ParameterSpace::Log;
    stream >> s.space.lower;
    stream >> s.space.upper;
    stream >> s.space.N;
}

static void initializeParameterPrior(
    PhiParameterSetting &s, const std::string &entry) {
    std::stringstream stream{entry};
    std::string kind;
    stream >> kind;
    if (kind == "linearnorm")
        s.priorProbability.kind = PriorProbabilityKind::LinearNorm;
    else if (kind == "lognorm")
        s.priorProbability.kind = PriorProbabilityKind::LogNorm;
    else if (kind == "flat")
        s.priorProbability.kind = PriorProbabilityKind::Flat;
    stream >> s.priorProbability.mu;
    stream >> s.priorProbability.sigma;
}

void AdaptiveTrackFactory::configure(
    const std::string &key, const std::string &value) {
    if (key == "alpha space")
        initializeParameterSpace(umlSettings.alpha, value);
    else if (key == "alpha prior")
        initializeParameterPrior(umlSettings.alpha, value);
    else if (key == "beta space")
        initializeParameterSpace(umlSettings.beta, value);
    else if (key == "beta prior")
        initializeParameterPrior(umlSettings.beta, value);
    else if (key == "gamma space")
        initializeParameterSpace(umlSettings.gamma, value);
    else if (key == "gamma prior")
        initializeParameterPrior(umlSettings.gamma, value);
    else if (key == "lambda space")
        initializeParameterSpace(umlSettings.lambda, value);
    else if (key == "lambda prior")
        initializeParameterPrior(umlSettings.lambda, value);
    else if (key == "trials")
        umlSettings.trials = integer(value);
    else if (key == "uml")
        uml = true;
}

static auto operator<<(std::ostream &os, ParameterSpaceSetting s)
    -> std::ostream & {
    return os << (s.space == ParameterSpace::Linear ? "linear" : "log") << ' '
              << s.lower << ' ' << s.upper << ' ' << s.N;
}

static constexpr auto name(PriorProbabilityKind k) -> const char * {
    switch (k) {
    case PriorProbabilityKind::LinearNorm:
        return "linearnorm";
    case PriorProbabilityKind::LogNorm:
        return "lognorm";
    case PriorProbabilityKind::Flat:
        return "flat";
    }
}

static auto operator<<(std::ostream &os, PriorProbabilitySetting s)
    -> std::ostream & {
    os << name(s.kind);
    if (s.kind != PriorProbabilityKind::Flat)
        os << ' ' << s.mu << ' ' << s.sigma;
    return os;
}

void AdaptiveTrackFactory::write(std::ostream &stream) {
    if (uml) {
        insertLabeledLine(stream, "alpha space", umlSettings.alpha.space);
        insertLabeledLine(
            stream, "alpha prior", umlSettings.alpha.priorProbability);
        insertLabeledLine(stream, "beta space", umlSettings.beta.space);
        insertLabeledLine(
            stream, "beta prior", umlSettings.beta.priorProbability);
        insertLabeledLine(stream, "gamma space", umlSettings.gamma.space);
        insertLabeledLine(
            stream, "gamma prior", umlSettings.gamma.priorProbability);
        insertLabeledLine(stream, "lambda space", umlSettings.lambda.space);
        insertLabeledLine(
            stream, "lambda prior", umlSettings.lambda.priorProbability);
        insertLabeledLine(stream, "up", umlSettings.up);
        insertLabeledLine(stream, "down", umlSettings.down);
        insertLabeledLine(stream, "trials", umlSettings.trials);
    } else {
        std::vector<int> up;
        std::vector<int> down;
        std::vector<int> runCounts;
        std::vector<int> stepSizes;
        for (auto sequence : levittSettings.trackingRule) {
            up.push_back(sequence.up);
            down.push_back(sequence.down);
            runCounts.push_back(sequence.runCount);
            stepSizes.push_back(sequence.stepSize);
        }
        // insertLabeledLine(stream, "up", up);
        // insertLabeledLine(stream, "down", down);
        // insertLabeledLine(stream, "reversals per step size", runCounts);
        // insertLabeledLine(stream, "step sizes (dB)", stepSizes);
    }
}

auto AdaptiveTrackFactory::make(
    const std::variant<UmlSettings, LevittSettings> &specific,
    const AdaptiveTrack::Settings &s) -> std::shared_ptr<AdaptiveTrack> {
    return std::visit(
        [&s](const auto &specific) {
            return av_speech_in_noise::make(specific, s);
        },
        specific);
}
}
