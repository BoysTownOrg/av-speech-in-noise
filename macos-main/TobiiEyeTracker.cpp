#include "TobiiEyeTracker.hpp"
#include "recognition-test/RecognitionTestModel.hpp"
#include "tobii_research.h"
#include "tobii_research_streams.h"
#include <gsl/gsl>
#include <cmath>
#include <iostream>

namespace av_speech_in_noise {
static auto eyeTracker(TobiiResearchEyeTrackers *eyeTrackers)
    -> TobiiResearchEyeTracker * {
    return eyeTrackers == nullptr || eyeTrackers->count == 0U
        ? nullptr
        : eyeTrackers->eyetrackers[0];
}

TobiiEyeTracker::TobiiEyeTracker() {
    tobii_research_find_all_eyetrackers(&eyeTrackers);
}

TobiiEyeTracker::~TobiiEyeTracker() {
    tobii_research_free_eyetrackers(eyeTrackers);
}

void TobiiEyeTracker::allocateRecordingTimeSeconds(double seconds) {
    float gaze_output_frequency_Hz{};
    tobii_research_get_gaze_output_frequency(
        eyeTracker(eyeTrackers), &gaze_output_frequency_Hz);
    gazeData.resize(gsl::narrow<std::size_t>(
        std::ceil(gaze_output_frequency_Hz * seconds)));
    head = 0;
}

void TobiiEyeTracker::start() {
    tobii_research_subscribe_to_gaze_data(
        eyeTracker(eyeTrackers), gaze_data_callback, this);
}

void TobiiEyeTracker::stop() {
    tobii_research_unsubscribe_from_gaze_data(
        eyeTracker(eyeTrackers), gaze_data_callback);
    for (auto d : gazeData) {
        std::cout << "system time: " << d.system_time_stamp << '\n';
        std::cout << "x (left eye): "
                  << d.left_eye.gaze_point.position_on_display_area.x << '\n';
        std::cout << "y (left eye): "
                  << d.left_eye.gaze_point.position_on_display_area.y << '\n';
    }
}

void TobiiEyeTracker::gaze_data_callback(
    TobiiResearchGazeData *gaze_data, void *self) {
    static_cast<TobiiEyeTracker *>(self)->gazeDataReceived(gaze_data);
}

void TobiiEyeTracker::gazeDataReceived(TobiiResearchGazeData *gaze_data) {
    if (head < gazeData.size())
        gazeData.at(head++) = *gaze_data;
}

static auto at(std::vector<BinocularGazes> &b, gsl::index i)
    -> BinocularGazes & {
    return b.at(i);
}

static auto at(const std::vector<TobiiResearchGazeData> &b, gsl::index i)
    -> const TobiiResearchGazeData & {
    return b.at(i);
}

static auto eyeGaze(const TobiiResearchEyeData &d)
    -> const TobiiResearchNormalizedPoint2D & {
    return d.gaze_point.position_on_display_area;
}

static auto leftEyeGaze(const std::vector<TobiiResearchGazeData> &gaze,
    gsl::index i) -> const TobiiResearchNormalizedPoint2D & {
    return eyeGaze(at(gaze, i).left_eye);
}

static auto rightEyeGaze(const std::vector<TobiiResearchGazeData> &gaze,
    gsl::index i) -> const TobiiResearchNormalizedPoint2D & {
    return eyeGaze(at(gaze, i).right_eye);
}

static auto x(const TobiiResearchNormalizedPoint2D &p) -> float {
    return p.x;
}

static auto y(const TobiiResearchNormalizedPoint2D &p) -> float {
    return p.y;
}

auto TobiiEyeTracker::gazes() -> std::vector<BinocularGazes> {
    std::vector<BinocularGazes> gazes_(gazeData.size());
    for (gsl::index i{0}; i < gazes_.size(); ++i) {
        at(gazes_, i).systemTimeMilliseconds =
            at(gazeData, i).system_time_stamp;
        at(gazes_, i).left.x = x(leftEyeGaze(gazeData, i));
        at(gazes_, i).left.y = y(leftEyeGaze(gazeData, i));
        at(gazes_, i).right.x = x(rightEyeGaze(gazeData, i));
        at(gazes_, i).right.y = y(rightEyeGaze(gazeData, i));
    }
    return gazes_;
}
}
