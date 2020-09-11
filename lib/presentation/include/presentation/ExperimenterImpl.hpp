#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTERIMPL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTERIMPL_HPP_

#include "Experimenter.hpp"
#include "View.hpp"
#include "Task.hpp"
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class ExperimenterControllerImpl : public ExperimenterInputView::Observer,
                                   public ExperimenterController {
  public:
    explicit ExperimenterControllerImpl(Model &, View &,
        ExperimenterInputView &, TaskController *consonantController,
        TaskPresenter *consonantPresenter,
        TaskController *coordinateResponseMeasureController,
        TaskPresenter *coordinateResponseMeasurePresenter,
        TaskController *freeResponseController,
        TaskPresenter *freeResponsePresenter,
        TaskController *correctKeywordsController,
        TaskPresenter *correctKeywordsPresenter,
        TaskController *passFailController, TaskPresenter *passFailPresenter);
    void attach(ExperimenterController::Observer *e) override;
    void attach(SessionController *p) override;
    void exitTest() override;
    void playTrial() override;
    void declineContinuingTesting() override;
    void acceptContinuingTesting() override;
    void notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion()
        override;
    void notifyThatUserIsDoneResponding() override;
    void notifyThatUserIsReadyForNextTrial() override;

  private:
    Model &model;
    View &mainView;
    ExperimenterController::Observer *listener{};
    SessionController *responder{};
};

class UninitializedTaskPresenter : public TaskPresenter {
  public:
    virtual void initialize(TaskPresenter *p) = 0;
};

class UninitializedTaskPresenterImpl : public UninitializedTaskPresenter {
  public:
    void initialize(TaskPresenter *p) override { presenter = p; }
    void showResponseSubmission() override {
        presenter->showResponseSubmission();
    }
    void notifyThatTaskHasStarted() override {
        presenter->notifyThatTaskHasStarted();
    }
    void notifyThatUserIsDoneResponding() override {
        presenter->notifyThatUserIsDoneResponding();
    }
    void notifyThatTrialHasStarted() override {
        presenter->notifyThatTrialHasStarted();
    }
    void start() override { presenter->start(); }
    void stop() override { presenter->stop(); }

  private:
    TaskPresenter *presenter;
};

class ExperimenterPresenterImpl : public Model::Observer,
                                  public ExperimenterPresenter {
  public:
    explicit ExperimenterPresenterImpl(Model &, ExperimenterOutputView &,
        TaskPresenter *consonantPresenter,
        TaskPresenter *coordinateResponseMeasurePresenter,
        TaskPresenter *freeResponsePresenter,
        TaskPresenter *correctKeywordsPresenter,
        TaskPresenter *passFailPresenter, UninitializedTaskPresenter *);
    void trialComplete() override;
    void start() override;
    void stop() override;
    void notifyThatTrialHasStarted() override;
    void notifyThatNextTrialIsReady() override;
    void display(const std::string &s) override;
    void secondaryDisplay(const std::string &s) override;
    void showContinueTestingDialog() override;
    void setContinueTestingDialogMessage(const std::string &s) override;
    void initialize(Method) override;

  private:
    auto taskPresenter(Method m) -> TaskPresenter *;

    Model &model;
    ExperimenterOutputView &view;
    TaskPresenter *consonantPresenter;
    TaskPresenter *coordinateResponseMeasurePresenter;
    TaskPresenter *freeResponsePresenter;
    TaskPresenter *correctKeywordsPresenter;
    TaskPresenter *passFailPresenter;
    TaskPresenter *taskPresenter_;
    UninitializedTaskPresenter *taskPresenter__;
};
}

#endif
