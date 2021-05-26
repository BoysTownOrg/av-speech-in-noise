#include "assert-utility.hpp"
#include <av-speech-in-noise/ui/EyeTrackerCalibration.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::eye_tracking::calibration {
static void assertEqual(const Point &expected, const Point &actual) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.x, actual.x);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.y, actual.y);
}

namespace validation {
namespace {
class ControlStub : public Control {
  public:
    void notifyThatMenuHasBeenSelected() {
        observer->notifyThatMenuHasBeenSelected();
    }

    void attach(Observer *a) override { observer = a; }

  private:
    Observer *observer{};
};

class InteractorStub : public Interactor {
  public:
    void start() override { started_ = true; }

    void finish() override { finished_ = true; }

    [[nodiscard]] auto started() const -> bool { return started_; }

    [[nodiscard]] auto finished() const -> bool { return finished_; }

  private:
    bool started_{};
    bool finished_{};
};
}
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

class InteractorStub : public Interactor {
  public:
    auto redoPoint() -> Point { return redoPoint_; }

    void redo(Point x) override { redoPoint_ = x; }

    void finish() override {}

    void start() override {}

  private:
    Point redoPoint_{};
};

class EyeTrackerCalibrationControllerTests : public ::testing::Test {};

class EyeTrackingCalibrationValidationControllerTests : public ::testing::Test {
};

#define EYE_TRACKER_CALIBRATION_CONTROLLER_TEST(a)                             \
    TEST_F(EyeTrackerCalibrationControllerTests, a)

#define EYE_TRACKING_CALIBRATION_VALIDATION_CONTROLLER_TEST(a)                 \
    TEST_F(EyeTrackingCalibrationValidationControllerTests, a)

EYE_TRACKER_CALIBRATION_CONTROLLER_TEST(respondsToWindowTouch) {
    ControlStub control;
    InteractorStub interactor;
    Controller controller{control, interactor};
    control.setWhiteCircleCenters({{0.1, 0.2}, {0.3, 0.4}, {0.5, 0.6}});
    control.setWhiteCircleDiameter(0.07);
    control.notifyObserverThatWindowHasBeenTouched({0.305, 0.406});
    assertEqual(Point{0.3, 1 - 0.4}, interactor.redoPoint());
}

EYE_TRACKING_CALIBRATION_VALIDATION_CONTROLLER_TEST(
    startsInteractorWhenMenuSelected) {
    validation::ControlStub control;
    validation::InteractorStub interactor;
    validation::Controller controller{control, interactor};
    control.notifyThatMenuHasBeenSelected();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(interactor.started());
}
}
}
