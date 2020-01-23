#ifndef APPS_TOBIIEYETRACKER_HPP_
#define APPS_TOBIIEYETRACKER_HPP_

#include "tobii_research_streams.h"
#include <recognition-test/RecognitionTestModel.hpp>
#include <vector>

static TobiiResearchEyeTracker *eyeTracker(
    TobiiResearchEyeTrackers *eyeTrackers) {
    return eyeTrackers->eyetrackers[0];
}

class TobiiEyeTracker : public av_speech_in_noise::EyeTracker {
  public:
    TobiiEyeTracker() { tobii_research_find_all_eyetrackers(&eyeTrackers); }

    ~TobiiEyeTracker() override {
        tobii_research_free_eyetrackers(eyeTrackers);
    }

    void allocateRecordingTimeSeconds(double s) override {
        gazeData.resize(60 * s);
        head = 0;
    }

    void start() override {
        tobii_research_subscribe_to_gaze_data(
            eyeTracker(eyeTrackers), gaze_data_callback, this);
    }

    void stop() override {
        tobii_research_unsubscribe_from_gaze_data(
            eyeTracker(eyeTrackers), gaze_data_callback);
    }

  private:
    static void gaze_data_callback(
        TobiiResearchGazeData *gaze_data, void *self) {
        static_cast<TobiiEyeTracker *>(self)->gazeDataReceived(gaze_data);
    }

    void gazeDataReceived(TobiiResearchGazeData *gaze_data) {
        gazeData.at(head++) = *gaze_data;
    }

    std::vector<TobiiResearchGazeData> gazeData{};
    TobiiResearchEyeTrackers *eyeTrackers{};
    std::size_t head{};
};

#endif
