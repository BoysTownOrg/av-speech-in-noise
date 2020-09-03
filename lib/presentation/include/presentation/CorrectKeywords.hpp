#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CORRECTKEYWORDS_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CORRECTKEYWORDS_HPP_

#include "Task.hpp"
#include "Experimenter.hpp"
#include "View.hpp"
#include "Input.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>
#include <sstream>

namespace av_speech_in_noise {
class CorrectKeywordsInputView {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
    };
    virtual ~CorrectKeywordsInputView() = default;
    virtual void subscribe(EventListener *) = 0;
    virtual auto correctKeywords() -> std::string = 0;
};

class CorrectKeywordsOutputView {
  public:
    virtual ~CorrectKeywordsOutputView() = default;
    virtual void showCorrectKeywordsSubmission() = 0;
    virtual void hideCorrectKeywordsSubmission() = 0;
};

class CorrectKeywordsResponder
    : public TaskResponder,
      public CorrectKeywordsInputView::EventListener {
  public:
    explicit CorrectKeywordsResponder(
        Model &model, View &mainView, CorrectKeywordsInputView &view)
        : model{model}, mainView{mainView}, view{view} {
        view.subscribe(this);
    }
    void subscribe(TaskResponder::EventListener *e) override { listener = e; }
    void notifyThatSubmitButtonHasBeenClicked() override {
        try {
            CorrectKeywords p{};
            p.count = readInteger(view.correctKeywords(), "number");
            model.submit(p);
            listener->notifyThatUserIsDoneResponding();
            parent->showContinueTestingDialogWithResultsWhenComplete();
        } catch (const std::runtime_error &e) {
            mainView.showErrorMessage(e.what());
        }
    }
    void becomeChild(ParentPresenter *p) override { parent = p; }

  private:
    Model &model;
    View &mainView;
    CorrectKeywordsInputView &view;
    TaskResponder::EventListener *listener{};
    ParentPresenter *parent{};
};

class CorrectKeywordsPresenter : public TaskPresenter {
  public:
    explicit CorrectKeywordsPresenter(
        ExperimenterView &experimenterView, CorrectKeywordsOutputView &view)
        : experimenterView{experimenterView}, view{view} {}
    void start() override {
        experimenterView.show();
        experimenterView.showNextTrialButton();
    }
    void stop() override {
        experimenterView.hide();
        view.hideCorrectKeywordsSubmission();
    }
    void notifyThatTaskHasStarted() override {
        experimenterView.hideNextTrialButton();
    }
    void notifyThatUserIsDoneResponding() override {
        view.hideCorrectKeywordsSubmission();
    }
    void showResponseSubmission() override {
        view.showCorrectKeywordsSubmission();
    }

  private:
    ExperimenterView &experimenterView;
    CorrectKeywordsOutputView &view;
};
}

#endif
