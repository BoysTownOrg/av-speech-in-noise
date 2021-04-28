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

    auto pointDotMovedTo() -> Point { return pointDotMovedTo_; }

    void moveDotTo(Point x) override { pointDotMovedTo_ = x; }

    [[nodiscard]] auto dotShrinked() const -> bool { return dotShrinked_; }

    void shrinkDot() override { dotShrinked_ = true; }

    void notifyObserverThatAnimationHasFinished() {
        observer->notifyThatAnimationHasFinished();
    }

    [[nodiscard]] auto dotGrew() const -> bool { return dotGrew_; }

    void growDot() override { dotGrew_ = true; }

  private:
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
}
}
}
