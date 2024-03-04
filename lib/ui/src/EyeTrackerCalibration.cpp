#include "EyeTrackerCalibration.hpp"

#include <sstream>
#include <algorithm>
#include <cmath>

namespace av_speech_in_noise::eye_tracker_calibration {
static auto windowPoint(Point p) -> WindowPoint { return {p.x, 1 - p.y}; }

TesterPresenterImpl::TesterPresenterImpl(TesterView &view) : view{view} {}

void TesterPresenterImpl::start() { view.show(); }

void TesterPresenterImpl::stop() { view.hide(); }

SubjectPresenterImpl::SubjectPresenterImpl(SubjectView &view,
    av_speech_in_noise::SubjectPresenter &parentPresenter, Timer &timer)
    : view{view}, parentPresenter{parentPresenter}, timer{timer} {
    timer.attach(this);
}

void SubjectPresenterImpl::attach(SubjectPresenter::Observer *a) {
    observer = a;
}

void SubjectPresenterImpl::start() {
    parentPresenter.start();
    view.show();
}

void SubjectPresenterImpl::stop() {
    view.hide();
    parentPresenter.stop();
}

void SubjectPresenterImpl::present(Point p) {
    view.moveDotTo(windowPoint(p));
    timer.scheduleCallbackAfterSeconds(1);
}

void SubjectPresenterImpl::callback() { observer->notifyThatPointIsReady(); }

void TesterPresenterImpl::present(const std::vector<Result> &results) {
    view.clear();
    for_each(results.begin(), results.end(), [&](const Result &result) {
        view.drawWhiteCircleWithCenter(windowPoint(result.point));
        for_each(result.samples.begin(), result.samples.end(),
            [&](const BinocularSample &sample) {
                view.drawRed(Line{
                    windowPoint(result.point), windowPoint(sample.left.point)});
                view.drawGreen(Line{windowPoint(result.point),
                    windowPoint(sample.right.point)});
            });
    });
}

Controller::Controller(Control &control, Interactor &interactor)
    : interactor{interactor}, control{control} {
    control.attach(this);
}

void Controller::notifyThatWindowHasBeenTouched(WindowPoint point) {
    const auto whiteCircleCenters{control.whiteCircleCenters()};
    const auto whiteCircleDiameter{control.whiteCircleDiameter()};
    if (const auto found{find_if(whiteCircleCenters.begin(),
            whiteCircleCenters.end(),
            [whiteCircleDiameter, point](WindowPoint candidate) {
                return std::hypot(point.x - candidate.x,
                           point.y - candidate.y) <= whiteCircleDiameter / 2;
            })};
        found != whiteCircleCenters.end())
        interactor.redo(Point{
            static_cast<float>(found->x), 1 - static_cast<float>(found->y)});
}

void Controller::notifyThatSubmitButtonHasBeenClicked() { interactor.finish(); }

void Controller::notifyThatMenuHasBeenSelected() { interactor.start(); }

namespace validation {
TesterPresenterImpl::TesterPresenterImpl(TesterView &view) : view{view} {}

void TesterPresenterImpl::start() { view.show(); }

void TesterPresenterImpl::stop() { view.hide(); }

static auto format(float x) -> std::string {
    std::stringstream stream;
    stream << x;
    return stream.str();
}

void TesterPresenterImpl::present(const Result &result) {
    view.setLeftEyeAccuracyDegrees(format(result.left.errorOfMeanGaze.degrees));
    view.setLeftEyePrecisionDegrees(
        format(result.left.standardDeviationFromTheMeanGaze.degrees));
    view.setRightEyeAccuracyDegrees(
        format(result.right.errorOfMeanGaze.degrees));
    view.setRightEyePrecisionDegrees(
        format(result.right.standardDeviationFromTheMeanGaze.degrees));
}

Controller::Controller(Control &control, Interactor &interactor)
    : interactor{interactor} {
    control.attach(this);
}

void Controller::notifyThatMenuHasBeenSelected() { interactor.start(); }

void Controller::notifyThatCloseButtonHasBeenClicked() { interactor.finish(); }
}
}
