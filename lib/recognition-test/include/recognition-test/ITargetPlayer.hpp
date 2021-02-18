#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_ITARGETPLAYER_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_ITARGETPLAYER_HPP_

#include "Player.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <string>

namespace av_speech_in_noise {
class TargetPlayer {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void playbackComplete() {}
        virtual void notifyThatPreRollHasCompleted() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TargetPlayer);
    virtual void attach(Observer *) = 0;
    virtual void setAudioDevice(std::string) = 0;
    virtual void play() = 0;
    virtual void playAt(const PlayerTimeWithDelay &) = 0;
    virtual auto playing() -> bool = 0;
    virtual void loadFile(const LocalUrl &) = 0;
    virtual void hideVideo() = 0;
    virtual void showVideo() = 0;
    virtual auto digitalLevel() -> DigitalLevel = 0;
    virtual void apply(LevelAmplification) = 0;
    virtual void subscribeToPlaybackCompletion() {}
    virtual auto duration() -> Duration = 0;
    virtual void useAllChannels() = 0;
    virtual void useFirstChannelOnly() = 0;
    virtual void preRoll() = 0;
};
}

#endif
