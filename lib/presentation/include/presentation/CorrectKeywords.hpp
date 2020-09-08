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

class CorrectKeywordsResponder
    : public TaskResponder,
      public CorrectKeywordsInputView::EventListener {
  public:
    explicit CorrectKeywordsResponder(
        Model &, View &, CorrectKeywordsInputView &);
    void subscribe(TaskResponder::EventListener *e) override;
    void subscribe(ExperimenterResponder *r) override;
    void notifyThatSubmitButtonHasBeenClicked() override;

  private:
    Model &model;
    View &view;
    CorrectKeywordsInputView &keywordsView;
    TaskResponder::EventListener *listener{};
    ExperimenterResponder *responder{};
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
