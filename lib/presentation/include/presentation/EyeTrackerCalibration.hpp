#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EYETRACKERCALIBRATION_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EYETRACKERCALIBRATION_HPP_

#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
struct Point {
    float x;
    float y;
};

namespace eye_tracker_calibration {
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
};

class Presenter : public View::Observer {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatPointIsReady() = 0;
    };

    enum class DotState { idle, moving, shrinking, shrunk, growing };

    explicit Presenter(View &view) : view{view} { view.attach(this); }

    void attach(Observer *a) { observer = a; }

    void present(Point x);
    void notifyThatAnimationHasFinished() override;

  private:
    Point pointPresenting{};
    View &view;
    Observer *observer{};
    DotState dotState{DotState::idle};
};

static void moveDotTo(View &view, Point x, Presenter::DotState &dotState) {
    view.moveDotTo(x);
    dotState = Presenter::DotState::moving;
}

void Presenter::present(Point x) {
    pointPresenting = x;
    if (dotState == DotState::shrunk) {
        view.growDot();
        dotState = DotState::growing;
    } else
        moveDotTo(view, pointPresenting, dotState);
}

void Presenter::notifyThatAnimationHasFinished() {
    if (dotState == DotState::growing)
        moveDotTo(view, pointPresenting, dotState);
    else if (dotState == DotState::shrinking) {
        if (observer != nullptr)
            observer->notifyThatPointIsReady();
        dotState = DotState::shrunk;
    } else {
        view.shrinkDot();
        dotState = DotState::shrinking;
    }
}
}
}

#endif
