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
    auto next() -> std::string override { return {}; }
    auto current() -> std::string override { return {}; }
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
    auto next() -> std::string override { return {}; }
    auto current() -> std::string override { return {}; }
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
    auto initializedWithSingleSpeaker() const {
        return initializedWithSingleSpeaker_;
    }

    auto initializedWithDelayedMasker() const {
        return initializedWithDelayedMasker_;
    }

    void initialize(TestMethod *tm, const Test &ct) override {
        testMethod_ = tm;
        test_ = &ct;
    }

    void initializeWithSingleSpeaker(TestMethod *tm, const Test &ct) override {
        testMethod_ = tm;
        test_ = &ct;
        initializedWithSingleSpeaker_ = true;
    }

    void initializeWithDelayedMasker(TestMethod *tm, const Test &ct) override {
        testMethod_ = tm;
        test_ = &ct;
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

    void run(ModelImpl &model) override {
        model.initializeTestWithSingleSpeaker(test_);
    }

    const Test &test() override { return test_; }

    const AdaptiveTest &adaptiveTest() override { return test_; }

    const TestMethod *testMethod() override { return method; }
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
        model.initializeTestWithDelayedMasker(test_);
    }

    const Test &test() override { return test_; }

    const AdaptiveTest &adaptiveTest() override { return test_; }

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
        model.initializeSilentIntervalsTest(test_);
    }

    const Test &test() override { return test_; }

    const TestMethod *testMethod() override { return method; }

    const FixedLevelTest &fixedLevelTest() override { return test_; }
};

class InitializingFixedLevelAllStimuliTest
    : public InitializingFixedLevelTest {
    FixedLevelTest test_;
    FixedLevelMethodStub *method;

  public:
    explicit InitializingFixedLevelAllStimuliTest(
        FixedLevelMethodStub *method)
        : method{method} {}

    void run(ModelImpl &model) override {
        model.initializeAllStimuliTest(test_);
    }

    const Test &test() override { return test_; }

    const TestMethod *testMethod() override { return method; }

    const FixedLevelTest &fixedLevelTest() override { return test_; }
};

class ModelTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub adaptiveMethod;
    FixedLevelMethodStub fixedLevelMethod;
    TargetListStub infiniteTargetList;
    TestConcluderStub fixedTrialTestConcluder;
    TargetListStub silentIntervals;
    TestConcluderStub emptyTargetListTestConcluder;
    RecognitionTestModelStub internalModel;
    ModelImpl model{&adaptiveMethod, &fixedLevelMethod, &infiniteTargetList,
        &fixedTrialTestConcluder, &silentIntervals,
        &emptyTargetListTestConcluder, &internalModel};
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
    InitializingFixedLevelAllStimuliTest
        initializingFixedLevelAllStimuliTest{&fixedLevelMethod};

    void initializeFixedLevelTest() { model.initializeTest(fixedLevelTest); }

    void initializeFixedLevelSilentIntervalsTest() {
        model.initializeSilentIntervalsTest(fixedLevelTest);
    }

    void initializeFixedLevelAllStimuliTest() {
        model.initializeAllStimuliTest(fixedLevelTest);
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
        assertEqual(&useCase.test(), internalModel.test());
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

    void assertInitializesFixedLevelTestWithTestConcluder(
        InitializingTestUseCase &useCase, TestConcluder &concluder) {
        run(useCase);
        assertEqual(&concluder, fixedLevelMethod.testConcluder());
    }
};

#define MODEL_TEST(a) TEST_F(ModelTests, a)

MODEL_TEST(initializeFixedLevelTestInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(initializingFixedLevelTest);
}

MODEL_TEST(
    initializeFixedLevelSilentIntervalsTestInitializesFixedLevelMethod) {
    assertInitializesFixedLevelMethod(
        initializingFixedLevelSilentIntervalsTest);
}

MODEL_TEST(initializeFixedLevelTestInitializesWithInfiniteTargetList) {
    initializeFixedLevelTest();
    assertEqual(static_cast<TargetList *>(&infiniteTargetList),
        fixedLevelMethod.targetList());
}

MODEL_TEST(
    initializeFixedLevelSilentIntervalsTestInitializesWithSilentIntervals) {
    initializeFixedLevelSilentIntervalsTest();
    assertEqual(static_cast<TargetList *>(&silentIntervals),
        fixedLevelMethod.targetList());
}

MODEL_TEST(initializeFixedLevelTestInitializesWithFixedTrialTestConcluder) {
    assertInitializesFixedLevelTestWithTestConcluder(
        initializingFixedLevelTest, fixedTrialTestConcluder);
}

MODEL_TEST(
    initializeFixedLevelSilentIntervalsTestInitializesWithEmptyTargetListTestConcluder) {
    assertInitializesFixedLevelTestWithTestConcluder(
        initializingFixedLevelSilentIntervalsTest, emptyTargetListTestConcluder);
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

MODEL_TEST(initializeFixedLevelSilentIntervalsTestTargetsInitializesInternalModel) {
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
