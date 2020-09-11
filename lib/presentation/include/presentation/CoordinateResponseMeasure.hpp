#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_COORDINATERESPONSEMEASURE_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_COORDINATERESPONSEMEASURE_HPP_

#include "Task.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class CoordinateResponseMeasureInputView {
  public:
    class Observer {
      public:
        virtual ~Observer() = default;
        virtual void notifyThatReadyButtonHasBeenClicked() = 0;
        virtual void notifyThatResponseButtonHasBeenClicked() = 0;
    };
    virtual ~CoordinateResponseMeasureInputView() = default;
    virtual void attach(Observer *) = 0;
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

class CoordinateResponseMeasureController
    : public TaskController,
      public CoordinateResponseMeasureInputView::Observer {
  public:
    explicit CoordinateResponseMeasureController(
        Model &, CoordinateResponseMeasureInputView &);
    void attach(TaskController::Observer *e) override;
    void notifyThatReadyButtonHasBeenClicked() override;
    void notifyThatResponseButtonHasBeenClicked() override;
    void attach(ExperimenterController *e) override;

  private:
    Model &model;
    CoordinateResponseMeasureInputView &view;
    TaskController::Observer *listener{};
    ExperimenterController *responder{};
};

class CoordinateResponseMeasurePresenter : public TaskPresenter {
  public:
    explicit CoordinateResponseMeasurePresenter(
        CoordinateResponseMeasureOutputView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void showResponseSubmission() override;

  private:
    CoordinateResponseMeasureOutputView &view;
};
}

#endif
