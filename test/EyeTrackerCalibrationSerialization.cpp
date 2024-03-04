#include "assert-utility.hpp"

#include <av-speech-in-noise/core/EyeTrackerCalibration.hpp>

#include <gtest/gtest.h>

#include <sstream>

namespace av_speech_in_noise::eye_tracker_calibration {
class EyeTrackerCalibrationSerializationTests : public ::testing::Test {};

#define EYE_TRACKER_CALIBRATION_SERIALIZATION_TEST(a)                          \
    TEST_F(EyeTrackerCalibrationSerializationTests, a)

EYE_TRACKER_CALIBRATION_SERIALIZATION_TEST(writesResults) {
    std::stringstream stream;

    Results results;
    BinocularSample s1{};
    s1.left.point = {0.49F, 0.51F};
    s1.right.point = {0.39F, 0.61F};
    BinocularSample s2{};
    s2.left.point = {0.48F, 0.5F};
    s2.right.point = {0.38F, 0.6F};
    BinocularSample s3{};
    s3.left.point = {0.5F, 0.49F};
    s3.right.point = {0.6F, 0.39F};
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
    assertEqual(R"(Point|Left|Right
[0.5, 0.5]|[0.49, 0.51; 0.48, 0.5; 0.5, 0.49]|[0.39, 0.61; 0.38, 0.6; 0.6, 0.39]
[0.3, 0.3]|[0.29, 0.31; 0.28, 0.3; 0.3, 0.29]|[0.19, 0.41; 0.18, 0.4; 0.4, 0.19]
[0.8, 0.8]|[0.79, 0.81; 0.78, 0.8; 0.8, 0.79]|[0.69, 0.91; 0.68, 0.9; 0.9, 0.69]
)",
        stream.str());
}
}
