#include "TestSettingsInterpreter.hpp"

#include <gsl/gsl>

#include <map>
#include <sstream>
#include <functional>
#include <stdexcept>
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

static auto trim(std::string s) -> std::string {

    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return std::isspace(ch) == 0;
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(),
                [](unsigned char ch) { return std::isspace(ch) == 0; })
                .base(),
        s.end());
    return s;
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
    return trim(line.substr(0, entryDelimiter(line)));
}

static auto size(const std::string &s) -> gsl::index { return s.size(); }

static auto entry(const std::string &line) -> std::string {
    return entryDelimiter(line) >= size(line) - 1
        ? ""
        : trim(line.substr(entryDelimiter(line) + 1));
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
    else if (entryName == name(TestSetting::keepVideoShown))
        test.keepVideoShown = entry == "true";
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
        {name(Method::
                 fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking),
            Method::
                fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking},
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

static void initialize(AdaptiveMethod &method, const AdaptiveTest &test,
    TargetPlaylistReader &reader) {
    method.initialize(test, &reader);
}

static void initialize(RunningATest &model, TestMethod &method,
    const Test &test, RunningATest::Observer *observer) {
    model.initialize(&method, test, observer);
}

static void initialize(FixedLevelMethod &method, const FixedLevelTest &test,
    FiniteTargetPlaylistWithRepeatables &targets) {
    method.initialize(test, &targets);
}

static void initialize(FixedLevelMethod &method,
    const FixedLevelFixedTrialsTest &test, TargetPlaylist &targets) {
    method.initialize(test, &targets);
}

static auto localUrlFromPath(const std::string &path) -> LocalUrl {
    LocalUrl url;
    url.path = path;
    return url;
}

void TestSettingsInterpreterImpl::initialize(
    SessionController &sessionController, const std::string &contents,
    const TestIdentity &identity, SNR startingSnr) {
    std::stringstream stream{contents};
    auto usingPuzzle = false;
    for (std::string line; std::getline(stream, line);) {
        const auto key{entryName(line)};
        const auto value{entry(line)};
        if (key == name(TestSetting::puzzle)) {
            puzzle.initialize(localUrlFromPath(value));
            usingPuzzle = true;
        }
    }
    freeResponseController.initialize(usingPuzzle);
    const auto method{av_speech_in_noise::method(contents)};

    switch (method) {
    case Method::adaptiveCoordinateResponseMeasureWithDelayedMasker:
        av_speech_in_noise::initialize(method, contents, identity, startingSnr,
            [&](const AdaptiveTest &test) {
                auto test_{test};
                test_.audioChannelOption = AudioChannelOption::delayedMasker;
                av_speech_in_noise::initialize(
                    adaptiveMethod, test_, targetsWithReplacementReader);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test_, nullptr);
            });
        break;
    case Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker:
        av_speech_in_noise::initialize(method, contents, identity, startingSnr,
            [&](const AdaptiveTest &test) {
                auto test_{test};
                test_.audioChannelOption = AudioChannelOption::singleSpeaker;
                av_speech_in_noise::initialize(
                    adaptiveMethod, test_, targetsWithReplacementReader);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test_, nullptr);
            });
        break;
    case Method::adaptiveCorrectKeywords:
        av_speech_in_noise::initialize(method, contents, identity, startingSnr,
            [&](const AdaptiveTest &test) {
                av_speech_in_noise::initialize(
                    adaptiveMethod, test, cyclicTargetsReader);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, nullptr);
            });
        break;
    case Method::adaptiveCorrectKeywordsWithEyeTracking:
        av_speech_in_noise::initialize(method, contents, identity, startingSnr,
            [&](const AdaptiveTest &test) {
                av_speech_in_noise::initialize(
                    adaptiveMethod, test, cyclicTargetsReader);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, &eyeTracking);
            });
        break;
    case Method::adaptiveCoordinateResponseMeasureWithEyeTracking:
    case Method::adaptivePassFailWithEyeTracking:
        av_speech_in_noise::initialize(method, contents, identity, startingSnr,
            [&](const AdaptiveTest &test) {
                av_speech_in_noise::initialize(
                    adaptiveMethod, test, targetsWithReplacementReader);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, &eyeTracking);
            });
        break;
    case Method::adaptiveCoordinateResponseMeasure:
    case Method::adaptivePassFail:
        av_speech_in_noise::initialize(method, contents, identity, startingSnr,
            [&](const AdaptiveTest &test) {
                av_speech_in_noise::initialize(
                    adaptiveMethod, test, targetsWithReplacementReader);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, nullptr);
            });
        break;
    case Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets:
    case Method::fixedLevelFreeResponseWithSilentIntervalTargets:
        av_speech_in_noise::initialize(method, contents, identity, startingSnr,
            [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, silentIntervalTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, nullptr);
            });
        break;
    case Method::fixedLevelFreeResponseWithAllTargets:
    case Method::fixedLevelChooseKeywordsWithAllTargets:
    case Method::fixedLevelSyllablesWithAllTargets:
        av_speech_in_noise::initialize(method, contents, identity, startingSnr,
            [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, everyTargetOnce);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, nullptr);
            });
        break;
    case Method::fixedLevelConsonants:
        av_speech_in_noise::initializeFixedLevelTestWithEachTargetNTimes(method,
            contents, identity, startingSnr,
            [&](const FixedLevelTestWithEachTargetNTimes &test) {
                eachTargetNTimes.setRepeats(test.timesEachTargetIsPlayed - 1);
                fixedLevelMethod.initialize(test, &eachTargetNTimes);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, nullptr);
            });
        break;
    case Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking:
        av_speech_in_noise::initialize(method, contents, identity, startingSnr,
            [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, everyTargetOnce);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, &eyeTracking);
            });
        break;
    case Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording:
        av_speech_in_noise::initialize(method, contents, identity, startingSnr,
            [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, everyTargetOnce);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, &audioRecording);
            });
        break;
    case Method::
        fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording:
        av_speech_in_noise::initialize(method, contents, identity, startingSnr,
            [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, predeterminedTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, &audioRecording);
            });
        break;
    case Method::fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking:
        av_speech_in_noise::initialize(method, contents, identity, startingSnr,
            [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, predeterminedTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, &eyeTracking);
            });
        break;
    case Method::
        fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking:
        av_speech_in_noise::initializeFixedLevelFixedTrialsTest(method,
            contents, identity, startingSnr,
            [&](const FixedLevelFixedTrialsTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, targetsWithReplacement);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, &eyeTracking);
            });
        break;
    case Method::fixedLevelFreeResponseWithTargetReplacement:
    case Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement:
        av_speech_in_noise::initializeFixedLevelFixedTrialsTest(method,
            contents, identity, startingSnr,
            [&](const FixedLevelFixedTrialsTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, targetsWithReplacement);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, nullptr);
            });
        break;
    case Method::unknown: {
        std::stringstream stream;
        stream << "Test method not recognized: " << methodName(contents);
        throw std::runtime_error{stream.str()};
    }
    }

    if (!runningATest.testComplete() && taskPresenters.count(method) != 0)
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
    std::map<Method, TaskPresenter &> taskPresenters,
    RunningATest &runningATest, AdaptiveMethod &adaptiveMethod,
    FixedLevelMethod &fixedLevelMethod, RunningATest::Observer &eyeTracking,
    RunningATest::Observer &audioRecording,
    TargetPlaylistReader &cyclicTargetsReader,
    TargetPlaylistReader &targetsWithReplacementReader,
    FiniteTargetPlaylistWithRepeatables &predeterminedTargets,
    FiniteTargetPlaylistWithRepeatables &everyTargetOnce,
    FiniteTargetPlaylistWithRepeatables &silentIntervalTargets,
    RepeatableFiniteTargetPlaylist &eachTargetNTimes,
    TargetPlaylist &targetsWithReplacement,
    submitting_free_response::Puzzle &puzzle,
    FreeResponseController &freeResponseController)
    : taskPresenters{std::move(taskPresenters)}, runningATest{runningATest},
      adaptiveMethod{adaptiveMethod}, fixedLevelMethod{fixedLevelMethod},
      eyeTracking{eyeTracking}, audioRecording{audioRecording},
      cyclicTargetsReader{cyclicTargetsReader},
      targetsWithReplacementReader{targetsWithReplacementReader},
      predeterminedTargets{predeterminedTargets},
      everyTargetOnce{everyTargetOnce},
      silentIntervalTargets{silentIntervalTargets},
      eachTargetNTimes{eachTargetNTimes},
      targetsWithReplacement{targetsWithReplacement}, puzzle{puzzle},
      freeResponseController{freeResponseController} {}
}
