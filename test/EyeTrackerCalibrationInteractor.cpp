#include "assert-utility.hpp"
#include <recognition-test/EyeTrackerCalibration.hpp>
#include <utility>
#include <gtest/gtest.h>

namespace av_speech_in_noise::eye_tracker_calibration {
static void assertEqual(const Point &expected, const Point &actual) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.x, actual.x);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.y, actual.y);
}

static void assertEqual(const Result &expected, const Result &actual) {
    ::assertEqual<Point>(expected.leftEyeMappedPoints,
        actual.leftEyeMappedPoints,
        [](const Point &a, const Point &b) { assertEqual(a, b); });
    ::assertEqual<Point>(expected.rightEyeMappedPoints,
        actual.rightEyeMappedPoints,
        [](const Point &a, const Point &b) { assertEqual(a, b); });
    assertEqual(expected.point, actual.point);
}

namespace {
class IPresenterStub : public IPresenter {
  public:
    void attach(Observer *a) override { observer = a; }
    void present(Point x) override { presentedPoint_ = x; }
    void present(const std::vector<Result> &r) override { results_ = r; }
    void notifyThatPointIsReady() { observer->notifyThatPointIsReady(); }
    auto presentedPoint() -> Point { return presentedPoint_; }
    auto results() -> std::vector<Result> { return results_; }

  private:
    Point presentedPoint_{};
    std::vector<Result> results_{};
    Observer *observer{};
};

class EyeTrackerCalibratorStub : public EyeTrackerCalibrator {
  public:
    void collect(Point x) override { calibratedPoint_ = x; }

    auto calibratedPoint() -> Point { return calibratedPoint_; }

    void set(std::vector<Result> r) { results_ = std::move(r); }

    auto results() -> std::vector<Result> override { return results_; }

    auto discardedPoint() -> Point { return discardedPoint_; }

    void discard(Point x) override { discardedPoint_ = x; }

    [[nodiscard]] auto acquired() const -> bool { return acquired_; }

    void acquire() override { acquired_ = true; }

    void release() override { released_ = true; }

    [[nodiscard]] auto released() const -> bool { return released_; }

  private:
    std::vector<Result> results_{};
    Point discardedPoint_{};
    Point calibratedPoint_{};
    bool acquired_{};
    bool released_{};
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

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(acquiresCalibratorOnCalibrate) {
    interactor.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(calibrator.acquired());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(releasesCalibratorOnFinish) {
    interactor.finish();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(calibrator.released());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(presentsFirstPointOnCalibrate) {
    interactor.start();
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
    calibrator.set({{{{0.11F, 0.22F}, {0.33F, 0.44F}},
                        {{0.55F, 0.66F}, {0.77F, 0.88F}}, {0.1F, 0.2F}},
        {{{0.99F, 0.111F}, {0.222F, 0.333F}},
            {{0.444F, 0.555F}, {0.666F, 0.777F}}, {0.3F, 0.4F}},
        {{{0.888F, 0.999F}, {0.01F, 0.02F}}, {{0.03F, 0.04F}, {0.05F, 0.06F}},
            {0.5F, 0.6F}}});
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    ::assertEqual<Result>(
        {{{{0.11F, 0.22F}, {0.33F, 0.44F}}, {{0.55F, 0.66F}, {0.77F, 0.88F}},
             {0.1F, 0.2F}},
            {{{0.99F, 0.111F}, {0.222F, 0.333F}},
                {{0.444F, 0.555F}, {0.666F, 0.777F}}, {0.3F, 0.4F}},
            {{{0.888F, 0.999F}, {0.01F, 0.02F}},
                {{0.03F, 0.04F}, {0.05F, 0.06F}}, {0.5F, 0.6F}}},
        presenter.results(),
        [](const Result &a, const Result &b) { assertEqual(a, b); });
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(presentsPointForRedo) {
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    interactor.redo(Point{0.1F, 0.2F});
    assertEqual(Point{0.1F, 0.2F}, presenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(
    findsPointClosestToThatRequestedWhenRedoing) {
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    interactor.redo(Point{0.31F, 0.42F});
    assertEqual(Point{0.3F, 0.4F}, presenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(
    removesDataCollectedForPointBeingRedone) {
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    interactor.redo(Point{0.31F, 0.42F});
    assertEqual(Point{0.3F, 0.4F}, calibrator.discardedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(
    doesNotAcceptAdditionalRedosWhileOneInProgress) {
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    presenter.notifyThatPointIsReady();
    interactor.redo(Point{0.31F, 0.42F});
    interactor.redo(Point{0.51F, 0.62F});
    assertEqual(Point{0.3F, 0.4F}, calibrator.discardedPoint());
}
}
}
