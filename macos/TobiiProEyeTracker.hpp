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

namespace av_speech_in_noise::eye_tracking {
class TobiiProEyeTracker : public Tracker {
  public:
    TobiiProEyeTracker();
    ~TobiiProEyeTracker() override;
    void allocateRecordingTimeSeconds(double s) override;
    void start() override;
    void stop() override;
    auto gazeSamples() -> BinocularGazeSamples override;
    auto currentSystemTime() -> EyeTrackerSystemTime override;
    void write(std::ostream &) override;

    class Calibration;
    class CalibrationValidation;

    auto calibration() -> Calibration;

    class Address {
      public:
        explicit Address(TobiiResearchEyeTracker *);
        auto get() -> const char * { return address; }
        ~Address();

      private:
        char *address{};
    };

    class CalibrationData {
      public:
        explicit CalibrationData(TobiiResearchEyeTracker *);
        void write(std::ostream &);
        ~CalibrationData();

      private:
        TobiiResearchCalibrationData *data{};
    };

    class Calibration : public eye_tracker_calibration::EyeTrackerCalibrator {
      public:
        explicit Calibration(TobiiResearchEyeTracker *);
        void acquire() override;
        void release() override;
        void discard(eye_tracker_calibration::Point) override;
        void collect(eye_tracker_calibration::Point) override;
        auto results() -> std::vector<eye_tracker_calibration::Result> override;

        class ComputeAndApply;

        auto computeAndApply() -> ComputeAndApply;

        class ComputeAndApply {
          public:
            explicit ComputeAndApply(TobiiResearchEyeTracker *);
            auto success() -> bool;
            auto results() -> std::vector<eye_tracker_calibration::Result>;
            ~ComputeAndApply();

          private:
            TobiiResearchCalibrationResult *result{};
        };

      private:
        TobiiResearchEyeTracker *eyetracker{};
    };

    class CalibrationValidation {
      public:
        explicit CalibrationValidation(TobiiResearchEyeTracker *eyetracker);
        void acquire();
        void collect(eye_tracker_calibration::Point);
        void release();
        ~CalibrationValidation();

        class Result;

        auto result() -> Result;

        class Result {
          public:
            explicit Result(CalibrationValidator *validator);
            ~Result();

          private:
            CalibrationValidationResult *result{};
        };

      private:
        CalibrationValidator *validator{};
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
