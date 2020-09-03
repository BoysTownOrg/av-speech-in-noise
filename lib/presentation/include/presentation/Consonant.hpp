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
    explicit ConsonantPresenter(Model &model, ConsonantOutputView &view)
        : model{model}, view{view} {}
    void start() override {
        view.show();
        view.showReadyButton();
    }
    void stop() override {
        view.hideResponseButtons();
        view.hide();
    }
    void notifyThatTaskHasStarted() override {
        view.hideReadyButton();
        view.hideCursor();
    }
    void notifyThatUserIsDoneResponding() override {
        view.hideResponseButtons();
        if (!model.testComplete())
            view.hideCursor();
    }
    void showResponseSubmission() override { view.showResponseButtons(); }

  private:
    Model &model;
    ConsonantOutputView &view;
};

class ConsonantResponder : public TaskResponder,
                           public ConsonantInputView::EventListener {
  public:
    explicit ConsonantResponder(Model &model, ConsonantInputView &view)
        : model{model}, view{view} {
        view.subscribe(this);
    }
    void subscribe(TaskResponder::EventListener *e) override { listener = e; }
    void notifyThatReadyButtonHasBeenClicked() override {
        parent->playTrial();
        listener->notifyThatTaskHasStarted();
    }
    void notifyThatResponseButtonHasBeenClicked() override {
        model.submit(ConsonantResponse{view.consonant().front()});
        parent->playNextTrialIfNeeded();
        listener->notifyThatUserIsDoneResponding();
    }
    void becomeChild(ParentPresenter *p) override { parent = p; }

  private:
    Model &model;
    ConsonantInputView &view;
    TaskResponder::EventListener *listener{};
    ParentPresenter *parent{};
};
}

#endif
