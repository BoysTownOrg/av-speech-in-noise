#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PASSFAIL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PASSFAIL_HPP_

#include <av-speech-in-noise/Interface.hpp>
#include "Task.hpp"
#include "Test.hpp"
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class PassFailControl {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatCorrectButtonHasBeenClicked() = 0;
        virtual void notifyThatIncorrectButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(PassFailControl);
    virtual void attach(Observer *) = 0;
};

class PassFailView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(PassFailView);
    virtual void showEvaluationButtons() = 0;
    virtual void hideEvaluationButtons() = 0;
};

class PassFailController : public TaskController,
                           public PassFailControl::Observer {
  public:
    PassFailController(Model &, PassFailControl &);
    void attach(TaskController::Observer *) override;
    void attach(TestController *) override;
    void notifyThatCorrectButtonHasBeenClicked() override;
    void notifyThatIncorrectButtonHasBeenClicked() override;

  private:
    Model &model;
    TaskController::Observer *observer{};
    TestController *controller{};
};

class PassFailPresenter : public TaskPresenter {
  public:
    PassFailPresenter(TestView &, PassFailView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void showResponseSubmission() override;

  private:
    TestView &testView;
    PassFailView &view;
};
}

#endif
