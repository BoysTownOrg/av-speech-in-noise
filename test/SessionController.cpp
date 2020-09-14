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
    void initialize(Method) {}
    void start() {}
    void stop() {}
    void notifyThatTrialHasStarted() {}
    void setContinueTestingDialogMessage(const std::string &) {}
    void showContinueTestingDialog() {}
    void display(const std::string &) {}
    void secondaryDisplay(const std::string &) {}
    void notifyThatNextTrialIsReady() {}
};

class TestSetupPresenterStub : public TestSetupPresenter {
  public:
    void notifyThatUserHasSelectedTestSettingsFile(const std::string &) {}
    void start() {}
    void stop() { stopped_ = true; }
    auto stopped() -> bool { return stopped_; }

  private:
    bool stopped_{};
};

class SessionControllerTests : public ::testing::Test {
  protected:
    ModelStub model;
    ViewStub view;
    TestSetupPresenterStub testSetupPresenter;
    TestPresenterStub experimenterPresenter;
    SessionControllerImpl controller{model, view, nullptr, &testSetupPresenter,
        nullptr, &experimenterPresenter};
};

#define SESSION_CONTROLLER_TEST(a) TEST_F(SessionControllerTests, a)

SESSION_CONTROLLER_TEST(prepareStopsTestSetup) {
    controller.prepare(Method::unknown);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testSetupPresenter.stopped());
}
}
}
