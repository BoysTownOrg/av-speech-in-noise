#include "assert-utility.hpp"

#include <av-speech-in-noise/core/EyeTrackerCalibration.hpp>

#include <gtest/gtest.h>

#include <optional>
#include <sstream>

namespace av_speech_in_noise::eye_tracker_calibration {
class EyeTrackerCalibrationSerializationTests : public ::testing::Test {};

#define EYE_TRACKER_CALIBRATION_SERIALIZATION_TEST(a)                          \
    TEST_F(EyeTrackerCalibrationSerializationTests, a)

EYE_TRACKER_CALIBRATION_SERIALIZATION_TEST(writesResults) {
    std::stringstream stream;

    SampleInfo info{};
    Results results;
    BinocularSample s1{};
    s1.left.point = {0.49F, 0.51F};
    info.used = true;
    info.valid = true;
    s1.left.info = std::make_optional(info);
    s1.right.point = {0.39F, 0.61F};
    info.used = false;
    info.valid = true;
    s1.right.info = std::make_optional(info);
    BinocularSample s2{};
    s2.left.point = {0.48F, 0.5F};
    info.used = false;
    info.valid = true;
    s2.left.info = std::make_optional(info);
    s2.right.point = {0.38F, 0.6F};
    info.used = false;
    info.valid = false;
    s2.right.info = std::make_optional(info);
    BinocularSample s3{};
    s3.left.point = {0.5F, 0.49F};
    info.used = true;
    info.valid = false;
    s3.left.info = std::make_optional(info);
    s3.right.point = {0.6F, 0.39F};
    info.used = false;
    info.valid = false;
    s3.right.info = std::make_optional(info);
    PointResult r1{};
    r1.samples = {s1, s2, s3};
    r1.point = {0.5F, 0.5F};
    results.pointResults.push_back(r1);

    BinocularSample s4{};
    s4.left.point = {0.29F, 0.31F};
    s4.right.point = {0.19F, 0.41F};
    BinocularSample s5{};
    s5.left.point = {0.28F, 0.3F};
    s5.right.point = {0.18F, 0.4F};
    BinocularSample s6{};
    s6.left.point = {0.3F, 0.29F};
    s6.right.point = {0.4F, 0.19F};
    PointResult r2{};
    r2.samples = {s4, s5, s6};
    r2.point = {0.3F, 0.3F};
    results.pointResults.push_back(r2);

    BinocularSample s7{};
    s7.left.point = {0.79F, 0.81F};
    s7.right.point = {0.69F, 0.91F};
    BinocularSample s8{};
    s8.left.point = {0.78F, 0.8F};
    s8.right.point = {0.68F, 0.9F};
    BinocularSample s9{};
    s9.left.point = {0.8F, 0.79F};
    s9.right.point = {0.9F, 0.69F};
    PointResult r3{};
    r3.samples = {s7, s8, s9};
    r3.point = {0.8F, 0.8F};
    results.pointResults.push_back(r3);
    write(stream, results);
    assertEqual(R"([0.5, 0.5]
  Left
    [0.49, 0.51] - used and valid
    [0.48, 0.5] - not used but valid
    [0.5, 0.49] - used but invalid
  Right
    [0.39, 0.61] - not used but valid
    [0.38, 0.6] - not used and invalid
    [0.6, 0.39] - not used and invalid
[0.3, 0.3]
  Left
    [0.29, 0.31] - ???
    [0.28, 0.3] - ???
    [0.3, 0.29] - ???
  Right
    [0.19, 0.41] - ???
    [0.18, 0.4] - ???
    [0.4, 0.19] - ???
[0.8, 0.8]
  Left
    [0.79, 0.81] - ???
    [0.78, 0.8] - ???
    [0.8, 0.79] - ???
  Right
    [0.69, 0.91] - ???
    [0.68, 0.9] - ???
    [0.9, 0.69] - ???
)",
        stream.str());
}
}
