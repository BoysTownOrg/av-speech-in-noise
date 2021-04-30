#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EYETRACKERCALIBRATION_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EYETRACKERCALIBRATION_HPP_

#include <av-speech-in-noise/Interface.hpp>
#include <recognition-test/EyeTrackerCalibration.hpp>

namespace av_speech_in_noise::eye_tracker_calibration {
struct Line {
    Point a;
    Point b;
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
    virtual void moveDotTo(Point) = 0;
    virtual void shrinkDot() = 0;
    virtual void growDot() = 0;
    virtual void drawRed(Line) = 0;
    virtual void drawGreen(Line) = 0;
    virtual void drawWhiteCircleWithCenter(Point) = 0;
};

class Presenter : public View::Observer, public IPresenter {
  public:
    enum class DotState { idle, moving, shrinking, shrunk, growing };

    explicit Presenter(View &view) : view{view} { view.attach(this); }
    void attach(IPresenter::Observer *a) override { observer = a; }
    void present(Point x) override;
    void present(const std::vector<Result> &) override;
    void notifyThatAnimationHasFinished() override;

  private:
    Point pointPresenting{};
    View &view;
    IPresenter::Observer *observer{};
    DotState dotState{DotState::idle};
};

struct WindowPoint {
    double x;
    double y;
};

class Control {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyObserverThatWindowHasBeenTouched(WindowPoint) = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Control);
    virtual void attach(Observer *) = 0;
};

class Controller : public Control::Observer {
  public:
    explicit Controller(Control &control, IInteractor &interactor)
        : interactor{interactor} {
        control.attach(this);
    }

    void notifyObserverThatWindowHasBeenTouched(WindowPoint p) override {
        interactor.redo(
            Point{1.F - static_cast<float>(p.x), static_cast<float>(p.y)});
    }

  private:
    IInteractor &interactor;
};
}

#endif
