#include "EyeTrackerCalibration.hpp"

namespace av_speech_in_noise::eye_tracker_calibration {
static auto windowPoint(Point p) -> WindowPoint { return {p.x, 1 - p.y}; }

static void moveDotTo(View &view, Point p, Presenter::DotState &dotState) {
    view.moveDotTo(windowPoint(p));
    dotState = Presenter::DotState::moving;
}

void Presenter::start() { view.show(); }

void Presenter::stop() { view.hide(); }

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
    view.clear();
    for (const auto &result : results) {
        view.drawWhiteCircleWithCenter(windowPoint(result.point));
        for (const auto point : result.leftEyeMappedPoints)
            view.drawRed(Line{windowPoint(result.point), windowPoint(point)});
        for (const auto point : result.rightEyeMappedPoints)
            view.drawGreen(Line{windowPoint(result.point), windowPoint(point)});
    }
}

Controller::Controller(Control &control, IInteractor &interactor)
    : interactor{interactor}, control{control} {
    control.attach(this);
}

void Controller::notifyThatWindowHasBeenTouched(WindowPoint point) {
    const auto whiteCircleCenters{control.whiteCircleCenters()};
    const auto whiteCircleDiameter{control.whiteCircleDiameter()};
    auto found{find_if(whiteCircleCenters.begin(), whiteCircleCenters.end(),
        [whiteCircleDiameter, point](WindowPoint candidate) {
            return std::hypot(point.x - candidate.x, point.y - candidate.y) <=
                whiteCircleDiameter / 2;
        })};
    if (found != whiteCircleCenters.end())
        interactor.redo(Point{
            static_cast<float>(found->x), 1 - static_cast<float>(found->y)});
}

void Controller::notifyThatSubmitButtonHasBeenClicked() { interactor.finish(); }

void Controller::notifyThatMenuHasBeenSelected() { interactor.start(); }
}
