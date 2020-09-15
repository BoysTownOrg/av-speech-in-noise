#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "SessionViewStub.hpp"
#include <presentation/SessionControllerImpl.hpp>
#include <gtest/gtest.h>
#include <utility>

namespace av_speech_in_noise {
namespace {
class TestPresenterStub : public TestPresenter {
  public:
    void initialize(Method m) { method_ = m; }
    auto method() -> Method { return method_; }
    void start() { started_ = true; }
    void stop() { stopped_ = true; }
    void notifyThatTrialHasStarted() {}
    void setContinueTestingDialogMessage(const std::string &) {}
    void showContinueTestingDialog() {}
    void display(const std::string &) {}
    void secondaryDisplay(const std::string &) {}
    void notifyThatNextTrialIsReady() {}
    auto started() -> bool { return started_; }
    auto stopped() -> bool { return stopped_; }

  private:
    Method method_{};
    bool started_{};
    bool stopped_{};
};

class TestSetupPresenterStub : public TestSetupPresenter {
  public:
    void notifyThatUserHasSelectedTestSettingsFile(const std::string &) {}
    void start() { started_ = true; }
    void stop() { stopped_ = true; }
    auto started() -> bool { return started_; }
    auto stopped() -> bool { return stopped_; }

  private:
    bool started_{};
    bool stopped_{};
};

class SessionControllerTests : public ::testing::Test {
  protected:
    ModelStub model;
    SessionViewStub view;
    TestSetupPresenterStub testSetupPresenter;
    TestPresenterStub testPresenter;
    SessionControllerImpl controller{
        model, view, nullptr, &testSetupPresenter, nullptr, &testPresenter};
};

#define SESSION_CONTROLLER_TEST(a) TEST_F(SessionControllerTests, a)

SESSION_CONTROLLER_TEST(prepareStopsTestSetup) {
    controller.prepare(Method::unknown);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testSetupPresenter.stopped());
}

SESSION_CONTROLLER_TEST(prepareStartsTest) {
    controller.prepare(Method::unknown);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testPresenter.started());
}

SESSION_CONTROLLER_TEST(prepareInitializesTest) {
    controller.prepare(Method::adaptivePassFail);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Method::adaptivePassFail, testPresenter.method());
}

SESSION_CONTROLLER_TEST(testStopsAfterTestIsComplete) {
    controller.notifyThatTestIsComplete();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testSetupPresenter.started());
}

SESSION_CONTROLLER_TEST(testSetupStartsAfterTestIsComplete) {
    controller.notifyThatTestIsComplete();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testPresenter.stopped());
}
}
}
