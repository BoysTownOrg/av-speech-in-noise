#include "LogString.hpp"
#include "assert-utility.hpp"
#include <functional>
#include <av-speech-in-noise/ui/EyeTrackerCalibration.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::eye_tracking::calibration {
static void assertEqual(WindowPoint expected, WindowPoint actual) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.x, actual.x);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.y, actual.y);
}

static void present(Presenter &p, Point x = {}) { p.present(x); }

static void assertEqual(Line expected, Line actual) {
    assertEqual(expected.a, actual.a);
    assertEqual(expected.b, actual.b);
}

namespace {
class PresenterObserverStub : public IPresenter::Observer {
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

class ViewStub : public View {
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
    Observer *observer{};
    WindowPoint pointDotMovedTo_{};
    bool shown_{};
    bool dotShrinked_{};
    bool dotGrew_{};
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

  private:
    std::string leftEyeAccuracyDegrees_;
    std::string leftEyePrecisionDegrees_;
    std::string rightEyeAccuracyDegrees_;
    std::string rightEyePrecisionDegrees_;
};
}
}

static void notifyObserverThatAnimationHasFinished(ViewStub &view) {
    view.notifyObserverThatAnimationHasFinished();
}

namespace {
class EyeTrackerCalibrationPresenterTests : public ::testing::Test {};

class EyeTrackerCalibrationValidationPresenterTests : public ::testing::Test {};

#define EYE_TRACKER_CALIBRATION_PRESENTER_TEST(a)                              \
    TEST_F(EyeTrackerCalibrationPresenterTests, a)

#define EYE_TRACKER_CALIBRATION_VALIDATION_PRESENTER_TEST(a)                   \
    TEST_F(EyeTrackerCalibrationValidationPresenterTests, a)

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(movesDotToPoint) {
    ViewStub view;
    Presenter presenter{view};
    present(presenter, {0.1F, 0.2F});
    assertEqual(WindowPoint{0.1F, 0.8F}, view.pointDotMovedTo());
}

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(startShowsView) {
    ViewStub view;
    Presenter presenter{view};
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(stopHidesView) {
    ViewStub view;
    Presenter presenter{view};
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(shrinksDotAfterDoneMoving) {
    ViewStub view;
    Presenter presenter{view};
    present(presenter);
    notifyObserverThatAnimationHasFinished(view);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.dotShrinked());
}

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(
    notifiesObserverThatPointIsReadyAfterDotShrinks) {
    ViewStub view;
    Presenter presenter{view};
    PresenterObserverStub observer;
    presenter.attach(&observer);
    present(presenter);
    notifyObserverThatAnimationHasFinished(view);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(observer.notifiedThatPointIsReady());
    notifyObserverThatAnimationHasFinished(view);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(observer.notifiedThatPointIsReady());
}

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(growsDotIfShrunk) {
    ViewStub view;
    Presenter presenter{view};
    PresenterObserverStub observer;
    presenter.attach(&observer);
    present(presenter);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(view.dotGrew());
    notifyObserverThatAnimationHasFinished(view);
    observer.callWhenNotifiedThatPointIsReady(
        [&presenter]() { present(presenter); });
    notifyObserverThatAnimationHasFinished(view);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.dotGrew());
}

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(movesDotAfterItGrows) {
    ViewStub view;
    Presenter presenter{view};
    present(presenter);
    notifyObserverThatAnimationHasFinished(view);
    notifyObserverThatAnimationHasFinished(view);
    present(presenter, {0.1F, 0.2F});
    notifyObserverThatAnimationHasFinished(view);
    assertEqual(WindowPoint{0.1F, 1 - 0.2F}, view.pointDotMovedTo());
}

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(results) {
    ViewStub view;
    Presenter presenter{view};
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

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(resultsFirstClearsView) {
    ViewStub view;
    Presenter presenter{view};
    presenter.present(std::vector<Result>{});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.cleared());
}

EYE_TRACKER_CALIBRATION_VALIDATION_PRESENTER_TEST(validationResult) {
    validation::TesterViewStub view;
    validation::TesterPresenter presenter{view};
    presenter.present(validation::Result{{{1}, {2}}, {{3}, {4}}});
    ::assertEqual("1", view.leftEyeAccuracyDegrees());
    ::assertEqual("2", view.leftEyePrecisionDegrees());
    ::assertEqual("3", view.rightEyeAccuracyDegrees());
    ::assertEqual("4", view.rightEyePrecisionDegrees());
}
}
}
