#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CORRECTKEYWORDS_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CORRECTKEYWORDS_HPP_

#include "Task.hpp"
#include "Experimenter.hpp"
#include "View.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>

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

class CorrectKeywordsController
    : public TaskController,
      public CorrectKeywordsInputView::EventListener {
  public:
    explicit CorrectKeywordsController(
        Model &, View &, CorrectKeywordsInputView &);
    void subscribe(TaskController::EventListener *e) override;
    void subscribe(ExperimenterController *r) override;
    void notifyThatSubmitButtonHasBeenClicked() override;

  private:
    Model &model;
    View &view;
    CorrectKeywordsInputView &keywordsView;
    TaskController::EventListener *listener{};
    ExperimenterController *responder{};
};

class CorrectKeywordsPresenter : public TaskPresenter {
  public:
    explicit CorrectKeywordsPresenter(
        ExperimenterOutputView &, CorrectKeywordsOutputView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void showResponseSubmission() override;

  private:
    ExperimenterOutputView &experimenterView;
    CorrectKeywordsOutputView &view;
};
}

#endif
