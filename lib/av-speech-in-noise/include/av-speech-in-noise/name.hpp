#ifndef AV_SPEECH_IN_NOISE_AV_SPEECH_IN_NOISE_INCLUDE_AV_SPEECH_IN_NOISE_NAME_HPP_
#define AV_SPEECH_IN_NOISE_AV_SPEECH_IN_NOISE_INCLUDE_AV_SPEECH_IN_NOISE_NAME_HPP_

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
