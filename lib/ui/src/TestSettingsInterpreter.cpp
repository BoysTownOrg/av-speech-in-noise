#include "TestSettingsInterpreter.hpp"
#include "SessionController.hpp"

#include <gsl/gsl>

#include <sstream>
#include <functional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>

namespace av_speech_in_noise {
static auto entryDelimiter(const std::string &s) -> gsl::index {
    return s.find(':');
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

static auto key(const std::string &line) -> std::string {
    return trim(line.substr(0, entryDelimiter(line)));
}

static auto size(const std::string &s) -> gsl::index { return s.size(); }

static auto value(const std::string &line) -> std::string {
    return entryDelimiter(line) >= size(line) - 1
        ? ""
        : trim(line.substr(entryDelimiter(line) + 1));
}

static void applyToEachEntry(
    const std::function<void(const std::string &, const std::string &)> &f,
    const std::string &contents) {
    std::stringstream stream{contents};
    for (std::string line; std::getline(stream, line);)
        f(key(line), value(line));
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

static void assign(Test &test,
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &key, const std::string &value) {
    if (key == name(TestSetting::masker))
        test.maskerFileUrl.path = value;
    else if (key == name(TestSetting::maskerLevel))
        test.maskerLevel.dB_SPL = integer(value);
    else
        broadcast(configurables, key, value);
}

static void assign(FixedLevelTest &test,
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &key, const std::string &value) {
    if (key == name(TestSetting::startingSnr))
        test.snr.dB = integer(value);
    else
        assign(static_cast<Test &>(test), configurables, key, value);
}

static void assign(Calibration &calibration, const std::string &key,
    const std::string &value) {
    if (key == name(TestSetting::masker))
        calibration.fileUrl.path = value;
    else if (key == name(TestSetting::maskerLevel))
        calibration.level.dB_SPL = integer(value);
}

static void assign(AdaptiveTest &test,
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &key, const std::string &value) {
    if (key == name(TestSetting::startingSnr))
        test.startingSnr.dB = integer(value);
    else
        assign(static_cast<Test &>(test), configurables, key, value);
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
        if (key(line) == name(TestSetting::method)) {
            const auto actual{value(line)};
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

static void initialize(AdaptiveTest &test,
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &contents, SNR startingSnr) {
    test.startingSnr = startingSnr;
    applyToEachEntry(
        [&](const auto &entryName, const auto &entry) {
            assign(test, configurables, entryName, entry);
        },
        contents);
    test.fullScaleLevel = SessionControllerImpl::fullScaleLevel;
}

static void initialize(FixedLevelTest &test, const std::string &contents,
    SNR startingSnr,
    const std::function<void(const std::string &, const std::string &)> &f) {
    test.snr = startingSnr;
    test.fullScaleLevel = SessionControllerImpl::fullScaleLevel;
    applyToEachEntry(f, contents);
}

static void initialize(FixedLevelTest &test,
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &contents, SNR startingSnr) {
    initialize(test, contents, startingSnr,
        [&](const auto &entryName, const auto &entry) {
            assign(test, configurables, entryName, entry);
        });
}

static void initialize(
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &contents, SNR startingSnr,
    const std::function<void(AdaptiveTest &)> &f) {
    AdaptiveTest test;
    av_speech_in_noise::initialize(test, configurables, contents, startingSnr);
    f(test);
}

static void initialize(
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &contents, SNR startingSnr,
    const std::function<void(FixedLevelTest &)> &f) {
    FixedLevelTest test;
    av_speech_in_noise::initialize(test, configurables, contents, startingSnr);
    f(test);
}

static void initializeFixedLevelFixedTrialsTest(
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &contents, SNR startingSnr,
    const std::function<void(const FixedLevelFixedTrialsTest &)> &f) {
    FixedLevelFixedTrialsTest test;
    av_speech_in_noise::initialize(test, configurables, contents, startingSnr);
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

void TestSettingsInterpreterImpl::initializeTest(const std::string &contents,
    const TestIdentity &testIdentity, SNR startingSnr) {
    broadcast(configurables, "relative output path",
        "Documents/AvSpeechInNoise Data");
    broadcast(configurables, "puzzle", "");
    broadcast(configurables, "subject ID", testIdentity.subjectId);
    broadcast(configurables, "tester ID", testIdentity.testerId);
    broadcast(configurables, "session", testIdentity.session);
    broadcast(configurables, "RME setting", testIdentity.rmeSetting);
    broadcast(configurables, "transducer", testIdentity.transducer);

    const auto [method, methodName] =
        av_speech_in_noise::methodWithName(contents);
    broadcast(configurables, "method", methodName);

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
    case Method::fixedLevelFreeResponseWithTargetReplacement:
    case Method::fixedLevelFreeResponseWithSilentIntervalTargets:
    case Method::fixedLevelFreeResponseWithAllTargets:
    case Method::fixedLevelFreeResponseWithPredeterminedTargets:
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
        break;
    case Method::fixedLevelConsonants:
        testObservers.emplace_back(submittingConsonantResponse);
        break;
    case Method::fixedLevelChooseKeywordsWithAllTargets:
    case Method::fixedLevelSyllablesWithAllTargets:
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
        av_speech_in_noise::initialize(configurables, contents, startingSnr,
            [&](const AdaptiveTest &test) {
                av_speech_in_noise::initialize(adaptiveMethod, test,
                    cyclicTargetsReader, adaptiveTrackFactory);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, testObservers);
            });
        break;
    case Method::adaptiveCoordinateResponseMeasure:
    case Method::adaptivePassFail:
        av_speech_in_noise::initialize(
            configurables, contents, startingSnr, [&](AdaptiveTest &test) {
                test.audioChannelOption = audioChannelOption;
                av_speech_in_noise::initialize(adaptiveMethod, test,
                    targetsWithReplacementReader, adaptiveTrackFactory);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test, testObservers);
            });
        break;
    case Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets:
    case Method::fixedLevelFreeResponseWithSilentIntervalTargets:
        av_speech_in_noise::initialize(configurables, contents, startingSnr,
            [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, silentIntervalTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, testObservers);
            });
        break;
    case Method::fixedLevelFreeResponseWithAllTargets:
    case Method::fixedLevelChooseKeywordsWithAllTargets:
    case Method::fixedLevelSyllablesWithAllTargets:
        av_speech_in_noise::initialize(configurables, contents, startingSnr,
            [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, everyTargetOnce);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, testObservers);
            });
        break;
    case Method::fixedLevelConsonants:
        av_speech_in_noise::initialize(configurables, contents, startingSnr,
            [&](const FixedLevelTest &test) {
                fixedLevelMethod.initialize(test, &eachTargetNTimes);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, testObservers);
            });
        break;
    case Method::fixedLevelEmotionsWithPredeterminedTargets:
    case Method::fixedLevelChildEmotionsWithPredeterminedTargets:
    case Method::fixedLevelFreeResponseWithPredeterminedTargets:
    case Method::fixedLevelPassFailWithPredeterminedTargets:
        av_speech_in_noise::initialize(configurables, contents, startingSnr,
            [&](const FixedLevelTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, predeterminedTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test, testObservers);
            });
        break;
    case Method::fixedLevelButtonResponseWithPredeterminedTargets:
    case Method::fixedLevelButtonThenPassFailResponseWithPredeterminedTargets:
        av_speech_in_noise::initialize(
            configurables, contents, startingSnr, [&](FixedLevelTest &test) {
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
            contents, startingSnr, [&](const FixedLevelFixedTrialsTest &test) {
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
        if (key(line) == "meta")
            return value(line);
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
    FiniteTargetPlaylist &eachTargetNTimes,
    TargetPlaylist &targetsWithReplacement,
    AdaptiveTrack::Factory &adaptiveTrackFactory,
    SessionController &sessionController,
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
      targetsWithReplacement{targetsWithReplacement},
      sessionController{sessionController},
      submittingConsonantResponse{submittingConsonantResponse},
      passFailPresenter{passFailPresenter},
      keypressPresenter{keypressPresenter},
      submittingKeyPressResponse{submittingKeyPressResponse},
      emotionPresenter{emotionPresenter},
      childEmotionPresenter{childEmotionPresenter},
      fixedPassFailPresenter{fixedPassFailPresenter} {}
}
