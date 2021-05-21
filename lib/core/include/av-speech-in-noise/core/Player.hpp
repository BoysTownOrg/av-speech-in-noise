#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_PLAYERHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_PLAYERHPP_

#include <av-speech-in-noise/Model.hpp>
#include <gsl/gsl>
#include <cstdint>

namespace av_speech_in_noise {
class InvalidAudioFile {};
class InvalidAudioDevice {};

struct Duration {
    double seconds;
};

struct Delay : Duration {
    explicit constexpr Delay(double seconds = 0.) : Duration{seconds} {}
};

using player_system_time_type = std::uintmax_t;

struct PlayerTime {
    player_system_time_type system{};
};

struct PlayerTimeWithDelay {
    PlayerTime playerTime;
    Delay delay{};
};

struct DigitalLevel {
    double dBov;
};

struct LevelAmplification {
    double dB;
};

struct AudioSampleTimeWithOffset {
    PlayerTime playerTime;
    gsl::index sampleOffset{};
};
}

#endif
