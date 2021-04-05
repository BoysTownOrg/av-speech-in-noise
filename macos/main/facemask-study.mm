#include "../run.h"
#include "../FacemaskStudySetupView.h"
#include "../EyeTrackerStub.hpp"
#include "../AppKit-utility.h"
#import <AppKit/AppKit.h>

namespace av_speech_in_noise {
class MetaConditionOutputFileNameFactory : public OutputFileNameFactory {
  public:
    auto make(TimeStamp &timeStamp)
        -> std::unique_ptr<OutputFileName> override {
        return std::make_unique<MetaConditionOutputFileName>(timeStamp);
    }
};

class CongratulatesUserWhenTestCompletes : public SessionController::Observer {
  public:
    void notifyThatTestIsComplete() override {
        const auto alert{[[NSAlert alloc] init]};
        [alert setMessageText:@""];
        [alert setInformativeText:@"Condition complete, great work!"];
        [alert addButtonWithTitle:@"Continue"];
        [alert runModal];
    }
};

static void main() {
    EyeTrackerStub eyeTracker;
    FacemaskStudySetupViewFactory testSetupViewFactory;
    MetaConditionOutputFileNameFactory outputFileNameFactory;
    CongratulatesUserWhenTestCompletes congratulatesUserWhenTestCompletes;
    const auto aboutViewController{nsTabViewControllerWithoutTabControl()};
    initializeAppAndRunEventLoop(eyeTracker, testSetupViewFactory,
        outputFileNameFactory, aboutViewController,
        &congratulatesUserWhenTestCompletes, "Desktop/check your data here");
}
}

int main() { av_speech_in_noise::main(); }
