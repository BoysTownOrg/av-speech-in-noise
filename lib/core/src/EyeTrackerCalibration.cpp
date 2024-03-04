#include "EyeTrackerCalibration.hpp"

#include <algorithm>
#include <cmath>
#include <functional>

namespace av_speech_in_noise::eye_tracker_calibration {
static void write(std::ostream &stream, const Point &p) {
    stream << p.x;
    stream << ", ";
    stream << p.y;
}

static void write(std::ostream &stream,
    const std::vector<BinocularSample> &samples,
    const std::function<const Point(const BinocularSample &)> &f) {
    stream << '[';
    auto first{true};
    for (const auto &s : samples) {
        if (!first) {
            stream << "; ";
        }
        write(stream, f(s));
        first = false;
    }
    stream << ']';
}

void write(std::ostream &stream, const std::vector<Result> &v) {
    stream << "Point|Left|Right\n";
    for (const auto &result : v) {
        stream << '[';
        write(stream, result.point);
        stream << ']';

        stream << '|';
        write(stream, result.samples,
            [](const BinocularSample &s) { return s.left.point; });
        stream << '|';
        write(stream, result.samples,
            [](const BinocularSample &s) { return s.right.point; });
        stream << '\n';
    }
}

static auto distance(Point a, Point b) -> float {
    return std::hypot(a.x - b.x, a.y - b.y);
}

static void present(
    SubjectPresenter &presenter, const std::vector<Point> &points) {
    presenter.present(points.front());
}

InteractorImpl::InteractorImpl(SubjectPresenter &subjectPresenter,
    TesterPresenter &testerPresenter, Calibrator &calibrator,
    ResultsWriter &writer, std::vector<Point> points)
    : points{std::move(points)}, subjectPresenter{subjectPresenter},
      testerPresenter{testerPresenter}, calibrator{calibrator}, writer{writer} {
    subjectPresenter.attach(this);
}

void InteractorImpl::notifyThatPointIsReady() {
    calibrator.collect(pointsToCalibrate.front());
    pointsToCalibrate.erase(pointsToCalibrate.begin());
    if (pointsToCalibrate.empty()) {
        const auto results{calibrator.results()};
        testerPresenter.present(results);
        writer.write(results);
    } else
        present(subjectPresenter, pointsToCalibrate);
}

void InteractorImpl::start() {
    calibrator.acquire();
    pointsToCalibrate = points;
    subjectPresenter.start();
    testerPresenter.start();
    present(subjectPresenter, pointsToCalibrate);
}

void InteractorImpl::finish() {
    if (pointsToCalibrate.empty()) {
        calibrator.release();
        subjectPresenter.stop();
        testerPresenter.stop();
    }
}

void InteractorImpl::redo(Point p) {
    if (!pointsToCalibrate.empty())
        return;
    const auto closestPoint{min_element(points.begin(), points.end(),
        [p](Point a, Point b) { return distance(p, a) < distance(p, b); })};
    calibrator.discard(*closestPoint);
    pointsToCalibrate.push_back(*closestPoint);
    present(subjectPresenter, pointsToCalibrate);
}

namespace validation {
InteractorImpl::InteractorImpl(SubjectPresenter &subjectPresenter,
    TesterPresenter &testerPresenter, Validator &validator,
    std::vector<Point> points)
    : points{std::move(points)}, subjectPresenter{subjectPresenter},
      testerPresenter{testerPresenter}, validator{validator} {
    subjectPresenter.attach(this);
}

void InteractorImpl::start() {
    validator.acquire();
    subjectPresenter.start();
    testerPresenter.start();
    pointsToValidate = points;
    present(subjectPresenter, pointsToValidate);
}

void InteractorImpl::finish() {
    if (pointsToValidate.empty()) {
        validator.release();
        subjectPresenter.stop();
        testerPresenter.stop();
    }
}

void InteractorImpl::notifyThatPointIsReady() {
    validator.collect(pointsToValidate.front());
    pointsToValidate.erase(pointsToValidate.begin());
    if (pointsToValidate.empty())
        testerPresenter.present(validator.result());
    else
        present(subjectPresenter, pointsToValidate);
}
}
}
