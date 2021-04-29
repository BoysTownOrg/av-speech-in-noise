#include "assert-utility.hpp"
#include <recognition-test/EyeTrackerCalibration.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::eye_tracker_calibration {
static void assertEqual(Point expected, Point actual) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.x, actual.x);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.y, actual.y);
}

namespace {
class IPresenterStub : public IPresenter {
  public:
    void attach(Observer *a) override { observer = a; }
    void present(Point x) override { presentedPoint_ = x; }
    void notifyThatPointIsReady() { observer->notifyThatPointIsReady(); }
    auto presentedPoint() -> Point { return presentedPoint_; }

  private:
    Point presentedPoint_{};
    Observer *observer{};
};

class EyeTrackerCalibratorStub : public EyeTrackerCalibrator {
  public:
    void calibrate(Point x) override { calibratedPoint_ = x; }

    auto calibratedPoint() -> Point { return calibratedPoint_; }

  private:
    Point calibratedPoint_{};
};

class EyeTrackerCalibrationInteractorTests : public ::testing::Test {};

#define EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(a)                             \
    TEST_F(EyeTrackerCalibrationInteractorTests, a)

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(presentsFirstPointOnCalibrate) {
    IPresenterStub presenter;
    EyeTrackerCalibratorStub calibrator;
    Interactor interactor{
        presenter, calibrator, {{0.1F, 0.2F}, {0.3F, 0.4F}, {0.5, 0.6F}}};
    interactor.calibrate();
    assertEqual(Point{0.1F, 0.2F}, presenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(calibratesPointWhenPointReady) {
    IPresenterStub presenter;
    EyeTrackerCalibratorStub calibrator;
    Interactor interactor{
        presenter, calibrator, {{0.1F, 0.2F}, {0.3F, 0.4F}, {0.5, 0.6F}}};
    presenter.notifyThatPointIsReady();
    assertEqual(Point{0.1F, 0.2F}, calibrator.calibratedPoint());
}
}
}
