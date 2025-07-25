#include "TestSettingsInterpreter.hpp"
#include "SessionController.hpp"
#include "av-speech-in-noise/Model.hpp"

#include <gsl/gsl>

#include <sstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <cstddef>
#include <string_view>
#include <tuple>
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

static void assignToEachElementOfTrackingRule(LevittSettings &s,
    const std::function<int &(TrackingSequence &)> &elementRef,
    const std::string &entry) {
    auto v{vectorOfInts(entry)};
    resizeTrackingRuleEnough(s, v);
    for (std::size_t i{0}; i < v.size(); ++i)
        elementRef(trackingRule(s).at(i)) = v.at(i);
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

static void broadcast(
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &key, const std::string &value) {
    if (const auto search{configurables.find(key)};
        search != configurables.end())
        for (const auto each : search->second)
            each.get().configure(key, value);
}

void broadcast(
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &entryName, const std::string &entry) {
    if (auto search = configurables.find(entryName);
        search != configurables.end())
        for (auto each : search->second)
            each.get().configure(entryName, entry);
}
static void assign(Test &test,
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &entryName, const std::string &entry) {
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
    else
        broadcast(configurables, entryName, entry);
}

static void assign(FixedLevelTest &test,
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &entryName, const std::string &entry) {
    if (entryName == name(TestSetting::startingSnr))
        test.snr.dB = integer(entry);
    else
        assign(static_cast<Test &>(test), configurables, entryName, entry);
}

static void assign(FixedLevelTestWithEachTargetNTimes &test,
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &entryName, const std::string &entry) {
    if (entryName == name(TestSetting::targetRepetitions))
        test.timesEachTargetIsPlayed = integer(entry);
    else
        assign(static_cast<FixedLevelTest &>(test), configurables, entryName,
            entry);
}

static void assign(Calibration &calibration, const std::string &entryName,
    const std::string &entry) {
    if (entryName == name(TestSetting::masker))
        calibration.fileUrl.path = entry;
    else if (entryName == name(TestSetting::maskerLevel))
        calibration.level.dB_SPL = integer(entry);
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

static void assign(AdaptiveTest &test, UmlSettings &umlSettings,
    LevittSettings &levittSettings,
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &entryName, const std::string &entry) {
    if (entryName == name(TestSetting::up))
        assignToEachElementOfTrackingRule(levittSettings, up, entry);
    else if (entryName == name(TestSetting::down))
        assignToEachElementOfTrackingRule(levittSettings, down, entry);
    else if (entryName == name(TestSetting::reversalsPerStepSize))
        assignToEachElementOfTrackingRule(levittSettings, runCount, entry);
    else if (entryName == name(TestSetting::stepSizes))
        assignToEachElementOfTrackingRule(levittSettings, stepSize, entry);
    else if (entryName == name(TestSetting::thresholdReversals))
        test.thresholdReversals = integer(entry);
    else if (entryName == name(TestSetting::startingSnr))
        test.startingSnr.dB = integer(entry);
    else if (entryName == name(TestSetting::uml))
        test.uml = boolean(entry);
    else if (entryName == name(TestSetting::alphaSpace))
        initializeParameterSpace(umlSettings.alpha, entry);
    else if (entryName == name(TestSetting::alphaPrior))
        initializeParameterPrior(umlSettings.alpha, entry);
    else if (entryName == name(TestSetting::betaSpace))
        initializeParameterSpace(umlSettings.beta, entry);
    else if (entryName == name(TestSetting::betaPrior))
        initializeParameterPrior(umlSettings.beta, entry);
    else if (entryName == name(TestSetting::gammaSpace))
        initializeParameterSpace(umlSettings.gamma, entry);
    else if (entryName == name(TestSetting::gammaPrior))
        initializeParameterPrior(umlSettings.gamma, entry);
    else if (entryName == name(TestSetting::lambdaSpace))
        initializeParameterSpace(umlSettings.lambda, entry);
    else if (entryName == name(TestSetting::lambdaPrior))
        initializeParameterPrior(umlSettings.lambda, entry);
    else if (entryName == name(TestSetting::trials))
        umlSettings.trials = integer(entry);
    else
        assign(static_cast<Test &>(test), configurables, entryName, entry);
}

// https://stackoverflow.com/a/31836401
template <typename C, C beginVal, C endVal> class EnumIterator {
    using val_t = std::underlying_type_t<C>;
    int val;

  public:
    EnumIterator(const C &f) : val(static_cast<val_t>(f)) {}
    EnumIterator() : val(static_cast<val_t>(beginVal)) {}
    auto operator++() -> EnumIterator {
        ++val;
        return *this;
    }
    auto operator*() -> C { return static_cast<C>(val); }
    auto begin() -> EnumIterator { return *this; }
    auto end() -> EnumIterator {
        static const EnumIterator endIter = ++EnumIterator(endVal);
        return endIter;
    }
    auto operator!=(const EnumIterator &i) -> bool { return val != i.val; }
};

static auto matches(const std::string_view &s, gsl::index position,
    const std::string_view &other) -> bool {
    return 0 == s.compare(position, other.length(), other);
}

static auto startsWith(const std::string_view &s, const std::string_view &what)
    -> bool {
    if (s.length() >= what.length())
        return matches(s, 0, what);
    return false;
}

static auto methodWithName(const std::string &contents)
    -> std::tuple<Method, std::string> {
    std::stringstream stream{contents};
    for (std::string line; std::getline(stream, line);)
        if (entryName(line) == name(TestSetting::method)) {
            const auto actual{entry(line)};
            for (const auto e : EnumIterator<Method, Method::adaptivePassFail,
                     Method::fixedLevelSyllablesWithAllTargets>{})
                if (startsWith(actual, name(e)))
                    return std::make_tuple(e, actual);
            std::stringstream stream;
            stream << "Test method not recognized: " << actual;
            throw std::runtime_error{stream.str()};
        }
    throw std::runtime_error{"Test method not found"};
}

static void initialize(AdaptiveTest &test, UmlSettings &umlSettings,
    LevittSettings &levittSettings,
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &contents, const std::string &methodName,
    const TestIdentity &identity, SNR startingSnr) {
    test.identity = identity;
    test.startingSnr = startingSnr;
    applyToEachEntry(
        [&](const auto &entryName, const auto &entry) {
            assign(test, umlSettings, levittSettings, configurables, entryName,
                entry);
        },
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

static void initialize(FixedLevelTest &test,
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &contents, const std::string &method,
    const TestIdentity &identity, SNR startingSnr) {
    initialize(test, contents, method, identity, startingSnr,
        [&](const auto &entryName, const auto &entry) {
            assign(test, configurables, entryName, entry);
        });
}

static void initialize(FixedLevelTestWithEachTargetNTimes &test,
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &contents, const std::string &method,
    const TestIdentity &identity, SNR startingSnr) {
    initialize(test, contents, method, identity, startingSnr,
        [&](const auto &entryName, const auto &entry) {
            assign(test, configurables, entryName, entry);
        });
}

static void initialize(
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &method, const std::string &contents,
    const TestIdentity &identity, SNR startingSnr,
    const std::function<void(AdaptiveTest &)> &f) {
    AdaptiveTest test;
    UmlSettings umlSettings;
    LevittSettings levittSettings;
    av_speech_in_noise::initialize(test, umlSettings, levittSettings,
        configurables, contents, method, identity, startingSnr);
    if (test.uml)
        test.trackSettings = umlSettings;
    else
        test.trackSettings = levittSettings;
    f(test);
}

static void initialize(
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &method, const std::string &contents,
    const TestIdentity &identity, SNR startingSnr,
    const std::function<void(FixedLevelTest &)> &f) {
    FixedLevelTest test;
    av_speech_in_noise::initialize(
        test, configurables, contents, method, identity, startingSnr);
    f(test);
}

static void initializeFixedLevelFixedTrialsTest(
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &method, const std::string &contents,
    const TestIdentity &identity, SNR startingSnr,
    const std::function<void(const FixedLevelFixedTrialsTest &)> &f) {
    FixedLevelFixedTrialsTest test;
    av_speech_in_noise::initialize(
        test, configurables, contents, method, identity, startingSnr);
    f(test);
}

static void initializeFixedLevelTestWithEachTargetNTimes(
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &method, const std::string &contents,
    const TestIdentity &identity, SNR startingSnr,
    const std::function<void(const FixedLevelTestWithEachTargetNTimes &)> &f) {
    FixedLevelTestWithEachTargetNTimes test;
    av_speech_in_noise::initialize(
        test, configurables, contents, method, identity, startingSnr);
    f(test);
}

static void initialize(AdaptiveMethod &method, const AdaptiveTest &test,
    TargetPlaylistReader &reader, AdaptiveTrack::Factory &factory) {
    method.initialize(test, &reader, &factory);
}

static void initialize(RunningATest &model, TestMethod &method,
    const Test &test,
    std::vector<std::reference_wrapper<RunningATest::TestObserver>> observer =
        {}) {
    model.initialize(&method, test, std::move(observer));
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
    broadcast(configurables, "relative output path",
        "Documents/AvSpeechInNoise Data");

    std::stringstream stream{contents};
    auto usingPuzzle = false;
    for (std::string line; std::getline(stream, line);) {
        const auto key{entryName(line)};
        const auto value{entry(line)};
        broadcast(configurables, key, value);
        if (key == name(TestSetting::puzzle)) {
            puzzle.initialize(localUrlFromPath(value));
            usingPuzzle = true;
        }
    }
    freeResponseController.initialize(usingPuzzle);

    const auto [method, methodName] =
        av_speech_in_noise::methodWithName(contents);

    std::vector<std::reference_wrapper<RunningATest::TestObserver>>
        testObservers;
    if (contains(methodName, "eye tracking"))
        testObservers.emplace_back(eyeTracking);
    if (contains(methodName, "audio recording"))
        testObservers.emplace_back(audioRecording);

    auto audioChannelOption{AudioChannelOption::all};
    if (contains(methodName, "not spatial"))
        audioChannelOption = AudioChannelOption::singleSpeaker;
    else if (contains(methodName, "spatial"))
        audioChannelOption = AudioChannelOption::delayedMasker;

    TaskPresenter *taskPresenter{};
    switch (method) {
    case Method::adaptiveCoordinateResponseMeasure:
    case Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement:
    case Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets:
        taskPresenter = &coordinateResponseMeasurePresenter;
        break;
    case Method::fixedLevelFreeResponseWithTargetReplacement:
    case Method::fixedLevelFreeResponseWithSilentIntervalTargets:
    case Method::fixedLevelFreeResponseWithAllTargets:
    case Method::fixedLevelFreeResponseWithPredeterminedTargets:
        taskPresenter = &freeResponsePresenter;
        break;
    case Method::fixedLevelButtonThenPassFailResponseWithPredeterminedTargets:
        keypressPresenter.enableDualTask(&fixedPassFailPresenter);
    case Method::fixedLevelButtonResponseWithPredeterminedTargets:
        testObservers.emplace_back(submittingKeyPressResponse);
        taskPresenter = &keypressPresenter;
        break;
    case Method::adaptivePassFail:
        taskPresenter = &passFailPresenter;
        break;
    case Method::adaptiveCorrectKeywords:
        taskPresenter = &correctKeywordsPresenter;
        break;
    case Method::fixedLevelConsonants:
        testObservers.emplace_back(submittingConsonantResponse);
        taskPresenter = &consonantPresenter;
        break;
    case Method::fixedLevelChooseKeywordsWithAllTargets:
        taskPresenter = &chooseKeywordsPresenter;
        break;
    case Method::fixedLevelSyllablesWithAllTargets:
        taskPresenter = &syllablesPresenter;
        break;
    case Method::fixedLevelEmotionsWithPredeterminedTargets:
        taskPresenter = &emotionPresenter;
        break;
    case Method::fixedLevelChildEmotionsWithPredeterminedTargets:
        taskPresenter = &childEmotionPresenter;
        break;
    case Method::fixedLevelPassFailWithPredeterminedTargets:
        taskPresenter = &fixedPassFailPresenter;
        break;
    case Method::unknown:
        break;
    }

    switch (method) {
    case Method::adaptiveCorrectKeywords:
        av_speech_in_noise::initialize(configurables, methodName, contents,
            identity, startingSnr, [&](const AdaptiveTest &test) {
                av_speech_in_noise::initialize(adaptiveMethod, test,
                    cyclicTargetsReader, adaptiveTrackFactory);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, testObservers);
            });
        break;
    case Method::adaptiveCoordinateResponseMeasure:
    case Method::adaptivePassFail:
        av_speech_in_noise::initialize(configurables, methodName, contents,
            identity, startingSnr, [&](AdaptiveTest &test) {
                test.audioChannelOption = audioChannelOption;
                av_speech_in_noise::initialize(adaptiveMethod, test,
                    targetsWithReplacementReader, adaptiveTrackFactory);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, testObservers);
            });
        break;
    case Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets:
    case Method::fixedLevelFreeResponseWithSilentIntervalTargets:
        av_speech_in_noise::initialize(configurables, methodName, contents,
            identity, startingSnr, [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, silentIntervalTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, testObservers);
            });
        break;
    case Method::fixedLevelFreeResponseWithAllTargets:
    case Method::fixedLevelChooseKeywordsWithAllTargets:
    case Method::fixedLevelSyllablesWithAllTargets:
        av_speech_in_noise::initialize(configurables, methodName, contents,
            identity, startingSnr, [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, everyTargetOnce);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, testObservers);
            });
        break;
    case Method::fixedLevelConsonants:
        av_speech_in_noise::initializeFixedLevelTestWithEachTargetNTimes(
            configurables, methodName, contents, identity, startingSnr,
            [&](const FixedLevelTestWithEachTargetNTimes &test) {
                eachTargetNTimes.setRepeats(test.timesEachTargetIsPlayed - 1);
                fixedLevelMethod.initialize(test, &eachTargetNTimes);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, testObservers);
            });
        break;
    case Method::fixedLevelEmotionsWithPredeterminedTargets:
    case Method::fixedLevelChildEmotionsWithPredeterminedTargets:
    case Method::fixedLevelFreeResponseWithPredeterminedTargets:
    case Method::fixedLevelPassFailWithPredeterminedTargets:
        av_speech_in_noise::initialize(configurables, methodName, contents,
            identity, startingSnr, [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, predeterminedTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, testObservers);
            });
        break;
    case Method::fixedLevelButtonResponseWithPredeterminedTargets:
    case Method::fixedLevelButtonThenPassFailResponseWithPredeterminedTargets:
        av_speech_in_noise::initialize(configurables, methodName, contents,
            identity, startingSnr, [&](FixedLevelTest &test) {
                test.enableVibrotactileStimulus = true;
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, predeterminedTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, testObservers);
            });
        break;
    case Method::fixedLevelFreeResponseWithTargetReplacement:
    case Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement:
        av_speech_in_noise::initializeFixedLevelFixedTrialsTest(configurables,
            methodName, contents, identity, startingSnr,
            [&](const FixedLevelFixedTrialsTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, targetsWithReplacement);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, testObservers);
            });
        break;
    case Method::unknown:
        break;
    }

    if (!runningATest.testComplete())
        sessionController.prepare(*taskPresenter);
}

auto TestSettingsInterpreterImpl::calibration(const std::string &contents)
    -> Calibration {
    Calibration calibration;
    applyToEachEntry(
        [&](const auto &entryName, const auto &entry) {
            assign(calibration, entryName, entry);
        },
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
    AdaptiveTrack::Factory &adaptiveTrackFactory,
    submitting_free_response::Puzzle &puzzle,
    FreeResponseController &freeResponseController,
    SessionController &sessionController,
    TaskPresenter &coordinateResponseMeasurePresenter,
    TaskPresenter &freeResponsePresenter,
    TaskPresenter &chooseKeywordsPresenter, TaskPresenter &syllablesPresenter,
    TaskPresenter &correctKeywordsPresenter, TaskPresenter &consonantPresenter,
    RunningATest::TestObserver &submittingConsonantResponse,
    TaskPresenter &passFailPresenter, TaskPresenter &keypressPresenter,
    RunningATest::TestObserver &submittingKeyPressResponse,
    TaskPresenter &emotionPresenter, TaskPresenter &childEmotionPresenter,
    TaskPresenter &fixedPassFailPresenter)
    : runningATest{runningATest}, adaptiveMethod{adaptiveMethod},
      fixedLevelMethod{fixedLevelMethod}, eyeTracking{eyeTracking},
      audioRecording{audioRecording},
      adaptiveTrackFactory{adaptiveTrackFactory},
      cyclicTargetsReader{cyclicTargetsReader},
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
      submittingConsonantResponse{submittingConsonantResponse},
      passFailPresenter{passFailPresenter},
      keypressPresenter{keypressPresenter},
      submittingKeyPressResponse{submittingKeyPressResponse},
      emotionPresenter{emotionPresenter},
      childEmotionPresenter{childEmotionPresenter},
      fixedPassFailPresenter{fixedPassFailPresenter} {}
}
