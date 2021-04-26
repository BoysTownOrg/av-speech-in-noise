#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
struct Point {
    float x;
    float y;
};

class EyeTrackerCalibrationView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        EyeTrackerCalibrationView);
    virtual void moveDotTo(Point) = 0;
    virtual void shrinkDot() = 0;
};

class EyeTrackerCalibrationPresenter {
  public:
    explicit EyeTrackerCalibrationPresenter(EyeTrackerCalibrationView &view)
        : view{view} {}

    void present(Point x) {
        view.shrinkDot();
        view.moveDotTo(x);
    }

  private:
    EyeTrackerCalibrationView &view;
};
}

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
    auto pointDotMovedTo() -> Point { return pointDotMovedTo_; }

    void moveDotTo(Point x) override { pointDotMovedTo_ = x; }

    [[nodiscard]] auto dotShrinked() const -> bool { return dotShrinked_; }

    void shrinkDot() { dotShrinked_ = true; }

  private:
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

EYE_TRACKER_CALIBRATION_PRESENTER_TEST(shrinksDot) {
    EyeTrackerCalibrationViewStub view;
    EyeTrackerCalibrationPresenter presenter{view};
    presenter.present({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.dotShrinked());
}
}
}
