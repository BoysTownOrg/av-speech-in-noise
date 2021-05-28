#include "OutputFileStub.hpp"
#include "ModelObserverStub.hpp"
#include "TargetPlaylistStub.hpp"
#include "TargetPlaylistSetReaderStub.hpp"
#include "assert-utility.hpp"
#include <av-speech-in-noise/core/SubmittingFreeResponse.hpp>
#include <av-speech-in-noise/core/Model.hpp>
#include <gtest/gtest.h>
#include <sstream>
#include <utility>

namespace av_speech_in_noise {
static auto operator==(const AdaptiveTestResult &a, const AdaptiveTestResult &b)
    -> bool {
    return a.targetsUrl.path == b.targetsUrl.path && a.threshold == b.threshold;
}

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
    void submit(const FreeResponse &) override {}
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

    auto submittedFreeResponse() -> bool { return submittedFreeResponse_; }

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

  private:
    KeywordsTestResults keywordsTestResults_{};
    std::stringstream log_{};
    LocalUrl currentTarget_;
    const FixedLevelTest *test_{};
    const FixedLevelFixedTrialsTest *fixedTrialsTest_{};
    TargetPlaylist *targetList_{};
    bool submittedConsonant_{};
    bool submittedFreeResponse_{};
};

class RecognitionTestModelStub : public RecognitionTestModel {
  public:
    explicit RecognitionTestModelStub(
        FixedLevelMethodStub &fixedLevelMethodStub)
        : fixedLevelMethodStub{fixedLevelMethodStub} {}

    [[nodiscard]] auto nextTrialPreparedIfNeeded() const -> bool {
        return nextTrialPreparedIfNeeded_;
    }

    void prepareNextTrialIfNeeded() override {
        nextTrialPreparedIfNeeded_ = true;
        fixedLevelMethodStub.setCurrentTargetPath("TOOLATE");
    }

    void initialize(TestMethod *method, const Test &test) override {
        testMethod_ = method;
        test_ = &test;
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

    void initializeWithEyeTracking(
        TestMethod *method, const Test &test) override {
        testMethod_ = method;
        test_ = &test;
        initializedWithEyeTracking_ = true;
    }

    auto trialNumber() -> int override { return trialNumber_; }

    void setTrialNumber(int n) { trialNumber_ = n; }

    void setTargetFileName(std::string s) { targetFileName_ = std::move(s); }

    auto targetFileName() -> std::string override { return targetFileName_; }

    void playTrial(const AudioSettings &s) override { playTrialSettings_ = &s; }

    void submit(const coordinate_response_measure::Response &p) override {
        coordinateResponse_ = &p;
    }

    void submit(const CorrectKeywords &p) override {
        correctKeywords_ = &p;
        if (testMethodToCallNextTargetOnSubmitCorrectKeywordsOrCorrectOrIncorrect_ !=
            nullptr)
            testMethodToCallNextTargetOnSubmitCorrectKeywordsOrCorrectOrIncorrect_
                ->nextTarget();
    }

    void submit(const ConsonantResponse &p) override {
        consonantResponse_ = &p;
        if (testMethodToCallNextTargetOnSubmitConsonants_ != nullptr)
            testMethodToCallNextTargetOnSubmitConsonants_->nextTarget();
    }

    auto testComplete() -> bool override { return complete_; }

    auto audioDevices() -> std::vector<std::string> override {
        return audioDevices_;
    }

    void attach(Model::Observer *e) override { listener_ = e; }

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

    [[nodiscard]] auto initializedWithEyeTracking() const -> bool {
        return initializedWithEyeTracking_;
    }

    [[nodiscard]] auto coordinateResponse() const {
        return coordinateResponse_;
    }

    [[nodiscard]] auto correctKeywords() const { return correctKeywords_; }

    [[nodiscard]] auto consonantResponse() const { return consonantResponse_; }

    auto threeKeywords() -> const ThreeKeywordsResponse * {
        return threeKeywords_;
    }

    auto syllableResponse() -> const SyllableResponse * {
        return syllableResponse_;
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

    void callNextOnSubmitConsonants(TestMethod *t) {
        testMethodToCallNextTargetOnSubmitConsonants_ = t;
    }

    void callNextOnSubmitCorrectKeywordsOrCorrectOrIncorrect(TestMethod *t) {
        testMethodToCallNextTargetOnSubmitCorrectKeywordsOrCorrectOrIncorrect_ =
            t;
    }

    void submitCorrectResponse() override {
        if (testMethodToCallNextTargetOnSubmitCorrectKeywordsOrCorrectOrIncorrect_ !=
            nullptr)
            testMethodToCallNextTargetOnSubmitCorrectKeywordsOrCorrectOrIncorrect_
                ->nextTarget();
    }

    void submitIncorrectResponse() override {
        if (testMethodToCallNextTargetOnSubmitCorrectKeywordsOrCorrectOrIncorrect_ !=
            nullptr)
            testMethodToCallNextTargetOnSubmitCorrectKeywordsOrCorrectOrIncorrect_
                ->nextTarget();
    }

    void submit(const ThreeKeywordsResponse &f) override {
        threeKeywords_ = &f;
    }

    void submit(const SyllableResponse &f) override { syllableResponse_ = &f; }

    auto playTrialTime() -> std::string override { return playTrialTime_; }

    void setPlayTrialTime(std::string s) { playTrialTime_ = std::move(s); }

  private:
    std::vector<std::string> audioDevices_{};
    std::string targetFileName_{};
    std::string playTrialTime_;
    FixedLevelMethodStub &fixedLevelMethodStub;
    TestMethod *testMethodToCallNextTargetOnSubmitConsonants_{};
    TestMethod *
        testMethodToCallNextTargetOnSubmitCorrectKeywordsOrCorrectOrIncorrect_{};
    const Model::Observer *listener_{};
    const Calibration *calibration_{};
    const Calibration *leftSpeakerCalibration_{};
    const Calibration *rightSpeakerCalibration_{};
    const AudioSettings *playTrialSettings_{};
    const Test *test_{};
    const TestMethod *testMethod_{};
    const coordinate_response_measure::Response *coordinateResponse_{};
    const CorrectKeywords *correctKeywords_{};
    const ConsonantResponse *consonantResponse_{};
    const ThreeKeywordsResponse *threeKeywords_{};
    const SyllableResponse *syllableResponse_{};
    int trialNumber_{};
    bool complete_{};
    bool initializedWithSingleSpeaker_{};
    bool initializedWithDelayedMasker_{};
    bool initializedWithEyeTracking_{};
    bool nextTrialPreparedIfNeeded_{};
};

class InitializingTestUseCase {
  public:
    virtual ~InitializingTestUseCase() = default;
    virtual void run(ModelImpl &) = 0;
    virtual auto test() -> const Test & = 0;
    virtual auto testMethod() -> const TestMethod * = 0;
};

class InitializingFixedLevelTest : public virtual InitializingTestUseCase {
  public:
    virtual void run(ModelImpl &model, const FixedLevelTest &test) = 0;
};

class InitializingFixedLevelFixedTrialsTest
    : public virtual InitializingTestUseCase {
  public:
    virtual void run(
        ModelImpl &model, const FixedLevelFixedTrialsTest &test) = 0;
};

class InitializingAdaptiveTest : public virtual InitializingTestUseCase {
  public:
    virtual void run(ModelImpl &model, const AdaptiveTest &test) = 0;
};

void initialize(ModelImpl &model, const AdaptiveTest &test) {
    model.initialize(test);
}

void initializeWithTargetReplacement(
    ModelImpl &model, const FixedLevelFixedTrialsTest &test) {
    model.initializeWithTargetReplacement(test);
}

void initializeWithSingleSpeaker(ModelImpl &model, const AdaptiveTest &test) {
    model.initializeWithSingleSpeaker(test);
}

void initializeWithDelayedMasker(ModelImpl &model, const AdaptiveTest &test) {
    model.initializeWithDelayedMasker(test);
}

void initializeWithCyclicTargets(ModelImpl &model, const AdaptiveTest &test) {
    model.initializeWithCyclicTargets(test);
}

void initializeWithCyclicTargetsAndEyeTracking(
    ModelImpl &model, const AdaptiveTest &test) {
    model.initializeWithCyclicTargetsAndEyeTracking(test);
}

void initializeWithSilentIntervalTargets(
    ModelImpl &model, const FixedLevelTest &test) {
    model.initializeWithSilentIntervalTargets(test);
}

void initializeWithTargetReplacementAndEyeTracking(
    ModelImpl &model, const FixedLevelFixedTrialsTest &test) {
    model.initializeWithTargetReplacementAndEyeTracking(test);
}

void initializeWithSilentIntervalTargetsAndEyeTracking(
    ModelImpl &model, const FixedLevelTest &test) {
    model.initializeWithSilentIntervalTargetsAndEyeTracking(test);
}

void initializeWithEyeTracking(ModelImpl &model, const AdaptiveTest &test) {
    model.initializeWithEyeTracking(test);
}

class InitializingDefaultAdaptiveTest : public InitializingAdaptiveTest {
    AdaptiveTest test_;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingDefaultAdaptiveTest(AdaptiveMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override { initialize(model, test_); }

    void run(ModelImpl &model, const AdaptiveTest &test) override {
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

    void run(ModelImpl &model) override {
        initializeWithEyeTracking(model, test_);
    }

    void run(ModelImpl &model, const AdaptiveTest &test) override {
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

    void run(ModelImpl &model) override {
        initializeWithSingleSpeaker(model, test_);
    }

    void run(ModelImpl &model, const AdaptiveTest &test) override {
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

    void run(ModelImpl &model) override {
        initializeWithDelayedMasker(model, test_);
    }

    void run(ModelImpl &model, const AdaptiveTest &test) override {
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

    void run(ModelImpl &model) override {
        initializeWithCyclicTargets(model, test_);
    }

    void run(ModelImpl &model, const AdaptiveTest &test) override {
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

    void run(ModelImpl &model) override {
        initializeWithCyclicTargetsAndEyeTracking(model, test_);
    }

    void run(ModelImpl &model, const AdaptiveTest &test) override {
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

    void run(ModelImpl &model) override {
        initializeWithTargetReplacement(model, test_);
    }

    void run(ModelImpl &model, const FixedLevelFixedTrialsTest &test) override {
        initializeWithTargetReplacement(model, test);
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

    void run(ModelImpl &model) override {
        initializeWithSilentIntervalTargets(model, test_);
    }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
        initializeWithSilentIntervalTargets(model, test);
    }

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
        : method{method} {}

    void run(ModelImpl &model) override {
        initializeWithTargetReplacementAndEyeTracking(model, test_);
    }

    void run(ModelImpl &model, const FixedLevelFixedTrialsTest &test) override {
        initializeWithTargetReplacementAndEyeTracking(model, test);
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
        : method{method} {}

    void run(ModelImpl &model) override {
        initializeWithSilentIntervalTargetsAndEyeTracking(model, test_);
    }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
        initializeWithSilentIntervalTargetsAndEyeTracking(model, test);
    }

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

    void run(ModelImpl &model) override {
        model.initializeWithAllTargets(test_);
    }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
        model.initializeWithAllTargets(test);
    }

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
        : method{method} {}

    void run(ModelImpl &model) override {
        model.initializeWithAllTargetsAndEyeTracking(test_);
    }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
        model.initializeWithAllTargetsAndEyeTracking(test);
    }

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

    void run(ModelImpl &model) override { model.initialize(test_); }

    void run(ModelImpl &model, const FixedLevelTestWithEachTargetNTimes &test) {
        model.initialize(test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

auto initializedWithEyeTracking(RecognitionTestModelStub &m) -> bool {
    return m.initializedWithEyeTracking();
}

class PlayingCalibrationUseCase {
  public:
    virtual ~PlayingCalibrationUseCase() = default;
    virtual void run(Model &model, const Calibration &c) = 0;
    virtual auto calibration(RecognitionTestModelStub &model)
        -> const Calibration * = 0;
};

class PlayingCalibration : public PlayingCalibrationUseCase {
  public:
    void run(Model &model, const Calibration &c) override {
        model.playCalibration(c);
    }

    auto calibration(RecognitionTestModelStub &model)
        -> const Calibration * override {
        return model.calibration();
    }
};

class PlayingLeftSpeakerCalibration : public PlayingCalibrationUseCase {
  public:
    void run(Model &model, const Calibration &c) override {
        model.playLeftSpeakerCalibration(c);
    }

    auto calibration(RecognitionTestModelStub &model)
        -> const Calibration * override {
        return model.leftSpeakerCalibration();
    }
};

class PlayingRightSpeakerCalibration : public PlayingCalibrationUseCase {
  public:
    void run(Model &model, const Calibration &c) override {
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
    RepeatableFiniteTargetPlaylistStub eachTargetNTimes;
    RecognitionTestModelStub internalModel{fixedLevelMethod};
    OutputFileStub outputFile;
    ModelImpl model{adaptiveMethod, fixedLevelMethod,
        targetsWithReplacementReader, cyclicTargetsReader,
        targetsWithReplacement, silentIntervals, everyTargetOnce,
        eachTargetNTimes, internalModel, outputFile};
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
        useCase.run(model, fixedLevelTest);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            &std::as_const(fixedLevelTest), fixedLevelMethod.test());
    }

    void assertInitializesFixedLevelMethod(
        InitializingFixedLevelFixedTrialsTest &useCase) {
        useCase.run(model, fixedLevelFixedTrialsTest);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            &std::as_const(fixedLevelFixedTrialsTest),
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
    FixedLevelMethodStub testMethod;
    RecognitionTestModelStub model{testMethod};
    OutputFileStub outputFile;
    submitting_free_response::InteractorImpl interactor{
        testMethod, model, outputFile};
    FreeResponse freeResponse;
};

#define SUBMITTING_FREE_RESPONSE_TEST(a) TEST_F(SubmittingFreeResponseTests, a)

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

MODEL_TEST(initializeAdaptiveTestWithEyeTrackingInitializesWithEyeTracking) {
    run(initializingAdaptiveTestWithEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(initializedWithEyeTracking(internalModel));
}

MODEL_TEST(
    initializeAdaptiveTestWithCyclicTargetsAndEyeTrackingInitializesWithEyeTracking) {
    run(initializingAdaptiveTestWithCyclicTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(initializedWithEyeTracking(internalModel));
}

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementAndEyeTrackingInitializesWithEyeTracking) {
    run(initializingFixedLevelTestWithTargetReplacementAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(initializedWithEyeTracking(internalModel));
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsAndEyeTrackingInitializesWithEyeTracking) {
    run(initializingFixedLevelTestWithSilentIntervalTargetsAndEyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(initializedWithEyeTracking(internalModel));
}

MODEL_TEST(submitResponsePassesCoordinateResponse) {
    coordinate_response_measure::Response response;
    model.submit(response);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(response), internalModel.coordinateResponse());
}

MODEL_TEST(submitCorrectKeywordsPassesCorrectKeywords) {
    CorrectKeywords k;
    model.submit(k);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(k), internalModel.correctKeywords());
}

MODEL_TEST(submitConsonantPassesConsonant) {
    ConsonantResponse r;
    model.submit(r);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(r), internalModel.consonantResponse());
}

MODEL_TEST(submitThreeKeywordsPassesThreeKeywords) {
    ThreeKeywordsResponse r;
    model.submit(r);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(r), internalModel.threeKeywords());
}

MODEL_TEST(submitSyllablePassesSyllable) {
    SyllableResponse r{};
    model.submit(r);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(r), internalModel.syllableResponse());
}

MODEL_TEST(submitConsonantSubmitsResponse) {
    ConsonantResponse r;
    model.submit(r);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(fixedLevelMethod.submittedConsonant());
}

MODEL_TEST(submitConsonantWritesTrialAfterSubmittingResponse) {
    ConsonantResponse r;
    model.submit(r);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(contains(
        fixedLevelMethod.log(), "submitConsonant writeLastConsonant "));
}

MODEL_TEST(submitCorrectKeywordsWritesTrialAfterSubmittingResponse) {
    CorrectKeywords r;
    model.submit(r);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(contains(adaptiveMethod.log(),
        "submitCorrectKeywords writeLastCorrectKeywords "));
}

MODEL_TEST(submitCorrectResponseWritesTrialAfterSubmittingResponse) {
    model.submitCorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(contains(adaptiveMethod.log(),
        "submitCorrectResponse writeLastCorrectResponse "));
}

MODEL_TEST(submitIncorrectResponseWritesTrialAfterSubmittingResponse) {
    model.submitIncorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(contains(adaptiveMethod.log(),
        "submitIncorrectResponse writeLastIncorrectResponse "));
}

MODEL_TEST(submitConsonantQueriesNextTargetAfterWritingResponse) {
    internalModel.callNextOnSubmitConsonants(&fixedLevelMethod);
    ConsonantResponse r;
    model.submit(r);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(fixedLevelMethod.log(), "writeLastConsonant nextTarget "));
}

MODEL_TEST(submitCorrectKeywordsQueriesNextTargetAfterWritingResponse) {
    internalModel.callNextOnSubmitCorrectKeywordsOrCorrectOrIncorrect(
        &adaptiveMethod);
    CorrectKeywords r;
    model.submit(r);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(adaptiveMethod.log(), "writeLastCorrectKeywords nextTarget "));
}

MODEL_TEST(submitCorrectResponseQueriesNextTargetAfterWritingResponse) {
    internalModel.callNextOnSubmitCorrectKeywordsOrCorrectOrIncorrect(
        &adaptiveMethod);
    model.submitCorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(adaptiveMethod.log(), "writeLastCorrectResponse nextTarget "));
}

MODEL_TEST(submitIncorrectResponseQueriesNextTargetAfterWritingResponse) {
    internalModel.callNextOnSubmitCorrectKeywordsOrCorrectOrIncorrect(
        &adaptiveMethod);
    model.submitIncorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(contains(
        adaptiveMethod.log(), "writeLastIncorrectResponse nextTarget "));
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
        static_cast<const Model::Observer *>(&listener),
        internalModel.listener());
}
}
}
