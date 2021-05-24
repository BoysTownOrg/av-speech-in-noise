#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EYETRACKERCALIBRATION_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EYETRACKERCALIBRATION_HPP_

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/core/EyeTrackerCalibration.hpp>
#include <algorithm>
#include <cmath>

namespace av_speech_in_noise::eye_tracking::calibration {
struct WindowPoint {
    double x;
    double y;
};

struct Line {
    WindowPoint a;
    WindowPoint b;
};

class View {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatAnimationHasFinished() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(View);
    virtual void attach(Observer *) = 0;
    virtual void moveDotTo(WindowPoint) = 0;
    virtual void shrinkDot() = 0;
    virtual void growDot() = 0;
    virtual void drawRed(Line) = 0;
    virtual void drawGreen(Line) = 0;
    virtual void drawWhiteCircleWithCenter(WindowPoint) = 0;
    virtual void clear() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
};

class Presenter : public View::Observer, public IPresenter {
  public:
    enum class DotState { idle, moving, shrinking, shrunk, growing };

    explicit Presenter(View &view) : view{view} { view.attach(this); }
    void attach(IPresenter::Observer *a) override { observer = a; }
    void present(Point x) override;
    void present(const std::vector<Result> &) override;
    void notifyThatAnimationHasFinished() override;
    void stop() override;
    void start() override;

  private:
    Point pointPresenting{};
    View &view;
    IPresenter::Observer *observer{};
    DotState dotState{DotState::idle};
};

class Control {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatWindowHasBeenTouched(WindowPoint) = 0;
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
        virtual void notifyThatMenuHasBeenSelected() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Control);
    virtual void attach(Observer *) = 0;
    virtual auto whiteCircleCenters() -> std::vector<WindowPoint> = 0;
    virtual auto whiteCircleDiameter() -> double = 0;
};

class Controller : public Control::Observer {
  public:
    Controller(Control &, IInteractor &);
    void notifyThatWindowHasBeenTouched(WindowPoint) override;
    void notifyThatSubmitButtonHasBeenClicked() override;
    void notifyThatMenuHasBeenSelected() override;

  private:
    IInteractor &interactor;
    Control &control;
};
}

#endif
