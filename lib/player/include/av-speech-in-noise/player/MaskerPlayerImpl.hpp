#ifndef AV_SPEECH_IN_NOISE_LIB_PLAYER_INCLUDE_AVSPEECHINNOISE_PLAYER_MASKERPLAYERIMPLHPP_
#define AV_SPEECH_IN_NOISE_LIB_PLAYER_INCLUDE_AVSPEECHINNOISE_PLAYER_MASKERPLAYERIMPLHPP_

#include "AudioReader.hpp"
#include <av-speech-in-noise/core/ITimer.hpp>
#include <av-speech-in-noise/core/IMaskerPlayer.hpp>
#include <gsl/gsl>
#include <string>
#include <vector>
#include <atomic>

namespace av_speech_in_noise {
using channel_buffer_type = gsl::span<float>;

class AudioPlayer {
  public:
    class Observer {
      public:
        virtual ~Observer() = default;
        virtual void fillAudioBuffer(
            const std::vector<channel_buffer_type> &audio,
            player_system_time_type) = 0;
    };

    virtual ~AudioPlayer() = default;
    virtual void attach(Observer *) = 0;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual auto playing() -> bool = 0;
    virtual void loadFile(std::string) = 0;
    virtual auto deviceCount() -> int = 0;
    virtual auto deviceDescription(int index) -> std::string = 0;
    virtual auto outputDevice(int index) -> bool = 0;
    virtual void setDevice(int index) = 0;
    virtual auto sampleRateHz() -> double = 0;
    virtual auto nanoseconds(PlayerTime) -> std::uintmax_t = 0;
    virtual auto currentSystemTime() -> PlayerTime = 0;
};

using cpp_core_guidelines_index_type = gsl::index;
using channel_index_type = cpp_core_guidelines_index_type;
using sample_index_type = cpp_core_guidelines_index_type;

struct LockFreeMessage {
    std::atomic<bool> execute{};
    std::atomic<bool> complete{};
};

class MaskerPlayerImpl : public MaskerPlayer,
                         public AudioPlayer::Observer,
                         public Timer::Observer {
  public:
    MaskerPlayerImpl(AudioPlayer *, AudioReader *, Timer *);
    void attach(MaskerPlayer::Observer *) override;
    void fadeIn() override;
    void loadFile(const LocalUrl &) override;
    auto playing() -> bool override;
    void setAudioDevice(std::string) override;
    void apply(LevelAmplification) override;
    void fillAudioBuffer(const std::vector<channel_buffer_type> &audio,
        player_system_time_type) override;
    void setRampFor(Duration);
    void setSteadyLevelFor(Duration) override;
    auto outputAudioDeviceDescriptions() -> std::vector<std::string> override;
    auto digitalLevel() -> DigitalLevel override;
    auto duration() -> Duration override;
    void seekSeconds(double) override;
    auto rampDuration() -> Duration override;
    auto sampleRateHz() -> double override;
    void callback() override;
    void setChannelDelaySeconds(
        channel_index_type channel, double seconds) override;
    void useFirstChannelOnly() override;
    void useSecondChannelOnly() override;
    void play() override;
    void stop() override;
    void clearChannelDelays() override;
    void useAllChannels() override;
    auto nanoseconds(PlayerTime) -> std::uintmax_t override;
    auto currentSystemTime() -> PlayerTime override;
    static constexpr Delay callbackDelay{1. / 30};

    struct SharedState {
        audio_type sourceAudio{};
        std::vector<sample_index_type> samplesToWaitPerChannel;
        std::vector<sample_index_type> audioFrameHeadsPerChannel;
        std::atomic<double> levelScalar{1};
        std::atomic<double> vibrotactileTimeScalar{};
        std::atomic<player_system_time_type> fadeInCompleteSystemTime{};
        std::atomic<gsl::index> fadeInCompleteSystemTimeSampleOffset{};
        std::atomic<gsl::index> rampSamples{};
        std::atomic<gsl::index> steadyLevelSamples{};
        std::atomic<gsl::index> vibrotactileSamples{};
        std::atomic<bool> firstChannelOnly{};
        std::atomic<bool> secondChannelOnly{};
        LockFreeMessage fadeInMessage{};
        LockFreeMessage fadeOutMessage{};
        LockFreeMessage disableAudioMessage{};
        LockFreeMessage enableAudioMessage{};
    };

  private:
    auto readAudio(std::string) -> audio_type;
    auto fading() -> bool;

    class AudioThreadContext {
      public:
        explicit AudioThreadContext(SharedState &sharedState)
            : sharedState{sharedState} {}
        void fillAudioBuffer(const std::vector<channel_buffer_type> &audio,
            player_system_time_type);

      private:
        auto doneFadingIn() -> bool;
        auto doneFadingOut() -> bool;

        SharedState &sharedState;
        double vibrotactileTimeScalar;
        gsl::index rampCounter{};
        gsl::index rampSamples{};
        gsl::index steadyLevelCounter{};
        gsl::index steadyLevelSamples{};
        gsl::index vibrotactileSamples{};
        gsl::index vibrotactileCounter{};
        bool fadingOut{};
        bool fadingIn{};
        bool steadyingLevel{};
        bool playingVibrotactile{};
        bool enabled{};
    };

    SharedState sharedState{};
    AudioThreadContext audioThreadContext;
    std::vector<double> channelDelaySeconds;
    AudioPlayer *player;
    AudioReader *reader;
    Timer *timer;
    MaskerPlayer::Observer *listener{};
    Duration rampDuration_{};
    bool playingFiniteSection{};
    bool audioEnabled{};
};
}

#endif
