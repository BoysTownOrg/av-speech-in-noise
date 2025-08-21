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

void apply(
    TestSettingsInterpreterImpl &interpreter, const std::string &contents) {
    interpreter.apply(contents, {});
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

class TestSettingsInterpreterTests : public ::testing::Test {
  protected:
    RunningATestStub runningATest;
    SessionControllerStub sessionController;
    TestSettingsInterpreterImpl interpreter;
    TestIdentity testIdentity;
};

#define TEST_SETTINGS_INTERPRETER_TEST(a)                                      \
    TEST_F(TestSettingsInterpreterTests, a)

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "boo");
    apply(interpreter, R"(method: adaptive pass fail
f:
boo: a
)");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, configurable.value);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine2) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "boo");
    apply(interpreter, R"(method: adaptive pass fail

boo: a
)");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, configurable.value);
}

TEST_SETTINGS_INTERPRETER_TEST(ignoresBadLine3) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "boo");
    apply(interpreter, R"(
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
    apply(interpreter, {});
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(sessionController.prepareCalled());
}

TEST_SETTINGS_INTERPRETER_TEST(tbd) {
    ConfigurableStub configurable;
    interpreter.subscribe(configurable, "hello");
    apply(interpreter, "hello: 1 2 3\n");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("hello", configurable.key);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("1 2 3", configurable.value);
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
