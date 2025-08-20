#include "AdaptiveMethodStub.hpp"
#include "FixedLevelMethodStub.hpp"
#include "RunningATestStub.hpp"
#include "TargetPlaylistSetReaderStub.hpp"
#include "TargetPlaylistStub.hpp"
#include "assert-utility.hpp"

#include <av-speech-in-noise/ui/TestSettingsInterpreter.hpp>
#include <av-speech-in-noise/ui/SessionController.hpp>

#include <gtest/gtest.h>

#include <stdexcept>
#include <string>

namespace av_speech_in_noise {
namespace {
class SessionControllerStub : public SessionController {
  public:
    void notifyThatTestIsComplete() override {}

    void prepare(TaskPresenter &p) override {
        taskPresenter_ = &p;
        prepareCalled_ = true;
    }

    auto taskPresenter() -> const TaskPresenter * { return taskPresenter_; }

    [[nodiscard]] auto prepareCalled() const -> bool { return prepareCalled_; }

  private:
    const TaskPresenter *taskPresenter_{};
    bool prepareCalled_{};
};

class TaskPresenterStub : public TaskPresenter {
  public:
    void showResponseSubmission() override {}
    void hideResponseSubmission() override {}
    void start() override {}
    void stop() override {}
    void complete() override {}
    void notifyThatTrialHasStarted() override {}
};

auto concatenate(const std::vector<std::string> &v) -> std::string {
    std::string result;
    for (const auto &v_ : v)
        result.append(v_);
    return result;
}

auto entry(TestSetting p, std::string s) -> std::string {
    return std::string{name(p)} + ": " + std::move(s);
}

auto withNewLine(std::string s) -> std::string { return std::move(s) + '\n'; }

auto entryWithNewline(TestSetting p, std::string s) -> std::string {
    return withNewLine(entry(p, std::move(s)));
}

auto entryWithNewline(TestSetting p, Method m) -> std::string {
    return entryWithNewline(p, name(m));
}

void initializeTest(TestSettingsInterpreterImpl &interpreter,
    const std::vector<std::string> &v, int startingSnr = {},
    const TestIdentity &identity = {}) {
    interpreter.initializeTest(
        concatenate(v), identity, std::to_string(startingSnr));
}

void initializeTest(TestSettingsInterpreterImpl &interpreter, Method m,
    const TestIdentity &identity = {}, int startingSnr = {}) {
    initializeTest(interpreter, {entryWithNewline(TestSetting::method, m)},
        startingSnr, identity);
}

void initializeTest(TestSettingsInterpreterImpl &interpreter, Method m,
    const std::string &extendedName, const TestIdentity &identity = {},
    int startingSnr = {}) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, name(m) + (" " + extendedName))},
        startingSnr, identity);
}

class ConfigurableStub : public Configurable {
  public:
    void configure(const std::string &key, const std::string &value) override {
        this->key = key;
        this->value = value;
    }
    std::string key;
    std::string value;
};

void assertOverridesStartingSnr(
    TestSettingsInterpreterImpl &interpreter, Method m) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "starting SNR (dB)");
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, m), "starting SNR (dB): 6\n"},
        5);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("6", configurable.value);
}

class TestSettingsInterpreterTests : public ::testing::Test {
  protected:
    RunningATestStub runningATest;
    AdaptiveMethodStub adaptiveMethod;
    FixedLevelMethodStub fixedLevelMethod;
    TargetPlaylistSetReaderStub cyclicTargetsReader;
    TargetPlaylistSetReaderStub targetsWithReplacementReader;
    FiniteTargetPlaylistWithRepeatablesStub everyTargetOnce;
    FiniteTargetPlaylistWithRepeatablesStub silentIntervalTargets;
    TargetPlaylistStub targetsWithReplacement;
    SessionControllerStub sessionController;
    TaskPresenterStub keypressPresenter;
    TaskPresenterStub fixedPassFailPresenter;
    TestSettingsInterpreterImpl interpreter{runningATest, adaptiveMethod,
        fixedLevelMethod, cyclicTargetsReader, targetsWithReplacementReader,
        everyTargetOnce, silentIntervalTargets, targetsWithReplacement,
        sessionController, keypressPresenter, fixedPassFailPresenter};
    TestIdentity testIdentity;
};

#define TEST_SETTINGS_INTERPRETER_TEST(a)                                      \
    TEST_F(TestSettingsInterpreterTests, a)

#define TEST_SETTINGS_INTREPRETER_TEST_EXPECT_INITIALIZES_FIXED_LEVEL_TEST_WITH_ALL_TARGETS( \
    m)                                                                                       \
    initializeTest(interpreter, m);                                                          \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(                                                         \
        fixedLevelMethod.targetList, &everyTargetOnce)

#define ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_METHOD(m)                       \
    initializeTest(interpreter, m);                                            \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &adaptiveMethod);

#define ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(m)                    \
    initializeTest(interpreter, m);                                            \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(runningATest.testMethod, &fixedLevelMethod);

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsInitializesFixedLevelTest) {
    TEST_SETTINGS_INTREPRETER_TEST_EXPECT_INITIALIZES_FIXED_LEVEL_TEST_WITH_ALL_TARGETS(
        Method::fixedLevelFreeResponseWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelChooseKeywordsWithAllTargetsInitializesFixedLevelTest) {
    TEST_SETTINGS_INTREPRETER_TEST_EXPECT_INITIALIZES_FIXED_LEVEL_TEST_WITH_ALL_TARGETS(
        Method::fixedLevelChooseKeywordsWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelSyllablesWithAllTargetsInitializesFixedLevelTest) {
    TEST_SETTINGS_INTREPRETER_TEST_EXPECT_INITIALIZES_FIXED_LEVEL_TEST_WITH_ALL_TARGETS(
        Method::fixedLevelSyllablesWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsAndAudioRecordingSelectsAudioRecordingPeripheral) {
    initializeTest(interpreter, Method::fixedLevelFreeResponseWithAllTargets,
        "audio recording");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &everyTargetOnce);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptivePassFailInitializesAdaptiveTest) {
    initializeTest(interpreter, Method::adaptivePassFail);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader, &targetsWithReplacementReader);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(runningATest.observer.empty());
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptivePassFailWithEyeTrackingInitializesAdaptiveTest) {
    initializeTest(interpreter, Method::adaptivePassFail, "eye tracking");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader, &targetsWithReplacementReader);
}

TEST_SETTINGS_INTERPRETER_TEST(adaptiveCorrectKeywordsInitializesAdaptiveTest) {
    initializeTest(interpreter, Method::adaptiveCorrectKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader, &cyclicTargetsReader);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(runningATest.observer.empty());
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCorrectKeywordsWithEyeTrackingInitializesAdaptiveTest) {
    initializeTest(
        interpreter, Method::adaptiveCorrectKeywords, "eye tracking");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader, &cyclicTargetsReader);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureInitializesAdaptiveTest) {
    initializeTest(interpreter, Method::adaptiveCoordinateResponseMeasure);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader, &targetsWithReplacementReader);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(runningATest.observer.empty());
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithEyeTrackingInitializesAdaptiveTest) {
    initializeTest(
        interpreter, Method::adaptiveCoordinateResponseMeasure, "eye tracking");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        adaptiveMethod.targetListReader, &targetsWithReplacementReader);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargetsInitializesFixedLevelTest) {
    initializeTest(interpreter,
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &silentIntervalTargets);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(runningATest.observer.empty());
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementInitializesFixedLevelTest) {
    initializeTest(interpreter,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &targetsWithReplacement);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(runningATest.observer.empty());
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTrackingInitializesFixedLevelTest) {
    initializeTest(interpreter,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement,
        "eye tracking");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &targetsWithReplacement);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsAndEyeTrackingInitializesFixedLevelTest) {
    initializeTest(interpreter, Method::fixedLevelFreeResponseWithAllTargets,
        "eye tracking");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &everyTargetOnce);
}

TEST_SETTINGS_INTERPRETER_TEST(usesMaskerForCalibration) {
    auto calibration{interpreter.calibration(
        concatenate({entryWithNewline(TestSetting::masker, "a"),
            entryWithNewline(TestSetting::maskerLevel, "1")}))};
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, calibration.fileUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, calibration.level.dB_SPL);
}

TEST_SETTINGS_INTERPRETER_TEST(throwsRuntimeErrorIfMethodUnknown) {
    try {
        initializeTest(interpreter,
            {
                entryWithNewline(
                    TestSetting::method, "this is not a real test method"),
            });
        FAIL() << "Expected std::runtime_error";
    } catch (const std::runtime_error &e) {
        AV_SPEECH_IN_NOISE_ASSERT_EQUAL(
            std::string{
                "Test method not recognized: this is not a real test method"},
            e.what());
    }
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "boo");
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            "f:\n", "boo: a\n"});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, configurable.value);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine2) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "boo");
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail), "\n",
            "boo: a\n"});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, configurable.value);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine3) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "boo");
    initializeTest(interpreter,
        {"\n", entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            "\n", "boo: a\n"});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, configurable.value);
}

TEST_SETTINGS_INTERPRETER_TEST(meta) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("a", interpreter.meta("meta: a\n"));
}

TEST_SETTINGS_INTERPRETER_TEST(
    doesNotPrepareTestAfterConfirmButtonIsClickedWhenTestWouldAlreadyBeComplete) {
    runningATest.testComplete_ = true;
    initializeTest(interpreter, Method::adaptivePassFail);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(sessionController.prepareCalled());
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelConsonantsOverridesStartingSnr) {
    assertOverridesStartingSnr(interpreter, Method::fixedLevelConsonants);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementOverridesStartingSnr) {
    assertOverridesStartingSnr(interpreter,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
}

TEST_SETTINGS_INTERPRETER_TEST(tbd) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "hello");
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::fixedLevelConsonants),
            "hello: 1 2 3"});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("hello", configurable.key);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("1 2 3", configurable.value);
}

TEST_SETTINGS_INTERPRETER_TEST(
    initializingTestBroadcastsDefaultRelativeOutputDirectory) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "relative output path");
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::fixedLevelConsonants)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("relative output path", configurable.key);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "Documents/AvSpeechInNoise Data", configurable.value);
}
}
}
