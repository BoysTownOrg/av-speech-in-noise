#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_COORDINATERESPONSEMEASURE_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_COORDINATERESPONSEMEASURE_HPP_

#include "View.hpp"
#include "Task.hpp"
#include "Test.hpp"

#include <av-speech-in-noise/core/IRecognitionTestModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

#include <string>

namespace av_speech_in_noise {
class CoordinateResponseMeasureControl {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatReadyButtonHasBeenClicked() = 0;
        virtual void notifyThatResponseButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        CoordinateResponseMeasureControl);
    virtual void attach(Observer *) = 0;
    virtual auto numberResponse() -> std::string = 0;
    virtual auto greenResponse() -> bool = 0;
    virtual auto blueResponse() -> bool = 0;
    virtual auto whiteResponse() -> bool = 0;
};

class CoordinateResponseMeasureView : public virtual View {
  public:
    virtual void showResponseButtons() = 0;
    virtual void hideResponseButtons() = 0;
    virtual void showNextTrialButton() = 0;
    virtual void hideNextTrialButton() = 0;
};

class CoordinateResponseMeasureController
    : public TaskController,
      public CoordinateResponseMeasureControl::Observer {
  public:
    CoordinateResponseMeasureController(
        TestController &, RunningATest &, CoordinateResponseMeasureControl &);
    void attach(TaskController::Observer *);
    void notifyThatReadyButtonHasBeenClicked() override;
    void notifyThatResponseButtonHasBeenClicked() override;

  private:
    TestController &testController;
    RunningATest &runningATest;
    CoordinateResponseMeasureControl &control;
    TaskController::Observer *observer{};
};

class CoordinateResponseMeasurePresenter : public TaskController::Observer,
                                           public TaskPresenter {
  public:
    explicit CoordinateResponseMeasurePresenter(
        CoordinateResponseMeasureView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void hideResponseSubmission() override;
    void showResponseSubmission() override;

  private:
    CoordinateResponseMeasureView &view;
};
}

#endif
