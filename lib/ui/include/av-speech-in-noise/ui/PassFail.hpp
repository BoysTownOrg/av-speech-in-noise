#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PASSFAIL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PASSFAIL_HPP_

#include "Task.hpp"
#include "Test.hpp"
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
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
    PassFailController(TestController &, Model &, PassFailControl &);
    void notifyThatCorrectButtonHasBeenClicked() override;
    void notifyThatIncorrectButtonHasBeenClicked() override;

  private:
    TestController &testController;
    Model &model;
};

class PassFailPresenter : public TaskPresenter {
  public:
    PassFailPresenter(TestView &, PassFailView &);
    void start() override;
    void stop() override;
    void hideResponseSubmission() override;
    void showResponseSubmission() override;

  private:
    TestView &testView;
    PassFailView &view;
};
}

#endif
