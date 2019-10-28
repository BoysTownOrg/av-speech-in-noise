#include "ModelEventListenerStub.h"
#include "TargetListStub.h"
#include "TestConcluderStub.h"
#include "assert-utility.h"
#include <gtest/gtest.h>
#include <recognition-test/RecognitionTestModel.hpp>

namespace av_speech_in_noise::tests {
namespace {
class AdaptiveMethodStub : public AdaptiveMethod {
    const AdaptiveTest *test_{};

  public:
    void initialize(const AdaptiveTest &t) override { test_ = &t; }

    auto test() const { return test_; }

    bool complete() override { return {}; }
    std::string next() override { return {}; }
    std::string current() override { return {}; }
    int snr_dB() override { return {}; }
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

    auto testConcluder() const { return testConcluder_; }

    auto targetList() const { return targetList_; }

    auto test() const { return test_; }

    bool complete() override { return {}; }
    std::string next() override { return {}; }
    std::string current() override { return {}; }
    int snr_dB() override { return {}; }
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

class RecognitionTestModel_InternalStub
    : public RecognitionTestModel_ {
    std::vector<std::string> audioDevices_{};
    const Model::EventListener *listener_{};
    const Calibration *calibration_{};
    const AudioSettings *playTrialSettings_{};
    const TestIdentity *testInformation_{};
    const Test *commonTest_{};
    const TestMethod *testMethod_{};
    const coordinate_response_measure::Response *coordinateResponse_{};
    int trialNumber_;
    bool complete_{};

  public:
    void initialize(TestMethod *tm, const Test &ct,
        const TestIdentity &ti) override {
        testMethod_ = tm;
        commonTest_ = &ct;
        testInformation_ = &ti;
    }

    int trialNumber() override { return trialNumber_; }

    void setTrialNumber(int n) { trialNumber_ = n; }

    void playTrial(const AudioSettings &s) override { playTrialSettings_ = &s; }

    void submitResponse(
        const coordinate_response_measure::Response &p) override {
        coordinateResponse_ = &p;
    }

    bool testComplete() override { return complete_; }

    std::vector<std::string> audioDevices() override { return audioDevices_; }

    void subscribe(Model::EventListener *e) override { listener_ = e; }

    void playCalibration(const Calibration &c) override { calibration_ = &c; }

    auto coordinateResponse() const { return coordinateResponse_; }

    auto testMethod() const { return testMethod_; }

    auto commonTest() const { return commonTest_; }

    auto testIdentity() const { return testInformation_; }

    auto playTrialSettings() const { return playTrialSettings_; }

    auto calibration() const { return calibration_; }

    void setComplete() { complete_ = true; }

    void setAudioDevices(std::vector<std::string> v) {
        audioDevices_ = std::move(v);
    }

    auto listener() const { return listener_; }

    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void submitResponse(const FreeResponse &) override {}
    void throwIfTrialInProgress() override {}
};

class InitializingTestUseCase {
  public:
    virtual ~InitializingTestUseCase() = default;
    virtual void run(RecognitionTestModel &) = 0;
    virtual const Test &commonTest() = 0;
    virtual const TestIdentity &testIdentity() = 0;
    virtual const TestMethod *testMethod() = 0;
};

class InitializingAdaptiveTest : public InitializingTestUseCase {
    AdaptiveTest test;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingAdaptiveTest(AdaptiveMethodStub *method)
        : method{method} {}

    void run(RecognitionTestModel &model) override {
        model.initializeTest(test);
    }

    const Test &commonTest() override { return test; }

    const TestIdentity &testIdentity() override {
        return test.identity;
    }

    const TestMethod *testMethod() override { return method; }
};

class InitializingFixedLevelTest : public InitializingTestUseCase {
    FixedLevelTest test;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTest(FixedLevelMethodStub *method)
        : method{method} {}

    void run(RecognitionTestModel &model) override {
        model.initializeTest(test);
    }

    const Test &commonTest() override { return test; }

    const TestIdentity &testIdentity() override {
        return test.identity;
    }

    const TestMethod *testMethod() override { return method; }
};

class InitializingFixedLevelTestWithFiniteTargets
    : public InitializingTestUseCase {
    FixedLevelTest test;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithFiniteTargets(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(RecognitionTestModel &model) override {
        model.initializeTestWithFiniteTargets(test);
    }

    const Test &commonTest() override { return test; }

    const TestIdentity &testIdentity() override {
        return test.identity;
    }

    const TestMethod *testMethod() override { return method; }
};

class RecognitionTestModelTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub adaptiveMethod;
    FixedLevelMethodStub fixedLevelMethod;
    TargetListStub infiniteTargetList;
    TestConcluderStub fixedTrialTestConcluder;
    TargetListStub finiteTargetList;
    TestConcluderStub emptyTargetListTestConcluder;
    RecognitionTestModel_InternalStub internalModel;
    RecognitionTestModel model{&adaptiveMethod, &fixedLevelMethod,
        &infiniteTargetList, &fixedTrialTestConcluder, &finiteTargetList,
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
        assertEqual(&useCase.commonTest(), internalModel.commonTest());
        assertEqual(
            &useCase.testIdentity(), internalModel.testIdentity());
    }
};

TEST_F(RecognitionTestModelTests,
    initializeFixedLevelTestInitializesFixedLevelMethod) {
    initializeFixedLevelTest();
    assertEqual(&std::as_const(fixedLevelTest), fixedLevelMethod.test());
}

TEST_F(RecognitionTestModelTests,
    initializeFixedLevelTestWithFiniteTargetsInitializesFixedLevelMethod) {
    initializeFixedLevelTestWithFiniteTargets();
    assertEqual(&std::as_const(fixedLevelTest), fixedLevelMethod.test());
}

TEST_F(RecognitionTestModelTests,
    initializeFixedLevelTestInitializesWithInfiniteTargetList) {
    initializeFixedLevelTest();
    assertEqual(static_cast<TargetList *>(&infiniteTargetList),
        fixedLevelMethod.targetList());
}

TEST_F(RecognitionTestModelTests,
    initializeFixedLevelTestWithFiniteTargetsInitializesWithFiniteTargets) {
    initializeFixedLevelTestWithFiniteTargets();
    assertEqual(static_cast<TargetList *>(&finiteTargetList),
        fixedLevelMethod.targetList());
}

TEST_F(RecognitionTestModelTests,
    initializeFixedLevelTestInitializesWithFixedTrialTestConcluder) {
    initializeFixedLevelTest();
    assertEqual(static_cast<TestConcluder *>(&fixedTrialTestConcluder),
        fixedLevelMethod.testConcluder());
}

TEST_F(RecognitionTestModelTests,
    initializeFixedLevelTestWithFiniteTargetsInitializesWithEmptyTargetListTestConcluder) {
    initializeFixedLevelTestWithFiniteTargets();
    assertEqual(static_cast<TestConcluder *>(&emptyTargetListTestConcluder),
        fixedLevelMethod.testConcluder());
}

TEST_F(RecognitionTestModelTests,
    initializeAdaptiveTestInitializesAdaptiveMethod) {
    initializeAdaptiveTest();
    assertEqual(&std::as_const(adaptiveTest), adaptiveMethod.test());
}

TEST_F(RecognitionTestModelTests,
    initializeFixedLevelTestInitializesInternalModel) {
    assertInitializesInternalModel(initializingFixedLevelTest);
}

TEST_F(RecognitionTestModelTests,
    initializeFixedLevelTestWithFiniteTargetsInitializesInternalModel) {
    assertInitializesInternalModel(initializingFixedLevelTestWithFiniteTargets);
}

TEST_F(
    RecognitionTestModelTests, initializeAdaptiveTestInitializesInternalModel) {
    assertInitializesInternalModel(initializingAdaptiveTest);
}

TEST_F(RecognitionTestModelTests, submitResponsePassesCoordinateResponse) {
    coordinate_response_measure::Response response;
    model.submitResponse(response);
    assertEqual(&std::as_const(response), internalModel.coordinateResponse());
}

TEST_F(RecognitionTestModelTests, playTrialPassesAudioSettings) {
    AudioSettings settings;
    model.playTrial(settings);
    assertEqual(&std::as_const(settings), internalModel.playTrialSettings());
}

TEST_F(RecognitionTestModelTests, playCalibrationPassesCalibration) {
    Calibration calibration;
    model.playCalibration(calibration);
    assertEqual(&std::as_const(calibration), internalModel.calibration());
}

TEST_F(RecognitionTestModelTests, testCompleteWhenComplete) {
    assertFalse(testComplete());
    internalModel.setComplete();
    assertTrue(testComplete());
}

TEST_F(RecognitionTestModelTests, returnsAudioDevices) {
    internalModel.setAudioDevices({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, model.audioDevices());
}

TEST_F(RecognitionTestModelTests, returnsTrialNumber) {
    internalModel.setTrialNumber(1);
    assertEqual(1, model.trialNumber());
}

TEST_F(RecognitionTestModelTests, subscribesToListener) {
    ModelEventListenerStub listener;
    model.subscribe(&listener);
    assertEqual(static_cast<const Model::EventListener *>(&listener),
        internalModel.listener());
}
} // namespace
} // namespace av_speech_in_noise::tests
