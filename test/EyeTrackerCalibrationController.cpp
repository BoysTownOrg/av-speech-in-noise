#include "assert-utility.hpp"
#include <av-speech-in-noise/ui/EyeTrackerCalibration.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::eye_tracker_calibration {
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

    void notifyThatCloseButtonHasBeenClicked() {
        observer->notifyThatCloseButtonHasBeenClicked();
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

class EyeTrackerCalibrationValidationControllerTests : public ::testing::Test {
  protected:
    ControlStub control;
    InteractorStub interactor;
    Controller controller{control, interactor};
};

#define EYE_TRACKER_CALIBRATION_VALIDATION_CONTROLLER_TEST(a)                  \
    TEST_F(EyeTrackerCalibrationValidationControllerTests, a)
}
}

namespace {
class ControlStub : public Control {
  public:
    void notifyThatMenuHasBeenSelected() {
        observer->notifyThatMenuHasBeenSelected();
    }

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

    void start() override { started_ = true; }

    [[nodiscard]] auto started() const -> bool { return started_; }

  private:
    Point redoPoint_{};
    bool started_{};
};

class EyeTrackerCalibrationControllerTests : public ::testing::Test {
  protected:
    ControlStub control;
    InteractorStub interactor;
    Controller controller{control, interactor};
};

#define EYE_TRACKER_CALIBRATION_CONTROLLER_TEST(a)                             \
    TEST_F(EyeTrackerCalibrationControllerTests, a)

EYE_TRACKER_CALIBRATION_CONTROLLER_TEST(respondsToWindowTouch) {
    control.setWhiteCircleCenters({{0.1, 0.2}, {0.3, 0.4}, {0.5, 0.6}});
    control.setWhiteCircleDiameter(0.07);
    control.notifyObserverThatWindowHasBeenTouched({0.305, 0.406});
    assertEqual(Point{0.3, 1 - 0.4}, interactor.redoPoint());
}

EYE_TRACKER_CALIBRATION_CONTROLLER_TEST(startsInteractorWhenMenuSelected) {
    control.notifyThatMenuHasBeenSelected();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(interactor.started());
}
}

namespace validation {
namespace {
EYE_TRACKER_CALIBRATION_VALIDATION_CONTROLLER_TEST(
    startsInteractorWhenMenuSelected) {
    control.notifyThatMenuHasBeenSelected();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(interactor.started());
}

EYE_TRACKER_CALIBRATION_VALIDATION_CONTROLLER_TEST(
    finishesInteractorWhenCloseButtonClicked) {
    control.notifyThatCloseButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(interactor.finished());
}
}
}
}
