#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETTINGSINTERPRETER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETTINGSINTERPRETER_HPP_

#include "Task.hpp"
#include "TestSetupImpl.hpp"

#include <av-speech-in-noise/core/Configuration.hpp>
#include <av-speech-in-noise/core/IAdaptiveMethod.hpp>
#include <av-speech-in-noise/core/IFixedLevelMethod.hpp>
#include <av-speech-in-noise/core/IRunningATest.hpp>
#include <av-speech-in-noise/core/TargetPlaylist.hpp>

#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <vector>

namespace av_speech_in_noise {
enum class Method : std::uint8_t {
    adaptivePassFail, // <-- this one should be first...
    adaptiveCorrectKeywords,
    adaptiveCoordinateResponseMeasure,
    fixedLevelPassFailWithPredeterminedTargets,
    fixedLevelFreeResponseWithTargetReplacement,
    fixedLevelFreeResponseWithSilentIntervalTargets,
    fixedLevelFreeResponseWithAllTargets,
    fixedLevelFreeResponseWithPredeterminedTargets,
    fixedLevelButtonResponseWithPredeterminedTargets,
    fixedLevelButtonThenPassFailResponseWithPredeterminedTargets,
    fixedLevelCoordinateResponseMeasureWithTargetReplacement,
    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets,
    fixedLevelConsonants,
    fixedLevelEmotionsWithPredeterminedTargets,
    fixedLevelChildEmotionsWithPredeterminedTargets,
    fixedLevelChooseKeywordsWithAllTargets,
    fixedLevelSyllablesWithAllTargets, // <-- this one should be last...
    unknown
};

// None of these should be prefixes of another...
constexpr auto name(Method c) -> const char * {
    switch (c) {
    case Method::adaptivePassFail:
        return "adaptive pass fail";
    case Method::adaptiveCorrectKeywords:
        return "adaptive number keywords";
    case Method::adaptiveCoordinateResponseMeasure:
        return "adaptive CRM";
    case Method::fixedLevelFreeResponseWithTargetReplacement:
        return "fixed-level free response with replacement";
    case Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement:
        return "fixed-level CRM with replacement";
    case Method::fixedLevelFreeResponseWithSilentIntervalTargets:
        return "fixed-level free response silent intervals";
    case Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets:
        return "fixed-level CRM silent intervals";
    case Method::fixedLevelFreeResponseWithAllTargets:
        return "fixed-level free response all stimuli";
    case Method::fixedLevelConsonants:
        return "fixed-level consonants";
    case Method::fixedLevelChooseKeywordsWithAllTargets:
        return "fixed-level choose keywords all stimuli";
    case Method::fixedLevelFreeResponseWithPredeterminedTargets:
        return "fixed-level free response predetermined stimuli";
    case Method::fixedLevelButtonResponseWithPredeterminedTargets:
        return "fixed-level button response predetermined stimuli";
    case Method::fixedLevelButtonThenPassFailResponseWithPredeterminedTargets:
        return "fixed-level button response pass fail predetermined stimuli";
    case Method::fixedLevelSyllablesWithAllTargets:
        return "fixed-level syllables all stimuli";
    case Method::fixedLevelEmotionsWithPredeterminedTargets:
        return "fixed-level emotions predetermined stimuli";
    case Method::fixedLevelChildEmotionsWithPredeterminedTargets:
        return "fixed-level child emotions predetermined stimuli";
    case Method::fixedLevelPassFailWithPredeterminedTargets:
        return "fixed-level pass fail predetermined stimuli";
    case Method::unknown:
        return "unknown";
    }
}

enum class TestSetting : std::uint8_t {
    method,
    masker,
    maskerLevel,
    startingSnr,
};

constexpr auto name(TestSetting p) -> const char * {
    switch (p) {
    case TestSetting::method:
        return "method";
    case TestSetting::masker:
        return "masker";
    case TestSetting::maskerLevel:
        return "masker level (dB SPL)";
    case TestSetting::startingSnr:
        return "starting SNR (dB)";
    }
}

class TestSettingsInterpreterImpl : public TestSettingsInterpreter,
                                    public ConfigurationRegistry {
  public:
    TestSettingsInterpreterImpl(RunningATest &runningATest,
        AdaptiveMethod &adaptiveMethod, FixedLevelMethod &fixedLevelMethod,
        RunningATest::TestObserver &eyeTracking,
        RunningATest::TestObserver &audioRecording,
        TargetPlaylistReader &cyclicTargetsReader,
        TargetPlaylistReader &targetsWithReplacementReader,
        FiniteTargetPlaylistWithRepeatables &predeterminedTargets,
        FiniteTargetPlaylistWithRepeatables &everyTargetOnce,
        FiniteTargetPlaylistWithRepeatables &silentIntervalTargets,
        FiniteTargetPlaylist &eachTargetNTimes,
        TargetPlaylist &targetsWithReplacement, AdaptiveTrack::Factory &,
        SessionController &sessionController, TaskPresenter &syllablesPresenter,
        TaskPresenter &consonantPresenter,
        RunningATest::TestObserver &submittingConsonantResponse,
        TaskPresenter &passFailPresenter, TaskPresenter &keypressPresenter,
        RunningATest::TestObserver &submittingKeyPressResponse,
        TaskPresenter &emotionPresenter, TaskPresenter &childEmotionPresenter,
        TaskPresenter &fixedPassFailPresenter);
    void initializeTest(
        const std::string &, const TestIdentity &, SNR) override;
    static auto meta(const std::string &) -> std::string;
    auto calibration(const std::string &) -> Calibration override;
    void subscribe(Configurable &c, const std::string &key) override {
        configurables[key].emplace_back(c);
    }

  private:
    std::map<std::string, std::vector<std::reference_wrapper<Configurable>>>
        configurables;
    RunningATest &runningATest;
    AdaptiveMethod &adaptiveMethod;
    FixedLevelMethod &fixedLevelMethod;
    RunningATest::TestObserver &eyeTracking;
    RunningATest::TestObserver &audioRecording;
    AdaptiveTrack::Factory &adaptiveTrackFactory;
    TargetPlaylistReader &cyclicTargetsReader;
    TargetPlaylistReader &targetsWithReplacementReader;
    FiniteTargetPlaylistWithRepeatables &predeterminedTargets;
    FiniteTargetPlaylistWithRepeatables &everyTargetOnce;
    FiniteTargetPlaylistWithRepeatables &silentIntervalTargets;
    FiniteTargetPlaylist &eachTargetNTimes;
    TargetPlaylist &targetsWithReplacement;
    SessionController &sessionController;
    TaskPresenter &syllablesPresenter;
    TaskPresenter &consonantPresenter;
    RunningATest::TestObserver &submittingConsonantResponse;
    TaskPresenter &passFailPresenter;
    TaskPresenter &keypressPresenter;
    RunningATest::TestObserver &submittingKeyPressResponse;
    TaskPresenter &emotionPresenter;
    TaskPresenter &childEmotionPresenter;
    TaskPresenter &fixedPassFailPresenter;
};
}

#endif
