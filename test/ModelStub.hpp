#ifndef AV_SPEECH_IN_NOISE_TESTS_MODELSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TESTS_MODELSTUB_HPP_

#include <av-speech-in-noise/Model.hpp>
#include <utility>

namespace av_speech_in_noise {
class ModelStub : public Model {
  public:
    void setAdaptiveTestResults(AdaptiveTestResults v) {
        adaptiveTestResults_ = std::move(v);
    }

    auto trialNumber() -> int override { return trialNumber_; }

    void setTrialNumber(int n) { trialNumber_ = n; }

    void setTargetFileName(std::string s) { targetFileName_ = std::move(s); }

    auto targetFileName() -> std::string override { return targetFileName_; }

    [[nodiscard]] auto
    fixedLevelTestWithSilentIntervalTargetsInitialized() const {
        return fixedLevelTestWithSilentIntervalTargetsInitialized_;
    }

    [[nodiscard]] auto fixedLevelTestWithAllTargetsInitialized() const {
        return fixedLevelTestWithAllTargetsInitialized_;
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

    [[nodiscard]] auto incorrectResponseSubmitted() const {
        return incorrectResponseSubmitted_;
    }

    [[nodiscard]] auto correctResponseSubmitted() const {
        return correctResponseSubmitted_;
    }

    [[nodiscard]] auto freeResponse() const { return freeResponse_; }

    auto consonantResponse() { return consonantResponse_; }

    [[nodiscard]] auto correctKeywords() const -> int {
        return correctKeywords_.count;
    }

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

    void initializeWithSingleSpeaker(const AdaptiveTest &p) override {
        adaptiveTest_ = p;
        initializedWithSingleSpeaker_ = true;
    }

    void initializeWithDelayedMasker(const AdaptiveTest &p) override {
        adaptiveTest_ = p;
        initializedWithDelayedMasker_ = true;
    }

    void initializeWithEyeTracking(const AdaptiveTest &p) override {
        adaptiveTest_ = p;
        adaptiveTestInitializedWithEyeTracking_ = true;
    }

    void initializeWithCyclicTargets(const AdaptiveTest &p) override {
        adaptiveTest_ = p;
        initializedWithCyclicTargets_ = true;
    }

    void initializeWithCyclicTargetsAndEyeTracking(
        const AdaptiveTest &p) override {
        adaptiveTest_ = p;
        adaptiveTestInitializedWithCyclicTargetsAndEyeTracking_ = true;
    }

    void initializeWithTargetReplacement(
        const FixedLevelFixedTrialsTest &p) override {
        fixedLevelFixedTrialsTest_ = p;
        defaultFixedLevelTestInitialized_ = true;
    }

    void initializeWithTargetReplacementAndEyeTracking(
        const FixedLevelFixedTrialsTest &p) override {
        fixedLevelFixedTrialsTest_ = p;
        fixedLevelTestWithTargetReplacementAndEyeTrackingInitialized_ = true;
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

    void initializeWithAllTargetsAndEyeTracking(
        const FixedLevelTest &p) override {
        fixedLevelTest_ = p;
        fixedLevelTestWithAllTargetsAndEyeTrackingInitialized_ = true;
    }

    void completeTrial() { listener_->trialComplete(); }

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

    void submitCorrectResponse() override { correctResponseSubmitted_ = true; }

    void submitIncorrectResponse() override {
        incorrectResponseSubmitted_ = true;
    }

    void submit(const FreeResponse &s) override { freeResponse_ = s; }

    void submit(const CorrectKeywords &s) override { correctKeywords_ = s; }

    void submit(const ConsonantResponse &s) override { consonantResponse_ = s; }

    void restartAdaptiveTestWhilePreservingTargets() override {
        adaptiveTestRestartedWhilePreservingCyclicTargets_ = true;
    }

    auto adaptiveTestResults() -> AdaptiveTestResults override {
        return adaptiveTestResults_;
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
    FreeResponse freeResponse_{};
    ConsonantResponse consonantResponse_{};
    CorrectKeywords correctKeywords_{};
    std::string targetFileName_{};
    Observer *listener_{};
    int trialNumber_{};
    bool testComplete_{};
    bool trialPlayed_{};
    bool defaultFixedLevelTestInitialized_{};
    bool defaultAdaptiveTestInitialized_{};
    bool fixedLevelTestWithSilentIntervalTargetsInitialized_{};
    bool fixedLevelTestWithAllTargetsInitialized_{};
    bool fixedLevelTestWithTargetReplacementAndEyeTrackingInitialized_{};
    bool fixedLevelTestWithAllTargetsAndEyeTrackingInitialized_{};
    bool initializedWithSingleSpeaker_{};
    bool fixedLevelTestWithEachTargetNTimesInitialized_{};
    bool initializedWithDelayedMasker_{};
    bool adaptiveTestInitializedWithEyeTracking_{};
    bool correctResponseSubmitted_{};
    bool incorrectResponseSubmitted_{};
    bool initializedWithCyclicTargets_{};
    bool adaptiveTestInitializedWithCyclicTargetsAndEyeTracking_{};
    bool adaptiveTestRestartedWhilePreservingCyclicTargets_{};
};
}

#endif
