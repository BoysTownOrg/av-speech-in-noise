#ifndef AV_SPEECH_IN_NOISE_TESTS_MODELSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TESTS_MODELSTUB_HPP_

#include <av-speech-in-noise/core/IModel.hpp>

#include <utility>

namespace av_speech_in_noise {
class ModelStub : public RunningATestFacade {
  public:
    [[nodiscard]] auto
    fixedLevelTestWithSilentIntervalTargetsInitialized() const {
        return fixedLevelTestWithSilentIntervalTargetsInitialized_;
    }

    [[nodiscard]] auto fixedLevelTestWithAllTargetsInitialized() const {
        return fixedLevelTestWithAllTargetsInitialized_;
    }

    [[nodiscard]] auto
    fixedLevelTestWithPredeterminedTargetsInitialized() const {
        return fixedLevelTestWithPredeterminedTargetsInitialized_;
    }

    [[nodiscard]] auto fixedLevelTestWithTargetReplacementInitialized() const
        -> bool {
        return fixedLevelTestWithTargetReplacementInitialized_;
    }

    [[nodiscard]] auto
    fixedLevelTestWithTargetReplacementAndEyeTrackingInitialized() const {
        return fixedLevelTestWithTargetReplacementAndEyeTrackingInitialized_;
    }

    [[nodiscard]] auto
    fixedLevelTestWithAllTargetsAndEyeTrackingInitialized() const {
        return fixedLevelTestWithAllTargetsAndEyeTrackingInitialized_;
    }

    [[nodiscard]] auto initializedWithSingleSpeaker() const {
        return initializedWithSingleSpeaker_;
    }

    [[nodiscard]] auto fixedLevelTestWithEachTargetNTimesInitialized() const
        -> bool {
        return fixedLevelTestWithEachTargetNTimesInitialized_;
    }

    [[nodiscard]] auto initializedWithDelayedMasker() const {
        return initializedWithDelayedMasker_;
    }

    [[nodiscard]] auto initializedWithCyclicTargets() const -> bool {
        return initializedWithCyclicTargets_;
    }

    [[nodiscard]] auto
    adaptiveTestInitializedWithCyclicTargetsAndEyeTracking() const -> bool {
        return adaptiveTestInitializedWithCyclicTargetsAndEyeTracking_;
    }

    [[nodiscard]] auto adaptiveTestInitializedWithEyeTracking() const {
        return adaptiveTestInitializedWithEyeTracking_;
    }

    [[nodiscard]] auto defaultFixedLevelTestInitialized() const {
        return defaultFixedLevelTestInitialized_;
    }

    [[nodiscard]] auto defaultAdaptiveTestInitialized() const {
        return defaultAdaptiveTestInitialized_;
    }

    [[nodiscard]] auto adaptiveTest() const -> auto & { return adaptiveTest_; }

    [[nodiscard]] auto fixedLevelTest() const -> auto & {
        return fixedLevelTest_;
    }

    [[nodiscard]] auto fixedLevelTestWithEachTargetNTimes() const
        -> const FixedLevelTestWithEachTargetNTimes & {
        return fixedLevelTestWithEachTargetNTimes_;
    }

    [[nodiscard]] auto fixedLevelFixedTrialsTest() const -> auto & {
        return fixedLevelFixedTrialsTest_;
    }

    auto consonantResponse() { return consonantResponse_; }

    auto threeKeywords() -> ThreeKeywordsResponse { return threeKeywords_; }

    void setKeywordTestResults(KeywordsTestResults k) {
        keywordsTestResults_ = k;
    }

  private:
    AdaptiveTest adaptiveTest_{};
    FixedLevelTest fixedLevelTest_{};
    FixedLevelFixedTrialsTest fixedLevelFixedTrialsTest_{};
    FixedLevelTestWithEachTargetNTimes fixedLevelTestWithEachTargetNTimes_{};
    ConsonantResponse consonantResponse_{};
    ThreeKeywordsResponse threeKeywords_{};
    KeywordsTestResults keywordsTestResults_{};
    bool defaultFixedLevelTestInitialized_{};
    bool defaultAdaptiveTestInitialized_{};
    bool fixedLevelTestWithSilentIntervalTargetsInitialized_{};
    bool fixedLevelTestWithAllTargetsInitialized_{};
    bool fixedLevelTestWithPredeterminedTargetsInitialized_{};
    bool fixedLevelTestWithTargetReplacementInitialized_{};
    bool fixedLevelTestWithTargetReplacementAndEyeTrackingInitialized_{};
    bool fixedLevelTestWithAllTargetsAndEyeTrackingInitialized_{};
    bool initializedWithSingleSpeaker_{};
    bool fixedLevelTestWithEachTargetNTimesInitialized_{};
    bool initializedWithDelayedMasker_{};
    bool adaptiveTestInitializedWithEyeTracking_{};
    bool initializedWithCyclicTargets_{};
    bool adaptiveTestInitializedWithCyclicTargetsAndEyeTracking_{};
};
}

#endif
