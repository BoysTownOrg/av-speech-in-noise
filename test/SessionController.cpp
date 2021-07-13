#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "SessionViewStub.hpp"

#include <av-speech-in-noise/ui/SessionController.hpp>

#include <gtest/gtest.h>

#include <utility>

namespace av_speech_in_noise {
namespace {
class TestPresenterStub : public TestPresenter {
  public:
    void start() override { started_ = true; }
    void stop() override { stopped_ = true; }
    void notifyThatTrialHasStarted() override {}
    void updateAdaptiveTestResults() override {}
    void updateTrialInformation() override {}
    void notifyThatNextTrialIsReady() override {}
    [[nodiscard]] auto started() const -> bool { return started_; }
    [[nodiscard]] auto stopped() const -> bool { return stopped_; }
    auto taskPresenter() -> TaskPresenter * { return taskPresenter_; }
    void initialize(TaskPresenter &p) override { taskPresenter_ = &p; }
    void hideResponseSubmission() override {}
    void completeTask() override {}

  private:
    TaskPresenter *taskPresenter_{};
    bool started_{};
    bool stopped_{};
};

class TestSetupPresenterStub : public TestSetupPresenter {
  public:
    void updateErrorMessage(std::string_view) override {}
    void start() override { started_ = true; }
    void stop() override { stopped_ = true; }
    [[nodiscard]] auto started() const -> bool { return started_; }
    [[nodiscard]] auto stopped() const -> bool { return stopped_; }

  private:
    bool started_{};
    bool stopped_{};
};

class TaskPresenterStub : public TaskPresenter {
  public:
    void showResponseSubmission() override {}
    void hideResponseSubmission() override {}
    void start() override {}
    void stop() override {}
};

class SessionControllerObserverStub : public SessionController::Observer {
  public:
    [[nodiscard]] auto notifiedThatTestIsComplete() const -> bool {
        return notifiedThatTestIsComplete_;
    }

    void notifyThatTestIsComplete() override {
        notifiedThatTestIsComplete_ = true;
    }

  private:
    bool notifiedThatTestIsComplete_{};
};

class SessionControllerTests : public ::testing::Test {
  protected:
    TestSetupPresenterStub testSetupPresenter;
    TestPresenterStub testPresenter;
    SessionControllerImpl controller{testSetupPresenter, testPresenter};
    TaskPresenterStub taskPresenter;
};

#define SESSION_CONTROLLER_TEST(a) TEST_F(SessionControllerTests, a)

SESSION_CONTROLLER_TEST(prepareStopsTestSetup) {
    TaskPresenterStub taskPresenter;
    controller.prepare(taskPresenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testSetupPresenter.stopped());
}

SESSION_CONTROLLER_TEST(prepareStartsTest) {
    TaskPresenterStub taskPresenter;
    controller.prepare(taskPresenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testPresenter.started());
}

SESSION_CONTROLLER_TEST(preparePassesTaskPresenter) {
    TaskPresenterStub taskPresenter;
    controller.prepare(taskPresenter);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &taskPresenter, testPresenter.taskPresenter());
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

class SessionPresenterTests : public ::testing::Test {};

#define SESSION_PRESENTER_TEST(a) TEST_F(SessionPresenterTests, a)

SESSION_PRESENTER_TEST(constructorPopulatesAudioDeviceMenu) {
    ModelStub model;
    SessionViewStub view;
    model.setAudioDevices({"a", "b", "c"});
    SessionPresenterImpl presenter{view, model};
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, view.audioDevices().at(0));
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, view.audioDevices().at(1));
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c"}, view.audioDevices().at(2));
}

SESSION_PRESENTER_TEST(constructorPopulatesSubjectScreenMenu) {
    ModelStub model;
    SessionViewStub view;
    view.setScreens({{"a"}, {"b"}, {"c"}});
    SessionPresenterImpl presenter{view, model};
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, view.subjectScreens().at(0).name);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, view.subjectScreens().at(1).name);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c"}, view.subjectScreens().at(2).name);
}
}
}
