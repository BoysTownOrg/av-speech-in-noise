#ifndef AV_SPEECH_IN_NOISE_MACOS_EYETRACKERSTUB_HPP_
#define AV_SPEECH_IN_NOISE_MACOS_EYETRACKERSTUB_HPP_

#include <recognition-test/RecognitionTestModel.hpp>

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
