
#include "TobiiProEyeTracker.hpp"
#include <gsl/gsl>
#include <thread>

namespace av_speech_in_noise::eye_tracking {
static auto eyeTracker(TobiiResearchEyeTrackers *eyeTrackers)
    -> TobiiResearchEyeTracker * {
    return eyeTrackers == nullptr || eyeTrackers->count == 0U
        ? nullptr
        : eyeTrackers->eyetrackers[0];
}

auto TobiiProTracker::calibrator() -> calibration::TobiiProCalibrator {
    return calibration::TobiiProCalibrator{eyeTracker(eyeTrackers)};
}

auto TobiiProTracker::calibrationValidator()
    -> calibration::validation::TobiiProValidator {
    return calibration::validation::TobiiProValidator{eyeTracker(eyeTrackers)};
}

TobiiProTracker::TobiiProTracker() {
    tobii_research_find_all_eyetrackers(&eyeTrackers);
}

TobiiProTracker::~TobiiProTracker() {
    tobii_research_free_eyetrackers(eyeTrackers);
}

void TobiiProTracker::allocateRecordingTimeSeconds(double seconds) {
    float gaze_output_frequency_Hz{};
    tobii_research_get_gaze_output_frequency(
        eyeTracker(eyeTrackers), &gaze_output_frequency_Hz);
    gazeData.resize(std::ceil(gaze_output_frequency_Hz * seconds) + 1);
    head = 0;
}

void TobiiProTracker::start() {
    tobii_research_subscribe_to_gaze_data(
        eyeTracker(eyeTrackers), gaze_data_callback, this);
}

void TobiiProTracker::stop() {
    tobii_research_unsubscribe_from_gaze_data(
        eyeTracker(eyeTrackers), gaze_data_callback);
}

void TobiiProTracker::gaze_data_callback(
    TobiiResearchGazeData *gaze_data, void *self) {
    static_cast<TobiiProTracker *>(self)->gazeDataReceived(gaze_data);
}

void TobiiProTracker::gazeDataReceived(TobiiResearchGazeData *gaze_data) {
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

auto TobiiProTracker::gazeSamples() -> BinocularGazeSamples {
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

auto TobiiProTracker::currentSystemTime() -> EyeTrackerSystemTime {
    EyeTrackerSystemTime currentSystemTime{};
    int64_t microseconds = 0;
    tobii_research_get_system_time_stamp(&microseconds);
    currentSystemTime.microseconds = microseconds;
    return currentSystemTime;
}

TobiiProTracker::Address::Address(TobiiResearchEyeTracker *eyetracker) {
    tobii_research_get_address(eyetracker, &address);
}

TobiiProTracker::Address::~Address() { tobii_research_free_string(address); }

namespace calibration {
TobiiProCalibrator::TobiiProCalibrator(TobiiResearchEyeTracker *eyetracker)
    : eyetracker{eyetracker} {}

void TobiiProCalibrator::acquire() {
    tobii_research_screen_based_calibration_enter_calibration_mode(eyetracker);
}

void TobiiProCalibrator::release() {
    tobii_research_screen_based_calibration_leave_calibration_mode(eyetracker);
}

void TobiiProCalibrator::discard(Point p) {
    tobii_research_screen_based_calibration_discard_data(eyetracker, p.x, p.y);
}

void TobiiProCalibrator::collect(Point p) {
    tobii_research_screen_based_calibration_collect_data(eyetracker, p.x, p.y);
}

auto TobiiProCalibrator::results() -> std::vector<Result> {
    ComputeAndApply computeAndApply{eyetracker};
    return computeAndApply.results();
}

auto TobiiProCalibrator::computeAndApply() -> ComputeAndApply {
    return ComputeAndApply{eyetracker};
}

TobiiProCalibrator::ComputeAndApply::ComputeAndApply(
    TobiiResearchEyeTracker *eyetracker) {
    tobii_research_screen_based_calibration_compute_and_apply(
        eyetracker, &result);
}

auto TobiiProCalibrator::ComputeAndApply::success() -> bool {
    return result != nullptr &&
        result->status == TOBII_RESEARCH_CALIBRATION_SUCCESS;
}

auto TobiiProCalibrator::ComputeAndApply::results() -> std::vector<Result> {
    if (result == nullptr)
        return {};
    std::vector<Result> results{result->calibration_point_count};
    const gsl::span<TobiiResearchCalibrationPoint> calibrationPoints{
        result->calibration_points, result->calibration_point_count};
    std::transform(calibrationPoints.begin(), calibrationPoints.end(),
        std::back_inserter(results),
        [](const TobiiResearchCalibrationPoint &p) {
            Result transformedResult;
            const gsl::span<TobiiResearchCalibrationSample> calibrationSamples{
                p.calibration_samples, p.calibration_sample_count};
            std::transform(calibrationSamples.begin(), calibrationSamples.end(),
                std::back_inserter(transformedResult.leftEyeMappedPoints),
                [](const TobiiResearchCalibrationSample &sample) {
                    return Point{sample.left_eye.position_on_display_area.x,
                        sample.left_eye.position_on_display_area.y};
                });
            std::transform(calibrationSamples.begin(), calibrationSamples.end(),
                std::back_inserter(transformedResult.rightEyeMappedPoints),
                [](const TobiiResearchCalibrationSample &sample) {
                    return Point{sample.right_eye.position_on_display_area.x,
                        sample.right_eye.position_on_display_area.y};
                });
            transformedResult.point = {
                p.position_on_display_area.x, p.position_on_display_area.y};
            return transformedResult;
        });

    return results;
}

TobiiProCalibrator::ComputeAndApply::~ComputeAndApply() {
    tobii_research_free_screen_based_calibration_result(result);
}

namespace validation {
TobiiProValidator::TobiiProValidator(TobiiResearchEyeTracker *eyetracker) {
    TobiiProTracker::Address address{eyetracker};
    if (tobii_research_screen_based_calibration_validation_init_default(
            address.get(), &validator) ==
        CALIBRATION_VALIDATION_STATUS_INVALID_EYETRACKER)
        validator = nullptr;
}

TobiiProValidator::~TobiiProValidator() {
    if (validator != nullptr)
        tobii_research_screen_based_calibration_validation_destroy(validator);
}

void TobiiProValidator::acquire() {
    if (validator != nullptr)
        tobii_research_screen_based_calibration_validation_enter_validation_mode(
            validator);
}

void TobiiProValidator::collect(calibration::Point p) {
    TobiiResearchNormalizedPoint2D point{p.x, p.y};
    if (validator != nullptr)
        tobii_research_screen_based_calibration_validation_start_collecting_data(
            validator, &point);
    while ((validator != nullptr) &&
        (tobii_research_screen_based_calibration_validation_is_collecting_data(
             validator) != 0))
        std::this_thread::sleep_for(std::chrono::milliseconds{100});
}

auto TobiiProValidator::resultAdapter() -> ResultAdapter {
    return ResultAdapter{validator};
}

void TobiiProValidator::release() {
    if (validator != nullptr)
        tobii_research_screen_based_calibration_validation_leave_validation_mode(
            validator);
}

TobiiProValidator::ResultAdapter::ResultAdapter(
    CalibrationValidator *validator) {
    if (validator != nullptr)
        tobii_research_screen_based_calibration_validation_compute(
            validator, &result_);
}

TobiiProValidator::ResultAdapter::~ResultAdapter() {
    tobii_research_screen_based_calibration_validation_destroy_result(result_);
}

auto TobiiProValidator::ResultAdapter::result() -> Result {
    Result result{};
    result.left.errorOfMeanGaze.degrees = result_->average_accuracy_left;
    result.left.standardDeviationFromTheMeanGaze.degrees =
        result_->average_precision_left;
    result.right.errorOfMeanGaze.degrees = result_->average_accuracy_right;
    result.right.standardDeviationFromTheMeanGaze.degrees =
        result_->average_precision_right;
    return result;
}

auto TobiiProValidator::result() -> Result {
    ResultAdapter adapter{validator};
    return adapter.result();
}
}
}

TobiiProTracker::CalibrationData::CalibrationData(
    TobiiResearchEyeTracker *eyeTracker) {
    tobii_research_retrieve_calibration_data(eyeTracker, &data);
}

void TobiiProTracker::CalibrationData::write(std::ostream &stream) {
    if (data != nullptr)
        stream.write(reinterpret_cast<const char *>(data->data), data->size);
}

TobiiProTracker::CalibrationData::~CalibrationData() {
    tobii_research_free_calibration_data(data);
}

void TobiiProTracker::write(std::ostream &stream) {
    CalibrationData data{eyeTracker(eyeTrackers)};
    data.write(stream);
}
}
