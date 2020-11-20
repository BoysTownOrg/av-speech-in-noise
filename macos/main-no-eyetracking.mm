#include "main.h"
#include "CocoaView.h"

namespace av_speech_in_noise {
class EyeTrackerStub : public EyeTracker {
    void allocateRecordingTimeSeconds(double s) override {}
    void start() override {}
    void stop() override {}
    auto gazeSamples() -> BinocularGazeSamples override { return {}; }
    auto currentSystemTime() -> EyeTrackerSystemTime override { return {}; }
};
}

int main() {
    av_speech_in_noise::EyeTrackerStub eyeTracker;
    av_speech_in_noise::CocoaTestSetupViewFactory testSetupViewFactory;
    av_speech_in_noise::DefaultOutputFileNameFactory outputFileNameFactory;
    av_speech_in_noise::main(
        eyeTracker, testSetupViewFactory, outputFileNameFactory);
}
