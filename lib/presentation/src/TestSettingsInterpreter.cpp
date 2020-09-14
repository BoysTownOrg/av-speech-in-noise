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
    else if (entryName == name(TestSetting::subjectId))
        test.identity.subjectId = entry;
    else if (entryName == name(TestSetting::testerId))
        test.identity.testerId = entry;
    else if (entryName == name(TestSetting::session))
        test.identity.session = entry;
    else if (entryName == name(TestSetting::rmeSetting))
        test.identity.rmeSetting = entry;
    else if (entryName == name(TestSetting::transducer))
        test.identity.transducer = entry;
    else if (entryName == name(TestSetting::condition))
        for (auto c : {Condition::auditoryOnly, Condition::audioVisual})
            if (entry == name(c))
                test.condition = c;
}

static void assign(FixedLevelTest &test, const std::string &entryName,
    const std::string &entry) {
    if (entryName == name(TestSetting::startingSnr))
        test.snr.dB = integer(entry);
    else
        assign(static_cast<Test &>(test), entryName, entry);
}

static void assign(FixedLevelTestWithEachTargetNTimes &test,
    const std::string &entryName, const std::string &entry) {
    if (entryName == name(TestSetting::targetRepetitions))
        test.timesEachTargetIsPlayed = integer(entry);
    else
        assign(static_cast<FixedLevelTest &>(test), entryName, entry);
}

static void assign(Calibration &calibration, const std::string &entryName,
    const std::string &entry) {
    if (entryName == name(TestSetting::masker))
        calibration.fileUrl.path = entry;
    else if (entryName == name(TestSetting::maskerLevel))
        calibration.level.dB_SPL = integer(entry);
}

static void assign(AdaptiveTest &test, const std::string &entryName,
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
    else if (entryName == name(TestSetting::startingSnr))
        test.startingSnr.dB = integer(entry);
    else
        assign(static_cast<Test &>(test), entryName, entry);
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

static void initialize(AdaptiveTest &test, const std::string &contents,
    Method method, const TestIdentity &identity, SNR startingSnr) {
    test.identity = identity;
    test.startingSnr = startingSnr;
    applyToEachEntry(
        [&](auto entryName, auto entry) { assign(test, entryName, entry); },
        contents);
    test.ceilingSnr = SessionControllerImpl::ceilingSnr;
    test.floorSnr = SessionControllerImpl::floorSnr;
    test.trackBumpLimit = SessionControllerImpl::trackBumpLimit;
    test.fullScaleLevel = SessionControllerImpl::fullScaleLevel;
    test.identity.method = name(method);
}

static void initialize(FixedLevelTest &test, const std::string &contents,
    Method method, const TestIdentity &identity, SNR startingSnr,
    const std::function<void(const std::string &, const std::string &)> &f) {
    test.snr = startingSnr;
    test.fullScaleLevel = SessionControllerImpl::fullScaleLevel;
    test.identity = identity;
    test.identity.method = name(method);
    applyToEachEntry(f, contents);
}

static void initialize(FixedLevelTest &test, const std::string &contents,
    Method method, const TestIdentity &identity, SNR startingSnr) {
    initialize(test, contents, method, identity, startingSnr,
        [&](auto entryName, auto entry) { assign(test, entryName, entry); });
}

static void initialize(FixedLevelTestWithEachTargetNTimes &test,
    const std::string &contents, Method method, const TestIdentity &identity,
    SNR startingSnr) {
    initialize(test, contents, method, identity, startingSnr,
        [&](auto entryName, auto entry) { assign(test, entryName, entry); });
}

void TestSettingsInterpreterImpl::initialize(Model &model,
    const std::string &contents, const TestIdentity &identity,
    SNR startingSnr) {
    const auto method{av_speech_in_noise::method(contents)};
    if (method == Method::adaptiveCoordinateResponseMeasureWithDelayedMasker) {
        AdaptiveTest test;
        av_speech_in_noise::initialize(
            test, contents, method, identity, startingSnr);
        model.initializeWithDelayedMasker(test);
    } else if (method ==
        Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker) {
        AdaptiveTest test;
        av_speech_in_noise::initialize(
            test, contents, method, identity, startingSnr);
        model.initializeWithSingleSpeaker(test);
    } else if (method == Method::adaptiveCorrectKeywords) {
        AdaptiveTest test;
        av_speech_in_noise::initialize(
            test, contents, method, identity, startingSnr);
        model.initializeWithCyclicTargets(test);
    } else if (method == Method::adaptiveCorrectKeywordsWithEyeTracking) {
        AdaptiveTest test;
        av_speech_in_noise::initialize(
            test, contents, method, identity, startingSnr);
        model.initializeWithCyclicTargetsAndEyeTracking(test);
    } else if (method ==
            Method::adaptiveCoordinateResponseMeasureWithEyeTracking ||
        method == Method::adaptivePassFailWithEyeTracking) {
        AdaptiveTest test;
        av_speech_in_noise::initialize(
            test, contents, method, identity, startingSnr);
        model.initializeWithEyeTracking(test);
    } else if (method == Method::adaptiveCoordinateResponseMeasure ||
        method == Method::adaptivePassFail) {
        AdaptiveTest test;
        av_speech_in_noise::initialize(
            test, contents, method, identity, startingSnr);
        model.initialize(test);
    } else if (method ==
            Method::
                fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets ||
        method == Method::fixedLevelFreeResponseWithSilentIntervalTargets) {
        FixedLevelTest test;
        av_speech_in_noise::initialize(
            test, contents, method, identity, startingSnr);
        model.initializeWithSilentIntervalTargets(test);
    } else if (method == Method::fixedLevelFreeResponseWithAllTargets) {
        FixedLevelTest test;
        av_speech_in_noise::initialize(
            test, contents, method, identity, startingSnr);
        model.initializeWithAllTargets(test);
    } else if (method == Method::fixedLevelConsonants) {
        FixedLevelTestWithEachTargetNTimes test;
        av_speech_in_noise::initialize(
            test, contents, method, identity, startingSnr);
        model.initialize(test);
    } else if (method ==
        Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking) {
        FixedLevelTest test;
        av_speech_in_noise::initialize(
            test, contents, method, identity, startingSnr);
        model.initializeWithAllTargetsAndEyeTracking(test);
    } else if (method ==
        Method::
            fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking) {
        FixedLevelFixedTrialsTest test;
        av_speech_in_noise::initialize(
            test, contents, method, identity, startingSnr);
        model.initializeWithTargetReplacementAndEyeTracking(test);
    } else {
        FixedLevelFixedTrialsTest test;
        av_speech_in_noise::initialize(
            test, contents, method, identity, startingSnr);
        model.initializeWithTargetReplacement(test);
    }
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
    calibration.fullScaleLevel = SessionControllerImpl::fullScaleLevel;
    return calibration;
}
}
