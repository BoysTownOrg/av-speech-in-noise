#ifndef AV_SPEECH_IN_NOISE_LIB_DOMAIN_INCLUDE_AVSPEECHINNOISE_NAMEHPP_
#define AV_SPEECH_IN_NOISE_LIB_DOMAIN_INCLUDE_AVSPEECHINNOISE_NAMEHPP_

#include "Model.hpp"

namespace av_speech_in_noise {
constexpr auto name(Condition c) -> const char * {
    switch (c) {
    case Condition::auditoryOnly:
        return "auditory-only";
    case Condition::audioVisual:
        return "audio-visual";
    }
}
}

#endif
