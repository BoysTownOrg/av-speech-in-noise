#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include <presentation/SessionControllerImpl.hpp>
#include <gtest/gtest.h>
#include <utility>

namespace av_speech_in_noise {
namespace {
class ViewStub : public SessionView {
  public:
    void setAudioDevice(std::string s) { audioDevice_ = std::move(s); }

    auto audioDevice() -> std::string override { return audioDevice_; }

    void showErrorMessage(std::string s) override {
        errorMessage_ = std::move(s);
    }

    auto errorMessage() { return errorMessage_; }

    void eventLoop() override { eventLoopCalled_ = true; }

    auto browseForDirectory() -> std::string override {
        return browseForDirectoryResult_;
    }

    auto browseCancelled() -> bool override { return browseCancelled_; }

    auto browseForOpeningFile() -> std::string override {
        return browseForOpeningFileResult_;
    }

    void setBrowseForOpeningFileResult(std::string s) {
        browseForOpeningFileResult_ = std::move(s);
    }

    void setBrowseCancelled() { browseCancelled_ = true; }

    void populateAudioDeviceMenu(std::vector<std::string> v) override {
        audioDevices_ = std::move(v);
    }

  private:
    std::vector<std::string> audioDevices_;
    std::string errorMessage_;
    std::string browseForDirectoryResult_;
    std::string browseForOpeningFileResult_;
    std::string audioDevice_;
    bool eventLoopCalled_{};
    bool browseCancelled_{};
};

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
    ViewStub view;
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