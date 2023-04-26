#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IMODELHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IMODELHPP_

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

#include <stdexcept>
#include <string>

namespace av_speech_in_noise {
namespace submitting_free_response {
class Interactor {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Interactor);
    virtual void submit(const FreeResponse &) = 0;
};
}

namespace submitting_pass_fail {
class Interactor {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Interactor);
    virtual void submitCorrectResponse() = 0;
    virtual void submitIncorrectResponse() = 0;
};
}

namespace submitting_keywords {
class Interactor {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Interactor);
    virtual void submit(const ThreeKeywordsResponse &) = 0;
};
}

struct Screen {
    std::string name;
};

namespace submitting_number_keywords {
class Interactor {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Interactor);
    virtual void submit(const CorrectKeywords &) = 0;
};
}

namespace submitting_syllable {
class Interactor {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Interactor);
    virtual void submit(const SyllableResponse &) = 0;
};
}

namespace submitting_consonant {
class Interactor {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Interactor);
    virtual void submit(const ConsonantResponse &) = 0;
};
}

class RunningATestFacade {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void trialComplete() = 0;
    };

    class RequestFailure : public std::runtime_error {
      public:
        explicit RequestFailure(const std::string &s) : std::runtime_error{s} {}
    };

    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(RunningATestFacade);
    virtual void initialize(const AdaptiveTest &) = 0;
    virtual void initializeWithCyclicTargets(const AdaptiveTest &) = 0;
    virtual void initializeWithTargetReplacement(
        const FixedLevelFixedTrialsTest &) = 0;
    virtual void initializeWithSilentIntervalTargets(
        const FixedLevelTest &) = 0;
    virtual void initializeWithAllTargets(const FixedLevelTest &) = 0;
    virtual void initialize(const FixedLevelTestWithEachTargetNTimes &) = 0;
    virtual void playCalibration(const Calibration &) = 0;
    virtual void playLeftSpeakerCalibration(const Calibration &) = 0;
    virtual void playRightSpeakerCalibration(const Calibration &) = 0;
    virtual void playTrial(const AudioSettings &) = 0;
    virtual void submit(const coordinate_response_measure::Response &) = 0;
    virtual auto testComplete() -> bool = 0;
    virtual void restartAdaptiveTestWhilePreservingTargets() = 0;
};
}

#endif
