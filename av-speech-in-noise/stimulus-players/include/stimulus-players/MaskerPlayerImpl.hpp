#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_MASKERPLAYERIMPL_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_MASKERPLAYERIMPL_HPP_

#include "AudioReader.hpp"
#include <recognition-test/Model.hpp>
#include <recognition-test/RecognitionTestModel.hpp>
#include <gsl/gsl>
#include <string>
#include <vector>
#include <atomic>

namespace stimulus_players {
using channel_buffer_type = gsl::span<float>;

class AudioPlayer {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void fillAudioBuffer(
            const std::vector<channel_buffer_type> &audio) = 0;
    };

    virtual ~AudioPlayer() = default;
    virtual void subscribe(EventListener *) = 0;
    virtual void play() = 0;
    virtual void stop() = 0;
    virtual auto playing() -> bool = 0;
    virtual void loadFile(std::string) = 0;
    virtual auto deviceCount() -> int = 0;
    virtual auto deviceDescription(int index) -> std::string = 0;
    virtual auto outputDevice(int index) -> bool = 0;
    virtual void setDevice(int index) = 0;
    virtual auto sampleRateHz() -> double = 0;
    virtual auto durationSeconds() -> double = 0;
};

class Timer {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void callback() = 0;
    };
    virtual ~Timer() = default;
    virtual void subscribe(EventListener *) = 0;
    virtual void scheduleCallbackAfterSeconds(double) = 0;
};

using cpp_core_guidelines_index_type = gsl::index;
using channel_index_type = cpp_core_guidelines_index_type;
using sample_index_type = cpp_core_guidelines_index_type;

class MaskerPlayerImpl : public av_speech_in_noise::MaskerPlayer,
                         public AudioPlayer::EventListener,
                         public Timer::EventListener {
  public:
    MaskerPlayerImpl(AudioPlayer *, AudioReader *, Timer *);
    void subscribe(MaskerPlayer::EventListener *) override;
    void fadeIn() override;
    void fadeOut() override;
    void loadFile(std::string) override;
    auto playing() -> bool override;
    void setAudioDevice(std::string) override;
    void setLevel_dB(double) override;
    void fillAudioBuffer(
        const std::vector<channel_buffer_type> &audio) override;
    void setFadeInOutSeconds(double);
    auto outputAudioDeviceDescriptions() -> std::vector<std::string> override;
    auto rms() -> double override;
    auto durationSeconds() -> double override;
    void seekSeconds(double) override;
    auto fadeTimeSeconds() -> double override;
    void callback() override;
    void setChannelDelaySeconds(
        channel_index_type channel, double seconds) override;
    void useFirstChannelOnly() override;
    void clearChannelDelays() override;
    void useAllChannels() override;

  private:
    auto readAudio(std::string) -> audio_type;
    auto audioDeviceDescriptions_() -> std::vector<std::string>;
    auto findDeviceIndex(const std::string &device) -> int;
    void recalculateSamplesToWaitPerChannel();

    class AudioThread {
      public:
        void setSharedState(MaskerPlayerImpl *);
        void fillAudioBuffer(const std::vector<channel_buffer_type> &audio);

      private:
        void copySourceAudio(
            const std::vector<channel_buffer_type> &audioBuffer);
        void applyLevel(const std::vector<channel_buffer_type> &audioBuffer);
        void updateWindowLength();
        void prepareToFadeIn();
        void checkForFadeIn();
        void prepareToFadeOut();
        void checkForFadeOut();
        auto doneFadingIn() -> bool;
        void checkForFadeInComplete();
        auto doneFadingOut() -> bool;
        void checkForFadeOutComplete();
        void advanceCounterIfStillFading();
        void updateFadeState();
        auto nextFadeScalar() -> double;
        auto sourceFrames() -> sample_index_type;

        MaskerPlayerImpl *sharedState{};
        int hannCounter{};
        int halfWindowLength{};
        bool fadingOut{};
        bool fadingIn{};
    };

    class MainThread {
      public:
        MainThread(AudioPlayer *, Timer *);
        void setSharedState(MaskerPlayerImpl *);
        void callback();
        void subscribe(MaskerPlayer::EventListener *);
        void fadeIn();
        void fadeOut();
        void setChannelDelaySeconds(channel_index_type channel, double seconds);
        void clearChannelDelays();
        auto channelDelaySeconds(channel_index_type channel) -> double;
        void setFadeInOutSeconds(double);
        auto fadeTimeSeconds() -> double;

      private:
        auto fading() -> bool;
        void scheduleCallbackAfterSeconds(double);

        std::vector<double> channelDelaySeconds_;
        MaskerPlayerImpl *sharedState{};
        AudioPlayer *player;
        MaskerPlayer::EventListener *listener{};
        Timer *timer;
        double fadeInOutSeconds{};
        bool fadingIn{};
        bool fadingOut{};
    };

    AudioThread audioThread;
    MainThread mainThread;
    audio_type sourceAudio{};
    std::vector<sample_index_type> samplesToWaitPerChannel;
    std::vector<sample_index_type> audioFrameHeadsPerChannel;
    AudioPlayer *player;
    AudioReader *reader;
    std::atomic<double> levelScalar{1};
    std::atomic<int> levelTransitionSamples_{};
    std::atomic<bool> firstChannelOnly{};
    std::atomic<bool> fadeOutComplete{};
    std::atomic<bool> fadeInComplete{};
    std::atomic<bool> pleaseFadeOut{};
    std::atomic<bool> pleaseFadeIn{};
};
}

#endif
