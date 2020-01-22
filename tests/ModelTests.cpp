#include "ModelEventListenerStub.h"
#include "TargetListStub.h"
#include "TestConcluderStub.h"
#include "assert-utility.h"
#include <recognition-test/Model.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
namespace {
class AdaptiveMethodStub : public AdaptiveMethod {
    const AdaptiveTest *test_{};

  public:
    void initialize(const AdaptiveTest &t) override { test_ = &t; }

    [[nodiscard]] auto test() const { return test_; }

    auto complete() -> bool override { return {}; }
    auto nextTarget() -> std::string override { return {}; }
    auto currentTarget() -> std::string override { return {}; }
    auto snr_dB() -> int override { return {}; }
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void submitResponse(const open_set::FreeResponse &) override {}
    void writeTestingParameters(OutputFile *) override {}
    void writeLastCoordinateResponse(OutputFile *) override {}
    void writeLastCorrectResponse(OutputFile *) override {}
    void writeLastIncorrectResponse(OutputFile *) override {}
    void submitResponse(
        const coordinate_response_measure::Response &) override {}
};

class FixedLevelMethodStub : public FixedLevelMethod {
    const FixedLevelTest *test_{};
    TargetList *targetList_{};
    TestConcluder *testConcluder_{};

  public:
    void initialize(const FixedLevelTest &t, TargetList *list,
        TestConcluder *concluder) override {
        testConcluder_ = concluder;
        targetList_ = list;
        test_ = &t;
    }

    [[nodiscard]] auto testConcluder() const { return testConcluder_; }

    [[nodiscard]] auto targetList() const { return targetList_; }

    [[nodiscard]] auto test() const { return test_; }

    auto complete() -> bool override { return {}; }
    auto nextTarget() -> std::string override { return {}; }
    auto currentTarget() -> std::string override { return {}; }
    auto snr_dB() -> int override { return {}; }
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void submitResponse(const open_set::FreeResponse &) override {}
    void writeTestingParameters(OutputFile *) override {}
    void writeLastCoordinateResponse(OutputFile *) override {}
    void writeLastCorrectResponse(OutputFile *) override {}
    void writeLastIncorrectResponse(OutputFile *) override {}
    void submitResponse(
        const coordinate_response_measure::Response &) override {}
};

class RecognitionTestModelStub : public RecognitionTestModel {
    std::vector<std::string> audioDevices_{};
    const Model::EventListener *listener_{};
    const Calibration *calibration_{};
    const AudioSettings *playTrialSettings_{};
    const Test *test_{};
    const TestMethod *testMethod_{};
    const coordinate_response_measure::Response *coordinateResponse_{};
    int trialNumber_{};
    bool complete_{};
    bool initializedWithSingleSpeaker_{};
    bool initializedWithDelayedMasker_{};
    bool initializedWithEyeTracking_{};

  public:
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

    void playTrial(const AudioSettings &s) override { playTrialSettings_ = &s; }

    void submitResponse(
        const coordinate_response_measure::Response &p) override {
        coordinateResponse_ = &p;
    }

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
    void submitResponse(const open_set::FreeResponse &) override {}
    void throwIfTrialInProgress() override {}
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

void initialize(ModelImpl &model, const FixedLevelTest &test) {
    model.initialize(test);
}

void initializeWithSingleSpeaker(ModelImpl &model, const AdaptiveTest &test) {
    model.initializeWithSingleSpeaker(test);
}

void initializeWithDelayedMasker(ModelImpl &model, const AdaptiveTest &test) {
    model.initializeWithDelayedMasker(test);
}

void initializeWithSilentIntervalTargets(
    ModelImpl &model, const FixedLevelTest &test) {
    model.initializeWithSilentIntervalTargets(test);
}

void initializeWithEyeTracking(ModelImpl &model, const FixedLevelTest &test) {
    model.initializeWithEyeTracking(test);
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

class InitializingDefaultFixedLevelTest : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingDefaultFixedLevelTest(FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override { initialize(model, test_); }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
        initialize(model, test);
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

class InitializingFixedLevelTestWithEyeTracking
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithEyeTracking(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        initializeWithEyeTracking(model, test_);
    }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
        initializeWithEyeTracking(model, test);
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

class ModelTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub adaptiveMethod;
    FixedLevelMethodStub fixedLevelMethod;
    TargetListStub infiniteTargetList;
    TestConcluderStub fixedTrialTestConcluder;
    TargetListStub silentIntervals;
    TestConcluderStub emptyTargetListTestConcluder;
    TargetListStub allStimuli;
    RecognitionTestModelStub internalModel;
    ModelImpl model{&adaptiveMethod, &fixedLevelMethod, &infiniteTargetList,
        &fixedTrialTestConcluder, &silentIntervals,
        &emptyTargetListTestConcluder, &allStimuli, &internalModel};
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
    InitializingDefaultFixedLevelTest initializingDefaultFixedLevelTest{
        &fixedLevelMethod};
    InitializingFixedLevelTestWithSilentIntervalTargets
        initializingFixedLevelTestWithSilentIntervalTargets{&fixedLevelMethod};
    InitializingFixedLevelTestWithEyeTracking
        initializingFixedLevelTestWithEyeTracking{&fixedLevelMethod};
    InitializingFixedLevelTestWithAllTargets
        initializingFixedLevelTestWithAllTargets{&fixedLevelMethod};

    auto testComplete() -> bool { return model.testComplete(); }

    void run(InitializingTestUseCase &useCase) { useCase.run(model); }

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

    void assertInitializesAdaptiveMethod(InitializingAdaptiveTest &useCase) {
        useCase.run(model, adaptiveTest);
        assertEqual(&std::as_const(adaptiveTest), adaptiveMethod.test());
    }

    void assertInitializesFixedLevelTestWithTestConcluder(
        InitializingTestUseCase &useCase, TestConcluder &concluder) {
        run(useCase);
        assertEqual(&concluder, fixedLevelMethod.testConcluder());
    }

    void assertInitializesFixedLevelTestWithTargetList(
        InitializingTestUseCase &useCase, TargetList &targetList) {
        run(useCase);
        assertEqual(&targetList, fixedLevelMethod.targetList());
    }
};

#define MODEL_TEST(a) TEST_F(ModelTests, a)

MODEL_TEST(initializeFixedLevelTestInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(initializingDefaultFixedLevelTest);
}

MODEL_TEST(initializeFixedLevelSilentIntervalsTestInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelTestWithSilentIntervalTargets);
}

MODEL_TEST(initializeFixedLevelEyeTrackingTestInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelTestWithEyeTracking);
}

MODEL_TEST(initializeFixedLevelTestInitializesWithInfiniteTargetList) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingDefaultFixedLevelTest, infiniteTargetList);
}

MODEL_TEST(
    initializeFixedLevelEyeTrackingTestInitializesWithInfiniteTargetList) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelTestWithEyeTracking, infiniteTargetList);
}

MODEL_TEST(
    initializeFixedLevelSilentIntervalsTestInitializesWithSilentIntervals) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelTestWithSilentIntervalTargets, silentIntervals);
}

MODEL_TEST(initializeFixedLevelAllStimuliTestInitializesWithAllStimuli) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelTestWithAllTargets, allStimuli);
}

MODEL_TEST(initializeFixedLevelTestInitializesWithFixedTrialTestConcluder) {
    assertInitializesFixedLevelTestWithTestConcluder(
        initializingDefaultFixedLevelTest, fixedTrialTestConcluder);
}

MODEL_TEST(
    initializeFixedLevelEyeTrackingTestInitializesWithFixedTrialTestConcluder) {
    assertInitializesFixedLevelTestWithTestConcluder(
        initializingFixedLevelTestWithEyeTracking, fixedTrialTestConcluder);
}

MODEL_TEST(
    initializeFixedLevelSilentIntervalsTestInitializesWithEmptyTargetListTestConcluder) {
    assertInitializesFixedLevelTestWithTestConcluder(
        initializingFixedLevelTestWithSilentIntervalTargets,
        emptyTargetListTestConcluder);
}

MODEL_TEST(
    initializeFixedLevelAllStimuliTestInitializesWithEmptyTargetListTestConcluder) {
    assertInitializesFixedLevelTestWithTestConcluder(
        initializingFixedLevelTestWithAllTargets, emptyTargetListTestConcluder);
}

TEST_F(ModelTests, initializeAdaptiveTestInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(initializingDefaultAdaptiveTest);
}

TEST_F(ModelTests, initializeAdaptiveEyeTrackingTestInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(initializingAdaptiveTestWithEyeTracking);
}

TEST_F(ModelTests,
    initializeAdaptiveTestWithSingleSpeakerInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(initializingAdaptiveTestWithSingleSpeaker);
}

TEST_F(ModelTests,
    initializeAdaptiveTestWithDelayedMaskerInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(initializingAdaptiveTestWithDelayedMasker);
}

MODEL_TEST(initializeFixedLevelTestInitializesInternalModel) {
    assertInitializesInternalModel(initializingDefaultFixedLevelTest);
}

MODEL_TEST(initializeFixedLevelEyeTrackingTestInitializesInternalModel) {
    assertInitializesInternalModel(initializingFixedLevelTestWithEyeTracking);
}

MODEL_TEST(initializeFixedLevelSilentIntervalsTestInitializesInternalModel) {
    assertInitializesInternalModel(
        initializingFixedLevelTestWithSilentIntervalTargets);
}

MODEL_TEST(initializeFixedLevelAllStimuliTestInitializesInternalModel) {
    assertInitializesInternalModel(initializingFixedLevelTestWithAllTargets);
}

MODEL_TEST(initializeAdaptiveTestInitializesInternalModel) {
    assertInitializesInternalModel(initializingDefaultAdaptiveTest);
}

MODEL_TEST(initializeAdaptiveEyeTrackingTestInitializesInternalModel) {
    assertInitializesInternalModel(initializingAdaptiveTestWithEyeTracking);
}

TEST_F(ModelTests,
    initializeAdaptiveTestWithSingleSpeakerInitializesInternalModel) {
    assertInitializesInternalModel(initializingAdaptiveTestWithSingleSpeaker);
}

TEST_F(ModelTests,
    initializeAdaptiveTestWithDelayedMaskerInitializesInternalModel) {
    assertInitializesInternalModel(initializingAdaptiveTestWithDelayedMasker);
}

TEST_F(ModelTests,
    initializeAdaptiveTestWithSingleSpeakerInitializesSingleSpeaker) {
    run(initializingAdaptiveTestWithSingleSpeaker);
    assertTrue(internalModel.initializedWithSingleSpeaker());
}

TEST_F(ModelTests,
    initializeAdaptiveTestWithDelayedMaskerInitializesSingleSpeaker) {
    run(initializingAdaptiveTestWithDelayedMasker);
    assertTrue(internalModel.initializedWithDelayedMasker());
}

TEST_F(
    ModelTests, initializeAdaptiveEyeTrackingTestInitializesWithEyeTracking) {
    run(initializingAdaptiveTestWithEyeTracking);
    assertTrue(internalModel.initializedWithEyeTracking());
}

TEST_F(
    ModelTests, initializeFixedLevelEyeTrackingTestInitializesWithEyeTracking) {
    run(initializingFixedLevelTestWithEyeTracking);
    assertTrue(internalModel.initializedWithEyeTracking());
}

TEST_F(ModelTests, submitResponsePassesCoordinateResponse) {
    coordinate_response_measure::Response response;
    model.submitResponse(response);
    assertEqual(&std::as_const(response), internalModel.coordinateResponse());
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

MODEL_TEST(returnsTrialNumber) {
    internalModel.setTrialNumber(1);
    assertEqual(1, model.trialNumber());
}

MODEL_TEST(subscribesToListener) {
    ModelEventListenerStub listener;
    model.subscribe(&listener);
    assertEqual(static_cast<const Model::EventListener *>(&listener),
        internalModel.listener());
}
}
}
