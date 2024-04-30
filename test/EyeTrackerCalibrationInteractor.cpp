#include "assert-utility.hpp"

#include <av-speech-in-noise/core/EyeTrackerCalibration.hpp>

#include <gtest/gtest.h>

#include <utility>

namespace av_speech_in_noise::eye_tracker_calibration {
static void assertEqual(const Point &expected, const Point &actual) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.x, actual.x);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.y, actual.y);
}

static auto operator==(const SampleInfo &a, const SampleInfo &b) -> bool {
    return a.used == b.used && a.valid == b.valid;
}

static void assertEqual(const Sample &expected, const Sample &actual) {
    assertEqual(expected.point, actual.point);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.info, actual.info);
}

static void assertEqual(
    const BinocularSample &expected, const BinocularSample &actual) {
    assertEqual(expected.left, actual.left);
    assertEqual(expected.right, actual.right);
}

static void assertEqual(
    const PointResult &expected, const PointResult &actual) {
    ::assertEqual<BinocularSample>(expected.samples, actual.samples,
        [](const BinocularSample &a, const BinocularSample &b) {
            assertEqual(a, b);
        });
    assertEqual(expected.point, actual.point);
}

namespace {
class SubjectPresenterStub : public SubjectPresenter {
  public:
    void attach(Observer *a) override { observer = a; }

    void present(Point x) override { presentedPoint_ = x; }

    void notifyThatPointIsReady() { observer->notifyThatPointIsReady(); }

    auto presentedPoint() -> Point { return presentedPoint_; }

    [[nodiscard]] auto stopped() const -> bool { return stopped_; }

    void stop() override { stopped_ = true; }

    [[nodiscard]] auto started() const -> bool { return started_; }

    void start() override { started_ = true; }

  private:
    Point presentedPoint_{};
    Results results_{};
    Observer *observer{};
    bool stopped_{};
    bool started_{};
};

class TesterPresenterStub : public TesterPresenter {
  public:
    void present(const Results &r) override { results_ = r; }

    auto results() -> Results { return results_; }

    [[nodiscard]] auto stopped() const -> bool { return stopped_; }

    void stop() override { stopped_ = true; }

    [[nodiscard]] auto started() const -> bool { return started_; }

    void start() override { started_ = true; }

  private:
    Results results_{};
    bool stopped_{};
    bool started_{};
};

class CalibratorStub : public Calibrator {
  public:
    void collect(Point x) override { calibratedPoint_ = x; }

    auto calibratedPoint() -> Point { return calibratedPoint_; }

    void set(Results r) { results_ = std::move(r); }

    auto results() -> Results override { return results_; }

    auto discardedPoint() -> Point { return discardedPoint_; }

    void discard(Point x) override { discardedPoint_ = x; }

    [[nodiscard]] auto acquired() const -> bool { return acquired_; }

    void acquire() override { acquired_ = true; }

    void release() override { released_ = true; }

    [[nodiscard]] auto released() const -> bool { return released_; }

  private:
    Results results_{};
    Point discardedPoint_{};
    Point calibratedPoint_{};
    bool acquired_{};
    bool released_{};
};

class ResultsWriterStub : public ResultsWriter {
  public:
    void write(const Results &v) override { results_ = v; };

    auto results() -> Results { return results_; }

  private:
    Results results_;
};

class EyeTrackerCalibrationInteractorTests : public ::testing::Test {
  protected:
    SubjectPresenterStub subjectPresenter;
    TesterPresenterStub testerPresenter;
    CalibratorStub calibrator;
    ResultsWriterStub writer;
    InteractorImpl interactor{subjectPresenter, testerPresenter, calibrator,
        writer, {{0.1F, 0.2F}, {0.3F, 0.4F}, {0.5, 0.6F}}};
};
}

namespace validation {
static void assertEqual(const Angle &expected, const Angle &actual) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.degrees, actual.degrees);
}

static void assertEqual(
    const MonocularResult &expected, const MonocularResult &actual) {
    assertEqual(expected.errorOfMeanGaze, actual.errorOfMeanGaze);
    assertEqual(expected.standardDeviationFromTheMeanGaze,
        actual.standardDeviationFromTheMeanGaze);
}

static void assertEqual(const Result &expected, const Result &actual) {
    assertEqual(expected.left, actual.left);
    assertEqual(expected.right, actual.right);
}

namespace {
class ValidatorStub : public Validator {
  public:
    [[nodiscard]] auto acquired() const -> bool { return acquired_; }

    void acquire() override { acquired_ = true; }

    [[nodiscard]] auto released() const -> bool { return released_; }

    void release() override { released_ = true; }

    auto validatedPoint() -> Point { return validatedPoint_; }

    void collect(Point p) override { validatedPoint_ = p; }

    void set(Result r) { result_ = r; }

    auto result() -> Result override { return result_; }

  private:
    validation::Result result_{};
    Point validatedPoint_{};
    bool acquired_{};
    bool released_{};
};

class TesterPresenterStub : public TesterPresenter {
  public:
    void present(const Result &b) override { result_ = b; }

    [[nodiscard]] auto stopped() const -> bool { return stopped_; }

    void stop() override { stopped_ = true; }

    [[nodiscard]] auto started() const -> bool { return started_; }

    void start() override { started_ = true; }

    auto result() -> Result { return result_; }

  private:
    Result result_{};
    bool stopped_{};
    bool started_{};
};

class EyeTrackerCalibrationValidationInteractorTests : public ::testing::Test {
  protected:
    SubjectPresenterStub subjectPresenter;
    TesterPresenterStub testerPresenter;
    ValidatorStub validator;
    InteractorImpl interactor{subjectPresenter, testerPresenter, validator,
        {{0.1F, 0.2F}, {0.3F, 0.4F}, {0.5, 0.6F}}};
};
}
}

static void notifyThatPointIsReady(SubjectPresenterStub &presenter) {
    presenter.notifyThatPointIsReady();
}

#define EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(a)                             \
    TEST_F(EyeTrackerCalibrationInteractorTests, a)

#define EYE_TRACKER_CALIBRATION_VALIDATION_INTERACTOR_TEST(a)                  \
    TEST_F(EyeTrackerCalibrationValidationInteractorTests, a)

namespace {
EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(acquiresCalibratorOnCalibrate) {
    interactor.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(calibrator.acquired());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(
    doesNotReleaseCalibratorUntilAllPointsAreCalibrated) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    interactor.finish();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(calibrator.released());
    notifyThatPointIsReady(subjectPresenter);
    interactor.finish();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(calibrator.released());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(stopsPresenterOnFinish) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    interactor.finish();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(subjectPresenter.stopped());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testerPresenter.stopped());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(
    writesResultsAfterFinalPointCalibrated) {
    Results results;
    BinocularSample s1;
    s1.left.point = {0.11, 0.22};
    s1.right.point = {0.55, 0.66};
    BinocularSample s2;
    s2.left.point = {0.33, 0.44};
    s2.right.point = {0.77, 0.88};
    PointResult r1;
    r1.samples = {s1, s2};
    r1.point = {0.1, 0.2};
    results.pointResults.push_back(r1);

    BinocularSample s4;
    s4.left.point = {0.99, 0.111};
    s4.right.point = {0.444, 0.555};
    BinocularSample s5;
    s5.left.point = {0.222, 0.333};
    s5.right.point = {0.666, 0.777};
    PointResult r2;
    r2.samples = {s4, s5};
    r2.point = {0.3, 0.4};
    results.pointResults.push_back(r2);

    BinocularSample s7;
    s7.left.point = {0.888, 0.999};
    s7.right.point = {0.03, 0.04};
    BinocularSample s8;
    s8.left.point = {0.01, 0.02};
    s8.right.point = {0.05, 0.06};
    PointResult r3;
    r3.samples = {s7, s8};
    r3.point = {0.5, 0.6};
    results.pointResults.push_back(r3);

    calibrator.set(results);
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);

    ::assertEqual<PointResult>(results.pointResults,
        writer.results().pointResults,
        [](const PointResult &a, const PointResult &b) { assertEqual(a, b); });
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(startsPresenter) {
    interactor.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(subjectPresenter.started());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testerPresenter.started());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(presentsFirstPointOnCalibrate) {
    interactor.start();
    assertEqual(Point{0.1F, 0.2F}, subjectPresenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(calibratesPointWhenPointReady) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    assertEqual(Point{0.1F, 0.2F}, calibrator.calibratedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(
    presentsNextPointAfterCalibratingPrevious) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    assertEqual(Point{0.3F, 0.4F}, subjectPresenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(doesNotPresentAnymorePoints) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    assertEqual(Point{0.5F, 0.6F}, subjectPresenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(restarts) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    interactor.finish();
    interactor.start();
    assertEqual(Point{0.1F, 0.2F}, subjectPresenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(
    presentsResultsAfterFinalPointCalibrated) {
    Results results;
    BinocularSample s1;
    s1.left.point = {0.11, 0.22};
    s1.right.point = {0.55, 0.66};
    BinocularSample s2;
    s2.left.point = {0.33, 0.44};
    s2.right.point = {0.77, 0.88};
    PointResult r1;
    r1.samples = {s1, s2};
    r1.point = {0.1, 0.2};
    results.pointResults.push_back(r1);

    BinocularSample s4;
    s4.left.point = {0.99, 0.111};
    s4.right.point = {0.444, 0.555};
    BinocularSample s5;
    s5.left.point = {0.222, 0.333};
    s5.right.point = {0.666, 0.777};
    PointResult r2;
    r2.samples = {s4, s5};
    r2.point = {0.3, 0.4};
    results.pointResults.push_back(r2);

    BinocularSample s7;
    s7.left.point = {0.888, 0.999};
    s7.right.point = {0.03, 0.04};
    BinocularSample s8;
    s8.left.point = {0.01, 0.02};
    s8.right.point = {0.05, 0.06};
    PointResult r3;
    r3.samples = {s7, s8};
    r3.point = {0.5, 0.6};
    results.pointResults.push_back(r3);

    calibrator.set(results);
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    ::assertEqual<PointResult>(results.pointResults,
        testerPresenter.results().pointResults,
        [](const PointResult &a, const PointResult &b) { assertEqual(a, b); });
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(presentsPointForRedo) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    interactor.redo(Point{0.1F, 0.2F});
    assertEqual(Point{0.1F, 0.2F}, subjectPresenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(
    findsPointClosestToThatRequestedWhenRedoing) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    interactor.redo(Point{0.31F, 0.42F});
    assertEqual(Point{0.3F, 0.4F}, subjectPresenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(
    removesDataCollectedForPointBeingRedone) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    interactor.redo(Point{0.31F, 0.42F});
    assertEqual(Point{0.3F, 0.4F}, calibrator.discardedPoint());
}

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(
    doesNotAcceptAdditionalRedosWhileOneInProgress) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    interactor.redo(Point{0.31F, 0.42F});
    interactor.redo(Point{0.51F, 0.62F});
    assertEqual(Point{0.3F, 0.4F}, calibrator.discardedPoint());
}
}

namespace validation {
namespace {
EYE_TRACKER_CALIBRATION_VALIDATION_INTERACTOR_TEST(acquiresValidatorOnStart) {
    interactor.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(validator.acquired());
}

EYE_TRACKER_CALIBRATION_VALIDATION_INTERACTOR_TEST(
    doesNotReleaseValidatorUntilAllPointsAreValidated) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    interactor.finish();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(validator.released());
    notifyThatPointIsReady(subjectPresenter);
    interactor.finish();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(validator.released());
}

EYE_TRACKER_CALIBRATION_VALIDATION_INTERACTOR_TEST(stopsPresenterOnFinish) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    interactor.finish();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(subjectPresenter.stopped());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testerPresenter.stopped());
}

EYE_TRACKER_CALIBRATION_VALIDATION_INTERACTOR_TEST(startsPresenter) {
    interactor.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(subjectPresenter.started());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testerPresenter.started());
}

EYE_TRACKER_CALIBRATION_VALIDATION_INTERACTOR_TEST(presentsFirstPointOnStart) {
    interactor.start();
    assertEqual(Point{0.1F, 0.2F}, subjectPresenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_VALIDATION_INTERACTOR_TEST(validatesPointWhenReady) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    assertEqual(Point{0.1F, 0.2F}, validator.validatedPoint());
}

EYE_TRACKER_CALIBRATION_VALIDATION_INTERACTOR_TEST(
    presentsNextPointAfterValidatingOne) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    assertEqual(Point{0.3F, 0.4F}, subjectPresenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_VALIDATION_INTERACTOR_TEST(
    doesNotPresentAnymorePointsWhenExhausted) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    assertEqual(Point{0.5F, 0.6F}, subjectPresenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_VALIDATION_INTERACTOR_TEST(restarts) {
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    interactor.finish();
    interactor.start();
    assertEqual(Point{0.1F, 0.2F}, subjectPresenter.presentedPoint());
}

EYE_TRACKER_CALIBRATION_VALIDATION_INTERACTOR_TEST(
    presentsResultsAfterFinalPointCalibrated) {
    validator.set(Result{{{1}, {2}}, {{3}, {4}}});
    interactor.start();
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    notifyThatPointIsReady(subjectPresenter);
    assertEqual(Result{{{1}, {2}}, {{3}, {4}}}, testerPresenter.result());
}
}
}
}
