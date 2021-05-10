#ifndef AV_SPEECH_IN_NOISE_MACOS_RUN_H_
#define AV_SPEECH_IN_NOISE_MACOS_RUN_H_

#include "AppKitTestSetupUIFactory.h"
#include <presentation/SessionController.hpp>
#include <presentation/FreeResponse.hpp>
#include <recognition-test/RecognitionTestModel.hpp>
#include <recognition-test/OutputFilePath.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <memory>
#include <filesystem>

namespace av_speech_in_noise {
class OutputFileNameFactory {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        OutputFileNameFactory);
    virtual auto make(TimeStamp &) -> std::unique_ptr<OutputFileName> = 0;
};

class DefaultOutputFileNameFactory : public OutputFileNameFactory {
  public:
    auto make(TimeStamp &timeStamp)
        -> std::unique_ptr<OutputFileName> override {
        return std::make_unique<DefaultOutputFileName>(timeStamp);
    }
};

class TestUI : public TestView, public TestControl {};

class SessionUI : public SessionView, public SessionControl {};

class FreeResponseUI : public FreeResponseView, public FreeResponseControl {};

void initializeAppAndRunEventLoop(EyeTracker &, AppKitTestSetupUIFactory &,
    OutputFileNameFactory &, NSViewController *aboutViewController,
    SessionController::Observer * = nullptr,
    std::filesystem::path relativeOutputDirectory =
        "Documents/AvSpeechInNoise Data",
    SessionUI * = nullptr, TestUI * = nullptr, FreeResponseUI * = nullptr);
}

#endif
