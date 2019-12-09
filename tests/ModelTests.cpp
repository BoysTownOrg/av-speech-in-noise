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

    [[nodiscard]] auto testConcluder() const { return testConcluder_; }

    [[nodiscard]] auto targetList() const { return targetList_; }

    [[nodiscard]] auto test() const { return test_; }

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

class RecognitionTestModelStub : public RecognitionTestModel {
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
    void initialize(
        TestMethod *tm, const Test &ct, const TestIdentity &ti) override {
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
    virtual void run(ModelImpl &) = 0;
    virtual const Test &test() = 0;
    virtual const TestIdentity &testIdentity() = 0;
    virtual const TestMethod *testMethod() = 0;
};

class InitializingFixedLevelTest : public virtual InitializingTestUseCase {
  public:
    virtual const FixedLevelTest &fixedLevelTest() = 0;
};

class InitializingAdaptiveTest : public virtual InitializingTestUseCase {
  public:
    virtual const AdaptiveTest &adaptiveTest() = 0;
};

class InitializingDefaultAdaptiveTest : public InitializingAdaptiveTest {
    AdaptiveTest test_;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingDefaultAdaptiveTest(AdaptiveMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override { model.initializeTest(test_); }

    const Test &test() override { return test_; }

    const AdaptiveTest &adaptiveTest() override { return test_; }

    const TestIdentity &testIdentity() override { return test_.identity; }

    const TestMethod *testMethod() override { return method; }
};

class InitializingAdaptiveTestWithSingleSpeaker
    : public InitializingAdaptiveTest {
    AdaptiveTest test_;
    AdaptiveMethodStub *method;

  public:
    explicit InitializingAdaptiveTestWithSingleSpeaker(
        AdaptiveMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override { model.initializeTest(test_); }

    const Test &test() override { return test_; }

    const AdaptiveTest &adaptiveTest() override { return test_; }

    const TestIdentity &testIdentity() override { return test_.identity; }

    const TestMethod *testMethod() override { return method; }
};

class InitializingDefaultFixedLevelTest : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingDefaultFixedLevelTest(FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override { model.initializeTest(test_); }

    const Test &test() override { return test_; }

    const FixedLevelTest &fixedLevelTest() override { return test_; }

    const TestIdentity &testIdentity() override { return test_.identity; }

    const TestMethod *testMethod() override { return method; }
};

class InitializingFixedLevelTestWithFiniteTargets
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelTestWithFiniteTargets(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        model.initializeTestWithFiniteTargets(test_);
    }

    const Test &test() override { return test_; }

    const TestIdentity &testIdentity() override { return test_.identity; }

    const TestMethod *testMethod() override { return method; }

    const FixedLevelTest &fixedLevelTest() override { return test_; }
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
    InitializingDefaultAdaptiveTest initializingAdaptiveTest{&adaptiveMethod};
    InitializingAdaptiveTestWithSingleSpeaker
        initializingAdaptiveTestWithSingleSpeaker{&adaptiveMethod};
    InitializingDefaultFixedLevelTest initializingFixedLevelTest{
        &fixedLevelMethod};
    InitializingFixedLevelTestWithFiniteTargets
        initializingFixedLevelTestWithFiniteTargets{&fixedLevelMethod};

    void initializeFixedLevelTest() { model.initializeTest(fixedLevelTest); }

    void initializeFixedLevelTestWithFiniteTargets() {
        model.initializeTestWithFiniteTargets(fixedLevelTest);
    }

    void initializeAdaptiveTest() { model.initializeTest(adaptiveTest); }

    void initializeAdaptiveTestWithSingleSpeaker() {
        model.initializeTestWithSingleSpeaker(adaptiveTest);
    }

    bool testComplete() { return model.testComplete(); }

    void run(InitializingTestUseCase &useCase) { useCase.run(model); }

    void assertInitializesInternalModel(InitializingTestUseCase &useCase) {
        run(useCase);
        assertEqual(useCase.testMethod(), internalModel.testMethod());
        assertEqual(&useCase.test(), internalModel.commonTest());
        assertEqual(&useCase.testIdentity(), internalModel.testIdentity());
    }

    void assertInitializesFixedLevelMethod(
        InitializingFixedLevelTest &useCase) {
        run(useCase);
        assertEqual(&useCase.fixedLevelTest(), fixedLevelMethod.test());
    }

    void assertInitializesAdaptiveMethod(InitializingAdaptiveTest &useCase) {
        run(useCase);
        assertEqual(&useCase.adaptiveTest(), adaptiveMethod.test());
    }
};

TEST_F(ModelTests, initializeFixedLevelTestInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(initializingFixedLevelTest);
}

TEST_F(ModelTests,
    initializeFixedLevelTestWithFiniteTargetsInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelTestWithFiniteTargets);
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
    assertInitializesAdaptiveMethod(initializingAdaptiveTest);
}

TEST_F(ModelTests,
    initializeAdaptiveTestWithSingleSpeakerInitializesAdaptiveMethod) {
    assertInitializesAdaptiveMethod(initializingAdaptiveTestWithSingleSpeaker);
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
