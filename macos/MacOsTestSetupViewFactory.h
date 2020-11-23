#ifndef AV_SPEECH_IN_NOISE_MACOS_MACOSTESTSETUPVIEWFACTORY_H_
#define AV_SPEECH_IN_NOISE_MACOS_MACOSTESTSETUPVIEWFACTORY_H_

#include <presentation/TestSetup.hpp>
#import <Cocoa/Cocoa.h>
#include <memory>

namespace av_speech_in_noise {
class TestSetupUI : public virtual TestSetupView,
                    public virtual TestSetupControl {};

class MacOsTestSetupViewFactory {
  public:
    virtual ~MacOsTestSetupViewFactory() = default;
    virtual auto make(NSViewController *) -> std::unique_ptr<TestSetupUI> = 0;
};
}

#endif
