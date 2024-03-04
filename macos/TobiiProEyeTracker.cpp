#include "TobiiProEyeTracker.hpp"

#include <tobii_research.h>
#include <tobii_research_calibration.h>
#include <tobii_research_streams.h>

#include <gsl/gsl>

#include <thread>
#include <functional>
#include <optional>

namespace av_speech_in_noise {
static auto eyeTracker(TobiiResearchEyeTrackers *eyeTrackers)
    -> TobiiResearchEyeTracker * {
    return eyeTrackers == nullptr || eyeTrackers->count == 0U
        ? nullptr
        : eyeTrackers->eyetrackers[0];
}

auto TobiiProTracker::calibrator()
    -> eye_tracker_calibration::TobiiProCalibrator {
    return eye_tracker_calibration::TobiiProCalibrator{eyeTracker(eyeTrackers)};
}

auto TobiiProTracker::calibrationValidator()
    -> eye_tracker_calibration::validation::TobiiProValidator {
    return eye_tracker_calibration::validation::TobiiProValidator{
        eyeTracker(eyeTrackers)};
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

static auto gazePositionOnDisplayArea(const TobiiResearchEyeData &d)
    -> const TobiiResearchNormalizedPoint2D & {
    return d.gaze_point.position_on_display_area;
}

static auto size(const std::vector<BinocularGazeSample> &v) -> gsl::index {
    return v.size();
}

static void assign(Point3D &p, TobiiResearchPoint3D other) {
    p.x = other.x;
    p.y = other.y;
    p.z = other.z;
}

static void assign(Point2D &p, TobiiResearchNormalizedPoint2D other) {
    p.x = other.x;
    p.y = other.y;
}

auto TobiiProTracker::gazeSamples() -> BinocularGazeSamples {
    BinocularGazeSamples gazeSamples(head > 0 ? head - 1 : 0);
    for (gsl::index i{0}; i < size(gazeSamples); ++i) {
        at(gazeSamples, i).systemTime.microseconds =
            at(gazeData, i).system_time_stamp;
        assign(at(gazeSamples, i).left.position.relativeScreen,
            gazePositionOnDisplayArea(at(gazeData, i).left_eye));
        assign(at(gazeSamples, i).right.position.relativeScreen,
            gazePositionOnDisplayArea(at(gazeData, i).right_eye));
        assign(at(gazeSamples, i).left.position.relativeTrackbox,
            at(gazeData, i).left_eye.gaze_point.position_in_user_coordinates);
        assign(at(gazeSamples, i).right.position.relativeTrackbox,
            at(gazeData, i).right_eye.gaze_point.position_in_user_coordinates);
        at(gazeSamples, i).left.position.valid =
            at(gazeData, i).left_eye.gaze_point.validity ==
            TOBII_RESEARCH_VALIDITY_VALID;
        at(gazeSamples, i).right.position.valid =
            at(gazeData, i).right_eye.gaze_point.validity ==
            TOBII_RESEARCH_VALIDITY_VALID;

        assign(at(gazeSamples, i).left.origin.relativeTrackbox,
            at(gazeData, i).left_eye.gaze_origin.position_in_user_coordinates);
        assign(at(gazeSamples, i).right.origin.relativeTrackbox,
            at(gazeData, i).right_eye.gaze_origin.position_in_user_coordinates);
        at(gazeSamples, i).left.origin.valid =
            at(gazeData, i).left_eye.gaze_origin.validity ==
            TOBII_RESEARCH_VALIDITY_VALID;
        at(gazeSamples, i).right.origin.valid =
            at(gazeData, i).right_eye.gaze_origin.validity ==
            TOBII_RESEARCH_VALIDITY_VALID;
    }
    return gazeSamples;
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

namespace eye_tracker_calibration {
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

auto TobiiProCalibrator::results() -> Results {
    ComputeAndApply computeAndApply{eyetracker};
    return computeAndApply.results();
}

auto TobiiProCalibrator::computeAndApply() -> ComputeAndApply {
    return ComputeAndApply{eyetracker};
}

TobiiProCalibrator::ComputeAndApply::ComputeAndApply(
    TobiiResearchEyeTracker *eyetracker) {
    tobii_research_screen_based_calibration_compute_and_apply(
        eyetracker, &tobiiResult);
}

auto TobiiProCalibrator::ComputeAndApply::success() -> bool {
    return tobiiResult != nullptr &&
        tobiiResult->status == TOBII_RESEARCH_CALIBRATION_SUCCESS;
}

static auto point(const TobiiResearchNormalizedPoint2D &tobiiPoint2D) -> Point {
    return {tobiiPoint2D.x, tobiiPoint2D.y};
}

static auto convertValidity(TobiiResearchCalibrationEyeValidity validity)
    -> std::optional<SampleInfo> {
    switch (validity) {
    case TOBII_RESEARCH_CALIBRATION_EYE_VALIDITY_INVALID_AND_NOT_USED: {
        SampleInfo info{};
        info.used = false;
        info.valid = false;
        return std::make_optional(info);
    }
    case TOBII_RESEARCH_CALIBRATION_EYE_VALIDITY_VALID_BUT_NOT_USED: {
        SampleInfo info{};
        info.used = false;
        info.valid = true;
        return std::make_optional(info);
    }
    case TOBII_RESEARCH_CALIBRATION_EYE_VALIDITY_VALID_AND_USED: {
        SampleInfo info{};
        info.used = true;
        info.valid = true;
        return std::make_optional(info);
    }
    case TOBII_RESEARCH_CALIBRATION_EYE_VALIDITY_UNKNOWN: {
        return std::nullopt;
    }
    }
}

static void transform(
    gsl::span<const TobiiResearchCalibrationSample> tobiiSamples,
    std::vector<BinocularSample> &samples) {
    transform(tobiiSamples.begin(), tobiiSamples.end(),
        std::back_inserter(samples),
        [](const TobiiResearchCalibrationSample &tobiiSample) {
            BinocularSample s;
            s.left.point = point(tobiiSample.left_eye.position_on_display_area);
            s.right.point =
                point(tobiiSample.right_eye.position_on_display_area);
            s.left.info = convertValidity(tobiiSample.left_eye.validity);
            s.right.info = convertValidity(tobiiSample.right_eye.validity);
            return s;
        });
}

auto TobiiProCalibrator::ComputeAndApply::results() -> Results {
    if (tobiiResult == nullptr)
        return {};
    Results results;
    switch (tobiiResult->status) {
    case TOBII_RESEARCH_CALIBRATION_FAILURE:
        results.success = false;
        results.successfulEye = std::nullopt;
        break;
    case TOBII_RESEARCH_CALIBRATION_SUCCESS:
        results.success = true;
        results.successfulEye = std::nullopt;
        break;
    case TOBII_RESEARCH_CALIBRATION_SUCCESS_LEFT_EYE:
        results.success = false;
        results.successfulEye = std::make_optional(Eye::Left);
        break;
    case TOBII_RESEARCH_CALIBRATION_SUCCESS_RIGHT_EYE:
        results.success = false;
        results.successfulEye = std::make_optional(Eye::Right);
        break;
    }
    results.pointResults.resize(tobiiResult->calibration_point_count);
    const gsl::span<const TobiiResearchCalibrationPoint> tobiiPoints{
        tobiiResult->calibration_points, tobiiResult->calibration_point_count};
    transform(tobiiPoints.begin(), tobiiPoints.end(),
        results.pointResults.begin(),
        [](const TobiiResearchCalibrationPoint &tobiiPoint) {
            PointResult pointResult;
            const gsl::span<const TobiiResearchCalibrationSample> tobiiSamples{
                tobiiPoint.calibration_samples,
                tobiiPoint.calibration_sample_count};
            transform(tobiiSamples, pointResult.samples);
            pointResult.point = point(tobiiPoint.position_on_display_area);
            return pointResult;
        });

    return results;
}

TobiiProCalibrator::ComputeAndApply::~ComputeAndApply() {
    tobii_research_free_screen_based_calibration_result(tobiiResult);
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

void TobiiProValidator::collect(Point p) {
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
    if (result_ == nullptr)
        return result;
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
