#include "assert-utility.hpp"

#include <av-speech-in-noise/core/EyeTrackerCalibration.hpp>

#include <gtest/gtest.h>

#include <sstream>

namespace av_speech_in_noise {
class EyeTrackerCalibrationSerializationTests : public ::testing::Test {};

#define EYE_TRACKER_CALIBRATION_SERIALIZATION_TEST(a)                          \
    TEST_F(EyeTrackerCalibrationSerializationTests, a)

EYE_TRACKER_CALIBRATION_SERIALIZATION_TEST(writesResults) {
    std::stringstream stream;
    std::vector<eye_tracker_calibration::Result> results = {
        {{{0.49, 0.51}, {0.48, 0.5}, {0.5, 0.49}},
            {{0.39, 0.61}, {0.38, 0.6}, {0.6, 0.39}}, {0.5, 0.5}},
        {{{0.29, 0.31}, {0.28, 0.3}, {0.3, 0.29}},
            {{0.19, 0.41}, {0.18, 0.4}, {0.4, 0.19}}, {0.3, 0.3}},
        {{{0.79, 0.81}, {0.78, 0.8}, {0.8, 0.79}},
            {{0.69, 0.91}, {0.68, 0.9}, {0.9, 0.69}}, {0.8, 0.8}}};
    write(stream, results);
    assertEqual(R"(Point|Left|Right
[0.5, 0.5]|[0.49, 0.51; 0.48, 0.5; 0.5, 0.49]|[0.39, 0.61; 0.38, 0.6; 0.6, 0.39]
[0.3, 0.3]|[0.29, 0.31; 0.28, 0.3; 0.3, 0.29]|[0.19, 0.41; 0.18, 0.4; 0.4, 0.19]
[0.8, 0.8]|[0.79, 0.81; 0.78, 0.8; 0.8, 0.79]|[0.69, 0.91; 0.68, 0.9; 0.9, 0.69]
)",
        stream.str());
}
}
