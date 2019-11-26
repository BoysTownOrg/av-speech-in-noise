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
class AudioPlayer {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void fillAudioBuffer(
            const std::vector<gsl::span<float>> &audio) = 0;
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
    virtual void seekSeconds(double) = 0;
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
    void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) override;
    void setFadeInOutSeconds(double);
    auto outputAudioDeviceDescriptions() -> std::vector<std::string> override;
    auto rms() -> double override;
    auto durationSeconds() -> double override;
    void seekSeconds(double) override;
    auto fadeTimeSeconds() -> double override;
    void callback() override;

  private:
    auto readAudio_() -> audio_type;
    auto audioDeviceDescriptions_() -> std::vector<std::string>;
    auto findDeviceIndex(const std::string &device) -> int;

    class AudioThread {
      public:
        explicit AudioThread(AudioPlayer *);
        void setSharedAtomics(MaskerPlayerImpl *);
        void fillAudioBuffer(const std::vector<gsl::span<float>> &audio);

      private:
        void updateWindowLength();
        void prepareToFadeIn();
        void checkForFadeIn();
        void prepareToFadeOut();
        void checkForFadeOut();
        auto levelTransitionSamples() -> int;
        void scaleAudio(const std::vector<gsl::span<float>> &);
        auto doneFadingIn() -> bool;
        void checkForFadeInComplete();
        auto doneFadingOut() -> bool;
        void checkForFadeOutComplete();
        void advanceCounterIfStillFading();
        void updateFadeState();
        auto fadeScalar() -> double;

        int hannCounter{};
        int halfWindowLength{};
        MaskerPlayerImpl *sharedAtomics{};
        AudioPlayer *player;
        bool fadingOut{};
        bool fadingIn{};
    };

    class MainThread {
      public:
        MainThread(AudioPlayer *, Timer *);
        void setSharedAtomics(MaskerPlayerImpl *);
        void callback();
        void subscribe(MaskerPlayer::EventListener *);
        void fadeIn();
        void fadeOut();

      private:
        auto fading() -> bool;
        void scheduleCallbackAfterSeconds(double);

        MaskerPlayerImpl *sharedAtomics{};
        AudioPlayer *player;
        MaskerPlayer::EventListener *listener{};
        Timer *timer;
        bool fadingIn{};
        bool fadingOut{};
    };

    AudioThread audioThread;
    MainThread mainThread;
    audio_type audio_{};
    std::string filePath_{};
    double rms_{};
    std::atomic<double> levelScalar{1};
    std::atomic<double> fadeInOutSeconds{};
    AudioPlayer *player;
    AudioReader *reader;
    std::size_t audioSampleIndex_{};
    std::atomic<bool> fadeOutComplete{};
    std::atomic<bool> fadeInComplete{};
    std::atomic<bool> pleaseFadeOut{};
    std::atomic<bool> pleaseFadeIn{};
};
}

#endif
