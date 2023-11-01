#include <av-speech-in-noise/core/EyeTrackerCalibration.hpp>

#include <ostream>
#include <vector>

namespace av_speech_in_noise {
void write(std::ostream &stream, const eye_tracker_calibration::Point &p) {
    stream << p.x;
    stream << ", ";
    stream << p.y;
}

void write(std::ostream &stream,
    const std::vector<eye_tracker_calibration::Point> &v) {
    stream << '[';
    auto first{true};
    for (const auto p : v) {
        if (!first) {
            stream << "; ";
        }
        write(stream, p);
        first = false;
    }
    stream << ']';
}

void write(
    std::ostream &stream, std::vector<eye_tracker_calibration::Result> &v) {
    stream << "Point|Left|Right\n";
    for (const auto &result : v) {
        stream << '[';
        write(stream, result.point);
        stream << ']';

        stream << '|';
        write(stream, result.leftEyeMappedPoints);
        stream << '|';
        write(stream, result.rightEyeMappedPoints);
        stream << '\n';
    }
}
}

#include "assert-utility.hpp"

#include <gtest/gtest.h>

#include <sstream>

namespace av_speech_in_noise {
class TbdTests : public ::testing::Test {
  protected:
};

#define TBD_TEST(a) TEST_F(TbdTests, a)

TBD_TEST(tbd) {
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
