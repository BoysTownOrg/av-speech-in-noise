#include "LogString.hpp"
#include "assert-utility.hpp"
#include "TimerStub.hpp"

#include <av-speech-in-noise/ui/EyeTrackerCalibration.hpp>

#include <gtest/gtest.h>

#include <functional>

namespace av_speech_in_noise {
class SubjectPresenterStub : public SubjectPresenter {
  public:
    void start() override { started_ = true; }
    void stop() override { stopped_ = true; }
    [[nodiscard]] auto started() const -> bool { return started_; }
    [[nodiscard]] auto stopped() const -> bool { return stopped_; }

  private:
    bool started_{};
    bool stopped_{};
};
}

namespace av_speech_in_noise::eye_tracker_calibration {
static void assertEqual(WindowPoint expected, WindowPoint actual) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.x, actual.x);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.y, actual.y);
}

static void present(SubjectPresenter &p, Point x = {}) { p.present(x); }

static void assertEqual(Line expected, Line actual) {
    assertEqual(expected.a, actual.a);
    assertEqual(expected.b, actual.b);
}

namespace {
class ObserverStub : public SubjectPresenter::Observer {
  public:
    [[nodiscard]] auto notifiedThatPointIsReady() const -> bool {
        return notifiedThatPointIsReady_;
    }

    void notifyThatPointIsReady() override {
        notifiedThatPointIsReady_ = true;
        if (callWhenNotifiedThatPointIsReady_)
            callWhenNotifiedThatPointIsReady_();
    }

    void callWhenNotifiedThatPointIsReady(std::function<void()> f) {
        callWhenNotifiedThatPointIsReady_ = std::move(f);
    }

  private:
    std::function<void()> callWhenNotifiedThatPointIsReady_;
    bool notifiedThatPointIsReady_{};
};

class SubjectViewStub : public SubjectView {
  public:
    void attach(Observer *a) override { observer = a; }

    [[nodiscard]] auto pointDotMovedTo() const -> WindowPoint {
        return pointDotMovedTo_;
    }

    void moveDotTo(WindowPoint x) override { pointDotMovedTo_ = x; }

    [[nodiscard]] auto dotShrinked() const -> bool { return dotShrinked_; }

    void shrinkDot() override { dotShrinked_ = true; }

    void notifyObserverThatAnimationHasFinished() const {
        observer->notifyThatAnimationHasFinished();
    }

    [[nodiscard]] auto dotGrew() const -> bool { return dotGrew_; }

    void growDot() override { dotGrew_ = true; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void show() override { shown_ = true; }

    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void hide() override { hidden_ = true; }

  private:
    std::vector<Line> redLinesDrawn_;
    std::vector<Line> greenLinesDrawn_;
    std::vector<WindowPoint> whiteCircleCenters_;
    Observer *observer{};
    WindowPoint pointDotMovedTo_{};
    bool shown_{};
    bool dotShrinked_{};
    bool dotGrew_{};
    bool hidden_{};
};

class TesterViewStub : public TesterView {
  public:
    auto redLinesDrawn() -> std::vector<Line> { return redLinesDrawn_; }

    void drawRed(Line line) override { redLinesDrawn_.push_back(line); }

    void drawGreen(Line line) override { greenLinesDrawn_.push_back(line); }

    auto greenLinesDrawn() -> std::vector<Line> { return greenLinesDrawn_; }

    auto whiteCircleCenters() -> std::vector<WindowPoint> {
        return whiteCircleCenters_;
    }

    void drawWhiteCircleWithCenter(WindowPoint x) override {
        whiteCircleCenters_.push_back(x);
    }

    void clear() override { cleared_ = true; }

    [[nodiscard]] auto cleared() const -> bool { return cleared_; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void show() override { shown_ = true; }

    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void hide() override { hidden_ = true; }

  private:
    std::vector<Line> redLinesDrawn_;
    std::vector<Line> greenLinesDrawn_;
    std::vector<WindowPoint> whiteCircleCenters_;
    bool shown_{};
    bool cleared_{};
    bool hidden_{};
};
}

namespace validation {
namespace {
class TesterViewStub : public TesterView {
  public:
    auto leftEyeAccuracyDegrees() -> std::string {
        return leftEyeAccuracyDegrees_;
    }

    auto leftEyePrecisionDegrees() -> std::string {
        return leftEyePrecisionDegrees_;
    }

    auto rightEyeAccuracyDegrees() -> std::string {
        return rightEyeAccuracyDegrees_;
    }

    auto rightEyePrecisionDegrees() -> std::string {
        return rightEyePrecisionDegrees_;
    }

    void setLeftEyeAccuracyDegrees(const std::string &s) override {
        leftEyeAccuracyDegrees_ = s;
    }

    void setLeftEyePrecisionDegrees(const std::string &s) override {
        leftEyePrecisionDegrees_ = s;
    }

    void setRightEyeAccuracyDegrees(const std::string &s) override {
        rightEyeAccuracyDegrees_ = s;
    }

    void setRightEyePrecisionDegrees(const std::string &s) override {
        rightEyePrecisionDegrees_ = s;
    }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void show() override { shown_ = true; }

    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void hide() override { hidden_ = true; }

  private:
    std::string leftEyeAccuracyDegrees_;
    std::string leftEyePrecisionDegrees_;
    std::string rightEyeAccuracyDegrees_;
    std::string rightEyePrecisionDegrees_;
    bool shown_{};
    bool hidden_{};
};
}
}

static void notifyObserverThatAnimationHasFinished(SubjectViewStub &view) {
    view.notifyObserverThatAnimationHasFinished();
}

namespace {
class EyeTrackerCalibrationSubjectPresenterTests : public ::testing::Test {
  protected:
    SubjectViewStub view;
    av_speech_in_noise::SubjectPresenterStub parentPresenter;
    TimerStub timer;
    SubjectPresenterImpl presenter{view, parentPresenter, timer};
};

class EyeTrackerCalibrationTesterPresenterTests : public ::testing::Test {
  protected:
    TesterViewStub view;
    TesterPresenterImpl presenter{view};
};

class EyeTrackerCalibrationValidationPresenterTests : public ::testing::Test {
  protected:
    validation::TesterViewStub view;
    validation::TesterPresenterImpl presenter{view};
};

#define EYE_TRACKER_CALIBRATION_SUBJECT_PRESENTER_TEST(a)                      \
    TEST_F(EyeTrackerCalibrationSubjectPresenterTests, a)

#define EYE_TRACKER_CALIBRATION_TESTER_PRESENTER_TEST(a)                       \
    TEST_F(EyeTrackerCalibrationTesterPresenterTests, a)

#define EYE_TRACKER_CALIBRATION_VALIDATION_PRESENTER_TEST(a)                   \
    TEST_F(EyeTrackerCalibrationValidationPresenterTests, a)

EYE_TRACKER_CALIBRATION_SUBJECT_PRESENTER_TEST(movesDotToPoint) {
    present(presenter, {0.1F, 0.2F});
    assertEqual(WindowPoint{0.1F, 0.8F}, view.pointDotMovedTo());
}

EYE_TRACKER_CALIBRATION_SUBJECT_PRESENTER_TEST(startShowsView) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

EYE_TRACKER_CALIBRATION_SUBJECT_PRESENTER_TEST(startStartsParent) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(parentPresenter.started());
}

EYE_TRACKER_CALIBRATION_SUBJECT_PRESENTER_TEST(stopHidesView) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

EYE_TRACKER_CALIBRATION_SUBJECT_PRESENTER_TEST(stopStopsParent) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(parentPresenter.stopped());
}

EYE_TRACKER_CALIBRATION_TESTER_PRESENTER_TEST(startShowsView) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

EYE_TRACKER_CALIBRATION_TESTER_PRESENTER_TEST(stopHidesView) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

EYE_TRACKER_CALIBRATION_SUBJECT_PRESENTER_TEST(
    schedulesCallbackAfterDoneMoving) {
    present(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(timer.callbackScheduled());
}

EYE_TRACKER_CALIBRATION_SUBJECT_PRESENTER_TEST(
    notifiesObserverThatPointIsReadyAfterTimerCallback) {
    ObserverStub observer;
    presenter.attach(&observer);
    present(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(observer.notifiedThatPointIsReady());
    timer.callback();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(observer.notifiedThatPointIsReady());
}

EYE_TRACKER_CALIBRATION_TESTER_PRESENTER_TEST(results) {
    presenter.present({{{{0.11F, 0.22F}, {0.33F, 0.44F}},
                           {{0.55F, 0.66F}, {0.77F, 0.88F}}, {0.1F, 0.2F}},
        {{{0.99F, 0.111F}, {0.222F, 0.333F}},
            {{0.444F, 0.555F}, {0.666F, 0.777F}}, {0.3F, 0.4F}},
        {{{0.888F, 0.999F}, {0.01F, 0.02F}}, {{0.03F, 0.04F}, {0.05F, 0.06F}},
            {0.5F, 0.6F}}});
    ::assertEqual<Line>({{{0.1F, 1 - 0.2F}, {0.11F, 1 - 0.22F}},
                            {{0.1F, 1 - 0.2F}, {0.33F, 1 - 0.44F}},
                            {{0.3F, 1 - 0.4F}, {0.99F, 1 - 0.111F}},
                            {{0.3F, 1 - 0.4F}, {0.222F, 1 - 0.333F}},
                            {{0.5F, 1 - 0.6F}, {0.888F, 1 - 0.999F}},
                            {{0.5F, 1 - 0.6F}, {0.01F, 1 - 0.02F}}},
        view.redLinesDrawn(),
        [](const Line &a, const Line &b) { assertEqual(a, b); });
    ::assertEqual<Line>({{{0.1F, 1 - 0.2F}, {0.55F, 1 - 0.66F}},
                            {{0.1F, 1 - 0.2F}, {0.77F, 1 - 0.88F}},
                            {{0.3F, 1 - 0.4F}, {0.444F, 1 - 0.555F}},
                            {{0.3F, 1 - 0.4F}, {0.666F, 1 - 0.777F}},
                            {{0.5F, 1 - 0.6F}, {0.03F, 1 - 0.04F}},
                            {{0.5F, 1 - 0.6F}, {0.05F, 1 - 0.06F}}},
        view.greenLinesDrawn(),
        [](const Line &a, const Line &b) { assertEqual(a, b); });
    ::assertEqual<WindowPoint>(
        {{0.1F, 1 - 0.2F}, {0.3F, 1 - 0.4F}, {0.5F, 1 - 0.6F}},
        view.whiteCircleCenters(),
        [](const WindowPoint &a, const WindowPoint &b) { assertEqual(a, b); });
}

EYE_TRACKER_CALIBRATION_TESTER_PRESENTER_TEST(resultsFirstClearsView) {
    presenter.present(std::vector<Result>{});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.cleared());
}

EYE_TRACKER_CALIBRATION_VALIDATION_PRESENTER_TEST(validationResult) {
    presenter.present(validation::Result{{{1}, {2}}, {{3}, {4}}});
    ::assertEqual("1", view.leftEyeAccuracyDegrees());
    ::assertEqual("2", view.leftEyePrecisionDegrees());
    ::assertEqual("3", view.rightEyeAccuracyDegrees());
    ::assertEqual("4", view.rightEyePrecisionDegrees());
}

EYE_TRACKER_CALIBRATION_VALIDATION_PRESENTER_TEST(startShowsView) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

EYE_TRACKER_CALIBRATION_VALIDATION_PRESENTER_TEST(stopHidesView) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}
}
}
