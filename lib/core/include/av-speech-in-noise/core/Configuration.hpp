#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_CONFIGURATIONHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_CONFIGURATIONHPP_

#include <av-speech-in-noise/Interface.hpp>

#include <stdexcept>
#include <string>

namespace av_speech_in_noise {
inline auto boolean(const std::string &s) -> bool { return s == "true"; }

inline auto integer(const std::string &s) -> int {
    try {
        return std::stoi(s);
    } catch (const std::invalid_argument &) {
        return 0;
    }
}

inline auto contains(const std::string &s, const std::string &what) -> bool {
    return s.find(what) != std::string::npos;
}

class Configurable {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Configurable);
    virtual void configure(
        const std::string &key, const std::string &value) = 0;
};

class ConfigurationRegistry {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        ConfigurationRegistry);
    virtual void subscribe(Configurable &, const std::string &key) = 0;
};
}

#endif
