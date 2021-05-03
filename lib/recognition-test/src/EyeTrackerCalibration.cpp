#include "EyeTrackerCalibration.hpp"
#include <algorithm>
#include <cmath>

namespace av_speech_in_noise::eye_tracker_calibration {
static auto transferOne(std::vector<Point> &a, std::vector<Point> &b) -> Point {
    const auto p{a.front()};
    a.erase(a.begin());
    b.push_back(p);
    return p;
}

static void present(IPresenter &presenter, const std::vector<Point> &points) {
    if (!points.empty())
        presenter.present(points.front());
}

Interactor::Interactor(IPresenter &presenter, EyeTrackerCalibrator &calibrator,
    std::vector<Point> points)
    : pointsToCalibrate{std::move(points)}, presenter{presenter},
      calibrator{calibrator} {
    presenter.attach(this);
}

void Interactor::notifyThatPointIsReady() {
    calibrator.calibrate(transferOne(pointsToCalibrate, pointsCalibrated));
    present(presenter, pointsToCalibrate);
    if (pointsToCalibrate.empty())
        presenter.present(calibrator.results());
}

void Interactor::calibrate() { present(presenter, pointsToCalibrate); }

void Interactor::redo(Point p) {
    if (pointsCalibrated.empty())
        return;
    const auto closestPoint{min_element(pointsCalibrated.begin(),
        pointsCalibrated.end(), [p](Point a, Point b) {
            return std::hypot(p.x - a.x, p.y - a.y) <
                std::hypot(p.x - b.x, p.y - b.y);
        })};
    calibrator.discard(*closestPoint);
    pointsToCalibrate.push_back(*closestPoint);
    pointsCalibrated.erase(closestPoint);
    present(presenter, pointsToCalibrate);
}
}
