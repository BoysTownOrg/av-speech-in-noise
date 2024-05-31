#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IMODELHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IMODELHPP_

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

#include <string>
#include <vector>

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

namespace submitting_keypress {
class Interactor {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Interactor);
    virtual auto submits(const std::vector<KeyPressResponse> &) -> bool = 0;
    virtual void forceSubmit(const std::vector<KeyPressResponse> &) = 0;
};
}

namespace submitting_emotion {
class Interactor {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Interactor);
    virtual void submit(const EmotionResponse &) = 0;
};
}
}

#endif
