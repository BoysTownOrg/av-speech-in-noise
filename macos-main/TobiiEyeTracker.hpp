#ifndef AV_SPEECH_IN_NOISE_MACOS_MAIN_TOBIIEYETRACKER_HPP_
#define AV_SPEECH_IN_NOISE_MACOS_MAIN_TOBIIEYETRACKER_HPP_

#include <recognition-test/RecognitionTestModel.hpp>
#include <tobii_research_streams.h>
#include <vector>

namespace av_speech_in_noise {
class TobiiEyeTracker : public EyeTracker {
  public:
    TobiiEyeTracker();
    ~TobiiEyeTracker() override;
    void allocateRecordingTimeSeconds(double s) override;
    void start() override;
    void stop() override;
    auto gazeSamples() -> BinocularGazeSamples override;
    auto currentSystemTime() -> EyeTrackerSystemTime override;

  private:
    static void gaze_data_callback(
        TobiiResearchGazeData *gaze_data, void *self);
    void gazeDataReceived(TobiiResearchGazeData *gaze_data);

    std::vector<TobiiResearchGazeData> gazeData{};
    TobiiResearchEyeTrackers *eyeTrackers{};
    std::size_t head{};
    void *library;
};
}

#endif
