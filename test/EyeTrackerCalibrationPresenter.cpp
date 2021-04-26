#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
struct Point {
    float x;
    float y;
};

namespace eye_tracker_calibration {
class View {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatAnimationHasFinished() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(View);
    virtual void attach(Observer *) = 0;
    virtual void moveDotTo(Point) = 0;
    virtual void shrinkDot() = 0;
};

class Presenter : public View::Observer {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatPointIsReady() = 0;
    };

    enum class DotState { idle, moving, shrinking };

    explicit Presenter(View &view) : view{view} { view.attach(this); }

    void attach(Observer *a) { observer = a; }

    void present(Point x) {
        view.moveDotTo(x);
        dotState = DotState::moving;
    }

    void notifyThatAnimationHasFinished() override {
        if (dotState == DotState::shrinking) {
            if (observer != nullptr)
                observer->notifyThatPointIsReady();
            dotState = DotState::idle;
        } else {
            view.shrinkDot();
            dotState = DotState::shrinking;
        }
    }

  private:
    View &view;
    Observer *observer{};
    DotState dotState{DotState::idle};
};
}
}

#include "LogString.hpp"
#include "assert-utility.hpp"
#include <gtest/gtest.h>

namespace av_speech_in_noise {
static void assertEqual(Point expected, Point actual) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.x, actual.x);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(expected.y, actual.y);
}

namespace eye_tracker_calibration {
namespace {
class PresenterObserverStub : public Presenter::Observer {
  public:
    [[nodiscard]] auto notifiedThatPointIsReady() const -> bool {
        return notifiedThatPointIsReady_;
    }

    void notifyThatPointIsReady() override { notifiedThatPointIsReady_ = true; }

  private:
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

  private:
    Observer *observer{};
    Point pointDotMovedTo_{};
    bool dotShrinked_{};
};

class EyeTrackerCalibrationPresenterTests : public ::testing::Test {};

#define EYE_TRACKER_CALIBRATION_PRESENTER_TEST(a)                              \
    TEST_F(EyeTrackerCalibrationPresenterTests, a)

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(movesDotToPoint) {
    ViewStub view;
    Presenter presenter{view};
    presenter.present({0.1F, 0.2F});
    assertEqual(Point{0.1F, 0.2F}, view.pointDotMovedTo());
}

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(shrinksDotAfterDoneMoving) {
    ViewStub view;
    Presenter presenter{view};
    presenter.present({});
    view.notifyObserverThatAnimationHasFinished();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.dotShrinked());
}

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(
    notifiesObserverThatPointIsReadyAfterDotShrinks) {
    ViewStub view;
    Presenter presenter{view};
    PresenterObserverStub observer;
    presenter.attach(&observer);
    presenter.present({});
    view.notifyObserverThatAnimationHasFinished();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(observer.notifiedThatPointIsReady());
    view.notifyObserverThatAnimationHasFinished();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(observer.notifiedThatPointIsReady());
}
}
}
}
