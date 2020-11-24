#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CORRECTKEYWORDS_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CORRECTKEYWORDS_HPP_

#include "Task.hpp"
#include "Test.hpp"
#include "SessionView.hpp"
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class CorrectKeywordsControl {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        CorrectKeywordsControl);
    virtual void attach(Observer *) = 0;
    virtual auto correctKeywords() -> std::string = 0;
};

class CorrectKeywordsView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(CorrectKeywordsView);
    virtual void showCorrectKeywordsSubmission() = 0;
    virtual void hideCorrectKeywordsSubmission() = 0;
};

class CorrectKeywordsController : public TaskController,
                                  public CorrectKeywordsControl::Observer {
  public:
    explicit CorrectKeywordsController(
        Model &, SessionView &, CorrectKeywordsControl &);
    void attach(TaskController::Observer *) override;
    void attach(TestController *) override;
    void notifyThatSubmitButtonHasBeenClicked() override;

  private:
    Model &model;
    SessionView &sessionView;
    CorrectKeywordsControl &control;
    TaskController::Observer *observer{};
    TestController *controller{};
};

class CorrectKeywordsPresenter : public TaskPresenter {
  public:
    explicit CorrectKeywordsPresenter(TestView &, CorrectKeywordsView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void showResponseSubmission() override;

  private:
    TestView &testView;
    CorrectKeywordsView &view;
};
}

#endif
