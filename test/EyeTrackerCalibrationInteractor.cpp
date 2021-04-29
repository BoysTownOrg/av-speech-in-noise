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
    void present(const std::vector<Result> &r) override { results_ = &r; }
    void notifyThatPointIsReady() { observer->notifyThatPointIsReady(); }
    auto presentedPoint() -> Point { return presentedPoint_; }
    auto results() -> const std::vector<Result> * { return results_; }

  private:
    Point presentedPoint_{};
    const std::vector<Result> *results_{};
    Observer *observer{};
};

class EyeTrackerCalibratorStub : public EyeTrackerCalibrator {
  public:
    void calibrate(Point x) override { calibratedPoint_ = x; }

    auto calibratedPoint() -> Point { return calibratedPoint_; }

    void set(const std::vector<Result> &r) { results_ = &r; }

    auto results() -> const std::vector<Result> & override { return *results_; }

  private:
    const std::vector<Result> *results_{};
    Point calibratedPoint_{};
};

class EyeTrackerCalibrationInteractorTests : public ::testing::Test {
  protected:
    IPresenterStub presenter;
    EyeTrackerCalibratorStub calibrator;
    Interactor interactor{
        presenter, calibrator, {{0.1F, 0.2F}, {0.3F, 0.4F}, {0.5, 0.6F}}};
};

#define EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(a)                             \
    TEST_F(EyeTrackerCalibrationInteractorTests, a)

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(presentsFirstPointOnCalibrate) {
    interactor.calibrate();
    assertEqual(Point{0.1F, 0.2F}, presenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(calibratesPointWhenPointReady) {
    presenter.notifyThatPointIsReady();
    assertEqual(Point{0.1F, 0.2F}, calibrator.calibratedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(
    presentsNextPointAfterCalibratingPrevious) {
    presenter.notifyThatPointIsReady();
    assertEqual(Point{0.3F, 0.4F}, presenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(doesNotPresentAnymorePoints) {
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    assertEqual(Point{0.5F, 0.6F}, presenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(
    presentsResultsAfterFinalPointCalibrated) {
    std::vector<Result> results;
    calibrator.set(results);
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(&results, presenter.results());
}
}
}
