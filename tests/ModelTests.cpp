#include "ModelEventListenerStub.h"
#include "TargetListStub.h"
#include "TestConcluderStub.h"
#include "assert-utility.h"
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
    auto next() -> std::string override { return {}; }
    auto current() -> std::string override { return {}; }
    auto snr_dB() -> int override { return {}; }
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void submitResponse(const FreeResponse &) override {}
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
    auto next() -> std::string override { return {}; }
    auto current() -> std::string override { return {}; }
    auto snr_dB() -> int override { return {}; }
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void submitResponse(const FreeResponse &) override {}
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

  public:
    void initialize(TestMethod *tm, const Test &ct) override {
        testMethod_ = tm;
        test_ = &ct;
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
    void submitResponse(const FreeResponse &) override {}
    void throwIfTrialInProgress() override {}
};

class InitializingTestUseCase {
  public:
    virtual ~InitializingTestUseCase() = default;
    virtual void run(ModelImpl &) = 0;
    virtual auto commonTest() -> const Test & = 0;
    virtual auto testMethod() -> const TestMethod * = 0;
};

class InitializingAdaptiveTest : public InitializingTestUseCase {
    AdaptiveTest test;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingAdaptiveTest(AdaptiveMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override { model.initializeTest(test); }

    auto commonTest() -> const Test & override { return test; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTest : public InitializingTestUseCase {
    FixedLevelTest test;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTest(FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override { model.initializeTest(test); }

    auto commonTest() -> const Test & override { return test; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class InitializingFixedLevelTestWithFiniteTargets
    : public InitializingTestUseCase {
    FixedLevelTest test;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithFiniteTargets(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        model.initializeTestWithFiniteTargets(test);
    }

    auto commonTest() -> const Test & override { return test; }

    auto testMethod() -> const TestMethod * override { return method; }
};

class ModelTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub adaptiveMethod;
    FixedLevelMethodStub fixedLevelMethod;
    TargetListStub infiniteTargetList;
    TestConcluderStub fixedTrialTestConcluder;
    TargetListStub finiteTargetList;
    TestConcluderStub emptyTargetListTestConcluder;
    RecognitionTestModelStub internalModel;
    ModelImpl model{&adaptiveMethod, &fixedLevelMethod, &infiniteTargetList,
        &fixedTrialTestConcluder, &finiteTargetList,
        &emptyTargetListTestConcluder, &internalModel};
    AdaptiveTest adaptiveTest;
    FixedLevelTest fixedLevelTest;
    InitializingAdaptiveTest initializingAdaptiveTest{&adaptiveMethod};
    InitializingFixedLevelTest initializingFixedLevelTest{&fixedLevelMethod};
    InitializingFixedLevelTestWithFiniteTargets
        initializingFixedLevelTestWithFiniteTargets{&fixedLevelMethod};

    void initializeFixedLevelTest() { model.initializeTest(fixedLevelTest); }

    void initializeFixedLevelTestWithFiniteTargets() {
        model.initializeTestWithFiniteTargets(fixedLevelTest);
    }

    void initializeAdaptiveTest() { model.initializeTest(adaptiveTest); }

    bool testComplete() { return model.testComplete(); }

    void run(InitializingTestUseCase &useCase) { useCase.run(model); }

    void assertInitializesInternalModel(InitializingTestUseCase &useCase) {
        run(useCase);
        assertEqual(useCase.testMethod(), internalModel.testMethod());
        assertEqual(&useCase.commonTest(), internalModel.test());
    }
};

TEST_F(ModelTests, initializeFixedLevelTestInitializesFixedLevelMethod) {
    initializeFixedLevelTest();
    assertEqual(&std::as_const(fixedLevelTest), fixedLevelMethod.test());
}

TEST_F(ModelTests,
    initializeFixedLevelTestWithFiniteTargetsInitializesFixedLevelMethod) {
    initializeFixedLevelTestWithFiniteTargets();
    assertEqual(&std::as_const(fixedLevelTest), fixedLevelMethod.test());
}

TEST_F(ModelTests, initializeFixedLevelTestInitializesWithInfiniteTargetList) {
    initializeFixedLevelTest();
    assertEqual(static_cast<TargetList *>(&infiniteTargetList),
        fixedLevelMethod.targetList());
}

TEST_F(ModelTests,
    initializeFixedLevelTestWithFiniteTargetsInitializesWithFiniteTargets) {
    initializeFixedLevelTestWithFiniteTargets();
    assertEqual(static_cast<TargetList *>(&finiteTargetList),
        fixedLevelMethod.targetList());
}

TEST_F(ModelTests,
    initializeFixedLevelTestInitializesWithFixedTrialTestConcluder) {
    initializeFixedLevelTest();
    assertEqual(static_cast<TestConcluder *>(&fixedTrialTestConcluder),
        fixedLevelMethod.testConcluder());
}

TEST_F(ModelTests,
    initializeFixedLevelTestWithFiniteTargetsInitializesWithEmptyTargetListTestConcluder) {
    initializeFixedLevelTestWithFiniteTargets();
    assertEqual(static_cast<TestConcluder *>(&emptyTargetListTestConcluder),
        fixedLevelMethod.testConcluder());
}

TEST_F(ModelTests, initializeAdaptiveTestInitializesAdaptiveMethod) {
    initializeAdaptiveTest();
    assertEqual(&std::as_const(adaptiveTest), adaptiveMethod.test());
}

TEST_F(ModelTests, initializeFixedLevelTestInitializesInternalModel) {
    assertInitializesInternalModel(initializingFixedLevelTest);
}

TEST_F(ModelTests,
    initializeFixedLevelTestWithFiniteTargetsInitializesInternalModel) {
    assertInitializesInternalModel(initializingFixedLevelTestWithFiniteTargets);
}

TEST_F(ModelTests, initializeAdaptiveTestInitializesInternalModel) {
    assertInitializesInternalModel(initializingAdaptiveTest);
}

TEST_F(ModelTests, submitResponsePassesCoordinateResponse) {
    coordinate_response_measure::Response response;
    model.submitResponse(response);
    assertEqual(&std::as_const(response), internalModel.coordinateResponse());
}

TEST_F(ModelTests, playTrialPassesAudioSettings) {
    AudioSettings settings;
    model.playTrial(settings);
    assertEqual(&std::as_const(settings), internalModel.playTrialSettings());
}

TEST_F(ModelTests, playCalibrationPassesCalibration) {
    Calibration calibration;
    model.playCalibration(calibration);
    assertEqual(&std::as_const(calibration), internalModel.calibration());
}

TEST_F(ModelTests, testCompleteWhenComplete) {
    assertFalse(testComplete());
    internalModel.setComplete();
    assertTrue(testComplete());
}

TEST_F(ModelTests, returnsAudioDevices) {
    internalModel.setAudioDevices({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, model.audioDevices());
}

TEST_F(ModelTests, returnsTrialNumber) {
    internalModel.setTrialNumber(1);
    assertEqual(1, model.trialNumber());
}

TEST_F(ModelTests, subscribesToListener) {
    ModelEventListenerStub listener;
    model.subscribe(&listener);
    assertEqual(static_cast<const Model::EventListener *>(&listener),
        internalModel.listener());
}
}
}
