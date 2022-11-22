#include "TestSettingsInterpreter.hpp"
#include <av-speech-in-noise/Model.hpp>

#include <gsl/gsl>

#include <map>
#include <sstream>
#include <functional>
#include <string>
#include <utility>

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
    else if (entryName == name(TestSetting::meta))
        test.identity.meta = entry;
    else if (entryName == name(TestSetting::relativeOutputPath))
        test.identity.relativeOutputUrl.path = entry;
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

static auto methodName(const std::string &contents) -> std::string {
    std::stringstream stream{contents};
    for (std::string line; std::getline(stream, line);)
        if (entryName(line) == name(TestSetting::method))
            return entry(line);
    return name(Method::unknown);
}

static auto method(const std::string &contents) -> Method {
    static std::map<std::string, Method> methods{
        {name(Method::adaptivePassFail), Method::adaptivePassFail},
        {name(Method::adaptivePassFailWithEyeTracking),
            Method::adaptivePassFailWithEyeTracking},
        {name(Method::adaptiveCorrectKeywords),
            Method::adaptiveCorrectKeywords},
        {name(Method::adaptiveCorrectKeywordsWithEyeTracking),
            Method::adaptiveCorrectKeywordsWithEyeTracking},
        {name(Method::adaptiveCoordinateResponseMeasure),
            Method::adaptiveCoordinateResponseMeasure},
        {name(Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker),
            Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker},
        {name(Method::adaptiveCoordinateResponseMeasureWithDelayedMasker),
            Method::adaptiveCoordinateResponseMeasureWithDelayedMasker},
        {name(Method::fixedLevelFreeResponseWithTargetReplacement),
            Method::fixedLevelFreeResponseWithTargetReplacement},
        {name(Method::fixedLevelFreeResponseWithSilentIntervalTargets),
            Method::fixedLevelFreeResponseWithSilentIntervalTargets},
        {name(Method::fixedLevelFreeResponseWithAllTargets),
            Method::fixedLevelFreeResponseWithAllTargets},
        {name(Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking),
            Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking},
        {name(Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording),
            Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording},
        {name(Method::
                 fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording),
            Method::
                fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording},
        {name(Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement),
            Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement},
        {name(Method::
                 fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking),
            Method::
                fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking},
        {name(Method::
                 fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets),
            Method::
                fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets},
        {name(Method::fixedLevelConsonants), Method::fixedLevelConsonants},
        {name(Method::fixedLevelChooseKeywordsWithAllTargets),
            Method::fixedLevelChooseKeywordsWithAllTargets},
        {name(Method::fixedLevelSyllablesWithAllTargets),
            Method::fixedLevelSyllablesWithAllTargets},
        {name(Method::adaptiveCoordinateResponseMeasureWithEyeTracking),
            Method::adaptiveCoordinateResponseMeasureWithEyeTracking}};
    const auto name{methodName(contents)};
    return methods.count(name) != 0 ? methods.at(name) : Method::unknown;
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

static void initialize(Method method, const std::string &contents,
    const TestIdentity &identity, SNR startingSnr,
    const std::function<void(const AdaptiveTest &)> &f) {
    AdaptiveTest test;
    av_speech_in_noise::initialize(
        test, contents, method, identity, startingSnr);
    f(test);
}

static void initialize(Method method, const std::string &contents,
    const TestIdentity &identity, SNR startingSnr,
    const std::function<void(const FixedLevelTest &)> &f) {
    FixedLevelTest test;
    av_speech_in_noise::initialize(
        test, contents, method, identity, startingSnr);
    f(test);
}

static void initializeFixedLevelFixedTrialsTest(Method method,
    const std::string &contents, const TestIdentity &identity, SNR startingSnr,
    const std::function<void(const FixedLevelFixedTrialsTest &)> &f) {
    FixedLevelFixedTrialsTest test;
    av_speech_in_noise::initialize(
        test, contents, method, identity, startingSnr);
    f(test);
}

static void initializeFixedLevelTestWithEachTargetNTimes(Method method,
    const std::string &contents, const TestIdentity &identity, SNR startingSnr,
    const std::function<void(const FixedLevelTestWithEachTargetNTimes &)> &f) {
    FixedLevelTestWithEachTargetNTimes test;
    av_speech_in_noise::initialize(
        test, contents, method, identity, startingSnr);
    f(test);
}

static void initialize(RunningATestFacade &model, Method method,
    const std::string &contents, const TestIdentity &identity,
    SNR startingSnr) {
    switch (method) {
    case Method::adaptiveCoordinateResponseMeasureWithDelayedMasker:
        return av_speech_in_noise::initialize(method, contents, identity,
            startingSnr, [&](const AdaptiveTest &test) {
                model.initializeWithDelayedMasker(test);
            });
    case Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker:
        return av_speech_in_noise::initialize(method, contents, identity,
            startingSnr, [&](const AdaptiveTest &test) {
                model.initializeWithSingleSpeaker(test);
            });
    case Method::adaptiveCorrectKeywords:
        return av_speech_in_noise::initialize(method, contents, identity,
            startingSnr, [&](const AdaptiveTest &test) {
                model.initializeWithCyclicTargets(test);
            });
    case Method::adaptiveCorrectKeywordsWithEyeTracking:
        return av_speech_in_noise::initialize(method, contents, identity,
            startingSnr, [&](const AdaptiveTest &test) {
                model.initializeWithCyclicTargetsAndEyeTracking(test);
            });
    case Method::adaptiveCoordinateResponseMeasureWithEyeTracking:
    case Method::adaptivePassFailWithEyeTracking:
        return av_speech_in_noise::initialize(method, contents, identity,
            startingSnr, [&](const AdaptiveTest &test) {
                model.initializeWithEyeTracking(test);
            });
    case Method::adaptiveCoordinateResponseMeasure:
    case Method::adaptivePassFail:
        return av_speech_in_noise::initialize(method, contents, identity,
            startingSnr,
            [&](const AdaptiveTest &test) { model.initialize(test); });
    case Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets:
    case Method::fixedLevelFreeResponseWithSilentIntervalTargets:
        return av_speech_in_noise::initialize(method, contents, identity,
            startingSnr, [&](const FixedLevelTest &test) {
                model.initializeWithSilentIntervalTargets(test);
            });
    case Method::fixedLevelFreeResponseWithAllTargets:
    case Method::fixedLevelChooseKeywordsWithAllTargets:
    case Method::fixedLevelSyllablesWithAllTargets:
        return av_speech_in_noise::initialize(method, contents, identity,
            startingSnr, [&](const FixedLevelTest &test) {
                model.initializeWithAllTargets(test);
            });
    case Method::fixedLevelConsonants:
        return av_speech_in_noise::initializeFixedLevelTestWithEachTargetNTimes(
            method, contents, identity, startingSnr,
            [&](const FixedLevelTestWithEachTargetNTimes &test) {
                model.initialize(test);
            });
    case Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking:
        return av_speech_in_noise::initialize(method, contents, identity,
            startingSnr, [&](const FixedLevelTest &test) {
                model.initializeWithAllTargetsAndEyeTracking(test);
            });
    case Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording:
        return av_speech_in_noise::initialize(method, contents, identity,
            startingSnr, [&](const FixedLevelTest &test) {
                model.initializeWithAllTargetsAndAudioRecording(test);
            });
    case Method::
        fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording:
        return av_speech_in_noise::initialize(method, contents, identity,
            startingSnr, [&](const FixedLevelTest &test) {
                model.initializeWithPredeterminedTargetsAndAudioRecording(test);
            });
    case Method::
        fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking:
        return av_speech_in_noise::initializeFixedLevelFixedTrialsTest(method,
            contents, identity, startingSnr,
            [&](const FixedLevelFixedTrialsTest &test) {
                model.initializeWithTargetReplacementAndEyeTracking(test);
            });
    case Method::fixedLevelFreeResponseWithTargetReplacement:
    case Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement:
    case Method::unknown:
        return av_speech_in_noise::initializeFixedLevelFixedTrialsTest(method,
            contents, identity, startingSnr,
            [&](const FixedLevelFixedTrialsTest &test) {
                model.initializeWithTargetReplacement(test);
            });
    }
}

void TestSettingsInterpreterImpl::initialize(RunningATestFacade &model,
    SessionController &sessionController, const std::string &contents,
    const TestIdentity &identity, SNR startingSnr) {
    const auto method{av_speech_in_noise::method(contents)};
    av_speech_in_noise::initialize(
        model, method, contents, identity, startingSnr);
    if (!model.testComplete() && taskPresenters.count(method) != 0)
        sessionController.prepare(taskPresenters.at(method));
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

auto TestSettingsInterpreterImpl::meta(const std::string &contents)
    -> std::string {
    std::stringstream stream{contents};
    for (std::string line; std::getline(stream, line);)
        if (entryName(line) == name(TestSetting::meta))
            return entry(line);
    return "";
}

TestSettingsInterpreterImpl::TestSettingsInterpreterImpl(
    std::map<Method, TaskPresenter &> taskPresenters)
    : taskPresenters{std::move(taskPresenters)} {}
}
