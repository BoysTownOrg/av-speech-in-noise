#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETTINGSINTERPRETER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETTINGSINTERPRETER_HPP_

#include "Presenter.hpp"

namespace av_speech_in_noise {
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
    case Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking:
        return "fixed-level free response all stimuli eye tracking";
    case Method::unknown:
        return "unknown";
    }
}

enum class TestSetting {
    method,
    targets,
    masker,
    maskerLevel,
    condition,
    up,
    down,
    reversalsPerStepSize,
    stepSizes,
    thresholdReversals,
    targetRepetitions
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
    }
}

class TestSettingsInterpreterImpl : public TestSettingsInterpreter {
  public:
    void initialize(
        Model &, const std::string &, const TestIdentity &, SNR) override;
    auto method(const std::string &) -> Method override;
    auto calibration(const std::string &) -> Calibration override;
};
}

#endif