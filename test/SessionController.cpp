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
    void initialize(Method m) override { method_ = m; }
    auto method() -> Method { return method_; }
    void start() override { started_ = true; }
    void stop() override { stopped_ = true; }
    void notifyThatTrialHasStarted() override {}
    void setContinueTestingDialogMessage(const std::string &) override {}
    void showContinueTestingDialog() override {}
    void display(const std::string &) override {}
    void secondaryDisplay(const std::string &) override {}
    void notifyThatNextTrialIsReady() override {}
    [[nodiscard]] auto started() const -> bool { return started_; }
    [[nodiscard]] auto stopped() const -> bool { return stopped_; }

  private:
    Method method_{};
    bool started_{};
    bool stopped_{};
};

class TestSetupPresenterStub : public TestSetupPresenter {
  public:
    void notifyThatUserHasSelectedTestSettingsFile(
        const std::string &) override {}
    void start() override { started_ = true; }
    void stop() override { stopped_ = true; }
    [[nodiscard]] auto started() const -> bool { return started_; }
    [[nodiscard]] auto stopped() const -> bool { return stopped_; }

  private:
    bool started_{};
    bool stopped_{};
};

class SessionControllerObserverStub : public SessionController::Observer {
  public:
    [[nodiscard]] auto notifiedThatTestIsComplete() const -> bool {
        return notifiedThatTestIsComplete_;
    }

    void notifyThatTestIsComplete() { notifiedThatTestIsComplete_ = true; }

  private:
    bool notifiedThatTestIsComplete_{};
};

class SessionControllerTests : public ::testing::Test {
  protected:
    ModelStub model;
    SessionViewStub view;
    TestSetupPresenterStub testSetupPresenter;
    TestPresenterStub testPresenter;
    SessionControllerImpl controller{
        model, view, testSetupPresenter, testPresenter};
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

SESSION_CONTROLLER_TEST(forwardsTestIsCompleteNotification) {
    SessionControllerObserverStub observer;
    controller.attach(&observer);
    controller.notifyThatTestIsComplete();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(observer.notifiedThatTestIsComplete());
}

class SessionControllerTBDTests : public ::testing::Test {};

TEST_F(SessionControllerTBDTests, constructorPopulatesAudioDeviceMenu) {
    ModelStub model;
    SessionViewStub view;
    TestSetupPresenterStub testSetupPresenter;
    TestPresenterStub testPresenter;
    model.setAudioDevices({"a", "b", "c"});
    SessionControllerImpl controller{
        model, view, testSetupPresenter, testPresenter};
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, view.audioDevices().at(0));
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, view.audioDevices().at(1));
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c"}, view.audioDevices().at(2));
}
}
}
