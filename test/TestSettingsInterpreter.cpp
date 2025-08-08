#include "AdaptiveMethodStub.hpp"
#include "FixedLevelMethodStub.hpp"
#include "RunningATestStub.hpp"
#include "TargetPlaylistSetReaderStub.hpp"
#include "TargetPlaylistStub.hpp"
#include "TrackStub.hpp"
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

void assertPassesSettingsWithExtraneousWhitespace(
    TestSettingsInterpreterImpl &interpreter, Method m,
    const Test &fixedLevelTest) {
    initializeTest(interpreter,
        {withNewLine(std::string{"  "} + name(TestSetting::method) +
             std::string{" :  "} + name(m) + "  "),
            "targets :a \n", entryWithNewline(TestSetting::masker, "b"),
            entryWithNewline(TestSetting::maskerLevel, "65")},
        5);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, fixedLevelTest.maskerFileUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(65, fixedLevelTest.maskerLevel.dB_SPL);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        SessionControllerImpl::fullScaleLevel.dB_SPL,
        fixedLevelTest.fullScaleLevel.dB_SPL);
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
    RunningATest::TestObserver eyeTracking;
    RunningATest::TestObserver audioRecording;
    TargetPlaylistSetReaderStub cyclicTargetsReader;
    TargetPlaylistSetReaderStub targetsWithReplacementReader;
    FiniteTargetPlaylistWithRepeatablesStub predeterminedTargets;
    FiniteTargetPlaylistWithRepeatablesStub everyTargetOnce;
    FiniteTargetPlaylistWithRepeatablesStub silentIntervalTargets;
    FiniteTargetPlaylistStub eachTargetNTimes;
    TargetPlaylistStub targetsWithReplacement;
    SessionControllerStub sessionController;
    TrackFactoryStub adaptiveTrackFactory;
    RunningATest::TestObserver submittingConsonantResponse;
    TaskPresenterStub passFailPresenter;
    TaskPresenterStub keypressPresenter;
    RunningATest::TestObserver submittingKeyPressResponse;
    TaskPresenterStub emotionPresenter;
    TaskPresenterStub childEmotionPresenter;
    TaskPresenterStub fixedPassFailPresenter;
    TestSettingsInterpreterImpl interpreter{runningATest, adaptiveMethod,
        fixedLevelMethod, eyeTracking, audioRecording, cyclicTargetsReader,
        targetsWithReplacementReader, predeterminedTargets, everyTargetOnce,
        silentIntervalTargets, eachTargetNTimes, targetsWithReplacement,
        adaptiveTrackFactory, sessionController, submittingConsonantResponse,
        passFailPresenter, keypressPresenter, submittingKeyPressResponse,
        emotionPresenter, childEmotionPresenter, fixedPassFailPresenter};
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
    initializeTestWith_adaptivePassFail_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_METHOD(Method::adaptivePassFail);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCorrectKeywords_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_METHOD(
        Method::adaptiveCorrectKeywords);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptiveCoordinateResponseMeasure_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_ADAPTIVE_METHOD(
        Method::adaptiveCoordinateResponseMeasure);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithTargetReplacement_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelFreeResponseWithTargetReplacement);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithSilentIntervalTargets_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelFreeResponseWithSilentIntervalTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithAllTargets_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelFreeResponseWithAllTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelButtonResponseWithPredeterminedTargets_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelButtonResponseWithPredeterminedTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelFreeResponseWithPredeterminedTargets_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelFreeResponseWithPredeterminedTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelCoordinateResponseMeasureWithTargetReplacement_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelConsonants_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelConsonants);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelChooseKeywordsWithAllTargets_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelChooseKeywordsWithAllTargets);
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelSyllablesWithAllTargets_PassesTestMethod) {
    ASSERT_INITIALIZE_TEST_PASSES_FIXED_LEVEL_METHOD(
        Method::fixedLevelSyllablesWithAllTargets);
}

TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_adaptivePassFail_PassesTaskPresenter) {
    initializeTest(interpreter, Method::adaptivePassFail);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &passFailPresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelButtonResponseWithPredeterminedTargets_PassesTaskPresenter) {
    initializeTest(
        interpreter, Method::fixedLevelButtonResponseWithPredeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &keypressPresenter, sessionController.taskPresenter());
}
TEST_SETTINGS_INTERPRETER_TEST(
    initializeTestWith_fixedLevelEmotionsWithPredeterminedTargets_PassesTaskPresenter) {
    initializeTest(
        interpreter, Method::fixedLevelEmotionsWithPredeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &emotionPresenter, sessionController.taskPresenter());
}

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
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer.front().get(), &audioRecording);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsAndAudioRecordingSelectsAudioRecordingPeripheral) {
    initializeTest(interpreter,
        Method::fixedLevelFreeResponseWithPredeterminedTargets,
        "audio recording");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &predeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer.front().get(), &audioRecording);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsAudioRecordingAndEyeTrackingSelectsAudioRecordingAndEyeTracking) {
    initializeTest(interpreter,
        Method::fixedLevelFreeResponseWithPredeterminedTargets,
        "audio recording eye tracking");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &predeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer[0].get(), &eyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer[1].get(), &audioRecording);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelButtonResponseWithPredeterminedTargetsAudioRecordingEyeTrackingAndVibrotactileSelectsAudioRecordingAndEyeTracking) {
    initializeTest(interpreter,
        Method::fixedLevelButtonResponseWithPredeterminedTargets,
        "audio recording eye tracking vibrotactile");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &predeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer[0].get(), &eyeTracking);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer[1].get(), &audioRecording);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer[2].get(), &submittingKeyPressResponse);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelButtonResponseWithPredeterminedTargetsAndVibrotactileEnablesVibrotactile) {
    initializeTest(interpreter,
        Method::fixedLevelButtonResponseWithPredeterminedTargets,
        "vibrotactile");
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        runningATest.test.enableVibrotactileStimulus);
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
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer.front().get(), &eyeTracking);
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
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer.front().get(), &eyeTracking);
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
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer.front().get(), &eyeTracking);
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
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer.front().get(), &eyeTracking);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsAndEyeTrackingInitializesFixedLevelTest) {
    initializeTest(interpreter,
        Method::fixedLevelFreeResponseWithPredeterminedTargets, "eye tracking");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &predeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer.front().get(), &eyeTracking);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithPredeterminedTargetsInitializesFixedLevelTest) {
    initializeTest(
        interpreter, Method::fixedLevelFreeResponseWithPredeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &predeterminedTargets);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(runningATest.observer.empty());
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsAndEyeTrackingInitializesFixedLevelTest) {
    initializeTest(interpreter, Method::fixedLevelFreeResponseWithAllTargets,
        "eye tracking");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &everyTargetOnce);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer.front().get(), &eyeTracking);
}

TEST_SETTINGS_INTERPRETER_TEST(usesMaskerForCalibration) {
    auto calibration{interpreter.calibration(
        concatenate({entryWithNewline(TestSetting::masker, "a"),
            entryWithNewline(TestSetting::maskerLevel, "1")}))};
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, calibration.fileUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, calibration.level.dB_SPL);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        SessionControllerImpl::fullScaleLevel.dB_SPL,
        calibration.fullScaleLevel.dB_SPL);
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

TEST_SETTINGS_INTERPRETER_TEST(badMaskerLevelResolvesToZero) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::adaptivePassFail),
            entryWithNewline(TestSetting::maskerLevel, "a")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(0, runningATest.test.maskerLevel.dB_SPL);
}

TEST_SETTINGS_INTERPRETER_TEST(meta) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("a", interpreter.meta("meta: a\n"));
}

TEST_SETTINGS_INTERPRETER_TEST(preparesTestAfterConfirmButtonIsClicked) {
    runningATest.testComplete_ = false;
    initializeTest(interpreter, Method::adaptivePassFail);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &passFailPresenter, sessionController.taskPresenter());
}

TEST_SETTINGS_INTERPRETER_TEST(
    doesNotPrepareTestAfterConfirmButtonIsClickedWhenTestWouldAlreadyBeComplete) {
    runningATest.testComplete_ = true;
    initializeTest(interpreter, Method::adaptivePassFail);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(sessionController.prepareCalled());
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithDelayedMaskerInitializesAdaptiveTest) {
    initializeTest(
        interpreter, Method::adaptiveCoordinateResponseMeasure, "spatial");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(AudioChannelOption::delayedMasker,
        runningATest.test.audioChannelOption);
}

TEST_SETTINGS_INTERPRETER_TEST(
    adaptiveCoordinateResponseMeasureWithSingleSpeakerInitializesAdaptiveTest) {
    initializeTest(
        interpreter, Method::adaptiveCoordinateResponseMeasure, "not spatial");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(AudioChannelOption::singleSpeaker,
        runningATest.test.audioChannelOption);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelConsonantsInitializesFixedLevelTest) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method, Method::fixedLevelConsonants)});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        fixedLevelMethod.targetList, &eachTargetNTimes);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &runningATest.observer.front().get(), &submittingConsonantResponse);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithTargetReplacementInitializesFixedLevelTest) {
    initializeTest(interpreter,
        {entryWithNewline(TestSetting::method,
             Method::fixedLevelFreeResponseWithTargetReplacement),
            entryWithNewline(TestSetting::maskerLevel, "5")});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(5, runningATest.test.maskerLevel.dB_SPL);
}

TEST_SETTINGS_INTERPRETER_TEST(fixedLevelConsonantsOverridesStartingSnr) {
    assertOverridesStartingSnr(interpreter, Method::fixedLevelConsonants);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelCoordinateResponseMeasureWithTargetReplacementOverridesStartingSnr) {
    assertOverridesStartingSnr(interpreter,
        Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
}

TEST_SETTINGS_INTERPRETER_TEST(
    fixedLevelFreeResponseWithAllTargetsPassesSettingsWithExtraneousWhitespace) {
    assertPassesSettingsWithExtraneousWhitespace(interpreter,
        Method::fixedLevelFreeResponseWithAllTargets, runningATest.test);
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
