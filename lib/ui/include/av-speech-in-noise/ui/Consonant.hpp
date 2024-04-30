#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CONSONANTTASK_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CONSONANTTASK_HPP_

#include "View.hpp"
#include "Task.hpp"
#include "Test.hpp"

#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise::submitting_consonant {
class Control {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatReadyButtonHasBeenClicked() = 0;
        virtual void notifyThatResponseButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Control);
    virtual void attach(Observer *) = 0;
    virtual auto consonant() -> Consonant = 0;
};

class View : public virtual av_speech_in_noise::View {
  public:
    virtual void showCursor() = 0;
    virtual void hideCursor() = 0;
    virtual void showReadyButton() = 0;
    virtual void hideReadyButton() = 0;
    virtual void hideResponseButtons() = 0;
    virtual void showResponseButtons() = 0;
};

class Presenter {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Presenter);
    virtual void hideReadyButton() = 0;
};

class Controller : public Control::Observer {
  public:
    Controller(TestController &, Interactor &, Control &, Presenter &);
    void notifyThatReadyButtonHasBeenClicked() override;
    void notifyThatResponseButtonHasBeenClicked() override;

  private:
    TestController &testController;
    Interactor &interactor;
    Control &control;
    Presenter &presenter;
};

class PresenterImpl : public Presenter, public TaskPresenter {
  public:
    explicit PresenterImpl(View &);
    void start() override;
    void stop() override;
    void showResponseSubmission() override;
    void hideResponseSubmission() override;
    void hideReadyButton() override;
    void notifyThatTrialHasStarted() override;

  private:
    View &view;
};
}

#endif
