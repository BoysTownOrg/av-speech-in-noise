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
        observer->notifyObserverThatWindowHasBeenTouched(x);
    }

    void attach(Observer *a) override { observer = a; }

  private:
    Observer *observer{};
};

class InteractorStub : public IInteractor {
  public:
    auto redoPoint() -> Point { return redoPoint_; }

    void redo(Point x) { redoPoint_ = x; }

  private:
    Point redoPoint_{};
};

class EyeTrackerCalibrationControllerTests : public ::testing::Test {};

#define EYE_TRACKER_CALIBRATION_CONTROLLER_TEST(a)                             \
    TEST_F(EyeTrackerCalibrationControllerTests, a)

EYE_TRACKER_CALIBRATION_CONTROLLER_TEST(tbd) {
    ControlStub control;
    InteractorStub interactor;
    Controller controller{control, interactor};
    control.notifyObserverThatWindowHasBeenTouched(WindowPoint{0.1, 0.2});
    assertEqual(Point{0.9F, 0.2F}, interactor.redoPoint());
}
}
}
