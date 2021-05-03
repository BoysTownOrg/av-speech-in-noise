#include "EyeTrackerCalibration.hpp"
#include <algorithm>
#include <cmath>

namespace av_speech_in_noise::eye_tracker_calibration {
static void present(IPresenter &presenter, const std::vector<Point> &points,
    std::vector<Point>::const_iterator it) {
    if (it != points.end())
        presenter.present(*it);
}

Interactor::Interactor(IPresenter &presenter, EyeTrackerCalibrator &calibrator,
    std::vector<Point> points)
    : points{std::move(points)}, presenter{presenter}, calibrator{calibrator} {
    pointIterator = this->points.begin();
    presenter.attach(this);
}

void Interactor::notifyThatPointIsReady() {
    calibrator.calibrate(*pointIterator++);
    present(presenter, points, pointIterator);
    if (pointIterator == points.end())
        presenter.present(calibrator.results());
}

void Interactor::calibrate() { present(presenter, points, pointIterator); }

void Interactor::redo(Point p) {
    const auto closestPoint{
        *min_element(points.begin(), points.end(), [p](Point a, Point b) {
            return std::hypot(p.x - a.x, p.y - a.y) <
                std::hypot(p.x - b.x, p.y - b.y);
        })};
    calibrator.discard(closestPoint);
    presenter.present(closestPoint);
}
}
