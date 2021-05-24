#ifndef AV_SPEECH_IN_NOISE_MACOS_RUN_H_
#define AV_SPEECH_IN_NOISE_MACOS_RUN_H_

#include "AppKitTestSetupUIFactory.h"
#include <av-speech-in-noise/ui/Consonant.hpp>
#include <av-speech-in-noise/ui/CorrectKeywords.hpp>
#include <av-speech-in-noise/ui/PassFail.hpp>
#include <av-speech-in-noise/ui/Syllables.hpp>
#include <av-speech-in-noise/ui/SessionController.hpp>
#include <av-speech-in-noise/ui/FreeResponse.hpp>
#include <av-speech-in-noise/ui/ChooseKeywords.hpp>
#include <av-speech-in-noise/core/RecognitionTestModel.hpp>
#include <av-speech-in-noise/core/OutputFilePath.hpp>
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

class AppKitRunMenuInitializer {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        AppKitRunMenuInitializer);
    virtual void initialize(NSMenu *) = 0;
};

class TestUI : public TestView, public TestControl {};

class SessionUI : public SessionView, public SessionControl {};

class FreeResponseUI : public FreeResponseView, public FreeResponseControl {};

class SyllablesUI : public SyllablesView, public SyllablesControl {};

class ChooseKeywordsUI : public ChooseKeywordsView,
                         public ChooseKeywordsControl {};

class CorrectKeywordsUI : public CorrectKeywordsView,
                          public CorrectKeywordsControl {};

class PassFailUI : public PassFailView, public PassFailControl {};

void initializeAppAndRunEventLoop(eye_tracking::Tracker &,
    OutputFileNameFactory &, AppKitTestSetupUIFactory &, SessionUI &, TestUI &,
    FreeResponseUI &, SyllablesUI &, ChooseKeywordsUI &, CorrectKeywordsUI &,
    PassFailUI &, SessionController::Observer * = nullptr,
    std::filesystem::path relativeOutputDirectory =
        "Documents/AvSpeechInNoise Data",
    AppKitRunMenuInitializer * = nullptr);
}

#endif
