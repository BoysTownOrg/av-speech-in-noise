#include "EyeTrackerCalibration.hpp"

namespace av_speech_in_noise::eye_tracker_calibration {
static auto consumeOne(std::vector<Point> &x) -> Point {
    auto point = x.front();
    x.erase(x.begin());
    return point;
}

static void present(IPresenter &presenter, const std::vector<Point> &points) {
    if (!points.empty())
        presenter.present(points.front());
}

Interactor::Interactor(IPresenter &presenter, EyeTrackerCalibrator &calibrator,
    std::vector<Point> points)
    : points{std::move(points)}, presenter{presenter}, calibrator{calibrator} {
    presenter.attach(this);
}

void Interactor::notifyThatPointIsReady() {
    calibrator.calibrate(consumeOne(points));
    present(presenter, points);
    if (points.empty())
        presenter.present(calibrator.results());
}

void Interactor::calibrate() { present(presenter, points); }

void Interactor::redo(Point p) { presenter.present(p); }
}
