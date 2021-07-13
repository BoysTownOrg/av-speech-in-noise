#ifndef AV_SPEECH_IN_NOISE_MACOS_TOBIIPROEYETRACKER_HPP_
#define AV_SPEECH_IN_NOISE_MACOS_TOBIIPROEYETRACKER_HPP_

#include <av-speech-in-noise/core/RecognitionTestModel.hpp>
#include <av-speech-in-noise/core/EyeTrackerCalibration.hpp>

#include <screen_based_calibration_validation.h>
#include <tobii_research.h>
#include <tobii_research_calibration.h>
#include <tobii_research_eyetracker.h>
#include <tobii_research_streams.h>

#include <gsl/gsl>

#include <vector>
#include <cstddef>
#include <ostream>

namespace av_speech_in_noise {
namespace eye_tracker_calibration {
class TobiiProCalibrator : public Calibrator {
  public:
    explicit TobiiProCalibrator(TobiiResearchEyeTracker *);
    void acquire() override;
    void release() override;
    void discard(Point) override;
    void collect(Point) override;
    auto results() -> std::vector<Result> override;

    class ComputeAndApply;

    auto computeAndApply() -> ComputeAndApply;

    class ComputeAndApply {
      public:
        explicit ComputeAndApply(TobiiResearchEyeTracker *);
        auto success() -> bool;
        auto results() -> std::vector<Result>;
        ~ComputeAndApply();
        ComputeAndApply(const ComputeAndApply &) = delete;
        ComputeAndApply(ComputeAndApply &&) = delete;
        auto operator=(const ComputeAndApply &) -> ComputeAndApply & = delete;
        auto operator=(ComputeAndApply &&) -> ComputeAndApply & = delete;

      private:
        TobiiResearchCalibrationResult *tobiiResult{};
    };

  private:
    TobiiResearchEyeTracker *eyetracker{};
};

namespace validation {
class TobiiProValidator : public Validator {
  public:
    explicit TobiiProValidator(TobiiResearchEyeTracker *eyetracker);
    void acquire() override;
    void collect(Point) override;
    auto result() -> Result override;
    void release() override;
    ~TobiiProValidator() override;
    TobiiProValidator(const TobiiProValidator &) = delete;
    TobiiProValidator(TobiiProValidator &&) = delete;
    auto operator=(const TobiiProValidator &) -> TobiiProValidator & = delete;
    auto operator=(TobiiProValidator &&) -> TobiiProValidator & = delete;

    class ResultAdapter;

    auto resultAdapter() -> ResultAdapter;

    class ResultAdapter {
      public:
        explicit ResultAdapter(CalibrationValidator *validator);
        auto result() -> Result;
        ~ResultAdapter();
        ResultAdapter(const ResultAdapter &) = delete;
        ResultAdapter(ResultAdapter &&) = delete;
        auto operator=(const ResultAdapter &) -> ResultAdapter & = delete;
        auto operator=(ResultAdapter &&) -> ResultAdapter & = delete;

      private:
        CalibrationValidationResult *result_{};
    };

  private:
    CalibrationValidator *validator{};
};
}
}

class TobiiProTracker : public EyeTracker {
  public:
    TobiiProTracker();
    ~TobiiProTracker() override;
    TobiiProTracker(const TobiiProTracker &) = delete;
    TobiiProTracker(TobiiProTracker &&) = delete;
    auto operator=(const TobiiProTracker &) -> TobiiProTracker & = delete;
    auto operator=(TobiiProTracker &&) -> TobiiProTracker & = delete;
    void allocateRecordingTimeSeconds(double s) override;
    void start() override;
    void stop() override;
    auto gazeSamples() -> BinocularGazeSamples override;
    auto currentSystemTime() -> EyeTrackerSystemTime override;
    void write(std::ostream &) override;

    auto calibrator() -> eye_tracker_calibration::TobiiProCalibrator;
    auto calibrationValidator()
        -> eye_tracker_calibration::validation::TobiiProValidator;

    class Address {
      public:
        explicit Address(TobiiResearchEyeTracker *);
        auto get() -> const char * { return address; }
        ~Address();
        Address(const Address &) = delete;
        Address(Address &&) = delete;
        auto operator=(const Address &) -> Address & = delete;
        auto operator=(Address &&) -> Address & = delete;

      private:
        char *address{};
    };

    class CalibrationData {
      public:
        explicit CalibrationData(TobiiResearchEyeTracker *);
        void write(std::ostream &);
        ~CalibrationData();
        CalibrationData(const CalibrationData &) = delete;
        CalibrationData(CalibrationData &&) = delete;
        auto operator=(const CalibrationData &) -> CalibrationData & = delete;
        auto operator=(CalibrationData &&) -> CalibrationData & = delete;

      private:
        TobiiResearchCalibrationData *data{};
    };

  private:
    static void gaze_data_callback(
        TobiiResearchGazeData *gaze_data, void *self);
    void gazeDataReceived(TobiiResearchGazeData *gaze_data);

    std::vector<TobiiResearchGazeData> gazeData{};
    TobiiResearchEyeTrackers *eyeTrackers{};
    std::size_t head{};
};
}

#endif
