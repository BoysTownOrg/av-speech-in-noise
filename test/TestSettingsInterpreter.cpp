#include "RunningATestStub.hpp"
#include "assert-utility.hpp"

#include <av-speech-in-noise/ui/TestSettingsInterpreter.hpp>
#include <av-speech-in-noise/ui/SessionController.hpp>

#include <gtest/gtest.h>

#include <string>

namespace av_speech_in_noise {
namespace {
class SessionControllerStub : public SessionController {
  public:
    void notifyThatTestIsComplete() override {}

    void prepare() override { prepareCalled_ = true; }

    [[nodiscard]] auto prepareCalled() const -> bool { return prepareCalled_; }

  private:
    bool prepareCalled_{};
};

void initializeTest(TestSettingsInterpreterImpl &interpreter,
    const std::string &contents, int startingSnr = {},
    const TestIdentity &identity = {}) {
    interpreter.initializeTest(contents, identity, std::to_string(startingSnr));
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

void assertOverridesStartingSnr(TestSettingsInterpreterImpl &interpreter) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "starting SNR (dB)");
    initializeTest(interpreter, {"starting SNR (dB): 6\n"}, 5);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("6", configurable.value);
}

class TestSettingsInterpreterTests : public ::testing::Test {
  protected:
    RunningATestStub runningATest;
    SessionControllerStub sessionController;
    TestSettingsInterpreterImpl interpreter{runningATest, sessionController};
    TestIdentity testIdentity;
};

#define TEST_SETTINGS_INTERPRETER_TEST(a)                                      \
    TEST_F(TestSettingsInterpreterTests, a)

TEST_SETTINGS_INTERPRETER_TEST(usesMaskerForCalibration) {
    auto calibration{interpreter.calibration(
        R"(masker: a
masker level (dB SPL): 1
)")};
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("a", calibration.fileUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, calibration.level.dB_SPL);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "boo");
    initializeTest(interpreter, R"(method: adaptive pass fail
f:
boo: a
)");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, configurable.value);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine2) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "boo");
    initializeTest(interpreter, R"(method: adaptive pass fail

boo: a
)");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, configurable.value);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine3) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "boo");
    initializeTest(interpreter, R"(
method: adaptive pass fail
boo: a
)");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, configurable.value);
}

TEST_SETTINGS_INTERPRETER_TEST(meta) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("a", interpreter.meta("meta: a\n"));
}

TEST_SETTINGS_INTERPRETER_TEST(
    doesNotPrepareTestAfterConfirmButtonIsClickedWhenTestWouldAlreadyBeComplete) {
    runningATest.testComplete_ = true;
    initializeTest(interpreter, {});
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(sessionController.prepareCalled());
}

TEST_SETTINGS_INTERPRETER_TEST(overridesStartingSnr) {
    assertOverridesStartingSnr(interpreter);
}

TEST_SETTINGS_INTERPRETER_TEST(tbd) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "hello");
    initializeTest(interpreter, "hello: 1 2 3\n");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("hello", configurable.key);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("1 2 3", configurable.value);
}

TEST_SETTINGS_INTERPRETER_TEST(
    initializingTestBroadcastsDefaultRelativeOutputDirectory) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "relative output path");
    initializeTest(interpreter, {});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("relative output path", configurable.key);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "Documents/AvSpeechInNoise Data", configurable.value);
}

TEST_SETTINGS_INTERPRETER_TEST(broadcastFiltered) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "hello");
    interpreter.subscribe(configurable, "howdy");
    interpreter.apply(R"(hello: 1 2 3
howdy: 4
)",
        {"hello"});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("hello", configurable.key);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("1 2 3", configurable.value);
}
}
}
