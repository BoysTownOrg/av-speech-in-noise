#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_KEYPRESS_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_KEYPRESS_HPP_

#include "Task.hpp"
#include "Test.hpp"

#include <av-speech-in-noise/core/IMaskerPlayer.hpp>
#include <av-speech-in-noise/core/Player.hpp>
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise::submitting_keypress {
class Control {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatKeyHasBeenPressed() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Control);
    virtual void attach(Observer *) = 0;
    virtual auto keyPressed() -> std::string = 0;
    virtual auto keyPressedSeconds() -> double = 0;
};

class Presenter : public TaskPresenter, public Control::Observer {
  public:
    Presenter(TestView &testView, TestController &testController,
        Interactor &interactor, Control &control, MaskerPlayer &);
    void notifyThatKeyHasBeenPressed() override;
    void start() override;
    void stop() override;
    void showResponseSubmission() override;
    void hideResponseSubmission() override;
    void notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &);

  private:
    TestView &testView;
    TestController &testController;
    Interactor &interactor;
    Control &control;
    MaskerPlayer &maskerPlayer;
    double targetStartTimeMilliseconds{};
    bool ready{};
};
}

#endif
