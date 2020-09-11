#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CONSONANT_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CONSONANT_HPP_

#include "Task.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class ConsonantInputView {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatReadyButtonHasBeenClicked() = 0;
        virtual void notifyThatResponseButtonHasBeenClicked() = 0;
    };
    virtual ~ConsonantInputView() = default;
    virtual void attach(EventListener *) = 0;
    virtual auto consonant() -> std::string = 0;
};

class ConsonantOutputView {
  public:
    virtual ~ConsonantOutputView() = default;
    virtual void showCursor() = 0;
    virtual void hideCursor() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void showReadyButton() = 0;
    virtual void hideReadyButton() = 0;
    virtual void hideResponseButtons() = 0;
    virtual void showResponseButtons() = 0;
};

class ConsonantPresenter : public TaskPresenter {
  public:
    explicit ConsonantPresenter(ConsonantOutputView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void notifyThatTrialHasStarted() override;
    void showResponseSubmission() override;

  private:
    ConsonantOutputView &view;
};

class ConsonantController : public TaskController,
                           public ConsonantInputView::EventListener {
  public:
    explicit ConsonantController(Model &, ConsonantInputView &);
    void attach(TaskController::EventListener *e) override;
    void attach(ExperimenterController *p) override;
    void notifyThatReadyButtonHasBeenClicked() override;
    void notifyThatResponseButtonHasBeenClicked() override;

  private:
    Model &model;
    ConsonantInputView &view;
    TaskController::EventListener *listener{};
    ExperimenterController *responder{};
};
}

#endif
