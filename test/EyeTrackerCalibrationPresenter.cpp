#include "LogString.hpp"
#include "assert-utility.hpp"
#include <functional>
#include <presentation/EyeTrackerCalibration.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
static void assertEqual(Point expected, Point actual) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.x, actual.x);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.y, actual.y);
}

namespace eye_tracker_calibration {
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

    [[nodiscard]] auto pointDotMovedTo() const -> Point {
        return pointDotMovedTo_;
    }

    void moveDotTo(Point x) override { pointDotMovedTo_ = x; }

    [[nodiscard]] auto dotShrinked() const -> bool { return dotShrinked_; }

    void shrinkDot() override { dotShrinked_ = true; }

    void notifyObserverThatAnimationHasFinished() const {
        observer->notifyThatAnimationHasFinished();
    }

    [[nodiscard]] auto dotGrew() const -> bool { return dotGrew_; }

    void growDot() override { dotGrew_ = true; }

    auto redLinesDrawn() -> std::vector<Line> { return redLinesDrawn_; }

    void drawRed(Line line) { redLinesDrawn_.push_back(line); }

  private:
    std::vector<Line> redLinesDrawn_;
    Observer *observer{};
    Point pointDotMovedTo_{};
    bool dotShrinked_{};
    bool dotGrew_{};
};
}

static void notifyObserverThatAnimationHasFinished(ViewStub &view) {
    view.notifyObserverThatAnimationHasFinished();
}

namespace {
class EyeTrackerCalibrationPresenterTests : public ::testing::Test {};

#define EYE_TRACKER_CALIBRATION_PRESENTER_TEST(a)                              \
    TEST_F(EyeTrackerCalibrationPresenterTests, a)

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(movesDotToPoint) {
    ViewStub view;
    Presenter presenter{view};
    present(presenter, {0.1F, 0.2F});
    assertEqual(Point{0.1F, 0.2F}, view.pointDotMovedTo());
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
    assertEqual(Point{0.1F, 0.2F}, view.pointDotMovedTo());
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
    ::assertEqual<Line>(
        {{{0.1F, 0.2F}, {0.11F, 0.22F}}, {{0.1F, 0.2F}, {0.33F, 0.44F}},
            {{0.3F, 0.4F}, {0.99F, 0.111F}}, {{0.3F, 0.4F}, {0.222F, 0.333F}},
            {{0.5F, 0.6F}, {0.888F, 0.999F}}, {{0.5F, 0.6F}, {0.01F, 0.02F}}},
        view.redLinesDrawn(), assertEqual);
}
}
}
}
