#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTIMPL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTIMPL_HPP_

#include "Test.hpp"
#include "Session.hpp"
#include "Task.hpp"

#include <av-speech-in-noise/core/IAdaptiveMethod.hpp>
#include <av-speech-in-noise/core/IRecognitionTestModel.hpp>
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class TestControllerImpl : public TestControl::Observer, public TestController {
  public:
    TestControllerImpl(SessionController &, RunningATestFacade &,
        SessionControl &, TestControl &, TestPresenter &);
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
    SessionController &sessionController;
    RunningATestFacade &model;
    SessionControl &sessionControl;
    TestPresenter &presenter;
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

    void complete() override {
        if (presenter != nullptr)
            presenter->complete();
    }

    void notifyThatTrialHasStarted() override {
        if (presenter != nullptr)
            presenter->notifyThatTrialHasStarted();
    }

  private:
    TaskPresenter *presenter{};
};

class TestPresenterImpl : public RunningATestFacade::Observer,
                          public TestPresenter {
  public:
    TestPresenterImpl(RunningATestFacade &, RunningATest &, AdaptiveMethod &,
        TestView &, UninitializedTaskPresenter *);
    void initialize(TaskPresenter &) override;
    void start() override;
    void stop() override;
    void trialComplete() override;
    void notifyThatTrialHasStarted() override;
    void notifyThatNextTrialIsReady() override;
    void hideResponseSubmission() override;
    void updateTrialInformation() override;
    void updateAdaptiveTestResults() override;
    void completeTask() override;

  private:
    RunningATestFacade &model;
    RunningATest &runningATest;
    AdaptiveMethod &adaptiveMethod;
    TestView &view;
    UninitializedTaskPresenter *taskPresenter;
};
}

#endif
