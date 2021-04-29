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
        dotState = DotState::shrunk;
        if (observer != nullptr)
            observer->notifyThatPointIsReady();
    } else {
        view.shrinkDot();
        dotState = DotState::shrinking;
    }
}

void Presenter::present(const std::vector<Result> &results) {
    for (const auto &result : results) {
        view.drawWhiteCircleWithCenter(result.point);
        for (const auto point : result.leftEyeMappedPoints)
            view.drawRed({result.point, point});
        for (const auto point : result.rightEyeMappedPoints)
            view.drawGreen({result.point, point});
    }
}
}
