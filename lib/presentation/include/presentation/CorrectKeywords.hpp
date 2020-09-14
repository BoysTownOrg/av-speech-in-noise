#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CORRECTKEYWORDS_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CORRECTKEYWORDS_HPP_

#include "Task.hpp"
#include "Experimenter.hpp"
#include "SessionView.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class CorrectKeywordsControl {
  public:
    class Observer {
      public:
        virtual ~Observer() = default;
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
    };
    virtual ~CorrectKeywordsControl() = default;
    virtual void attach(Observer *) = 0;
    virtual auto correctKeywords() -> std::string = 0;
};

class CorrectKeywordsView {
  public:
    virtual ~CorrectKeywordsView() = default;
    virtual void showCorrectKeywordsSubmission() = 0;
    virtual void hideCorrectKeywordsSubmission() = 0;
};

class CorrectKeywordsController : public TaskController,
                                  public CorrectKeywordsControl::Observer {
  public:
    explicit CorrectKeywordsController(
        Model &, SessionView &, CorrectKeywordsControl &);
    void attach(TaskController::Observer *e) override;
    void attach(ExperimenterController *r) override;
    void notifyThatSubmitButtonHasBeenClicked() override;

  private:
    Model &model;
    SessionView &view;
    CorrectKeywordsControl &keywordsView;
    TaskController::Observer *listener{};
    ExperimenterController *responder{};
};

class CorrectKeywordsPresenter : public TaskPresenter {
  public:
    explicit CorrectKeywordsPresenter(
        ExperimenterView &, CorrectKeywordsView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void showResponseSubmission() override;

  private:
    ExperimenterView &experimenterView;
    CorrectKeywordsView &view;
};
}

#endif
