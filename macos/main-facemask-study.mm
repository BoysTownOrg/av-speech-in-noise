#include "main.h"
#include "MacOsTestSetupViewFactory.h"
#include "FacemaskStudySetupView.h"
#include "common-objc.h"
#include <presentation/SessionControllerImpl.hpp>
#include <presentation/TestSettingsInterpreter.hpp>
#import <AppKit/AppKit.h>
#include <string>
#include <vector>
#include <algorithm>

namespace av_speech_in_noise {
class EyeTrackerStub : public EyeTracker {
    void allocateRecordingTimeSeconds(double) override {}
    void start() override {}
    void stop() override {}
    auto gazeSamples() -> BinocularGazeSamples override { return {}; }
    auto currentSystemTime() -> EyeTrackerSystemTime override { return {}; }
};

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
}

int main() {
    av_speech_in_noise::EyeTrackerStub eyeTracker;
    av_speech_in_noise::FacemaskStudySetupViewFactory testSetupViewFactory;
    av_speech_in_noise::MetaConditionOutputFileNameFactory
        outputFileNameFactory;
    av_speech_in_noise::CongratulatesUserWhenTestCompletes
        congratulatesUserWhenTestCompletes;
    av_speech_in_noise::main(eyeTracker, testSetupViewFactory,
        outputFileNameFactory, &congratulatesUserWhenTestCompletes,
        "Desktop/check your data here");
}
