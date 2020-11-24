#ifndef AV_SPEECH_IN_NOISE_MACOS_APPKITTESTSETUPVIEWFACTORY_H_
#define AV_SPEECH_IN_NOISE_MACOS_APPKITTESTSETUPVIEWFACTORY_H_

#include <presentation/TestSetup.hpp>
#include <av-speech-in-noise/Interface.hpp>
#import <AppKit/AppKit.h>
#include <memory>

namespace av_speech_in_noise {
class TestSetupUI : public virtual TestSetupView,
                    public virtual TestSetupControl {};

class AppKitTestSetupUIFactory {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        AppKitTestSetupUIFactory);
    virtual auto make(NSViewController *) -> std::unique_ptr<TestSetupUI> = 0;
};
}

#endif
