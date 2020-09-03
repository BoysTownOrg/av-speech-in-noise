#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_COORDINATERESPONSEMEASURE_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_COORDINATERESPONSEMEASURE_HPP_

#include "Task.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class CoordinateResponseMeasureInputView {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatReadyButtonHasBeenClicked() = 0;
        virtual void notifyThatResponseButtonHasBeenClicked() = 0;
    };
    virtual ~CoordinateResponseMeasureInputView() = default;
    virtual void subscribe(EventListener *) = 0;
    virtual auto numberResponse() -> std::string = 0;
    virtual auto greenResponse() -> bool = 0;
    virtual auto blueResponse() -> bool = 0;
    virtual auto whiteResponse() -> bool = 0;
};

class CoordinateResponseMeasureOutputView {
  public:
    virtual ~CoordinateResponseMeasureOutputView() = default;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void showResponseButtons() = 0;
    virtual void hideResponseButtons() = 0;
    virtual void showNextTrialButton() = 0;
    virtual void hideNextTrialButton() = 0;
};

static auto colorResponse(CoordinateResponseMeasureInputView *inputView)
    -> coordinate_response_measure::Color {
    if (inputView->greenResponse())
        return coordinate_response_measure::Color::green;
    if (inputView->blueResponse())
        return coordinate_response_measure::Color::blue;
    if (inputView->whiteResponse())
        return coordinate_response_measure::Color::white;

    return coordinate_response_measure::Color::red;
}

static auto subjectResponse(CoordinateResponseMeasureInputView *inputView)
    -> coordinate_response_measure::Response {
    coordinate_response_measure::Response p{};
    p.color = colorResponse(inputView);
    p.number = std::stoi(inputView->numberResponse());
    return p;
}

class CoordinateResponseMeasureResponder
    : public TaskResponder,
      public CoordinateResponseMeasureInputView::EventListener {
  public:
    explicit CoordinateResponseMeasureResponder(
        Model &model, CoordinateResponseMeasureInputView &view)
        : model{model}, view{view} {
        view.subscribe(this);
    }
    void subscribe(TaskResponder::EventListener *e) override { listener = e; }
    void notifyThatReadyButtonHasBeenClicked() override {
        parent->playTrial();
        listener->notifyThatTaskHasStarted();
    }
    void notifyThatResponseButtonHasBeenClicked() override {
        model.submit(subjectResponse(&view));
        parent->playNextTrialIfNeeded();
        listener->notifyThatUserIsDoneResponding();
    }
    void becomeChild(ParentPresenter *p) override { parent = p; }

  private:
    Model &model;
    CoordinateResponseMeasureInputView &view;
    TaskResponder::EventListener *listener{};
    ParentPresenter *parent{};
};

class CoordinateResponseMeasurePresenter : public TaskPresenter {
  public:
    explicit CoordinateResponseMeasurePresenter(
        CoordinateResponseMeasureOutputView &view)
        : view{view} {}
    void start() override {
        view.show();
        view.showNextTrialButton();
    }
    void stop() override {
        view.hideResponseButtons();
        view.hide();
    }
    void notifyThatTaskHasStarted() override { view.hideNextTrialButton(); }
    void notifyThatUserIsDoneResponding() override {
        view.hideResponseButtons();
    }
    void showResponseSubmission() override { view.showResponseButtons(); }

  private:
    CoordinateResponseMeasureOutputView &view;
};
}

#endif
