#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_IMASKERPLAYER_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_IMASKERPLAYER_HPP_

#include "Player.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <gsl/gsl>
#include <string>
#include <vector>
#include <cstdint>

namespace av_speech_in_noise {
class MaskerPlayer {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void fadeInComplete(const AudioSampleTimeWithOffset &) = 0;
        virtual void fadeOutComplete() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(MaskerPlayer);
    virtual void attach(Observer *) = 0;
    virtual auto outputAudioDeviceDescriptions()
        -> std::vector<std::string> = 0;
    virtual void setAudioDevice(std::string) = 0;
    virtual void fadeIn() = 0;
    virtual void loadFile(const LocalUrl &) = 0;
    virtual auto playing() -> bool = 0;
    virtual auto digitalLevel() -> DigitalLevel = 0;
    virtual void apply(LevelAmplification) = 0;
    virtual auto duration() -> Duration = 0;
    virtual auto sampleRateHz() -> double = 0;
    virtual void seekSeconds(double) = 0;
    virtual auto rampDuration() -> Duration = 0;
    virtual void useAllChannels() = 0;
    virtual void useFirstChannelOnly() = 0;
    virtual void useSecondChannelOnly() = 0;
    virtual void clearChannelDelays() = 0;
    virtual void setChannelDelaySeconds(gsl::index channel, double seconds) = 0;
    virtual auto nanoseconds(PlayerTime) -> std::uintmax_t = 0;
    virtual auto currentSystemTime() -> PlayerTime = 0;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual void setSteadyLevelFor(Duration) {}
};
}

#endif
