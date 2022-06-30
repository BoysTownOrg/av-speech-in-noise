#ifndef AV_SPEECH_IN_NOISE_MACOS_EYETRACKERSTUB_HPP_
#define AV_SPEECH_IN_NOISE_MACOS_EYETRACKERSTUB_HPP_

#include <av-speech-in-noise/core/EyeTracking.hpp>

namespace av_speech_in_noise {
class EyeTrackerStub : public EyeTracker {
    void allocateRecordingTimeSeconds(double) override {}
    void start() override {}
    void stop() override {}
    auto gazeSamples() -> BinocularGazeSamples override { return {}; }
    auto currentSystemTime() -> EyeTrackerSystemTime override { return {}; }
    void write(std::ostream &) override {}
};
}

#endif
