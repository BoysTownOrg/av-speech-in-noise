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
    virtual void subscribe(EventListener *) = 0;
    virtual auto consonant() -> std::string = 0;
};

class ConsonantOutputView {
  public:
    virtual ~ConsonantOutputView() = default;
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
    explicit ConsonantPresenter(Model &, ConsonantOutputView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void showResponseSubmission() override;

  private:
    Model &model;
    ConsonantOutputView &view;
};

class ConsonantResponder : public TaskResponder,
                           public ConsonantInputView::EventListener {
  public:
    explicit ConsonantResponder(Model &, ConsonantInputView &);
    void subscribe(TaskResponder::EventListener *e) override;
    void subscribe(ExperimenterResponder *p) override;
    void notifyThatReadyButtonHasBeenClicked() override;
    void notifyThatResponseButtonHasBeenClicked() override;

  private:
    Model &model;
    ConsonantInputView &view;
    TaskResponder::EventListener *listener{};
    ExperimenterResponder *responder{};
};
}

#endif
