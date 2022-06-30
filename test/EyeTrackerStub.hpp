#ifndef AV_SPEECH_IN_NOISE_TEST_EYETRACKERSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_EYETRACKERSTUB_HPP_

#include "LogString.hpp"

#include <av-speech-in-noise/core/EyeTracking.hpp>

#include <sstream>

namespace av_speech_in_noise {
class EyeTrackerStub : public EyeTracker {
  public:
    [[nodiscard]] auto recordingTimeAllocatedSeconds() const -> double {
        return recordingTimeAllocatedSeconds_;
    }

    [[nodiscard]] auto started() const -> bool { return started_; }

    [[nodiscard]] auto stopped() const -> bool { return stopped_; }

    [[nodiscard]] auto log() const -> const std::stringstream & { return log_; }

    void allocateRecordingTimeSeconds(double x) override {
        insert(log_, "allocateRecordingTimeSeconds ");
        recordingTimeAllocatedSeconds_ = x;
        recordingTimeAllocated_ = true;
    }

    void start() override {
        insert(log_, "start ");
        started_ = true;
    }

    void stop() override {
        insert(log_, "stop ");
        stopped_ = true;
    }

    [[nodiscard]] auto recordingTimeAllocated() const -> bool {
        return recordingTimeAllocated_;
    }

    auto gazeSamples() -> BinocularGazeSamples override {
        insert(log_, "gazeSamples ");
        return gazeSamples_;
    }

    void setGazes(BinocularGazeSamples g) { gazeSamples_ = std::move(g); }

    auto currentSystemTime() -> EyeTrackerSystemTime override {
        return currentSystemTime_;
    }

    void setCurrentSystemTime(EyeTrackerSystemTime t) {
        currentSystemTime_ = t;
    }

    void write(std::ostream &) override {}

  private:
    BinocularGazeSamples gazeSamples_;
    std::stringstream log_{};
    EyeTrackerSystemTime currentSystemTime_{};
    double recordingTimeAllocatedSeconds_{};
    bool recordingTimeAllocated_{};
    bool started_{};
    bool stopped_{};
};
}

#endif
