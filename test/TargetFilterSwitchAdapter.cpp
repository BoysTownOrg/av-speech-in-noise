#include "assert-utility.hpp"
#include <recognition-test/TargetFilterSwitchAdapter.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
class SignalProcessorSwitchStub : public SignalProcessorSwitch {
  public:
    void initializeProcessor(const LocalUrl &url) override { url_ = url; }

    auto url() -> LocalUrl { return url_; }

    void clearProcessor() override { processorCleared_ = true; }

    [[nodiscard]] auto processorCleared() const -> bool {
        return processorCleared_;
    }

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
