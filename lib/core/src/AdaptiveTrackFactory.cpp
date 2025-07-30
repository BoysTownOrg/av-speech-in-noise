#include "AdaptiveTrackFactory.hpp"

#include <sstream>

namespace av_speech_in_noise {
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
    registry.subscribe(*this, "up");
    registry.subscribe(*this, "down");
    registry.subscribe(*this, "reversals per step size");
    registry.subscribe(*this, "step sizes (dB)");
    registry.subscribe(*this, "threshold");
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

static auto vectorOfInts(const std::string &s) -> std::vector<int> {
    std::vector<int> v;
    std::stringstream stream{s};
    int x{};
    while (stream >> x)
        v.push_back(x);
    return v;
}

static auto trackingRule(LevittSettings &s) -> TrackingRule & {
    return s.trackingRule;
}

static void resizeTrackingRuleEnough(
    LevittSettings &s, const std::vector<int> &v) {
    if (trackingRule(s).size() < v.size())
        trackingRule(s).resize(v.size());
}

static auto up(TrackingSequence &sequence) -> int & { return sequence.up; }

static auto down(TrackingSequence &sequence) -> int & { return sequence.down; }

static auto runCount(TrackingSequence &sequence) -> int & {
    return sequence.runCount;
}

static auto stepSize(TrackingSequence &sequence) -> int & {
    return sequence.stepSize;
}

static void assignToEachElementOfTrackingRule(LevittSettings &s,
    const std::function<int &(TrackingSequence &)> &elementRef,
    const std::string &entry) {
    auto v{vectorOfInts(entry)};
    resizeTrackingRuleEnough(s, v);
    for (std::size_t i{0}; i < v.size(); ++i)
        elementRef(trackingRule(s).at(i)) = v.at(i);
}

void AdaptiveTrackFactory::configure(
    const std::string &key, const std::string &value) {
    if (key == "threshold")
        levittSettings.thresholdReversals = integer(value);
    else if (key == "up")
        assignToEachElementOfTrackingRule(levittSettings, up, value);
    else if (key == "down")
        assignToEachElementOfTrackingRule(levittSettings, down, value);
    else if (key == "reversals per step size")
        assignToEachElementOfTrackingRule(levittSettings, runCount, value);
    else if (key == "step sizes (dB)")
        assignToEachElementOfTrackingRule(levittSettings, stepSize, value);
    else if (key == "alpha space")
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
        insertLabeledLine(stream, "up", up);
        insertLabeledLine(stream, "down", down);
        insertLabeledLine(stream, "reversals per step size", runCounts);
        insertLabeledLine(stream, "step sizes (dB)", stepSizes);
        insertLabeledLine(
            stream, "threshold reversals", levittSettings.thresholdReversals);
    }
}

auto AdaptiveTrackFactory::make(const AdaptiveTrack::Settings &s)
    -> std::shared_ptr<AdaptiveTrack> {
    if (uml)
        return std::make_shared<UpdatedMaximumLikelihood>(umlSettings, s);
    return std::make_shared<LevittTrack>(levittSettings, s);
}
}
