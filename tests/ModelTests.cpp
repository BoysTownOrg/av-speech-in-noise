#include "ModelEventListenerStub.h"
#include "TargetListStub.h"
#include "TestConcluderStub.h"
#include "assert-utility.h"
#include "av-speech-in-noise/Model.hpp"
#include <gtest/gtest.h>
#include <recognition-test/Model.hpp>

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

  public:
    [[nodiscard]] auto initializedWithSingleSpeaker() const -> bool {
        return initializedWithSingleSpeaker_;
    }

    [[nodiscard]] auto initializedWithDelayedMasker() const -> bool {
        return initializedWithDelayedMasker_;
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

    [[nodiscard]] auto coordinateResponse() const {
        return coordinateResponse_;
    }

    [[nodiscard]] auto testMethod() const { return testMethod_; }

    [[nodiscard]] auto test() const { return test_; }

    [[nodiscard]] auto playTrialSettings() const { return playTrialSettings_; }

    [[nodiscard]] auto calibration() const { return calibration_; }

    void setComplete() { complete_ = true; }

    void setAudioDevices(std::vector<std::string> v) {
        audioDevices_ = std::move(v);
    }

    [[nodiscard]] auto listener() const { return listener_; }

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

void initializeTest(ModelImpl &model, const AdaptiveTest &test) {
    model.initializeTest(test);
}

void initializeTest(ModelImpl &model, const FixedLevelTest &test) {
    model.initializeTest(test);
}

void initializeTestWithSingleSpeaker(
    ModelImpl &model, const AdaptiveTest &test) {
    model.initializeTestWithSingleSpeaker(test);
}

void initializeTestWithDelayedMasker(
    ModelImpl &model, const AdaptiveTest &test) {
    model.initializeTestWithDelayedMasker(test);
}

void initializeSilentIntervalsTest(
    ModelImpl &model, const FixedLevelTest &test) {
    model.initializeSilentIntervalsTest(test);
}

void initializeEyeTrackingTest(
    ModelImpl &model, const FixedLevelTest &test) {
    model.initializeEyeTrackingTest(test);
}

class InitializingDefaultAdaptiveTest : public InitializingAdaptiveTest {
    AdaptiveTest test_;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingDefaultAdaptiveTest(AdaptiveMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override { initializeTest(model, test_); }

    void run(ModelImpl &model, const AdaptiveTest &test) override {
        initializeTest(model, test);
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
        initializeTestWithSingleSpeaker(model, test_);
    }

    void run(ModelImpl &model, const AdaptiveTest &test) override {
        initializeTestWithSingleSpeaker(model, test);
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
        initializeTestWithDelayedMasker(model, test_);
    }

    void run(ModelImpl &model, const AdaptiveTest &test) override {
        initializeTestWithDelayedMasker(model, test);
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

    void run(ModelImpl &model) override { initializeTest(model, test_); }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
        initializeTest(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelSilentIntervalsTest
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelSilentIntervalsTest(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        initializeSilentIntervalsTest(model, test_);
    }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
        initializeSilentIntervalsTest(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelEyeTrackingTest
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelEyeTrackingTest(FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        initializeEyeTrackingTest(model, test_);
    }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
        initializeEyeTrackingTest(model, test);
    }

    auto test() -> const Test & override { return test_; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelAllStimuliTest : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelAllStimuliTest(FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        model.initializeAllStimuliTest(test_);
    }

    void run(ModelImpl &model, const FixedLevelTest &test) override {
        model.initializeAllStimuliTest(test);
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
    InitializingDefaultAdaptiveTest initializingAdaptiveTest{&adaptiveMethod};
    InitializingAdaptiveTestWithSingleSpeaker
        initializingAdaptiveTestWithSingleSpeaker{&adaptiveMethod};
    InitializingAdaptiveTestWithDelayedMasker
        initializingAdaptiveTestWithDelayedMasker{&adaptiveMethod};
    InitializingDefaultFixedLevelTest initializingFixedLevelTest{
        &fixedLevelMethod};
    InitializingFixedLevelSilentIntervalsTest
        initializingFixedLevelSilentIntervalsTest{&fixedLevelMethod};
    InitializingFixedLevelEyeTrackingTest initializingFixedLevelEyeTrackingTest{
        &fixedLevelMethod};
    InitializingFixedLevelAllStimuliTest initializingFixedLevelAllStimuliTest{
        &fixedLevelMethod};

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
    assertInitializesFixedLevelMethod(initializingFixedLevelTest);
}

MODEL_TEST(initializeFixedLevelSilentIntervalsTestInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelSilentIntervalsTest);
}

MODEL_TEST(initializeFixedLevelEyeTrackingTestInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(initializingFixedLevelEyeTrackingTest);
}

MODEL_TEST(initializeFixedLevelTestInitializesWithInfiniteTargetList) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelTest, infiniteTargetList);
}

MODEL_TEST(initializeFixedLevelEyeTrackingTestInitializesWithInfiniteTargetList) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelEyeTrackingTest, infiniteTargetList);
}

MODEL_TEST(
    initializeFixedLevelSilentIntervalsTestInitializesWithSilentIntervals) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelSilentIntervalsTest, silentIntervals);
}

MODEL_TEST(initializeFixedLevelAllStimuliTestInitializesWithAllStimuli) {
    assertInitializesFixedLevelTestWithTargetList(
        initializingFixedLevelAllStimuliTest, allStimuli);
}

MODEL_TEST(initializeFixedLevelTestInitializesWithFixedTrialTestConcluder) {
    assertInitializesFixedLevelTestWithTestConcluder(
        initializingFixedLevelTest, fixedTrialTestConcluder);
}

MODEL_TEST(
    initializeFixedLevelSilentIntervalsTestInitializesWithEmptyTargetListTestConcluder) {
    assertInitializesFixedLevelTestWithTestConcluder(
        initializingFixedLevelSilentIntervalsTest,
        emptyTargetListTestConcluder);
}

MODEL_TEST(
    initializeFixedLevelAllStimuliTestInitializesWithEmptyTargetListTestConcluder) {
    assertInitializesFixedLevelTestWithTestConcluder(
        initializingFixedLevelAllStimuliTest, emptyTargetListTestConcluder);
}

TEST_F(ModelTests, initializeAdaptiveTestInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(initializingAdaptiveTest);
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
    assertInitializesInternalModel(initializingFixedLevelTest);
}

MODEL_TEST(
    initializeFixedLevelSilentIntervalsTestTargetsInitializesInternalModel) {
    assertInitializesInternalModel(initializingFixedLevelSilentIntervalsTest);
}

MODEL_TEST(initializeAdaptiveTestInitializesInternalModel) {
    assertInitializesInternalModel(initializingAdaptiveTest);
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
