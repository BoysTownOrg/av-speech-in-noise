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

static void assign(Calibration &calibration, const std::string &key,
    const std::string &value) {
    if (key == name(TestSetting::masker))
        calibration.fileUrl.path = value;
    else if (key == name(TestSetting::maskerLevel))
        calibration.level.dB_SPL = integer(value);
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

static void initialize(Test &test,
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &contents) {
    applyToEachEntry(
        [&](const auto &key, const auto &value) {
            broadcast(configurables, key, value);
        },
        contents);
    test.fullScaleLevel = SessionControllerImpl::fullScaleLevel;
}

static void initialize(
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &contents, const std::function<void(Test &)> &f) {
    Test test;
    av_speech_in_noise::initialize(test, configurables, contents);
    f(test);
}

static void initializeFixedLevelFixedTrialsTest(
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &contents,
    const std::function<void(const FixedLevelFixedTrialsTest &)> &f) {
    FixedLevelFixedTrialsTest test;
    av_speech_in_noise::initialize(test, configurables, contents);
    f(test);
}

static void initialize(AdaptiveMethod &method, TargetPlaylistReader &reader,
    AdaptiveTrack::Factory &factory) {
    method.initialize(&reader, &factory);
}

static void initialize(
    RunningATest &model, TestMethod &method, const Test &test) {
    model.initialize(&method, test);
}

static void initialize(
    FixedLevelMethod &method, FiniteTargetPlaylistWithRepeatables &targets) {
    method.initialize(&targets);
}

static void initialize(FixedLevelMethod &method,
    const FixedLevelFixedTrialsTest &test, TargetPlaylist &targets) {
    method.initialize(test, &targets);
}

void TestSettingsInterpreterImpl::initializeTest(const std::string &contents,
    const TestIdentity &testIdentity, const std::string &startingSnr) {
    broadcast(configurables, "relative output path",
        "Documents/AvSpeechInNoise Data");
    broadcast(configurables, "puzzle", "");
    broadcast(configurables, "subject ID", testIdentity.subjectId);
    broadcast(configurables, "tester ID", testIdentity.testerId);
    broadcast(configurables, "session", testIdentity.session);
    broadcast(configurables, "RME setting", testIdentity.rmeSetting);
    broadcast(configurables, "transducer", testIdentity.transducer);
    broadcast(configurables, "starting SNR (dB)", startingSnr);

    const auto [method, methodName] =
        av_speech_in_noise::methodWithName(contents);
    broadcast(configurables, "method", methodName);

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
        break;
    case Method::adaptivePassFail:
        break;
    case Method::adaptiveCorrectKeywords:
        break;
    case Method::fixedLevelConsonants:
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
        break;
    case Method::unknown:
        break;
    }

    switch (method) {
    case Method::adaptiveCorrectKeywords:
        av_speech_in_noise::initialize(
            configurables, contents, [&](const Test &test) {
                av_speech_in_noise::initialize(
                    adaptiveMethod, cyclicTargetsReader, adaptiveTrackFactory);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test);
            });
        break;
    case Method::adaptiveCoordinateResponseMeasure:
    case Method::adaptivePassFail:
        av_speech_in_noise::initialize(
            configurables, contents, [&](Test &test) {
                av_speech_in_noise::initialize(adaptiveMethod,
                    targetsWithReplacementReader, adaptiveTrackFactory);
                av_speech_in_noise::initialize(
                    runningATest, adaptiveMethod, test);
            });
        break;
    case Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets:
    case Method::fixedLevelFreeResponseWithSilentIntervalTargets:
        av_speech_in_noise::initialize(
            configurables, contents, [&](const Test &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, silentIntervalTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test);
            });
        break;
    case Method::fixedLevelFreeResponseWithAllTargets:
    case Method::fixedLevelChooseKeywordsWithAllTargets:
    case Method::fixedLevelSyllablesWithAllTargets:
        av_speech_in_noise::initialize(
            configurables, contents, [&](const Test &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, everyTargetOnce);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test);
            });
        break;
    case Method::fixedLevelConsonants:
        av_speech_in_noise::initialize(
            configurables, contents, [&](const Test &test) {
                fixedLevelMethod.initialize(&eachTargetNTimes);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test);
            });
        break;
    case Method::fixedLevelEmotionsWithPredeterminedTargets:
    case Method::fixedLevelChildEmotionsWithPredeterminedTargets:
    case Method::fixedLevelFreeResponseWithPredeterminedTargets:
    case Method::fixedLevelPassFailWithPredeterminedTargets:
        av_speech_in_noise::initialize(
            configurables, contents, [&](const Test &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, predeterminedTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test);
            });
        break;
    case Method::fixedLevelButtonResponseWithPredeterminedTargets:
    case Method::fixedLevelButtonThenPassFailResponseWithPredeterminedTargets:
        av_speech_in_noise::initialize(
            configurables, contents, [&](Test &test) {
                test.enableVibrotactileStimulus = true;
                av_speech_in_noise::initialize(
                    fixedLevelMethod, predeterminedTargets);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test);
            });
        break;
    case Method::fixedLevelFreeResponseWithTargetReplacement:
    case Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement:
        av_speech_in_noise::initializeFixedLevelFixedTrialsTest(configurables,
            contents, [&](const FixedLevelFixedTrialsTest &test) {
                av_speech_in_noise::initialize(
                    fixedLevelMethod, test, targetsWithReplacement);
                av_speech_in_noise::initialize(
                    runningATest, fixedLevelMethod, test);
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
    FixedLevelMethod &fixedLevelMethod,
    TargetPlaylistReader &cyclicTargetsReader,
    TargetPlaylistReader &targetsWithReplacementReader,
    FiniteTargetPlaylistWithRepeatables &predeterminedTargets,
    FiniteTargetPlaylistWithRepeatables &everyTargetOnce,
    FiniteTargetPlaylistWithRepeatables &silentIntervalTargets,
    FiniteTargetPlaylist &eachTargetNTimes,
    TargetPlaylist &targetsWithReplacement,
    AdaptiveTrack::Factory &adaptiveTrackFactory,
    SessionController &sessionController, TaskPresenter &keypressPresenter,
    TaskPresenter &emotionPresenter, TaskPresenter &childEmotionPresenter,
    TaskPresenter &fixedPassFailPresenter)
    : runningATest{runningATest}, adaptiveMethod{adaptiveMethod},
      fixedLevelMethod{fixedLevelMethod},
      adaptiveTrackFactory{adaptiveTrackFactory},
      cyclicTargetsReader{cyclicTargetsReader},
      targetsWithReplacementReader{targetsWithReplacementReader},
      predeterminedTargets{predeterminedTargets},
      everyTargetOnce{everyTargetOnce},
      silentIntervalTargets{silentIntervalTargets},
      eachTargetNTimes{eachTargetNTimes},
      targetsWithReplacement{targetsWithReplacement},
      sessionController{sessionController},
      keypressPresenter{keypressPresenter}, emotionPresenter{emotionPresenter},
      childEmotionPresenter{childEmotionPresenter},
      fixedPassFailPresenter{fixedPassFailPresenter} {}
}
