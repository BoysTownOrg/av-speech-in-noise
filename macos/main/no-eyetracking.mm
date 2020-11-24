#include "../run.h"
#include "../AppKitView.h"
#include "../EyeTrackerStub.hpp"

namespace av_speech_in_noise {
static void main() {
    EyeTrackerStub eyeTracker;
    AppKitTestSetupUIFactoryImpl testSetupViewFactory;
    DefaultOutputFileNameFactory outputFileNameFactory;
    initializeAppAndRunEventLoop(
        eyeTracker, testSetupViewFactory, outputFileNameFactory);
}
}

int main() { av_speech_in_noise::main(); }
