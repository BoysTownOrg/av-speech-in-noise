#include <recognition-test/Model.hpp>

namespace av_speech_in_noise {
class SignalProcessorSwitch {
  public:
    virtual ~SignalProcessorSwitch() = default;
    virtual void initializeProcessor(const LocalUrl &) = 0;
    virtual void clearProcessor() = 0;
};

class TargetFilterSwitchAdapter : public TargetFilterSwitch {
  public:
    explicit TargetFilterSwitchAdapter(SignalProcessorSwitch &processorSwitch)
        : processorSwitch{processorSwitch} {}

    void turnOn(const LocalUrl &url) override {
        processorSwitch.initializeProcessor(url);
    }

    void turnOff() override { processorSwitch.clearProcessor(); }

  private:
    SignalProcessorSwitch &processorSwitch;
};
}

#include "assert-utility.hpp"
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
class SignalProcessorSwitchStub : public SignalProcessorSwitch {
  public:
    void initializeProcessor(const LocalUrl &url) override { url_ = url; }

    auto url() -> LocalUrl { return url_; }

    void clearProcessor() override { processorCleared_ = true; }

    auto processorCleared() -> bool { return processorCleared_; }

  private:
    LocalUrl url_;
    bool processorCleared_{};
};

class TargetFilterSwitchAdapterTests : public ::testing::Test {
  protected:
    SignalProcessorSwitchStub processorSwitch;
    TargetFilterSwitchAdapter adapter{processorSwitch};
};

#define TARGET_FILTER_SWITCH_ADAPTER_TEST(a)                                   \
    TEST_F(TargetFilterSwitchAdapterTests, a)

TARGET_FILTER_SWITCH_ADAPTER_TEST(a) {
    adapter.turnOn({"a"});
    AV_SPEECH_IN_NOISE_EXPECT_STRING_EQUAL("a", processorSwitch.url().path);
}

TARGET_FILTER_SWITCH_ADAPTER_TEST(b) {
    adapter.turnOff();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(processorSwitch.processorCleared());
}
}
}
