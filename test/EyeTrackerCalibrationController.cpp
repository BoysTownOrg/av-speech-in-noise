#include "assert-utility.hpp"
#include <presentation/EyeTrackerCalibration.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::eye_tracker_calibration {
static void assertEqual(const Point &expected, const Point &actual) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.x, actual.x);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.y, actual.y);
}

namespace {
class ControlStub : public Control {
  public:
    void notifyObserverThatWindowHasBeenTouched(WindowPoint x) {
        observer->notifyThatWindowHasBeenTouched(x);
    }

    void attach(Observer *a) override { observer = a; }

    void setWhiteCircleCenters(std::vector<WindowPoint> v) {
        whiteCircleCenters_ = std::move(v);
    }

    auto whiteCircleCenters() -> std::vector<WindowPoint> override {
        return whiteCircleCenters_;
    }

    void setWhiteCircleDiameter(double x) { whiteCircleDiameter_ = x; }

    auto whiteCircleDiameter() -> double override {
        return whiteCircleDiameter_;
    }

  private:
    std::vector<WindowPoint> whiteCircleCenters_;
    Observer *observer{};
    double whiteCircleDiameter_{};
};

class InteractorStub : public IInteractor {
  public:
    auto redoPoint() -> Point { return redoPoint_; }

    void redo(Point x) override { redoPoint_ = x; }

    void finish() override {}

    void start() override {}

  private:
    Point redoPoint_{};
};

class EyeTrackerCalibrationControllerTests : public ::testing::Test {};

#define EYE_TRACKER_CALIBRATION_CONTROLLER_TEST(a)                             \
    TEST_F(EyeTrackerCalibrationControllerTests, a)

EYE_TRACKER_CALIBRATION_CONTROLLER_TEST(respondsToWindowTouch) {
    ControlStub control;
    InteractorStub interactor;
    Controller controller{control, interactor};
    control.setWhiteCircleCenters({{0.1, 0.2}, {0.3, 0.4}, {0.5, 0.6}});
    control.setWhiteCircleDiameter(0.07);
    control.notifyObserverThatWindowHasBeenTouched({0.305, 0.406});
    assertEqual(Point{0.3, 1 - 0.4}, interactor.redoPoint());
}
}
}
