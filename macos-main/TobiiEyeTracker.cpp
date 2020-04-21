#include "TobiiEyeTracker.hpp"
#include <tobii_research.h>
#include <tobii_research_streams.h>
#include <dlfcn.h>
#include <gsl/gsl>

namespace av_speech_in_noise {
static auto eyeTracker(TobiiResearchEyeTrackers *eyeTrackers)
    -> TobiiResearchEyeTracker * {
    return eyeTrackers == nullptr || eyeTrackers->count == 0U
        ? nullptr
        : eyeTrackers->eyetrackers[0];
}

TobiiEyeTracker::TobiiEyeTracker()
    : library{dlopen(
          "/usr/local/lib/tobii_research/libtobii_research.dylib", RTLD_LAZY)} {
    auto tobii_research_find_all_eyetrackers_ =
        reinterpret_cast<decltype(&tobii_research_find_all_eyetrackers)>(
            dlsym(library, "tobii_research_find_all_eyetrackers"));
    if (tobii_research_find_all_eyetrackers_ != nullptr)
        tobii_research_find_all_eyetrackers_(&eyeTrackers);
}

TobiiEyeTracker::~TobiiEyeTracker() {
    auto tobii_research_free_eyetrackers_ =
        reinterpret_cast<decltype(&tobii_research_free_eyetrackers)>(
            dlsym(library, "tobii_research_free_eyetrackers"));
    if (tobii_research_free_eyetrackers_ != nullptr)
        tobii_research_free_eyetrackers_(eyeTrackers);
    dlclose(library);
}

void TobiiEyeTracker::allocateRecordingTimeSeconds(double seconds) {
    float gaze_output_frequency_Hz{};
    auto tobii_research_get_gaze_output_frequency_ =
        reinterpret_cast<decltype(&tobii_research_get_gaze_output_frequency)>(
            dlsym(library, "tobii_research_get_gaze_output_frequency"));
    if (tobii_research_get_gaze_output_frequency_ != nullptr)
        tobii_research_get_gaze_output_frequency_(
            eyeTracker(eyeTrackers), &gaze_output_frequency_Hz);
    gazeData.resize(std::ceil(gaze_output_frequency_Hz * seconds) + 1);
    head = 0;
}

void TobiiEyeTracker::start() {
    auto tobii_research_subscribe_to_gaze_data_ =
        reinterpret_cast<decltype(&tobii_research_subscribe_to_gaze_data)>(
            dlsym(library, "tobii_research_subscribe_to_gaze_data"));
    if (tobii_research_subscribe_to_gaze_data_ != nullptr)
        tobii_research_subscribe_to_gaze_data_(
            eyeTracker(eyeTrackers), gaze_data_callback, this);
}

void TobiiEyeTracker::stop() {
    auto tobii_research_unsubscribe_from_gaze_data_ =
        reinterpret_cast<decltype(&tobii_research_unsubscribe_from_gaze_data)>(
            dlsym(library, "tobii_research_unsubscribe_from_gaze_data"));
    if (tobii_research_unsubscribe_from_gaze_data_ != nullptr)
        tobii_research_unsubscribe_from_gaze_data_(
            eyeTracker(eyeTrackers), gaze_data_callback);
}

void TobiiEyeTracker::gaze_data_callback(
    TobiiResearchGazeData *gaze_data, void *self) {
    static_cast<TobiiEyeTracker *>(self)->gazeDataReceived(gaze_data);
}

void TobiiEyeTracker::gazeDataReceived(TobiiResearchGazeData *gaze_data) {
    if (head < gazeData.size())
        gazeData.at(head++) = *gaze_data;
}

static auto at(std::vector<BinocularGazeSample> &b, gsl::index i)
    -> BinocularGazeSample & {
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

static auto x(const TobiiResearchNormalizedPoint2D &p) -> float { return p.x; }

static auto y(const TobiiResearchNormalizedPoint2D &p) -> float { return p.y; }

static auto leftEyeGaze(std::vector<BinocularGazeSample> &b, gsl::index i)
    -> EyeGaze & {
    return at(b, i).left;
}

static auto rightEyeGaze(BinocularGazeSamples &b, gsl::index i) -> EyeGaze & {
    return at(b, i).right;
}

static auto x(EyeGaze &p) -> float & { return p.x; }

static auto y(EyeGaze &p) -> float & { return p.y; }

static auto size(const std::vector<BinocularGazeSample> &v) -> gsl::index {
    return v.size();
}

auto TobiiEyeTracker::gazeSamples() -> BinocularGazeSamples {
    BinocularGazeSamples gazeSamples_(head > 0 ? head - 1 : 0);
    for (gsl::index i{0}; i < size(gazeSamples_); ++i) {
        at(gazeSamples_, i).systemTime.microseconds =
            at(gazeData, i).system_time_stamp;
        x(leftEyeGaze(gazeSamples_, i)) = x(leftEyeGaze(gazeData, i));
        y(leftEyeGaze(gazeSamples_, i)) = y(leftEyeGaze(gazeData, i));
        x(rightEyeGaze(gazeSamples_, i)) = x(rightEyeGaze(gazeData, i));
        y(rightEyeGaze(gazeSamples_, i)) = y(rightEyeGaze(gazeData, i));
    }
    return gazeSamples_;
}

auto TobiiEyeTracker::currentSystemTime() -> EyeTrackerSystemTime {
    EyeTrackerSystemTime currentSystemTime{};
    int64_t microseconds = 0;
    auto tobii_research_get_system_time_stamp_ =
        reinterpret_cast<decltype(&tobii_research_get_system_time_stamp)>(
            dlsym(library, "tobii_research_get_system_time_stamp"));
    if (tobii_research_get_system_time_stamp_ != nullptr)
        tobii_research_get_system_time_stamp_(&microseconds);
    currentSystemTime.microseconds = microseconds;
    return currentSystemTime;
}
}
