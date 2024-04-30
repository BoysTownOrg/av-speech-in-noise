#include "TestSettingsInterpreter.hpp"

#include <gsl/gsl>

#include <sstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <cstddef>

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

static auto up(TrackingSequence &sequence) -> int & { return sequence.up; }

static auto down(TrackingSequence &sequence) -> int & { return sequence.down; }

static auto runCount(TrackingSequence &sequence) -> int & {
    return sequence.runCount;
}

static auto stepSize(TrackingSequence &sequence) -> int & {
    return sequence.stepSize;
}

// https://stackoverflow.com/a/217605
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

static void assignToEachElementOfTrackingRule(AdaptiveTest &test,
    const std::function<int &(TrackingSequence &)> &elementRef,
    const std::string &entry) {
    auto v{vectorOfInts(entry)};
    resizeTrackingRuleEnough(test, v);
    for (std::size_t i{0}; i < v.size(); ++i)
        elementRef(trackingRule(test).at(i)) = v.at(i);
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
    else if (entryName == name(TestSetting::videoScaleNumerator))
        test.videoScale.numerator = integer(entry);
    else if (entryName == name(TestSetting::videoScaleDenominator))
        test.videoScale.denominator = integer(entry);
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
        assignToEachElementOfTrackingRule(test, up, entry);
    else if (entryName == name(TestSetting::down))
        assignToEachElementOfTrackingRule(test, down, entry);
    else if (entryName == name(TestSetting::reversalsPerStepSize))
        assignToEachElementOfTrackingRule(test, runCount, entry);
    else if (entryName == name(TestSetting::stepSizes))
        assignToEachElementOfTrackingRule(test, stepSize, entry);
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

static void initialize(AdaptiveTest &test, const std::string &contents,
    const std::string &methodName, const TestIdentity &identity,
    SNR startingSnr) {
    test.identity = identity;
    test.startingSnr = startingSnr;
    applyToEachEntry(
        [&](auto entryName, auto entry) { assign(test, entryName, entry); },
        contents);
    test.ceilingSnr = SessionControllerImpl::ceilingSnr;
    test.floorSnr = SessionControllerImpl::floorSnr;
    test.trackBumpLimit = SessionControllerImpl::trackBumpLimit;
    test.fullScaleLevel = SessionControllerImpl::fullScaleLevel;
    test.identity.method = methodName;
}

static void initialize(FixedLevelTest &test, const std::string &contents,
    const std::string &methodName, const TestIdentity &identity,
    SNR startingSnr,
    const std::function<void(const std::string &, const std::string &)> &f) {
    test.snr = startingSnr;
    test.fullScaleLevel = SessionControllerImpl::fullScaleLevel;
    test.identity = identity;
    test.identity.method = methodName;
    applyToEachEntry(f, contents);
}

static void initialize(FixedLevelTest &test, const std::string &contents,
    const std::string &method, const TestIdentity &identity, SNR startingSnr) {
    initialize(test, contents, method, identity, startingSnr,
        [&](auto entryName, auto entry) { assign(test, entryName, entry); });
}

static void initialize(FixedLevelTestWithEachTargetNTimes &test,
    const std::string &contents, const std::string &method,
    const TestIdentity &identity, SNR startingSnr) {
    initialize(test, contents, method, identity, startingSnr,
        [&](auto entryName, auto entry) { assign(test, entryName, entry); });
}

static void initialize(const std::string &method, const std::string &contents,
    const TestIdentity &identity, SNR startingSnr,
    const std::function<void(AdaptiveTest &)> &f) {
    AdaptiveTest test;
    av_speech_in_noise::initialize(
        test, contents, method, identity, startingSnr);
    f(test);
}

static void initialize(const std::string &method, const std::string &contents,
    const TestIdentity &identity, SNR startingSnr,
    const std::function<void(const FixedLevelTest &)> &f) {
    FixedLevelTest test;
    av_speech_in_noise::initialize(
        test, contents, method, identity, startingSnr);
    f(test);
}

static void initializeFixedLevelFixedTrialsTest(const std::string &method,
    const std::string &contents, const TestIdentity &identity, SNR startingSnr,
    const std::function<void(const FixedLevelFixedTrialsTest &)> &f) {
    FixedLevelFixedTrialsTest test;
    av_speech_in_noise::initialize(
        test, contents, method, identity, startingSnr);
    f(test);
}

static void initializeFixedLevelTestWithEachTargetNTimes(
    const std::string &method, const std::string &contents,
    const TestIdentity &identity, SNR startingSnr,
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
    const Test &test, RunningATest::TestObserver *observer) {
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

void TestSettingsInterpreterImpl::initializeTest(const std::string &contents,
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

    const auto methodName{av_speech_in_noise::methodName(contents)};

    TaskPresenter *taskPresenter{};
    if (methodName == name(Method::adaptiveCoordinateResponseMeasure) ||
        methodName ==
            name(Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker) ||
        methodName ==
            name(Method::adaptiveCoordinateResponseMeasureWithDelayedMasker) ||
        methodName ==
            name(Method::adaptiveCoordinateResponseMeasureWithEyeTracking) ||
        methodName ==
            name(Method::
                    fixedLevelCoordinateResponseMeasureWithTargetReplacement) ||
        methodName ==
            name(Method::
                    fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking) ||
        methodName ==
            name(Method::
                    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets)) {
        taskPresenter = &coordinateResponseMeasurePresenter;
    } else if (methodName ==
            name(Method::fixedLevelFreeResponseWithTargetReplacement) ||
        methodName ==
            name(Method::fixedLevelFreeResponseWithSilentIntervalTargets) ||
        methodName == name(Method::fixedLevelFreeResponseWithAllTargets) ||
        methodName ==
            name(Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking) ||
        methodName ==
            name(Method::
                    fixedLevelFreeResponseWithAllTargetsAndAudioRecording) ||
        methodName ==
            name(Method::fixedLevelFreeResponseWithPredeterminedTargets) ||
        methodName ==
            name(Method::
                    fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording) ||
        methodName ==
            name(Method::
                    fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking)) {
        taskPresenter = &freeResponsePresenter;
    } else if (methodName == name(Method::adaptivePassFail) ||
        methodName == name(Method::adaptivePassFailWithEyeTracking)) {
        taskPresenter = &passFailPresenter;
    } else if (methodName == name(Method::adaptiveCorrectKeywords) ||
        methodName == name(Method::adaptiveCorrectKeywordsWithEyeTracking)) {
        taskPresenter = &correctKeywordsPresenter;
    } else if (methodName == name(Method::fixedLevelConsonants)) {
        taskPresenter = &consonantPresenter;
    } else if (methodName ==
        name(Method::fixedLevelChooseKeywordsWithAllTargets)) {
        taskPresenter = &chooseKeywordsPresenter;
    } else if (methodName == name(Method::fixedLevelSyllablesWithAllTargets)) {
        taskPresenter = &syllablesPresenter;
    } else {
        std::stringstream stream;
        stream << "Test method not recognized: " << methodName;
        throw std::runtime_error{stream.str()};
    }

    if (methodName ==
        name(Method::adaptiveCoordinateResponseMeasureWithDelayedMasker)) {
        av_speech_in_noise::initialize(methodName, contents, identity,
            startingSnr, [&](AdaptiveTest &test) {
                test.audioChannelOption = AudioChannelOption::delayedMasker;
                av_speech_in_noise::initialize(
                    adaptiveMethod, test, targetsWithReplacementReader);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, nullptr);
            });
    } else if (methodName ==
        name(Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker)) {
        av_speech_in_noise::initialize(methodName, contents, identity,
            startingSnr, [&](AdaptiveTest &test) {
                test.audioChannelOption = AudioChannelOption::singleSpeaker;
                av_speech_in_noise::initialize(
                    adaptiveMethod, test, targetsWithReplacementReader);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, nullptr);
            });
    } else if (methodName == name(Method::adaptiveCorrectKeywords)) {
        av_speech_in_noise::initialize(methodName, contents, identity,
            startingSnr, [&](const AdaptiveTest &test) {
                av_speech_in_noise::initialize(
                    adaptiveMethod, test, cyclicTargetsReader);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, nullptr);
            });
    } else if (methodName ==
        name(Method::adaptiveCorrectKeywordsWithEyeTracking)) {
        av_speech_in_noise::initialize(methodName, contents, identity,
            startingSnr, [&](const AdaptiveTest &test) {
                av_speech_in_noise::initialize(
                    adaptiveMethod, test, cyclicTargetsReader);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, &eyeTracking);
            });
    } else if (methodName ==
            name(Method::adaptiveCoordinateResponseMeasureWithEyeTracking) ||
        methodName == name(Method::adaptivePassFailWithEyeTracking)) {
        av_speech_in_noise::initialize(methodName, contents, identity,
            startingSnr, [&](const AdaptiveTest &test) {
                av_speech_in_noise::initialize(
                    adaptiveMethod, test, targetsWithReplacementReader);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, &eyeTracking);
            });
    } else if (methodName == name(Method::adaptiveCoordinateResponseMeasure) ||
        methodName == name(Method::adaptivePassFail)) {
        av_speech_in_noise::initialize(methodName, contents, identity,
            startingSnr, [&](const AdaptiveTest &test) {
                av_speech_in_noise::initialize(
                    adaptiveMethod, test, targetsWithReplacementReader);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, nullptr);
            });
    } else if (methodName ==
            name(Method::
                    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets) ||
        methodName ==
            name(Method::fixedLevelFreeResponseWithSilentIntervalTargets)) {
        av_speech_in_noise::initialize(methodName, contents, identity,
            startingSnr, [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, silentIntervalTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, nullptr);
            });
    } else if (methodName ==
            name(Method::fixedLevelFreeResponseWithAllTargets) ||
        methodName == name(Method::fixedLevelChooseKeywordsWithAllTargets) ||
        methodName == name(Method::fixedLevelSyllablesWithAllTargets)) {
        av_speech_in_noise::initialize(methodName, contents, identity,
            startingSnr, [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, everyTargetOnce);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, nullptr);
            });
    } else if (methodName == name(Method::fixedLevelConsonants)) {
        av_speech_in_noise::initializeFixedLevelTestWithEachTargetNTimes(
            methodName, contents, identity, startingSnr,
            [&](const FixedLevelTestWithEachTargetNTimes &test) {
                eachTargetNTimes.setRepeats(test.timesEachTargetIsPlayed - 1);
                fixedLevelMethod.initialize(test, &eachTargetNTimes);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, nullptr);
            });
    } else if (methodName ==
        name(Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking)) {
        av_speech_in_noise::initialize(methodName, contents, identity,
            startingSnr, [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, everyTargetOnce);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, &eyeTracking);
            });
    } else if (methodName ==
        name(Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording)) {
        av_speech_in_noise::initialize(methodName, contents, identity,
            startingSnr, [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, everyTargetOnce);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, &audioRecording);
            });
    } else if (methodName ==
        name(Method::fixedLevelFreeResponseWithPredeterminedTargets)) {
        av_speech_in_noise::initialize(methodName, contents, identity,
            startingSnr, [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, predeterminedTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, nullptr);
            });
    } else if (methodName ==
        name(Method::
                fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording)) {
        av_speech_in_noise::initialize(methodName, contents, identity,
            startingSnr, [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, predeterminedTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, &audioRecording);
            });
    } else if (methodName ==
        name(Method::
                fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking)) {
        av_speech_in_noise::initialize(methodName, contents, identity,
            startingSnr, [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, predeterminedTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, &eyeTracking);
            });
    } else if (methodName ==
        name(Method::
                fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking)) {
        av_speech_in_noise::initializeFixedLevelFixedTrialsTest(methodName,
            contents, identity, startingSnr,
            [&](const FixedLevelFixedTrialsTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, targetsWithReplacement);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, &eyeTracking);
            });
    } else if (methodName ==
            name(Method::fixedLevelFreeResponseWithTargetReplacement) ||
        methodName ==
            name(Method::
                    fixedLevelCoordinateResponseMeasureWithTargetReplacement)) {
        av_speech_in_noise::initializeFixedLevelFixedTrialsTest(methodName,
            contents, identity, startingSnr,
            [&](const FixedLevelFixedTrialsTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, targetsWithReplacement);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, nullptr);
            });
    }

    if (!runningATest.testComplete())
        sessionController.prepare(*taskPresenter);
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
    RunningATest &runningATest, AdaptiveMethod &adaptiveMethod,
    FixedLevelMethod &fixedLevelMethod, RunningATest::TestObserver &eyeTracking,
    RunningATest::TestObserver &audioRecording,
    TargetPlaylistReader &cyclicTargetsReader,
    TargetPlaylistReader &targetsWithReplacementReader,
    FiniteTargetPlaylistWithRepeatables &predeterminedTargets,
    FiniteTargetPlaylistWithRepeatables &everyTargetOnce,
    FiniteTargetPlaylistWithRepeatables &silentIntervalTargets,
    RepeatableFiniteTargetPlaylist &eachTargetNTimes,
    TargetPlaylist &targetsWithReplacement,
    submitting_free_response::Puzzle &puzzle,
    FreeResponseController &freeResponseController,
    SessionController &sessionController,
    TaskPresenter &coordinateResponseMeasurePresenter,
    TaskPresenter &freeResponsePresenter,
    TaskPresenter &chooseKeywordsPresenter, TaskPresenter &syllablesPresenter,
    TaskPresenter &correctKeywordsPresenter, TaskPresenter &consonantPresenter,
    TaskPresenter &passFailPresenter)
    : runningATest{runningATest}, adaptiveMethod{adaptiveMethod},
      fixedLevelMethod{fixedLevelMethod}, eyeTracking{eyeTracking},
      audioRecording{audioRecording}, cyclicTargetsReader{cyclicTargetsReader},
      targetsWithReplacementReader{targetsWithReplacementReader},
      predeterminedTargets{predeterminedTargets},
      everyTargetOnce{everyTargetOnce},
      silentIntervalTargets{silentIntervalTargets},
      eachTargetNTimes{eachTargetNTimes},
      targetsWithReplacement{targetsWithReplacement}, puzzle{puzzle},
      freeResponseController{freeResponseController},
      sessionController{sessionController},
      coordinateResponseMeasurePresenter{coordinateResponseMeasurePresenter},
      freeResponsePresenter{freeResponsePresenter},
      chooseKeywordsPresenter{chooseKeywordsPresenter},
      syllablesPresenter{syllablesPresenter},
      correctKeywordsPresenter{correctKeywordsPresenter},
      consonantPresenter{consonantPresenter},
      passFailPresenter{passFailPresenter} {}
}
