#include "OutputFileStub.hpp"
#include "ModelObserverStub.hpp"
#include "TargetPlaylistStub.hpp"
#include "TargetPlaylistSetReaderStub.hpp"
#include "AudioRecorderStub.hpp"
#include "assert-utility.hpp"
#include "av-speech-in-noise/Model.hpp"

#include <av-speech-in-noise/core/SubmittingConsonant.hpp>
#include <av-speech-in-noise/core/SubmittingFreeResponse.hpp>
#include <av-speech-in-noise/core/SubmittingPassFail.hpp>
#include <av-speech-in-noise/core/SubmittingKeywords.hpp>
#include <av-speech-in-noise/core/SubmittingNumberKeywords.hpp>
#include <av-speech-in-noise/core/SubmittingSyllable.hpp>
#include <av-speech-in-noise/core/Model.hpp>

#include <gtest/gtest.h>

#include <sstream>
#include <utility>

namespace av_speech_in_noise {
static auto operator==(const AdaptiveTestResult &a, const AdaptiveTestResult &b)
    -> bool {
    return a.targetsUrl.path == b.targetsUrl.path && a.threshold == b.threshold;
}

class RunningATestObserverStub : public RunningATest::Observer {
  public:
    void notifyThatNewTestIsReady(std::string_view session) override {}

    void notifyThatTrialWillBegin(int trialNumber) override {}

    void notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &) override {}

    void notifyThatStimulusHasEnded() override {}

    void notifyThatSubjectHasResponded() override {}
};

namespace {
class AdaptiveMethodStub : public AdaptiveMethod {
  public:
    [[nodiscard]] auto tracksResetted() const -> bool {
        return tracksResetted_;
    }

    void initialize(
        const AdaptiveTest &t, TargetPlaylistReader *reader) override {
        test_ = &t;
        targetListReader_ = reader;
    }

    [[nodiscard]] auto test() const { return test_; }

    [[nodiscard]] auto targetListReader() const -> TargetPlaylistReader * {
        return targetListReader_;
    }

    void setTestResults(AdaptiveTestResults v) { testResults_ = std::move(v); }

    auto testResults() -> AdaptiveTestResults override { return testResults_; }

    void resetTracks() override { tracksResetted_ = true; }

    auto log() -> const std::stringstream & { return log_; }

    void log(const std::string &s) { log_ << s; }

    auto complete() -> bool override { return {}; }
    auto nextTarget() -> LocalUrl override {
        log_ << "nextTarget ";
        return {};
    }
    auto currentTarget() -> LocalUrl override { return {}; }
    auto snr() -> SNR override { return SNR{}; }
    void submitCorrectResponse() override { log_ << "submitCorrectResponse "; }
    void submitIncorrectResponse() override {
        log_ << "submitIncorrectResponse ";
    }
    void submit(const CorrectKeywords &) override {
        log_ << "submitCorrectKeywords ";
    }
    void writeTestingParameters(OutputFile &) override {}
    void writeLastCoordinateResponse(OutputFile &) override {}
    void writeLastCorrectResponse(OutputFile &) override {
        log_ << "writeLastCorrectResponse ";
    }
    void writeLastIncorrectResponse(OutputFile &) override {
        log_ << "writeLastIncorrectResponse ";
    }
    void writeLastCorrectKeywords(OutputFile &) override {
        log_ << "writeLastCorrectKeywords ";
    }
    void writeTestResult(OutputFile &) override {}
    void submit(const coordinate_response_measure::Response &) override {}

  private:
    AdaptiveTestResults testResults_;
    std::stringstream log_{};
    const AdaptiveTest *test_{};
    TargetPlaylistReader *targetListReader_{};
    bool tracksResetted_{};
};

class FixedLevelMethodStub : public FixedLevelMethod {
  public:
    void initialize(
        const FixedLevelFixedTrialsTest &t, TargetPlaylist *list) override {
        targetList_ = list;
        fixedTrialsTest_ = &t;
    }

    void initialize(const FixedLevelTest &t,
        FiniteTargetPlaylistWithRepeatables *list) override {
        targetList_ = list;
        test_ = &t;
    }

    void initialize(
        const FixedLevelTest &t, FiniteTargetPlaylist *list) override {
        targetList_ = list;
        test_ = &t;
    }

    [[nodiscard]] auto targetList() const { return targetList_; }

    [[nodiscard]] auto test() const { return test_; }

    auto fixedTrialsTest() -> const FixedLevelFixedTrialsTest * {
        return fixedTrialsTest_;
    }

    auto submittedConsonant() const -> bool { return submittedConsonant_; }

    auto log() -> const std::stringstream & { return log_; }

    void log(const std::string &s) { log_ << s; }

    auto complete() -> bool override { return {}; }

    auto nextTarget() -> LocalUrl override {
        log_ << "nextTarget ";
        return {};
    }

    auto currentTarget() -> LocalUrl override { return currentTarget_; }

    void setCurrentTargetPath(std::string s) {
        currentTarget_.path = std::move(s);
    }

    auto snr() -> SNR override { return SNR{}; }

    void submit(const FreeResponse &) override {
        submittedFreeResponse_ = true;
    }

    auto submittedFreeResponse() const -> bool {
        return submittedFreeResponse_;
    }

    void submit(const ConsonantResponse &) override {
        submittedConsonant_ = true;
        log_ << "submitConsonant ";
    }

    void writeTestingParameters(OutputFile &) override {}

    void writeLastCoordinateResponse(OutputFile &) override {}

    void writeLastConsonant(OutputFile &) override {
        log_ << "writeLastConsonant ";
    }

    void writeTestResult(OutputFile &) override {}

    void submit(const coordinate_response_measure::Response &) override {}

    auto keywordsTestResults() -> KeywordsTestResults override {
        return keywordsTestResults_;
    }

    void setKeywordsTestResults(const KeywordsTestResults &r) {
        keywordsTestResults_ = r;
    }

    void submit(const ThreeKeywordsResponse &r) override {
        threeKeywords_ = &r;
    }

    auto threeKeywords() -> const ThreeKeywordsResponse * {
        return threeKeywords_;
    }

    auto syllableResponse() -> const SyllableResponse * {
        return syllableResponse_;
    }

    void submit(const SyllableResponse &r) override { syllableResponse_ = &r; }

  private:
    KeywordsTestResults keywordsTestResults_{};
    std::stringstream log_{};
    LocalUrl currentTarget_;
    const ThreeKeywordsResponse *threeKeywords_{};
    const SyllableResponse *syllableResponse_{};
    const FixedLevelTest *test_{};
    const FixedLevelFixedTrialsTest *fixedTrialsTest_{};
    TargetPlaylist *targetList_{};
    bool submittedConsonant_{};
    bool submittedFreeResponse_{};
};

class RecognitionTestModelStub : public RunningATest {
  public:
    explicit RecognitionTestModelStub(AdaptiveMethodStub &adaptiveMethod,
        FixedLevelMethodStub &fixedLevelMethodStub)
        : adaptiveMethod{adaptiveMethod}, fixedLevelMethodStub{
                                              fixedLevelMethodStub} {}

    [[nodiscard]] auto nextTrialPreparedIfNeeded() const -> bool {
        return nextTrialPreparedIfNeeded_;
    }

    void prepareNextTrialIfNeeded() override {
        nextTrialPreparedIfNeeded_ = true;
        fixedLevelMethodStub.setCurrentTargetPath("TOOLATE");
        adaptiveMethod.log("TOOLATE ");
        fixedLevelMethodStub.log("TOOLATE ");
    }

    void initialize(
        TestMethod *method, const Test &test, Observer *observer) override {
        testMethod_ = method;
        test_ = &test;
        this->observer = observer;
    }

    void initializeWithSingleSpeaker(
        TestMethod *method, const Test &test) override {
        testMethod_ = method;
        test_ = &test;
        initializedWithSingleSpeaker_ = true;
    }

    void initializeWithDelayedMasker(
        TestMethod *method, const Test &test) override {
        testMethod_ = method;
        test_ = &test;
        initializedWithDelayedMasker_ = true;
    }

    auto trialNumber() -> int override { return trialNumber_; }

    void setTrialNumber(int n) { trialNumber_ = n; }

    void setTargetFileName(std::string s) { targetFileName_ = std::move(s); }

    auto targetFileName() -> std::string override { return targetFileName_; }

    void playTrial(const AudioSettings &s) override { playTrialSettings_ = &s; }

    void submit(const coordinate_response_measure::Response &p) override {
        coordinateResponse_ = &p;
    }

    auto testComplete() -> bool override { return complete_; }

    auto audioDevices() -> std::vector<std::string> override {
        return audioDevices_;
    }

    void attach(RunningATestFacade::Observer *e) override { listener_ = e; }

    void playCalibration(const Calibration &c) override { calibration_ = &c; }

    void playLeftSpeakerCalibration(const Calibration &c) override {
        leftSpeakerCalibration_ = &c;
    }

    void playRightSpeakerCalibration(const Calibration &c) override {
        rightSpeakerCalibration_ = &c;
    }

    [[nodiscard]] auto initializedWithSingleSpeaker() const -> bool {
        return initializedWithSingleSpeaker_;
    }

    [[nodiscard]] auto initializedWithDelayedMasker() const -> bool {
        return initializedWithDelayedMasker_;
    }

    [[nodiscard]] auto coordinateResponse() const {
        return coordinateResponse_;
    }

    [[nodiscard]] auto testMethod() const { return testMethod_; }

    [[nodiscard]] auto test() const { return test_; }

    [[nodiscard]] auto playTrialSettings() const { return playTrialSettings_; }

    [[nodiscard]] auto calibration() const { return calibration_; }

    auto leftSpeakerCalibration() -> const Calibration * {
        return leftSpeakerCalibration_;
    }

    auto rightSpeakerCalibration() -> const Calibration * {
        return rightSpeakerCalibration_;
    }

    [[nodiscard]] auto listener() const { return listener_; }

    void setComplete() { complete_ = true; }

    void setAudioDevices(std::vector<std::string> v) {
        audioDevices_ = std::move(v);
    }

    auto playTrialTime() -> std::string override { return playTrialTime_; }

    void setPlayTrialTime(std::string s) { playTrialTime_ = std::move(s); }

    const Observer *observer{};

  private:
    std::vector<std::string> audioDevices_{};
    std::string targetFileName_{};
    std::string playTrialTime_;
    AdaptiveMethodStub &adaptiveMethod;
    FixedLevelMethodStub &fixedLevelMethodStub;
    const RunningATestFacade::Observer *listener_{};
    const Calibration *calibration_{};
    const Calibration *leftSpeakerCalibration_{};
    const Calibration *rightSpeakerCalibration_{};
    const AudioSettings *playTrialSettings_{};
    const Test *test_{};
    const TestMethod *testMethod_{};
    const coordinate_response_measure::Response *coordinateResponse_{};
    int trialNumber_{};
    bool complete_{};
    bool initializedWithSingleSpeaker_{};
    bool initializedWithDelayedMasker_{};
    bool nextTrialPreparedIfNeeded_{};
};

class InitializingTestUseCase {
  public:
    virtual ~InitializingTestUseCase() = default;
    virtual void run(RunningATestFacadeImpl &) = 0;
    virtual auto test() -> const Test & = 0;
    virtual auto testMethod() -> const TestMethod * = 0;
};

class InitializingFixedLevelTest : public virtual InitializingTestUseCase {
  public:
    virtual auto fixedLevelTest() -> const FixedLevelTest & = 0;
};

class InitializingFixedLevelFixedTrialsTest
    : public virtual InitializingTestUseCase {
  public:
    virtual auto fixedLevelFixedTrialsTest()
        -> const FixedLevelFixedTrialsTest & = 0;
};

class InitializingAdaptiveTest : public virtual InitializingTestUseCase {
  public:
    virtual void run(
        RunningATestFacadeImpl &model, const AdaptiveTest &test) = 0;
};

void initialize(RunningATestFacadeImpl &model, const AdaptiveTest &test) {
    model.initialize(test);
}

void initializeWithTargetReplacement(
    RunningATestFacadeImpl &model, const FixedLevelFixedTrialsTest &test) {
    model.initializeWithTargetReplacement(test);
}

void initializeWithSingleSpeaker(
    RunningATestFacadeImpl &model, const AdaptiveTest &test) {
    model.initializeWithSingleSpeaker(test);
}

void initializeWithDelayedMasker(
    RunningATestFacadeImpl &model, const AdaptiveTest &test) {
    model.initializeWithDelayedMasker(test);
}

void initializeWithCyclicTargets(
    RunningATestFacadeImpl &model, const AdaptiveTest &test) {
    model.initializeWithCyclicTargets(test);
}

void initializeWithCyclicTargetsAndEyeTracking(
    RunningATestFacadeImpl &model, const AdaptiveTest &test) {
    model.initializeWithCyclicTargetsAndEyeTracking(test);
}

void initializeWithSilentIntervalTargets(
    RunningATestFacadeImpl &model, const FixedLevelTest &test) {
    model.initializeWithSilentIntervalTargets(test);
}

void initializeWithTargetReplacementAndEyeTracking(
    RunningATestFacadeImpl &model, const FixedLevelFixedTrialsTest &test) {
    model.initializeWithTargetReplacementAndEyeTracking(test);
}

void initializeWithEyeTracking(
    RunningATestFacadeImpl &model, const AdaptiveTest &test) {
    model.initializeWithEyeTracking(test);
}

class InitializingDefaultAdaptiveTest : public InitializingAdaptiveTest {
    AdaptiveTest test_;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingDefaultAdaptiveTest(AdaptiveMethodStub *method)
        : method{method} {}

    void run(RunningATestFacadeImpl &model) override {
        initialize(model, test_);
    }

    void run(RunningATestFacadeImpl &model, const AdaptiveTest &test) override {
        initialize(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingAdaptiveTestWithEyeTracking
    : public InitializingAdaptiveTest {
    AdaptiveTest test_;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingAdaptiveTestWithEyeTracking(AdaptiveMethodStub *method)
        : method{method} {}

    void run(RunningATestFacadeImpl &model) override {
        initializeWithEyeTracking(model, test_);
    }

    void run(RunningATestFacadeImpl &model, const AdaptiveTest &test) override {
        initializeWithEyeTracking(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingAdaptiveTestWithSingleSpeaker
    : public InitializingAdaptiveTest {
    AdaptiveTest test_;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingAdaptiveTestWithSingleSpeaker(
        AdaptiveMethodStub *method)
        : method{method} {}

    void run(RunningATestFacadeImpl &model) override {
        initializeWithSingleSpeaker(model, test_);
    }

    void run(RunningATestFacadeImpl &model, const AdaptiveTest &test) override {
        initializeWithSingleSpeaker(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingAdaptiveTestWithDelayedMasker
    : public InitializingAdaptiveTest {
    AdaptiveTest test_;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingAdaptiveTestWithDelayedMasker(
        AdaptiveMethodStub *method)
        : method{method} {}

    void run(RunningATestFacadeImpl &model) override {
        initializeWithDelayedMasker(model, test_);
    }

    void run(RunningATestFacadeImpl &model, const AdaptiveTest &test) override {
        initializeWithDelayedMasker(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingAdaptiveTestWithCyclicTargets
    : public InitializingAdaptiveTest {
    AdaptiveTest test_;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingAdaptiveTestWithCyclicTargets(
        AdaptiveMethodStub *method)
        : method{method} {}

    void run(RunningATestFacadeImpl &model) override {
        initializeWithCyclicTargets(model, test_);
    }

    void run(RunningATestFacadeImpl &model, const AdaptiveTest &test) override {
        initializeWithCyclicTargets(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingAdaptiveTestWithCyclicTargetsAndEyeTracking
    : public InitializingAdaptiveTest {
    AdaptiveTest test_;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingAdaptiveTestWithCyclicTargetsAndEyeTracking(
        AdaptiveMethodStub *method)
        : method{method} {}

    void run(RunningATestFacadeImpl &model) override {
        initializeWithCyclicTargetsAndEyeTracking(model, test_);
    }

    void run(RunningATestFacadeImpl &model, const AdaptiveTest &test) override {
        initializeWithCyclicTargetsAndEyeTracking(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithTargetReplacement
    : public InitializingFixedLevelFixedTrialsTest {
    FixedLevelFixedTrialsTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithTargetReplacement(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(RunningATestFacadeImpl &model) override {
        initializeWithTargetReplacement(model, test_);
    }

    auto fixedLevelFixedTrialsTest()
        -> const FixedLevelFixedTrialsTest & override {
        return test_;
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithSilentIntervalTargets
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithSilentIntervalTargets(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(RunningATestFacadeImpl &model) override {
        initializeWithSilentIntervalTargets(model, test_);
    }

    auto fixedLevelTest() -> const FixedLevelTest & override { return test_; }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithTargetReplacementAndEyeTracking
    : public InitializingFixedLevelFixedTrialsTest {
    FixedLevelFixedTrialsTest test_{};
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithTargetReplacementAndEyeTracking(
        FixedLevelMethodStub *method)
        : method{method} {
        test_.peripheral = TestPeripheral::eyeTracking;
    }

    void run(RunningATestFacadeImpl &model) override {
        model.initializeWithTargetReplacement(test_);
    }

    auto fixedLevelFixedTrialsTest()
        -> const FixedLevelFixedTrialsTest & override {
        return test_;
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithSilentIntervalTargetsAndEyeTracking
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithSilentIntervalTargetsAndEyeTracking(
        FixedLevelMethodStub *method)
        : method{method} {
        test_.peripheral = TestPeripheral::eyeTracking;
    }

    void run(RunningATestFacadeImpl &model) override {
        model.initializeWithSilentIntervalTargets(test_);
    }

    auto fixedLevelTest() -> const FixedLevelTest & override { return test_; }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithAllTargets
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithAllTargets(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(RunningATestFacadeImpl &model) override {
        model.initializeWithAllTargets(test_);
    }

    auto fixedLevelTest() -> const FixedLevelTest & override { return test_; }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithAllTargetsAndEyeTracking
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithAllTargetsAndEyeTracking(
        FixedLevelMethodStub *method)
        : method{method} {
        test_.peripheral = TestPeripheral::eyeTracking;
    }

    void run(RunningATestFacadeImpl &model) override {
        model.initializeWithAllTargets(test_);
    }

    auto fixedLevelTest() -> const FixedLevelTest & override { return test_; }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithEachTargetNTimes
    : public InitializingTestUseCase {
    FixedLevelTestWithEachTargetNTimes test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithEachTargetNTimes(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(RunningATestFacadeImpl &model) override {
        model.initialize(test_);
    }

    void run(RunningATestFacadeImpl &model,
        const FixedLevelTestWithEachTargetNTimes &test) {
        model.initialize(test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithAllTargetsAndAudioRecording
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithAllTargetsAndAudioRecording(
        FixedLevelMethodStub *method)
        : method{method} {
        test_.peripheral = TestPeripheral::audioRecording;
    }

    void run(RunningATestFacadeImpl &model) override {
        model.initializeWithAllTargets(test_);
    }

    auto fixedLevelTest() -> const FixedLevelTest & override { return test_; }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithPredeterminedTargetsAndAudioRecording
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithPredeterminedTargetsAndAudioRecording(
        FixedLevelMethodStub *method)
        : method{method} {
        test_.peripheral = TestPeripheral::audioRecording;
    }

    void run(RunningATestFacadeImpl &model) override {
        model.initializeWithPredeterminedTargets(test_);
    }

    auto fixedLevelTest() -> const FixedLevelTest & override { return test_; }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithPredeterminedTargetsAndEyeTracking
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithPredeterminedTargetsAndEyeTracking(
        FixedLevelMethodStub *method)
        : method{method} {
        test_.peripheral = TestPeripheral::eyeTracking;
    }

    void run(RunningATestFacadeImpl &model) override {
        model.initializeWithPredeterminedTargets(test_);
    }

    auto fixedLevelTest() -> const FixedLevelTest & override { return test_; }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

auto initializedWithEyeTracking(
    RecognitionTestModelStub &m, RunningATest::Observer *observer) -> bool {
    return m.observer == observer;
}

class PlayingCalibrationUseCase {
  public:
    virtual ~PlayingCalibrationUseCase() = default;
    virtual void run(RunningATestFacade &model, const Calibration &c) = 0;
    virtual auto calibration(RecognitionTestModelStub &model)
        -> const Calibration * = 0;
};

class PlayingCalibration : public PlayingCalibrationUseCase {
  public:
    void run(RunningATestFacade &model, const Calibration &c) override {
        model.playCalibration(c);
    }

    auto calibration(RecognitionTestModelStub &model)
        -> const Calibration * override {
        return model.calibration();
    }
};

class PlayingLeftSpeakerCalibration : public PlayingCalibrationUseCase {
  public:
    void run(RunningATestFacade &model, const Calibration &c) override {
        model.playLeftSpeakerCalibration(c);
    }

    auto calibration(RecognitionTestModelStub &model)
        -> const Calibration * override {
        return model.leftSpeakerCalibration();
    }
};

class PlayingRightSpeakerCalibration : public PlayingCalibrationUseCase {
  public:
    void run(RunningATestFacade &model, const Calibration &c) override {
        model.playRightSpeakerCalibration(c);
    }

    auto calibration(RecognitionTestModelStub &model)
        -> const Calibration * override {
        return model.rightSpeakerCalibration();
    }
};

class ModelTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub adaptiveMethod;
    FixedLevelMethodStub fixedLevelMethod;
    TargetPlaylistStub targetsWithReplacement;
    TargetPlaylistSetReaderStub targetsWithReplacementReader;
    TargetPlaylistSetReaderStub cyclicTargetsReader;
    FiniteTargetPlaylistWithRepeatablesStub silentIntervals;
    FiniteTargetPlaylistWithRepeatablesStub everyTargetOnce;
    FiniteTargetPlaylistWithRepeatablesStub predeterminedTargets;
    RepeatableFiniteTargetPlaylistStub eachTargetNTimes;
    RecognitionTestModelStub internalModel{adaptiveMethod, fixedLevelMethod};
    OutputFileStub outputFile;
    RunningATestObserverStub audioRecording;
    RunningATestObserverStub eyeTracking;
    RunningATestFacadeImpl model{adaptiveMethod, fixedLevelMethod,
        targetsWithReplacementReader, cyclicTargetsReader,
        targetsWithReplacement, silentIntervals, everyTargetOnce,
        eachTargetNTimes, predeterminedTargets, internalModel, outputFile,
        audioRecording, eyeTracking};
    AdaptiveTest adaptiveTest;
    FixedLevelTest fixedLevelTest;
    FixedLevelTestWithEachTargetNTimes fixedLevelTestWithEachTargetNTimes;
    FixedLevelFixedTrialsTest fixedLevelFixedTrialsTest;
    InitializingDefaultAdaptiveTest initializingDefaultAdaptiveTest{
        &adaptiveMethod};
    InitializingAdaptiveTestWithEyeTracking
        initializingAdaptiveTestWithEyeTracking{&adaptiveMethod};
    InitializingAdaptiveTestWithSingleSpeaker
        initializingAdaptiveTestWithSingleSpeaker{&adaptiveMethod};
    InitializingAdaptiveTestWithDelayedMasker
        initializingAdaptiveTestWithDelayedMasker{&adaptiveMethod};
    InitializingAdaptiveTestWithCyclicTargets
        initializingAdaptiveTestWithCyclicTargets{&adaptiveMethod};
    InitializingAdaptiveTestWithCyclicTargetsAndEyeTracking
        initializingAdaptiveTestWithCyclicTargetsAndEyeTracking{
            &adaptiveMethod};
    InitializingFixedLevelTestWithTargetReplacement
        initializingFixedLevelTestWithTargetReplacement{&fixedLevelMethod};
    InitializingFixedLevelTestWithSilentIntervalTargets
        initializingFixedLevelTestWithSilentIntervalTargets{&fixedLevelMethod};
    InitializingFixedLevelTestWithTargetReplacementAndEyeTracking
        initializingFixedLevelTestWithTargetReplacementAndEyeTracking{
            &fixedLevelMethod};
    InitializingFixedLevelTestWithSilentIntervalTargetsAndEyeTracking
        initializingFixedLevelTestWithSilentIntervalTargetsAndEyeTracking{
            &fixedLevelMethod};
    InitializingFixedLevelTestWithAllTargets
        initializingFixedLevelTestWithAllTargets{&fixedLevelMethod};
    InitializingFixedLevelTestWithAllTargetsAndEyeTracking
        initializingFixedLevelTestWithAllTargetsAndEyeTracking{
            &fixedLevelMethod};
    InitializingFixedLevelTestWithEachTargetNTimes
        initializingFixedLevelTestWithEachTargetNTimes{&fixedLevelMethod};
    InitializingFixedLevelTestWithAllTargetsAndAudioRecording
        initializingFixedLevelTestWithAllTargetsAndAudioRecording{
            &fixedLevelMethod};
    InitializingFixedLevelTestWithPredeterminedTargetsAndAudioRecording
        initializingFixedLevelTestWithPredeterminedTargetsAndAudioRecording{
            &fixedLevelMethod};
    InitializingFixedLevelTestWithPredeterminedTargetsAndEyeTracking
        initializingFixedLevelTestWithPredeterminedTargetsAndEyeTracking{
            &fixedLevelMethod};

    void run(InitializingTestUseCase &useCase) { useCase.run(model); }

    auto testComplete() -> bool { return model.testComplete(); }

    void assertInitializesInternalModel(InitializingTestUseCase &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            useCase.testMethod(), internalModel.testMethod());
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(&useCase.test(), internalModel.test());
    }

    void assertInitializesFixedLevelMethod(
        InitializingFixedLevelTest &useCase) {
        useCase.run(model);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            &std::as_const(useCase.fixedLevelTest()), fixedLevelMethod.test());
    }

    void assertInitializesFixedLevelMethod(
        InitializingFixedLevelFixedTrialsTest &useCase) {
        useCase.run(model);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            &std::as_const(useCase.fixedLevelFixedTrialsTest()),
            fixedLevelMethod.fixedTrialsTest());
    }

    void assertInitializesAdaptiveMethod(
        InitializingAdaptiveTest &useCase, TargetPlaylistReader &reader) {
        useCase.run(model, adaptiveTest);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            &std::as_const(adaptiveTest), adaptiveMethod.test());
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            &reader, adaptiveMethod.targetListReader());
    }

    void assertInitializesFixedLevelTestWithTargetPlaylist(
        InitializingTestUseCase &useCase, TargetPlaylist &targetList) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            &targetList, fixedLevelMethod.targetList());
    }

    void assertPassesCalibration(PlayingCalibrationUseCase &useCase) {
        Calibration calibration;
        useCase.run(model, calibration);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            &std::as_const(calibration), useCase.calibration(internalModel));
    }
};

class SubmittingFreeResponseTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub adaptiveTestMethod;
    FixedLevelMethodStub testMethod;
    RecognitionTestModelStub model{adaptiveTestMethod, testMethod};
    OutputFileStub outputFile;
    submitting_free_response::InteractorImpl interactor{
        testMethod, model, outputFile};
    FreeResponse freeResponse;
};

class SubmittingConsonantTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub adaptiveTestMethod;
    FixedLevelMethodStub testMethod;
    RecognitionTestModelStub model{adaptiveTestMethod, testMethod};
    OutputFileStub outputFile;
    submitting_consonant::InteractorImpl interactor{
        testMethod, model, outputFile};
    ConsonantResponse response;
};

class SubmittingPassFailTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub testMethod;
    FixedLevelMethodStub fixedLevelMethod;
    RecognitionTestModelStub model{testMethod, fixedLevelMethod};
    OutputFileStub outputFile;
    submitting_pass_fail::InteractorImpl interactor{
        testMethod, model, outputFile};
};

class SubmittingKeywordsTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub testMethod;
    FixedLevelMethodStub fixedLevelMethod;
    RecognitionTestModelStub model{testMethod, fixedLevelMethod};
    OutputFileStub outputFile;
    submitting_keywords::InteractorImpl interactor{
        fixedLevelMethod, model, outputFile};
    ThreeKeywordsResponse threeKeywords;
};

class SubmittingNumberKeywordsTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub testMethod;
    FixedLevelMethodStub fixedLevelMethod;
    RecognitionTestModelStub model{testMethod, fixedLevelMethod};
    OutputFileStub outputFile;
    submitting_number_keywords::InteractorImpl interactor{
        testMethod, model, outputFile};
    CorrectKeywords correctKeywords;
};

class SubmittingSyllableTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub testMethod;
    FixedLevelMethodStub fixedLevelMethod;
    RecognitionTestModelStub model{testMethod, fixedLevelMethod};
    OutputFileStub outputFile;
    submitting_syllable::InteractorImpl interactor{
        fixedLevelMethod, model, outputFile};
    SyllableResponse syllableResponse;
};

#define SUBMITTING_FREE_RESPONSE_TEST(a) TEST_F(SubmittingFreeResponseTests, a)

#define SUBMITTING_PASS_FAIL_TEST(a) TEST_F(SubmittingPassFailTests, a)

#define SUBMITTING_KEYWORDS_TEST(a) TEST_F(SubmittingKeywordsTests, a)

#define SUBMITTING_CONSONANT_TEST(a) TEST_F(SubmittingConsonantTests, a)

#define SUBMITTING_NUMBER_KEYWORDS_TEST(a)                                     \
    TEST_F(SubmittingNumberKeywordsTests, a)

#define SUBMITTING_SYLLABLE(a) TEST_F(SubmittingSyllableTests, a)

SUBMITTING_FREE_RESPONSE_TEST(submittingFreeResponsePreparesNextTrialIfNeeded) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_FREE_RESPONSE_TEST(
    submitFreeResponseSavesOutputFileAfterWritingTrial) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_FREE_RESPONSE_TEST(submitFreeResponseWritesResponse) {
    freeResponse.response = "a";
    interactor.submit(freeResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, outputFile.freeResponseTrial().response);
}

SUBMITTING_FREE_RESPONSE_TEST(submitFreeResponseWritesFlagged) {
    freeResponse.flagged = true;
    interactor.submit(freeResponse);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputFile.freeResponseTrial().flagged);
}

SUBMITTING_FREE_RESPONSE_TEST(submitFreeResponseWritesWithoutFlag) {
    interactor.submit(freeResponse);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(outputFile.freeResponseTrial().flagged);
}

SUBMITTING_FREE_RESPONSE_TEST(submitWritesTime) {
    model.setPlayTrialTime("a");
    interactor.submit(freeResponse);
    assertEqual("a", outputFile.freeResponseTrial().time);
}

SUBMITTING_FREE_RESPONSE_TEST(submitFreeResponseWritesTarget) {
    testMethod.setCurrentTargetPath("a/b/c.txt");
    interactor.submit(freeResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c.txt"}, outputFile.freeResponseTrial().target);
}

SUBMITTING_FREE_RESPONSE_TEST(submitFreeResponseSubmitsResponse) {
    interactor.submit(freeResponse);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testMethod.submittedFreeResponse());
}

SUBMITTING_PASS_FAIL_TEST(
    submitCorrectResponseWritesTrialAfterSubmittingResponse) {
    interactor.submitCorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(contains(
        testMethod.log(), "submitCorrectResponse writeLastCorrectResponse "));
}

SUBMITTING_PASS_FAIL_TEST(
    submitIncorrectResponseWritesTrialAfterSubmittingResponse) {
    interactor.submitIncorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(contains(testMethod.log(),
        "submitIncorrectResponse writeLastIncorrectResponse "));
}

SUBMITTING_PASS_FAIL_TEST(
    submitCorrectResponseQueriesNextTargetAfterWritingResponse) {
    interactor.submitCorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(testMethod.log(), "writeLastCorrectResponse TOOLATE "));
}

SUBMITTING_PASS_FAIL_TEST(
    submitIncorrectResponseQueriesNextTargetAfterWritingResponse) {
    interactor.submitIncorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(testMethod.log(), "writeLastIncorrectResponse TOOLATE "));
}

SUBMITTING_PASS_FAIL_TEST(submittingCorrectPreparesNextTrialIfNeeded) {
    interactor.submitCorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_PASS_FAIL_TEST(submittingIncorrectPreparesNextTrialIfNeeded) {
    interactor.submitIncorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_PASS_FAIL_TEST(submitCorrectSavesOutputFileAfterWritingTrial) {
    interactor.submitCorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_PASS_FAIL_TEST(submitIncorrectSavesOutputFileAfterWritingTrial) {
    interactor.submitIncorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_KEYWORDS_TEST(preparesNextTrialIfNeeded) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_KEYWORDS_TEST(savesOutputFileAfterWritingTrial) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_KEYWORDS_TEST(writesFlagged) {
    threeKeywords.flagged = true;
    interactor.submit(threeKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputFile.threeKeywordsTrial().flagged);
}

SUBMITTING_KEYWORDS_TEST(writesWithoutFlag) {
    interactor.submit(threeKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(outputFile.threeKeywordsTrial().flagged);
}

SUBMITTING_KEYWORDS_TEST(writesTarget) {
    fixedLevelMethod.setCurrentTargetPath("a/b/c.txt");
    interactor.submit(threeKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c.txt"}, outputFile.threeKeywordsTrial().target);
}

SUBMITTING_KEYWORDS_TEST(writesEachKeywordEvaluation) {
    threeKeywords.firstCorrect = true;
    threeKeywords.secondCorrect = false;
    threeKeywords.thirdCorrect = true;
    interactor.submit(threeKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        outputFile.threeKeywordsTrial().firstCorrect);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(
        outputFile.threeKeywordsTrial().secondCorrect);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        outputFile.threeKeywordsTrial().thirdCorrect);
}

SUBMITTING_KEYWORDS_TEST(submitThreeKeywordsSubmitsToTestMethod) {
    interactor.submit(threeKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &threeKeywords, fixedLevelMethod.threeKeywords());
}

SUBMITTING_NUMBER_KEYWORDS_TEST(preparesNextTrialIfNeeded) {
    interactor.submit(correctKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_NUMBER_KEYWORDS_TEST(writesTrialAfterSubmittingResponse) {
    interactor.submit(correctKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(contains(
        testMethod.log(), "submitCorrectKeywords writeLastCorrectKeywords "));
}

SUBMITTING_NUMBER_KEYWORDS_TEST(queriesNextTargetAfterWritingResponse) {
    interactor.submit(correctKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(testMethod.log(), "writeLastCorrectKeywords TOOLATE "));
}

SUBMITTING_NUMBER_KEYWORDS_TEST(savesOutputFileAfterWritingTrial) {
    interactor.submit(correctKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_SYLLABLE(preparesNextTrialIfNeeded) {
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_SYLLABLE(savesOutputFileAfterWritingTrial) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_SYLLABLE(writesSubjectSyllable) {
    syllableResponse.syllable = Syllable::gi;
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Syllable::gi, outputFile.syllableTrial().subjectSyllable);
}

SUBMITTING_SYLLABLE(writesCorrectSyllable) {
    fixedLevelMethod.setCurrentTargetPath("a/b/say_vi_2-25ao.mov");
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Syllable::vi, outputFile.syllableTrial().correctSyllable);
}

SUBMITTING_SYLLABLE(writesFlagged) {
    syllableResponse.flagged = true;
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputFile.syllableTrial().flagged);
}

SUBMITTING_SYLLABLE(writesTarget) {
    fixedLevelMethod.setCurrentTargetPath("a/b/c.txt");
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c.txt"}, outputFile.syllableTrial().target);
}

SUBMITTING_SYLLABLE(submitsToTestMethod) {
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &syllableResponse, fixedLevelMethod.syllableResponse());
}

SUBMITTING_SYLLABLE(submitCorrectSyllable) {
    fixedLevelMethod.setCurrentTargetPath("a/b/say_dji_3-25av.mov");
    syllableResponse.syllable = Syllable::dji;
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputFile.syllableTrial().correct);
}

SUBMITTING_CONSONANT_TEST(submitConsonantSubmitsResponse) {
    ConsonantResponse r;
    interactor.submit(r);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testMethod.submittedConsonant());
}

SUBMITTING_CONSONANT_TEST(submitConsonantWritesTrialAfterSubmittingResponse) {
    ConsonantResponse r;
    interactor.submit(r);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(testMethod.log(), "submitConsonant writeLastConsonant "));
}

SUBMITTING_CONSONANT_TEST(queriesNextTargetAfterWritingResponse) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(testMethod.log(), "writeLastConsonant TOOLATE "));
}

SUBMITTING_CONSONANT_TEST(preparesNextTrialIfNeeded) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_CONSONANT_TEST(savesOutputFileAfterWritingTrial) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

#define MODEL_TEST(a) TEST_F(ModelTests, a)

MODEL_TEST(
    restartAdaptiveTestWhilePreservingCyclicTargetsPreparesNextTrialIfNeeded) {
    model.restartAdaptiveTestWhilePreservingTargets();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(internalModel.nextTrialPreparedIfNeeded());
}

MODEL_TEST(
    restartAdaptiveTestWhilePreservingCyclicTargetsResetsAdaptiveMethodTracks) {
    model.restartAdaptiveTestWhilePreservingTargets();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(adaptiveMethod.tracksResetted());
}

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelTestWithTargetReplacement);
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelTestWithSilentIntervalTargets);
}

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementAndEyeTrackingInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelTestWithTargetReplacementAndEyeTracking);
}

MODEL_TEST(
    initializingFixedLevelTestWithAllTargetsAndAudioRecordingInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelTestWithAllTargetsAndAudioRecording);
}

MODEL_TEST(
    initializingFixedLevelTestWithPredeterminedTargetsAndAudioRecordingInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelTestWithPredeterminedTargetsAndAudioRecording);
}

MODEL_TEST(
    initializingFixedLevelTestWithPredeterminedTargetsAndEyeTrackingInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelTestWithPredeterminedTargetsAndEyeTracking);
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsAndEyeTrackingInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelTestWithSilentIntervalTargetsAndEyeTracking);
}

MODEL_TEST(
    initializingFixedLevelTestWithEachTargetNTimesInitializesFixedLevelMethod) {
    initializingFixedLevelTestWithEachTargetNTimes.run(
        model, fixedLevelTestWithEachTargetNTimes);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &static_cast<const FixedLevelTest &>(
            std::as_const(fixedLevelTestWithEachTargetNTimes)),
        fixedLevelMethod.test());
}

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementInitializesWithTargetsWithReplacement) {
    assertInitializesFixedLevelTestWithTargetPlaylist(
        initializingFixedLevelTestWithTargetReplacement,
        targetsWithReplacement);
}

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementAndEyeTrackingInitializesWithTargetsWithReplacement) {
    assertInitializesFixedLevelTestWithTargetPlaylist(
        initializingFixedLevelTestWithTargetReplacementAndEyeTracking,
        targetsWithReplacement);
}

MODEL_TEST(
    initializingFixedLevelTestWithSilentIntervalTargetsAndEyeTrackingInitializesWithSilentIntervalTargets) {
    assertInitializesFixedLevelTestWithTargetPlaylist(
        initializingFixedLevelTestWithSilentIntervalTargetsAndEyeTracking,
        silentIntervals);
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsInitializesWithSilentIntervals) {
    assertInitializesFixedLevelTestWithTargetPlaylist(
        initializingFixedLevelTestWithSilentIntervalTargets, silentIntervals);
}

MODEL_TEST(initializeFixedLevelTestWithAllTargetsInitializesWithAllTargets) {
    assertInitializesFixedLevelTestWithTargetPlaylist(
        initializingFixedLevelTestWithAllTargets, everyTargetOnce);
}

MODEL_TEST(
    initializeFixedLevelTestWithAllTargetsAndEyeTrackingInitializesWithAllTargets) {
    assertInitializesFixedLevelTestWithTargetPlaylist(
        initializingFixedLevelTestWithAllTargetsAndEyeTracking,
        everyTargetOnce);
}

MODEL_TEST(
    initializeFixedLevelTestWithPredeterminedTargetsAndAudioRecordingInitializesWithPredeterminedTargets) {
    assertInitializesFixedLevelTestWithTargetPlaylist(
        initializingFixedLevelTestWithPredeterminedTargetsAndAudioRecording,
        predeterminedTargets);
}

MODEL_TEST(
    initializeFixedLevelTestWithPredeterminedTargetsAndEyeTrackingInitializesWithPredeterminedTargets) {
    assertInitializesFixedLevelTestWithTargetPlaylist(
        initializingFixedLevelTestWithPredeterminedTargetsAndEyeTracking,
        predeterminedTargets);
}

MODEL_TEST(
    initializeFixedLevelTestWithEachTargetNTimesInitializesWithEachTargetNTimes) {
    assertInitializesFixedLevelTestWithTargetPlaylist(
        initializingFixedLevelTestWithEachTargetNTimes, eachTargetNTimes);
}

MODEL_TEST(initializeFixedLevelTestWithEachTargetNTimesSetsTargetRepeats) {
    fixedLevelTestWithEachTargetNTimes.timesEachTargetIsPlayed = 2;
    initializingFixedLevelTestWithEachTargetNTimes.run(
        model, fixedLevelTestWithEachTargetNTimes);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(gsl::index{1}, eachTargetNTimes.repeats());
}

MODEL_TEST(initializeDefaultAdaptiveTestInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(
        initializingDefaultAdaptiveTest, targetsWithReplacementReader);
}

MODEL_TEST(initializeAdaptiveTestWithEyeTrackingInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(
        initializingAdaptiveTestWithEyeTracking, targetsWithReplacementReader);
}

MODEL_TEST(initializeAdaptiveTestWithSingleSpeakerInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(initializingAdaptiveTestWithSingleSpeaker,
        targetsWithReplacementReader);
}

MODEL_TEST(initializeAdaptiveTestWithDelayedMaskerInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(initializingAdaptiveTestWithDelayedMasker,
        targetsWithReplacementReader);
}

MODEL_TEST(initializeAdaptiveTestWithCyclicTargetsInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(
        initializingAdaptiveTestWithCyclicTargets, cyclicTargetsReader);
}

MODEL_TEST(
    initializeAdaptiveTestWithCyclicTargetsAndEyeTrackingInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(
        initializingAdaptiveTestWithCyclicTargetsAndEyeTracking,
        cyclicTargetsReader);
}

MODEL_TEST(
    initializingFixedLevelTestWithEachTargetNTimesInitializesInternalModel) {
    assertInitializesInternalModel(
        initializingFixedLevelTestWithEachTargetNTimes);
}

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementInitializesInternalModel) {
    assertInitializesInternalModel(
        initializingFixedLevelTestWithTargetReplacement);
}

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementAndEyeTrackingInitializesInternalModel) {
    assertInitializesInternalModel(
        initializingFixedLevelTestWithTargetReplacementAndEyeTracking);
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsAndEyeTrackingInitializesInternalModel) {
    assertInitializesInternalModel(
        initializingFixedLevelTestWithSilentIntervalTargetsAndEyeTracking);
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsInitializesInternalModel) {
    assertInitializesInternalModel(
        initializingFixedLevelTestWithSilentIntervalTargets);
}

MODEL_TEST(initializeFixedLevelTestWithAllTargetsInitializesInternalModel) {
    assertInitializesInternalModel(initializingFixedLevelTestWithAllTargets);
}

MODEL_TEST(
    initializeFixedLevelTestWithAllTargetsAndEyeTrackingInitializesInternalModel) {
    assertInitializesInternalModel(
        initializingFixedLevelTestWithAllTargetsAndEyeTracking);
}

MODEL_TEST(
    initializeFixedLevelTestWithAllTargetsAndAudioRecordingInitializesInternalModel) {
    assertInitializesInternalModel(
        initializingFixedLevelTestWithAllTargetsAndAudioRecording);
}

MODEL_TEST(
    initializeFixedLevelTestWithPredeterminedTargetsAndAudioRecordingInitializesInternalModel) {
    assertInitializesInternalModel(
        initializingFixedLevelTestWithPredeterminedTargetsAndAudioRecording);
}

MODEL_TEST(initializeDefaultAdaptiveTestInitializesInternalModel) {
    assertInitializesInternalModel(initializingDefaultAdaptiveTest);
}

MODEL_TEST(initializeAdaptiveTestWithEyeTrackingInitializesInternalModel) {
    assertInitializesInternalModel(initializingAdaptiveTestWithEyeTracking);
}

MODEL_TEST(initializeAdaptiveTestWithSingleSpeakerInitializesInternalModel) {
    assertInitializesInternalModel(initializingAdaptiveTestWithSingleSpeaker);
}

MODEL_TEST(initializeAdaptiveTestWithDelayedMaskerInitializesInternalModel) {
    assertInitializesInternalModel(initializingAdaptiveTestWithDelayedMasker);
}

MODEL_TEST(initializeAdaptiveTestWithCyclicTargetsInitializesInternalModel) {
    assertInitializesInternalModel(initializingAdaptiveTestWithCyclicTargets);
}

MODEL_TEST(
    initializeAdaptiveTestWithCyclicTargetsAndEyeTrackingInitializesInternalModel) {
    assertInitializesInternalModel(
        initializingAdaptiveTestWithCyclicTargetsAndEyeTracking);
}

MODEL_TEST(initializeAdaptiveTestWithSingleSpeakerInitializesSingleSpeaker) {
    run(initializingAdaptiveTestWithSingleSpeaker);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        internalModel.initializedWithSingleSpeaker());
}

MODEL_TEST(initializeAdaptiveTestWithDelayedMaskerInitializesSingleSpeaker) {
    run(initializingAdaptiveTestWithDelayedMasker);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        internalModel.initializedWithDelayedMasker());
}

MODEL_TEST(
    initializeFixedLevelTestWithAllTargetsAndAudioRecordingInitializesWithAudioRecording) {
    run(initializingFixedLevelTestWithAllTargetsAndAudioRecording);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(internalModel.observer, &audioRecording);
}

MODEL_TEST(
    initializeFixedLevelTestWithPredeterminedTargetsAndAudioRecordingInitializesWithAudioRecording) {
    run(initializingFixedLevelTestWithPredeterminedTargetsAndAudioRecording);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(internalModel.observer, &audioRecording);
}

MODEL_TEST(
    initializeFixedLevelTestWithAllTargetsAndEyeTrackingInitializesWithEyeTracking) {
    run(initializingFixedLevelTestWithAllTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        initializedWithEyeTracking(internalModel, &eyeTracking));
}

MODEL_TEST(initializeAdaptiveTestWithEyeTrackingInitializesWithEyeTracking) {
    run(initializingAdaptiveTestWithEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        initializedWithEyeTracking(internalModel, &eyeTracking));
}

MODEL_TEST(
    initializeFixedLevelTestWithPredertiminedTargetsAndEyeTrackingInitializesWithEyeTracking) {
    run(initializingFixedLevelTestWithPredeterminedTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        initializedWithEyeTracking(internalModel, &eyeTracking));
}

MODEL_TEST(
    initializeAdaptiveTestWithCyclicTargetsAndEyeTrackingInitializesWithEyeTracking) {
    run(initializingAdaptiveTestWithCyclicTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        initializedWithEyeTracking(internalModel, &eyeTracking));
}

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementAndEyeTrackingInitializesWithEyeTracking) {
    run(initializingFixedLevelTestWithTargetReplacementAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        initializedWithEyeTracking(internalModel, &eyeTracking));
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsAndEyeTrackingInitializesWithEyeTracking) {
    run(initializingFixedLevelTestWithSilentIntervalTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        initializedWithEyeTracking(internalModel, &eyeTracking));
}

MODEL_TEST(submitResponsePassesCoordinateResponse) {
    coordinate_response_measure::Response response;
    model.submit(response);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(response), internalModel.coordinateResponse());
}

MODEL_TEST(playTrialPassesAudioSettings) {
    AudioSettings settings;
    model.playTrial(settings);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(settings), internalModel.playTrialSettings());
}

MODEL_TEST(playCalibrationPassesCalibration) {
    PlayingCalibration useCase;
    assertPassesCalibration(useCase);
}

MODEL_TEST(playLeftSpeakerCalibrationPassesCalibration) {
    PlayingLeftSpeakerCalibration useCase;
    assertPassesCalibration(useCase);
}

MODEL_TEST(playRightSpeakerCalibrationPassesCalibration) {
    PlayingRightSpeakerCalibration useCase;
    assertPassesCalibration(useCase);
}

MODEL_TEST(testCompleteWhenComplete) {
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(testComplete());
    internalModel.setComplete();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testComplete());
}

MODEL_TEST(returnsAudioDevices) {
    internalModel.setAudioDevices({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, model.audioDevices());
}

MODEL_TEST(returnsAdaptiveTestResults) {
    adaptiveMethod.setTestResults({{{"a"}, 1.}, {{"b"}, 2.}, {{"c"}, 3.}});
    assertEqual(
        {{{"a"}, 1.}, {{"b"}, 2.}, {{"c"}, 3.}}, model.adaptiveTestResults());
}

MODEL_TEST(returnsKeywordsTestResults) {
    fixedLevelMethod.setKeywordsTestResults({1., 2});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        1., model.keywordsTestResults().percentCorrect);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        2, model.keywordsTestResults().totalCorrect);
}

MODEL_TEST(returnsTrialNumber) {
    internalModel.setTrialNumber(1);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, model.trialNumber());
}

MODEL_TEST(returnsTargetFileName) {
    internalModel.setTargetFileName("a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, model.targetFileName());
}

MODEL_TEST(subscribesToListener) {
    ModelObserverStub listener;
    model.attach(&listener);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        static_cast<const RunningATestFacade::Observer *>(&listener),
        internalModel.listener());
}
}
}
