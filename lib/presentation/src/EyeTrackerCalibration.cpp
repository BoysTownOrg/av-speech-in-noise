#include "EyeTrackerCalibration.hpp"

namespace av_speech_in_noise::eye_tracker_calibration {
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
