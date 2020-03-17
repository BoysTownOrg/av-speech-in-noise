#include "TestSettingsInterpreter.hpp"
#include <gsl/gsl>
#include <sstream>
#include <functional>

namespace av_speech_in_noise {
static auto entryDelimiter(const std::string &s) -> gsl::index {
    return s.find(':');
}

static auto vectorOfInts(const std::string &s) -> std::vector<int> {
    std::vector<int> v;
    std::stringstream stream{s};
    int x;
    while (stream >> x)
        v.push_back(x);
    return v;
}

static auto trackingRule(AdaptiveTest &test) -> TrackingRule & {
    return test.trackingRule;
}

static void resizeTrackingRuleEnough(
    AdaptiveTest &test, const std::vector<int> &v) {
    if (trackingRule(test).size() < v.size())
        trackingRule(test).resize(v.size());
}

static void applyToUp(TrackingSequence &sequence, int x) { sequence.up = x; }

static void applyToDown(TrackingSequence &sequence, int x) {
    sequence.down = x;
}

static void applyToRunCount(TrackingSequence &sequence, int x) {
    sequence.runCount = x;
}

static void applyToStepSize(TrackingSequence &sequence, int x) {
    sequence.stepSize = x;
}

static void applyToEachTrackingRule(AdaptiveTest &test,
    const std::function<void(TrackingSequence &, int)> &f,
    const std::string &entry) {
    auto v{vectorOfInts(entry)};
    resizeTrackingRuleEnough(test, v);
    for (gsl::index i{0}; i < v.size(); ++i)
        f(trackingRule(test).at(i), v.at(i));
}

static auto entryName(const std::string &line) -> std::string {
    return line.substr(0, entryDelimiter(line));
}

static auto entry(const std::string &line) -> std::string {
    return entryDelimiter(line) + 2 > line.size()
        ? ""
        : line.substr(entryDelimiter(line) + 2);
}

static void applyToEachEntry(
    const std::function<void(const std::string &, const std::string &)> &f,
    const std::string &contents) {
    std::stringstream stream{contents};
    for (std::string line; std::getline(stream, line);)
        f(entryName(line), entry(line));
}

static auto integer(const std::string &s) -> int {
    try {
        return std::stoi(s);
    } catch (const std::invalid_argument &) {
        return 0;
    }
}

static void assign(
    Test &test, const std::string &entryName, const std::string &entry) {
    if (entryName == name(TestSetting::targets))
        test.targetListDirectory = entry;
    else if (entryName == name(TestSetting::masker))
        test.maskerFilePath = entry;
    else if (entryName == name(TestSetting::maskerLevel))
        test.maskerLevel_dB_SPL = integer(entry);
    else if (entryName == name(TestSetting::condition))
        for (auto c : {Condition::auditoryOnly, Condition::audioVisual})
            if (entry == conditionName(c))
                test.condition = c;
}

static void assign(Calibration &calibration, const std::string &entryName,
    const std::string &entry) {
    if (entryName == name(TestSetting::masker))
        calibration.filePath = entry;
    else if (entryName == name(TestSetting::maskerLevel))
        calibration.level_dB_SPL = integer(entry);
}

static void assignAdaptive(AdaptiveTest &test, const std::string &entryName,
    const std::string &entry) {
    if (entryName == name(TestSetting::up))
        applyToEachTrackingRule(test, applyToUp, entry);
    else if (entryName == name(TestSetting::down))
        applyToEachTrackingRule(test, applyToDown, entry);
    else if (entryName == name(TestSetting::reversalsPerStepSize))
        applyToEachTrackingRule(test, applyToRunCount, entry);
    else if (entryName == name(TestSetting::stepSizes))
        applyToEachTrackingRule(test, applyToStepSize, entry);
    else if (entryName == name(TestSetting::startingSnr))
        test.startingSnr_dB = integer(entry);
    else if (entryName == name(TestSetting::thresholdReversals))
        test.thresholdReversals = integer(entry);
}

static void assignFixedLevel(FixedLevelTest &test, const std::string &entryName,
    const std::string &entry) {
    if (entryName == name(TestSetting::startingSnr))
        test.snr_dB = integer(entry);
}

static auto methodName(const std::string &contents) -> std::string {
    std::stringstream stream{contents};
    for (std::string line; std::getline(stream, line);)
        if (entryName(line) == name(TestSetting::method))
            return entry(line);
    return methodName(Method::unknown);
}

static auto method(const std::string &s) -> Method {
    for (auto m : {Method::adaptivePassFail, Method::adaptiveCorrectKeywords,
             Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker,
             Method::adaptiveCoordinateResponseMeasureWithDelayedMasker,
             Method::fixedLevelFreeResponseWithTargetReplacement,
             Method::fixedLevelFreeResponseWithSilentIntervalTargets,
             Method::fixedLevelFreeResponseWithAllTargets,
             Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement,
             Method::
                 fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets,
             Method::defaultAdaptiveCoordinateResponseMeasure})
        if (methodName(s) == methodName(m))
            return m;
    return Method::unknown;
}

static auto adaptive(const std::string &contents) -> bool {
    auto method_{av_speech_in_noise::method(contents)};
    return method_ == Method::adaptivePassFail ||
        method_ == Method::adaptiveCorrectKeywords ||
        method_ == Method::adaptiveCoordinateResponseMeasureWithDelayedMasker ||
        method_ == Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker ||
        method_ == Method::defaultAdaptiveCoordinateResponseMeasure;
}

static void initializeAdaptiveTest(
    Model &model, const std::string &contents, const TestIdentity &identity) {
    AdaptiveTest test;
    applyToEachEntry(
        [&](auto entryName, auto entry) {
            assignAdaptive(test, entryName, entry);
        },
        contents);
    applyToEachEntry(
        [&](auto entryName, auto entry) { assign(test, entryName, entry); },
        contents);
    test.ceilingSnr_dB = Presenter::ceilingSnr_dB;
    test.floorSnr_dB = Presenter::floorSnr_dB;
    test.trackBumpLimit = Presenter::trackBumpLimit;
    test.fullScaleLevel_dB_SPL = Presenter::fullScaleLevel_dB_SPL;
    test.identity = identity;
    auto method_{av_speech_in_noise::method(contents)};
    test.identity.method = methodName(method_);
    if (method_ == Method::adaptiveCoordinateResponseMeasureWithDelayedMasker)
        model.initializeWithDelayedMasker(test);
    else if (method_ ==
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker)
        model.initializeWithSingleSpeaker(test);
    else if (method_ == Method::adaptiveCorrectKeywords)
        model.initializeWithCyclicTargets(test);
    else
        model.initialize(test);
}

static void initializeFixedLevelTest(
    Model &model, const std::string &contents, const TestIdentity &identity) {
    FixedLevelTest test;
    applyToEachEntry(
        [&](auto entryName, auto entry) {
            assignFixedLevel(test, entryName, entry);
        },
        contents);
    applyToEachEntry(
        [&](auto entryName, auto entry) { assign(test, entryName, entry); },
        contents);
    test.fullScaleLevel_dB_SPL = Presenter::fullScaleLevel_dB_SPL;
    test.identity = identity;
    auto method_{av_speech_in_noise::method(contents)};
    test.identity.method = methodName(method_);
    if (method_ ==
            Method::
                fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets ||
        method_ == Method::fixedLevelFreeResponseWithSilentIntervalTargets)
        model.initializeWithSilentIntervalTargets(test);
    else if (method_ == Method::fixedLevelFreeResponseWithAllTargets)
        model.initializeWithAllTargets(test);
    else
        model.initializeWithTargetReplacement(test);
}

void TestSettingsInterpreterImpl::initialize(
    Model &model, const std::string &contents, const TestIdentity &identity) {
    if (adaptive(contents))
        initializeAdaptiveTest(model, contents, identity);
    else
        initializeFixedLevelTest(model, contents, identity);
}

auto TestSettingsInterpreterImpl::method(const std::string &s) -> Method {
    return av_speech_in_noise::method(s);
}

auto TestSettingsInterpreterImpl::calibration(const std::string &contents)
    -> Calibration {
    Calibration calibration;
    applyToEachEntry([&](auto entryName,
                         auto entry) { assign(calibration, entryName, entry); },
        contents);
    calibration.fullScaleLevel_dB_SPL = Presenter::fullScaleLevel_dB_SPL;
    return calibration;
}
}
