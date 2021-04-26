#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
struct Point {
    float x;
    float y;
};

class EyeTrackerCalibrationView {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatAnimationHasFinished() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        EyeTrackerCalibrationView);
    virtual void attach(Observer *) = 0;
    virtual void moveDotTo(Point) = 0;
    virtual void shrinkDot() = 0;
};

class EyeTrackerCalibrationPresenter
    : public EyeTrackerCalibrationView::Observer {
  public:
    explicit EyeTrackerCalibrationPresenter(EyeTrackerCalibrationView &view)
        : view{view} {
        view.attach(this);
    }

    void present(Point x) { view.moveDotTo(x); }

    void notifyThatAnimationHasFinished() override { view.shrinkDot(); }

  private:
    EyeTrackerCalibrationView &view;
};
}

#include "LogString.hpp"
#include "assert-utility.hpp"
#include <gtest/gtest.h>

namespace av_speech_in_noise {
static void assertEqual(Point expected, Point actual) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.x, actual.x);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.y, actual.y);
}

namespace {
class EyeTrackerCalibrationViewStub : public EyeTrackerCalibrationView {
  public:
    void attach(Observer *a) override { observer = a; }

    auto pointDotMovedTo() -> Point { return pointDotMovedTo_; }

    void moveDotTo(Point x) override { pointDotMovedTo_ = x; }

    [[nodiscard]] auto dotShrinked() const -> bool { return dotShrinked_; }

    void shrinkDot() override { dotShrinked_ = true; }

    void notifyObserverThatAnimationHasFinished() {
        observer->notifyThatAnimationHasFinished();
    }

  private:
    Observer *observer{};
    Point pointDotMovedTo_{};
    bool dotShrinked_{};
};

class EyeTrackerCalibrationPresenterTests : public ::testing::Test {};

#define EYE_TRACKER_CALIBRATION_PRESENTER_TEST(a)                              \
    TEST_F(EyeTrackerCalibrationPresenterTests, a)

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(movesDotToPoint) {
    EyeTrackerCalibrationViewStub view;
    EyeTrackerCalibrationPresenter presenter{view};
    presenter.present({0.1F, 0.2F});
    assertEqual(Point{0.1F, 0.2F}, view.pointDotMovedTo());
}

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(shrinksDotAfterDoneMoving) {
    EyeTrackerCalibrationViewStub view;
    EyeTrackerCalibrationPresenter presenter{view};
    presenter.present({});
    view.notifyObserverThatAnimationHasFinished();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.dotShrinked());
}
}
}
