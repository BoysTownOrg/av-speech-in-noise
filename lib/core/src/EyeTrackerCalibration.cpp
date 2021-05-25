#include "EyeTrackerCalibration.hpp"
#include <algorithm>
#include <cmath>

namespace av_speech_in_noise::eye_tracking::calibration {
static auto distance(Point a, Point b) -> float {
    return std::hypot(a.x - b.x, a.y - b.y);
}

static void present(IPresenter &presenter, const std::vector<Point> &points) {
    presenter.present(points.front());
}

Interactor::Interactor(
    IPresenter &presenter, Calibrator &calibrator, std::vector<Point> points)
    : points{std::move(points)}, presenter{presenter}, calibrator{calibrator} {
    presenter.attach(this);
}

void Interactor::notifyThatPointIsReady() {
    calibrator.collect(pointsToCalibrate.front());
    pointsToCalibrate.erase(pointsToCalibrate.begin());
    if (pointsToCalibrate.empty())
        presenter.present(calibrator.results());
    else
        present(presenter, pointsToCalibrate);
}

void Interactor::start() {
    calibrator.acquire();
    pointsToCalibrate = points;
    presenter.start();
    present(presenter, pointsToCalibrate);
}

void Interactor::finish() {
    if (pointsToCalibrate.empty()) {
        calibrator.release();
        presenter.stop();
    }
}

void Interactor::redo(Point p) {
    if (!pointsToCalibrate.empty())
        return;
    const auto closestPoint{min_element(points.begin(), points.end(),
        [p](Point a, Point b) { return distance(p, a) < distance(p, b); })};
    calibrator.discard(*closestPoint);
    pointsToCalibrate.push_back(*closestPoint);
    present(presenter, pointsToCalibrate);
}

namespace validation {
Interactor::Interactor(
    IPresenter &presenter, Validator &validator, std::vector<Point> points)
    : points{std::move(points)}, presenter{presenter}, validator{validator} {
    presenter.attach(this);
}

void Interactor::start() {
    validator.acquire();
    presenter.start();
    pointsToValidate = points;
    present(presenter, pointsToValidate);
}

void Interactor::finish() {
    if (pointsToValidate.empty()) {
        validator.release();
        presenter.stop();
    }
}

void Interactor::notifyThatPointIsReady() {
    validator.collect(pointsToValidate.front());
    pointsToValidate.erase(pointsToValidate.begin());
    if (pointsToValidate.empty())
        presenter.present(validator.result());
    else
        present(presenter, pointsToValidate);
}
}
}
