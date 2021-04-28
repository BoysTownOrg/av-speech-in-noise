#include <recognition-test/EyeTrackerCalibration.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::eye_tracker_calibration {
namespace {
class EyeTrackerCalibrationInteractorTests : public ::testing::Test {};

#define EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(a)                             \
    TEST_F(EyeTrackerCalibrationInteractorTests, a)

EYE_TRACKER_CALIBRATION_INTERACTOR_TEST(tbd) {
    Interactor interactor;
    FAIL();
}
}
}
