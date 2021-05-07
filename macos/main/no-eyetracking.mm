#include "../run.h"
#include "../AppKitView.h"
#include "../AppKit-utility.h"
#include "../EyeTrackerStub.hpp"

namespace av_speech_in_noise {
static void main() {
    EyeTrackerStub eyeTracker;
    AppKitTestSetupUIFactoryImpl testSetupViewFactory;
    DefaultOutputFileNameFactory outputFileNameFactory;
    const auto aboutViewController{nsTabViewControllerWithoutTabControl()};
    initializeAppAndRunEventLoop(eyeTracker, testSetupViewFactory,
        outputFileNameFactory, aboutViewController);
}
}

int mainz() { av_speech_in_noise::main(); }
