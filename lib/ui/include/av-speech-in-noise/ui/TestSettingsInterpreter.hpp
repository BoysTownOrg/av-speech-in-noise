#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETTINGSINTERPRETER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETTINGSINTERPRETER_HPP_

#include "FreeResponse.hpp"
#include "TestSetupImpl.hpp"
#include "SessionController.hpp"

#include "av-speech-in-noise/core/IAdaptiveMethod.hpp"
#include "av-speech-in-noise/core/IFixedLevelMethod.hpp"
#include "av-speech-in-noise/core/IRecognitionTestModel.hpp"
#include "av-speech-in-noise/core/TargetPlaylist.hpp"

#include <map>

namespace av_speech_in_noise {
enum class Method {
    adaptivePassFail,
    adaptivePassFailWithEyeTracking,
    adaptiveCorrectKeywords,
    adaptiveCorrectKeywordsWithEyeTracking,
    adaptiveCoordinateResponseMeasure,
    adaptiveCoordinateResponseMeasureWithSingleSpeaker,
    adaptiveCoordinateResponseMeasureWithDelayedMasker,
    adaptiveCoordinateResponseMeasureWithEyeTracking,
    fixedLevelFreeResponseWithTargetReplacement,
    fixedLevelFreeResponseWithSilentIntervalTargets,
    fixedLevelFreeResponseWithAllTargets,
    fixedLevelFreeResponseWithAllTargetsAndEyeTracking,
    fixedLevelFreeResponseWithAllTargetsAndAudioRecording,
    fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording,
    fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking,
    fixedLevelCoordinateResponseMeasureWithTargetReplacement,
    fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking,
    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets,
    fixedLevelConsonants,
    fixedLevelChooseKeywordsWithAllTargets,
    fixedLevelSyllablesWithAllTargets,
    unknown
};

constexpr auto name(Method c) -> const char * {
    switch (c) {
    case Method::adaptivePassFail:
        return "adaptive pass fail";
    case Method::adaptivePassFailWithEyeTracking:
        return "adaptive pass fail eye tracking";
    case Method::adaptiveCorrectKeywords:
        return "adaptive number keywords";
    case Method::adaptiveCorrectKeywordsWithEyeTracking:
        return "adaptive number keywords eye tracking";
    case Method::adaptiveCoordinateResponseMeasure:
        return "adaptive CRM";
    case Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker:
        return "adaptive CRM not spatial";
    case Method::adaptiveCoordinateResponseMeasureWithDelayedMasker:
        return "adaptive CRM spatial";
    case Method::adaptiveCoordinateResponseMeasureWithEyeTracking:
        return "adaptive CRM eye tracking";
    case Method::fixedLevelFreeResponseWithTargetReplacement:
        return "fixed-level free response with replacement";
    case Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement:
        return "fixed-level CRM with replacement";
    case Method::
        fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking:
        return "fixed-level CRM with replacement eye tracking";
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
    case Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking:
        return "fixed-level free response all stimuli eye tracking";
    case Method::fixedLevelFreeResponseWithAllTargetsAndAudioRecording:
        return "fixed-level free response all stimuli audio recording";
    case Method::
        fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecording:
        return "fixed-level free response predetermined stimuli audio "
               "recording";
    case Method::fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTracking:
        return "fixed-level free response predetermined stimuli eye "
               "tracking";
    case Method::fixedLevelSyllablesWithAllTargets:
        return "fixed-level syllables all stimuli";
    case Method::unknown:
        return "unknown";
    }
}

enum class TestSetting {
    method,
    targets,
    masker,
    maskerLevel,
    startingSnr,
    condition,
    up,
    down,
    reversalsPerStepSize,
    stepSizes,
    thresholdReversals,
    targetRepetitions,
    subjectId,
    testerId,
    session,
    rmeSetting,
    transducer,
    meta,
    relativeOutputPath,
    keepVideoShown,
    puzzle
};

constexpr auto name(TestSetting p) -> const char * {
    switch (p) {
    case TestSetting::method:
        return "method";
    case TestSetting::targets:
        return "targets";
    case TestSetting::masker:
        return "masker";
    case TestSetting::maskerLevel:
        return "masker level (dB SPL)";
    case TestSetting::startingSnr:
        return "starting SNR (dB)";
    case TestSetting::condition:
        return "condition";
    case TestSetting::up:
        return "up";
    case TestSetting::down:
        return "down";
    case TestSetting::reversalsPerStepSize:
        return "reversals per step size";
    case TestSetting::stepSizes:
        return "step sizes (dB)";
    case TestSetting::thresholdReversals:
        return "threshold";
    case TestSetting::targetRepetitions:
        return "target repetitions";
    case TestSetting::subjectId:
        return "subject ID";
    case TestSetting::testerId:
        return "tester ID";
    case TestSetting::session:
        return "session";
    case TestSetting::rmeSetting:
        return "RME setting";
    case TestSetting::transducer:
        return "transducer";
    case TestSetting::meta:
        return "meta";
    case TestSetting::relativeOutputPath:
        return "relative output path";
    case TestSetting::keepVideoShown:
        return "keep video shown";
    case TestSetting::puzzle:
        return "puzzle";
    }
}

class TestSettingsInterpreterImpl : public TestSettingsInterpreter {
  public:
    TestSettingsInterpreterImpl(std::map<Method, TaskPresenter &>,
        RunningATest &, AdaptiveMethod &, FixedLevelMethod &,
        RunningATest::TestObserver &, RunningATest::TestObserver &,
        TargetPlaylistReader &, TargetPlaylistReader &,
        FiniteTargetPlaylistWithRepeatables &,
        FiniteTargetPlaylistWithRepeatables &,
        FiniteTargetPlaylistWithRepeatables &, RepeatableFiniteTargetPlaylist &,
        TargetPlaylist &, submitting_free_response::Puzzle &,
        FreeResponseController &);
    void initialize(SessionController &, const std::string &,
        const TestIdentity &, SNR) override;
    static auto meta(const std::string &) -> std::string;
    auto calibration(const std::string &) -> Calibration override;

  private:
    std::map<Method, TaskPresenter &> taskPresenters;
    RunningATest &runningATest;
    AdaptiveMethod &adaptiveMethod;
    FixedLevelMethod &fixedLevelMethod;
    RunningATest::TestObserver &eyeTracking;
    RunningATest::TestObserver &audioRecording;
    TargetPlaylistReader &cyclicTargetsReader;
    TargetPlaylistReader &targetsWithReplacementReader;
    FiniteTargetPlaylistWithRepeatables &predeterminedTargets;
    FiniteTargetPlaylistWithRepeatables &everyTargetOnce;
    FiniteTargetPlaylistWithRepeatables &silentIntervalTargets;
    RepeatableFiniteTargetPlaylist &eachTargetNTimes;
    TargetPlaylist &targetsWithReplacement;
    submitting_free_response::Puzzle &puzzle;
    FreeResponseController &freeResponseController;
};
}

#endif
