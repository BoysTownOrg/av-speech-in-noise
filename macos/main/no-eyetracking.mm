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
    const auto aboutWindow{
        [NSWindow windowWithContentViewController:aboutViewController]};
    aboutWindow.styleMask = NSWindowStyleMaskClosable | NSWindowStyleMaskTitled;
    initializeAppAndRunEventLoop(
        eyeTracker, testSetupViewFactory, outputFileNameFactory, aboutWindow);
}
}

int main() { av_speech_in_noise::main(); }
