#include "TestSettingsInterpreter.hpp"
#include <av-speech-in-noise/name.hpp>
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
    int x{};
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

static auto size(const std::string &s) -> gsl::index { return s.size(); }

static auto entry(const std::string &line) -> std::string {
    return entryDelimiter(line) + 2 > size(line)
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
        test.targetsUrl.path = entry;
    else if (entryName == name(TestSetting::masker))
        test.maskerFileUrl.path = entry;
    else if (entryName == name(TestSetting::maskerLevel))
        test.maskerLevel.dB_SPL = integer(entry);
    else if (entryName == name(TestSetting::condition))
        for (auto c : {Condition::auditoryOnly, Condition::audioVisual})
            if (entry == name(c))
                test.condition = c;
}

static void assign(Calibration &calibration, const std::string &entryName,
    const std::string &entry) {
    if (entryName == name(TestSetting::masker))
        calibration.fileUrl.path = entry;
    else if (entryName == name(TestSetting::maskerLevel))
        calibration.level.dB_SPL = integer(entry);
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
    else if (entryName == name(TestSetting::thresholdReversals))
        test.thresholdReversals = integer(entry);
}

static auto name(const std::string &contents) -> std::string {
    std::stringstream stream{contents};
    for (std::string line; std::getline(stream, line);)
        if (entryName(line) == name(TestSetting::method))
            return entry(line);
    return name(Method::unknown);
}

static auto method(const std::string &s) -> Method {
    for (auto m :
        {Method::adaptivePassFail, Method::adaptivePassFailWithEyeTracking,
            Method::adaptiveCorrectKeywords,
            Method::adaptiveCorrectKeywordsWithEyeTracking,
            Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker,
            Method::adaptiveCoordinateResponseMeasureWithDelayedMasker,
            Method::fixedLevelFreeResponseWithTargetReplacement,
            Method::fixedLevelFreeResponseWithSilentIntervalTargets,
            Method::fixedLevelFreeResponseWithAllTargets,
            Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking,
            Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement,
            Method::
                fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking,
            Method::
                fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets,
            Method::adaptiveCoordinateResponseMeasure,
            Method::fixedLevelConsonants,
            Method::adaptiveCoordinateResponseMeasureWithEyeTracking})
        if (name(s) == name(m))
            return m;
    return Method::unknown;
}

static auto adaptive(const std::string &contents) -> bool {
    const auto method_{av_speech_in_noise::method(contents)};
    return method_ == Method::adaptivePassFail ||
        method_ == Method::adaptivePassFailWithEyeTracking ||
        method_ == Method::adaptiveCorrectKeywords ||
        method_ == Method::adaptiveCorrectKeywordsWithEyeTracking ||
        method_ == Method::adaptiveCoordinateResponseMeasureWithDelayedMasker ||
        method_ == Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker ||
        method_ == Method::adaptiveCoordinateResponseMeasure ||
        method_ == Method::adaptiveCoordinateResponseMeasureWithEyeTracking;
}

static void initializeAdaptiveTest(Model &model, const std::string &contents,
    const TestIdentity &identity, SNR startingSnr) {
    AdaptiveTest test;
    applyToEachEntry(
        [&](auto entryName, auto entry) {
            assignAdaptive(test, entryName, entry);
        },
        contents);
    applyToEachEntry(
        [&](auto entryName, auto entry) { assign(test, entryName, entry); },
        contents);
    test.startingSnr = startingSnr;
    test.ceilingSnr = Presenter::ceilingSnr;
    test.floorSnr = Presenter::floorSnr;
    test.trackBumpLimit = Presenter::trackBumpLimit;
    test.fullScaleLevel = Presenter::fullScaleLevel;
    test.identity = identity;
    const auto method_{av_speech_in_noise::method(contents)};
    test.identity.method = name(method_);
    if (method_ == Method::adaptiveCoordinateResponseMeasureWithDelayedMasker)
        model.initializeWithDelayedMasker(test);
    else if (method_ ==
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker)
        model.initializeWithSingleSpeaker(test);
    else if (method_ == Method::adaptiveCorrectKeywords)
        model.initializeWithCyclicTargets(test);
    else if (method_ == Method::adaptiveCorrectKeywordsWithEyeTracking)
        model.initializeWithCyclicTargetsAndEyeTracking(test);
    else if (method_ ==
            Method::adaptiveCoordinateResponseMeasureWithEyeTracking ||
        method_ == Method::adaptivePassFailWithEyeTracking)
        model.initializeWithEyeTracking(test);
    else
        model.initialize(test);
}

static void initialize(FixedLevelTest &test, const std::string &contents,
    Method method, const TestIdentity &identity, SNR startingSnr) {
    applyToEachEntry(
        [&](auto entryName, auto entry) { assign(test, entryName, entry); },
        contents);
    test.snr = startingSnr;
    test.fullScaleLevel = Presenter::fullScaleLevel;
    test.identity = identity;
    test.identity.method = name(method);
}

static void initializeFixedLevelTest(Model &model, const std::string &contents,
    const TestIdentity &identity, SNR startingSnr) {
    const auto method_{av_speech_in_noise::method(contents)};
    if (method_ ==
            Method::
                fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets ||
        method_ == Method::fixedLevelFreeResponseWithSilentIntervalTargets) {
        FixedLevelTest test;
        initialize(test, contents, method_, identity, startingSnr);
        model.initializeWithSilentIntervalTargets(test);
    } else if (method_ == Method::fixedLevelFreeResponseWithAllTargets) {
        FixedLevelTest test;
        initialize(test, contents, method_, identity, startingSnr);
        model.initializeWithAllTargets(test);
    } else if (method_ == Method::fixedLevelConsonants) {
        FixedLevelTestWithEachTargetNTimes test;
        initialize(test, contents, method_, identity, startingSnr);
        model.initialize(test);
    } else if (method_ ==
        Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking) {
        FixedLevelTest test;
        initialize(test, contents, method_, identity, startingSnr);
        model.initializeWithAllTargetsAndEyeTracking(test);
    } else if (method_ ==
        Method::
            fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking) {
        FixedLevelFixedTrialsTest test;
        initialize(test, contents, method_, identity, startingSnr);
        model.initializeWithTargetReplacementAndEyeTracking(test);
    } else {
        FixedLevelFixedTrialsTest test;
        initialize(test, contents, method_, identity, startingSnr);
        model.initializeWithTargetReplacement(test);
    }
}

void TestSettingsInterpreterImpl::initialize(Model &model,
    const std::string &contents, const TestIdentity &identity,
    SNR startingSnr) {
    if (adaptive(contents))
        initializeAdaptiveTest(model, contents, identity, startingSnr);
    else
        initializeFixedLevelTest(model, contents, identity, startingSnr);
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
    calibration.fullScaleLevel = Presenter::fullScaleLevel;
    return calibration;
}
}
