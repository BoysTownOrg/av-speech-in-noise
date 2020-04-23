#include "ModelEventListenerStub.hpp"
#include "TargetListStub.hpp"
#include "TargetListSetReaderStub.hpp"
#include "assert-utility.hpp"
#include <recognition-test/Model.hpp>
#include <gtest/gtest.h>

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

    void initialize(const AdaptiveTest &t, TargetListReader *reader) override {
        test_ = &t;
        targetListReader_ = reader;
    }

    [[nodiscard]] auto test() const { return test_; }

    [[nodiscard]] auto targetListReader() const -> TargetListReader * {
        return targetListReader_;
    }

    void setTestResults(AdaptiveTestResults v) { testResults_ = std::move(v); }

    auto testResults() -> AdaptiveTestResults override { return testResults_; }

    void resetTracks() override { tracksResetted_ = true; }

    auto complete() -> bool override { return {}; }
    auto nextTarget() -> LocalUrl override { return {}; }
    auto currentTarget() -> LocalUrl override { return {}; }
    auto snr() -> SNR override { return {}; }
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void submit(const FreeResponse &) override {}
    void submit(const CorrectKeywords &) override {}
    void writeTestingParameters(OutputFile &) override {}
    void writeLastCoordinateResponse(OutputFile &) override {}
    void writeLastCorrectResponse(OutputFile &) override {}
    void writeLastIncorrectResponse(OutputFile &) override {}
    void writeLastCorrectKeywords(OutputFile &) override {}
    void writeTestResult(OutputFile &) override {}
    void submit(const coordinate_response_measure::Response &) override {}

  private:
    AdaptiveTestResults testResults_;
    const AdaptiveTest *test_{};
    TargetListReader *targetListReader_{};
    bool tracksResetted_{};
};

class FixedLevelMethodStub : public FixedLevelMethod {
    const FixedLevelTest *test_{};
    TargetList *targetList_{};
    bool initializedWithFiniteTargetList_{};

  public:
    void initialize(const FixedLevelTest &t, TargetList *list) override {
        targetList_ = list;
        test_ = &t;
    }

    void initialize(const FixedLevelTest &t, FiniteTargetList *list) override {
        targetList_ = list;
        test_ = &t;
        initializedWithFiniteTargetList_ = true;
    }

    [[nodiscard]] auto initializedWithFiniteTargetList() const -> bool {
        return initializedWithFiniteTargetList_;
    }

    [[nodiscard]] auto targetList() const { return targetList_; }

    [[nodiscard]] auto test() const { return test_; }

    auto complete() -> bool override { return {}; }
    auto nextTarget() -> LocalUrl override { return {}; }
    auto currentTarget() -> LocalUrl override { return {}; }
    auto snr() -> SNR override { return {}; }
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void submit(const FreeResponse &) override {}
    void submit(const CorrectKeywords &) override {}
    void writeTestingParameters(OutputFile &) override {}
    void writeLastCoordinateResponse(OutputFile &) override {}
    void writeLastCorrectResponse(OutputFile &) override {}
    void writeLastIncorrectResponse(OutputFile &) override {}
    void writeLastCorrectKeywords(OutputFile &) override {}
    void writeTestResult(OutputFile &) override {}
    void submit(const coordinate_response_measure::Response &) override {}
};

class RecognitionTestModelStub : public RecognitionTestModel {
    std::vector<std::string> audioDevices_{};
    std::string targetFileName_{};
    const Model::EventListener *listener_{};
    const Calibration *calibration_{};
    const AudioSettings *playTrialSettings_{};
    const Test *test_{};
    const TestMethod *testMethod_{};
    const coordinate_response_measure::Response *coordinateResponse_{};
    const CorrectKeywords *correctKeywords_{};
    int trialNumber_{};
    bool complete_{};
    bool initializedWithSingleSpeaker_{};
    bool initializedWithDelayedMasker_{};
    bool initializedWithEyeTracking_{};
    bool nextTrialPreparedIfNeeded_{};

  public:
    [[nodiscard]] auto nextTrialPreparedIfNeeded() const -> bool {
        return nextTrialPreparedIfNeeded_;
    }

    void prepareNextTrialIfNeeded() override {
        nextTrialPreparedIfNeeded_ = true;
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

    void submit(const CorrectKeywords &p) override { correctKeywords_ = &p; }

    auto testComplete() -> bool override { return complete_; }

    auto audioDevices() -> std::vector<std::string> override {
        return audioDevices_;
    }

    void subscribe(Model::EventListener *e) override { listener_ = e; }

    void playCalibration(const Calibration &c) override { calibration_ = &c; }

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

    [[nodiscard]] auto testMethod() const { return testMethod_; }

    [[nodiscard]] auto test() const { return test_; }

    [[nodiscard]] auto playTrialSettings() const { return playTrialSettings_; }

    [[nodiscard]] auto calibration() const { return calibration_; }

    [[nodiscard]] auto listener() const { return listener_; }

    void setComplete() { complete_ = true; }

    void setAudioDevices(std::vector<std::string> v) {
        audioDevices_ = std::move(v);
    }

    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void submit(const FreeResponse &) override {}
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

class InitializingAdaptiveTest : public virtual InitializingTestUseCase {
  public:
    virtual void run(ModelImpl &model, const AdaptiveTest &test) = 0;
};

void initialize(ModelImpl &model, const AdaptiveTest &test) {
    model.initialize(test);
}

void initializeWithTargetReplacement(
    ModelImpl &model, const FixedLevelTest &test) {
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

void initializeWithSilentIntervalTargets(
    ModelImpl &model, const FixedLevelTest &test) {
    model.initializeWithSilentIntervalTargets(test);
}

void initializeWithTargetReplacementAndEyeTracking(
    ModelImpl &model, const FixedLevelTest &test) {
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

class InitializingFixedLevelTestWithTargetReplacement
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithTargetReplacement(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        initializeWithTargetReplacement(model, test_);
    }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
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
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithTargetReplacementAndEyeTracking(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        initializeWithTargetReplacementAndEyeTracking(model, test_);
    }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
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

class ModelTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub adaptiveMethod;
    FixedLevelMethodStub fixedLevelMethod;
    TargetListStub targetsWithReplacement;
    TargetListSetReaderStub targetsWithReplacementReader;
    TargetListSetReaderStub cyclicTargetsReader;
    FiniteTargetListStub silentIntervals;
    FiniteTargetListStub everyTargetOnce;
    RecognitionTestModelStub internalModel;
    ModelImpl model{adaptiveMethod, fixedLevelMethod,
        targetsWithReplacementReader, cyclicTargetsReader,
        targetsWithReplacement, silentIntervals, everyTargetOnce,
        internalModel};
    AdaptiveTest adaptiveTest;
    FixedLevelTest fixedLevelTest;
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

    void run(InitializingTestUseCase &useCase) { useCase.run(model); }

    auto testComplete() -> bool { return model.testComplete(); }

    void assertInitializesInternalModel(InitializingTestUseCase &useCase) {
        run(useCase);
        assertEqual(useCase.testMethod(), internalModel.testMethod());
        assertEqual(&useCase.test(), internalModel.test());
    }

    void assertInitializesFixedLevelMethod(
        InitializingFixedLevelTest &useCase) {
        useCase.run(model, fixedLevelTest);
        assertEqual(&std::as_const(fixedLevelTest), fixedLevelMethod.test());
    }

    void assertInitializesAdaptiveMethod(
        InitializingAdaptiveTest &useCase, TargetListReader &reader) {
        useCase.run(model, adaptiveTest);
        assertEqual(&std::as_const(adaptiveTest), adaptiveMethod.test());
        assertEqual(&reader, adaptiveMethod.targetListReader());
    }

    void assertInitializesFixedLevelTestWithTargetList(
        InitializingTestUseCase &useCase, TargetList &targetList) {
        run(useCase);
        assertEqual(&targetList, fixedLevelMethod.targetList());
    }

    void assertInitializesFixedLevelMethodWithFiniteTargetList(
        InitializingTestUseCase &useCase) {
        run(useCase);
        assertTrue(fixedLevelMethod.initializedWithFiniteTargetList());
    }
};

#define MODEL_TEST(a) TEST_F(ModelTests, a)

MODEL_TEST(
    restartAdaptiveTestWhilePreservingCyclicTargetsPreparesNextTrialIfNeeded) {
    model.restartAdaptiveTestWhilePreservingTargets();
    assertTrue(internalModel.nextTrialPreparedIfNeeded());
}

MODEL_TEST(
    restartAdaptiveTestWhilePreservingCyclicTargetsResetsAdaptiveMethodTracks) {
    model.restartAdaptiveTestWhilePreservingTargets();
    assertTrue(adaptiveMethod.tracksResetted());
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
    initializeFixedLevelTestWithTargetReplacementInitializesWithTargetsWithReplacement) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelTestWithTargetReplacement,
        targetsWithReplacement);
}

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementAndEyeTrackingInitializesWithTargetsWithReplacement) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelTestWithTargetReplacementAndEyeTracking,
        targetsWithReplacement);
}

MODEL_TEST(
    initializingFixedLevelTestWithSilentIntervalTargetsAndEyeTrackingInitializesWithSilentIntervalTargets) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelTestWithSilentIntervalTargetsAndEyeTracking,
        silentIntervals);
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsInitializesWithSilentIntervals) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelTestWithSilentIntervalTargets, silentIntervals);
}

MODEL_TEST(initializeFixedLevelTestWithAllTargetsInitializesWithAllTargets) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelTestWithAllTargets, everyTargetOnce);
}

MODEL_TEST(
    initializeFixedLevelTestWithAllTargetsAndEyeTrackingInitializesWithAllTargets) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelTestWithAllTargetsAndEyeTracking,
        everyTargetOnce);
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsInitializesFixedLevelMethodWithFiniteTargetList) {
    assertInitializesFixedLevelMethodWithFiniteTargetList(
        initializingFixedLevelTestWithSilentIntervalTargets);
}

MODEL_TEST(
    initializingFixedLevelTestWithAllTargetsInitializesFixedLevelMethodWithFiniteTargetList) {
    assertInitializesFixedLevelMethodWithFiniteTargetList(
        initializingFixedLevelTestWithAllTargets);
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

MODEL_TEST(initializeAdaptiveTestWithSingleSpeakerInitializesSingleSpeaker) {
    run(initializingAdaptiveTestWithSingleSpeaker);
    assertTrue(internalModel.initializedWithSingleSpeaker());
}

MODEL_TEST(initializeAdaptiveTestWithDelayedMaskerInitializesSingleSpeaker) {
    run(initializingAdaptiveTestWithDelayedMasker);
    assertTrue(internalModel.initializedWithDelayedMasker());
}

MODEL_TEST(initializeAdaptiveTestWithEyeTrackingInitializesWithEyeTracking) {
    run(initializingAdaptiveTestWithEyeTracking);
    assertTrue(internalModel.initializedWithEyeTracking());
}

MODEL_TEST(
    initializeFixedLevelTestWithTargetReplacementAndEyeTrackingInitializesWithEyeTracking) {
    run(initializingFixedLevelTestWithTargetReplacementAndEyeTracking);
    assertTrue(internalModel.initializedWithEyeTracking());
}

MODEL_TEST(
    initializeFixedLevelTestWithSilentIntervalTargetsAndEyeTrackingInitializesWithEyeTracking) {
    run(initializingFixedLevelTestWithSilentIntervalTargetsAndEyeTracking);
    assertTrue(internalModel.initializedWithEyeTracking());
}

MODEL_TEST(submitResponsePassesCoordinateResponse) {
    coordinate_response_measure::Response response;
    model.submit(response);
    assertEqual(&std::as_const(response), internalModel.coordinateResponse());
}

MODEL_TEST(submitCorrectKeywordsPassesCorrectKeywords) {
    CorrectKeywords k;
    model.submit(k);
    assertEqual(&std::as_const(k), internalModel.correctKeywords());
}

MODEL_TEST(playTrialPassesAudioSettings) {
    AudioSettings settings;
    model.playTrial(settings);
    assertEqual(&std::as_const(settings), internalModel.playTrialSettings());
}

MODEL_TEST(playCalibrationPassesCalibration) {
    Calibration calibration;
    model.playCalibration(calibration);
    assertEqual(&std::as_const(calibration), internalModel.calibration());
}

MODEL_TEST(testCompleteWhenComplete) {
    assertFalse(testComplete());
    internalModel.setComplete();
    assertTrue(testComplete());
}

MODEL_TEST(returnsAudioDevices) {
    internalModel.setAudioDevices({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, model.audioDevices());
}

MODEL_TEST(returnsAdaptiveTestResults) {
    adaptiveMethod.setTestResults({{"a", 1.}, {"b", 2.}, {"c", 3.}});
    assertEqual({{"a", 1.}, {"b", 2.}, {"c", 3.}}, model.adaptiveTestResults());
}

MODEL_TEST(returnsTrialNumber) {
    internalModel.setTrialNumber(1);
    assertEqual(1, model.trialNumber());
}

MODEL_TEST(returnsTargetFileName) {
    internalModel.setTargetFileName("a");
    assertEqual("a", model.targetFileName());
}

MODEL_TEST(subscribesToListener) {
    ModelEventListenerStub listener;
    model.subscribe(&listener);
    assertEqual(static_cast<const Model::EventListener *>(&listener),
        internalModel.listener());
}
}
}
