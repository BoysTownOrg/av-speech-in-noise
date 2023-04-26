#ifndef AV_SPEECH_IN_NOISE_TESTS_MODELSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TESTS_MODELSTUB_HPP_

#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/core/IModel.hpp>

#include <utility>

namespace av_speech_in_noise {
class ModelStub : public RunningATestFacade {
  public:
    void setAdaptiveTestResults(AdaptiveTestResults v) {
        adaptiveTestResults_ = std::move(v);
    }

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

    [[nodiscard]] auto trialPlayed() const { return trialPlayed_; }

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

    [[nodiscard]] auto calibration() const -> auto & { return calibration_; }

    auto leftSpeakerCalibration() -> Calibration {
        return leftSpeakerCalibration_;
    }

    auto rightSpeakerCalibration() -> Calibration {
        return rightSpeakerCalibration_;
    }

    auto consonantResponse() { return consonantResponse_; }

    [[nodiscard]] auto adaptiveTestRestartedWhilePreservingCyclicTargets() const
        -> bool {
        return adaptiveTestRestartedWhilePreservingCyclicTargets_;
    }

    [[nodiscard]] auto responseParameters() const -> auto & {
        return responseParameters_;
    }

    [[nodiscard]] auto trialParameters() const -> auto & {
        return trialParameters_;
    }

    void initialize(const AdaptiveTest &p) override {
        adaptiveTest_ = p;
        defaultAdaptiveTestInitialized_ = true;
    }

    void initializeWithCyclicTargets(const AdaptiveTest &p) override {
        adaptiveTest_ = p;
        initializedWithCyclicTargets_ = true;
    }

    void initializeWithTargetReplacement(
        const FixedLevelFixedTrialsTest &p) override {
        fixedLevelFixedTrialsTest_ = p;
        fixedLevelTest_ = p;
        defaultFixedLevelTestInitialized_ = true;
        fixedLevelTestWithTargetReplacementInitialized_ = true;
    }

    void initializeWithSilentIntervalTargets(const FixedLevelTest &p) override {
        fixedLevelTest_ = p;
        fixedLevelTestWithSilentIntervalTargetsInitialized_ = true;
    }

    void initialize(const FixedLevelTestWithEachTargetNTimes &p) override {
        fixedLevelTestWithEachTargetNTimes_ = p;
        fixedLevelTestWithEachTargetNTimesInitialized_ = true;
    }

    void initializeWithAllTargets(const FixedLevelTest &p) override {
        fixedLevelTest_ = p;
        fixedLevelTestWithAllTargetsInitialized_ = true;
    }

    void setAudioDevices(std::vector<std::string> v) {
        audioDevices_ = std::move(v);
    }

    void setTestComplete() { testComplete_ = true; }

    auto testComplete() -> bool override { return testComplete_; }

    void playTrial(const AudioSettings &p) override {
        trialParameters_ = p;
        trialPlayed_ = true;
    }

    auto audioDevices() -> std::vector<std::string> override {
        return audioDevices_;
    }

    void submit(const coordinate_response_measure::Response &p) override {
        responseParameters_ = p;
    }

    void attach(Observer *listener) override { listener_ = listener; }

    void playCalibration(const Calibration &p) override { calibration_ = p; }

    void playLeftSpeakerCalibration(const Calibration &p) override {
        leftSpeakerCalibration_ = p;
    }

    void playRightSpeakerCalibration(const Calibration &p) override {
        rightSpeakerCalibration_ = p;
    }

    void restartAdaptiveTestWhilePreservingTargets() override {
        adaptiveTestRestartedWhilePreservingCyclicTargets_ = true;
    }

    auto adaptiveTestResults() -> AdaptiveTestResults override {
        return adaptiveTestResults_;
    }

    auto threeKeywords() -> ThreeKeywordsResponse { return threeKeywords_; }

    void setKeywordTestResults(KeywordsTestResults k) {
        keywordsTestResults_ = k;
    }

  private:
    AdaptiveTest adaptiveTest_{};
    FixedLevelTest fixedLevelTest_{};
    FixedLevelFixedTrialsTest fixedLevelFixedTrialsTest_{};
    FixedLevelTestWithEachTargetNTimes fixedLevelTestWithEachTargetNTimes_{};
    Calibration calibration_{};
    Calibration leftSpeakerCalibration_{};
    Calibration rightSpeakerCalibration_{};
    AudioSettings trialParameters_{};
    coordinate_response_measure::Response responseParameters_{};
    std::vector<std::string> audioDevices_{};
    AdaptiveTestResults adaptiveTestResults_{};
    ConsonantResponse consonantResponse_{};
    ThreeKeywordsResponse threeKeywords_{};
    KeywordsTestResults keywordsTestResults_{};
    Observer *listener_{};
    bool testComplete_{};
    bool trialPlayed_{};
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
    bool adaptiveTestRestartedWhilePreservingCyclicTargets_{};
};
}

#endif
