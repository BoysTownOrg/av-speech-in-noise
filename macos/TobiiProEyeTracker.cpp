
#include "TobiiProEyeTracker.hpp"
#include <gsl/gsl>

namespace av_speech_in_noise {
static auto eyeTracker(TobiiResearchEyeTrackers *eyeTrackers)
    -> TobiiResearchEyeTracker * {
    return eyeTrackers == nullptr || eyeTrackers->count == 0U
        ? nullptr
        : eyeTrackers->eyetrackers[0];
}

auto TobiiEyeTracker::calibration() -> Calibration {
    return Calibration{eyeTracker(eyeTrackers)};
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
    gazeData.resize(std::ceil(gaze_output_frequency_Hz * seconds) + 1);
    head = 0;
}

void TobiiEyeTracker::start() {
    tobii_research_subscribe_to_gaze_data(
        eyeTracker(eyeTrackers), gaze_data_callback, this);
}

void TobiiEyeTracker::stop() {
    tobii_research_unsubscribe_from_gaze_data(
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
    tobii_research_get_system_time_stamp(&microseconds);
    currentSystemTime.microseconds = microseconds;
    return currentSystemTime;
}

TobiiEyeTracker::Address::Address(TobiiResearchEyeTracker *eyetracker) {
    tobii_research_get_address(eyetracker, &address);
}

TobiiEyeTracker::Address::~Address() { tobii_research_free_string(address); }

TobiiEyeTracker::Calibration::Calibration(TobiiResearchEyeTracker *eyetracker)
    : eyetracker{eyetracker} {}

void TobiiEyeTracker::Calibration::acquire() {
    tobii_research_screen_based_calibration_enter_calibration_mode(eyetracker);
}

void TobiiEyeTracker::Calibration::release() {
    tobii_research_screen_based_calibration_leave_calibration_mode(eyetracker);
}

void TobiiEyeTracker::Calibration::discard(eye_tracker_calibration::Point p) {
    tobii_research_screen_based_calibration_discard_data(eyetracker, p.x, p.y);
}

void TobiiEyeTracker::Calibration::collect(eye_tracker_calibration::Point p) {
    tobii_research_screen_based_calibration_collect_data(eyetracker, p.x, p.y);
}

auto TobiiEyeTracker::Calibration::results()
    -> std::vector<eye_tracker_calibration::Result> {
    ComputeAndApply computeAndApply{eyetracker};
    return computeAndApply.results();
}

auto TobiiEyeTracker::Calibration::computeAndApply() -> ComputeAndApply {
    return ComputeAndApply{eyetracker};
}

TobiiEyeTracker::Calibration::ComputeAndApply::ComputeAndApply(
    TobiiResearchEyeTracker *eyetracker) {
    tobii_research_screen_based_calibration_compute_and_apply(
        eyetracker, &result);
}

auto TobiiEyeTracker::Calibration::ComputeAndApply::success() -> bool {
    return result != nullptr &&
        result->status == TOBII_RESEARCH_CALIBRATION_SUCCESS;
}

auto TobiiEyeTracker::Calibration::ComputeAndApply::results()
    -> std::vector<eye_tracker_calibration::Result> {
    if (result == nullptr)
        return {};
    std::vector<eye_tracker_calibration::Result> results{
        result->calibration_point_count};
    const gsl::span<TobiiResearchCalibrationPoint> calibrationPoints{
        result->calibration_points, result->calibration_point_count};
    std::transform(calibrationPoints.begin(), calibrationPoints.end(),
        std::back_inserter(results),
        [](const TobiiResearchCalibrationPoint &p) {
            eye_tracker_calibration::Result transformedResult;
            const gsl::span<TobiiResearchCalibrationSample> calibrationSamples{
                p.calibration_samples, p.calibration_sample_count};
            std::transform(calibrationSamples.begin(), calibrationSamples.end(),
                std::back_inserter(transformedResult.leftEyeMappedPoints),
                [](const TobiiResearchCalibrationSample &sample) {
                    return eye_tracker_calibration::Point{
                        sample.left_eye.position_on_display_area.x,
                        sample.left_eye.position_on_display_area.y};
                });
            std::transform(calibrationSamples.begin(), calibrationSamples.end(),
                std::back_inserter(transformedResult.rightEyeMappedPoints),
                [](const TobiiResearchCalibrationSample &sample) {
                    return eye_tracker_calibration::Point{
                        sample.right_eye.position_on_display_area.x,
                        sample.right_eye.position_on_display_area.y};
                });
            transformedResult.point = {
                p.position_on_display_area.x, p.position_on_display_area.y};
            return transformedResult;
        });

    return results;
}

TobiiEyeTracker::Calibration::ComputeAndApply::~ComputeAndApply() {
    tobii_research_free_screen_based_calibration_result(result);
}

TobiiEyeTracker::CalibrationData::CalibrationData(
    TobiiResearchEyeTracker *eyeTracker) {
    tobii_research_retrieve_calibration_data(eyeTracker, &data);
}

void TobiiEyeTracker::CalibrationData::write(std::ostream &stream) {
    if (data != nullptr)
        stream.write(reinterpret_cast<const char *>(data->data), data->size);
}

TobiiEyeTracker::CalibrationData::~CalibrationData() {
    tobii_research_free_calibration_data(data);
}

void TobiiEyeTracker::write(std::ostream &stream) {
    CalibrationData data{eyeTracker(eyeTrackers)};
    data.write(stream);
}
}
