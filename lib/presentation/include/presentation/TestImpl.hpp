#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTIMPL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTIMPL_HPP_

#include "Test.hpp"
#include "SessionView.hpp"
#include "Task.hpp"
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class TestControllerImpl : public TestControl::Observer, public TestController {
  public:
    explicit TestControllerImpl(Model &, SessionView &, TestControl &);
    void attach(TestController::Observer *) override;
    void attach(SessionController *) override;
    void exitTest() override;
    void playTrial() override;
    void declineContinuingTesting() override;
    void acceptContinuingTesting() override;
    void notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion()
        override;
    void notifyThatUserIsDoneResponding() override;
    void notifyThatUserIsReadyForNextTrial() override;
    void notifyThatUserIsDoneRespondingAndIsReadyForNextTrial() override;

  private:
    Model &model;
    SessionView &sessionView;
    TestController::Observer *observer{};
    SessionController *controller{};
};

class UninitializedTaskPresenter : public TaskPresenter {
  public:
    virtual void initialize(TaskPresenter *p) = 0;
};

class UninitializedTaskPresenterImpl : public UninitializedTaskPresenter {
  public:
    void initialize(TaskPresenter *p) override { presenter = p; }

    void showResponseSubmission() override {
        if (presenter != nullptr)
            presenter->showResponseSubmission();
    }

    void hideResponseSubmission() override {
        if (presenter != nullptr)
            presenter->hideResponseSubmission();
    }

    void start() override {
        if (presenter != nullptr)
            presenter->start();
    }

    void stop() override {
        if (presenter != nullptr)
            presenter->stop();
    }

  private:
    TaskPresenter *presenter{};
};

class TestPresenterImpl : public Model::Observer, public TestPresenter {
  public:
    explicit TestPresenterImpl(
        Model &, TestView &, UninitializedTaskPresenter *);
    void trialComplete() override;
    void start() override;
    void stop() override;
    void notifyThatTrialHasStarted() override;
    void notifyThatNextTrialIsReady() override;
    void hideResponseSubmission() override;
    void display(const std::string &s) override;
    void secondaryDisplay(const std::string &s) override;
    void showContinueTestingDialog() override;
    void setContinueTestingDialogMessage(const std::string &s) override;
    void initialize(TaskPresenter &) override;

  private:
    Model &model;
    TestView &view;
    UninitializedTaskPresenter *taskPresenter_;
};
}

#endif
