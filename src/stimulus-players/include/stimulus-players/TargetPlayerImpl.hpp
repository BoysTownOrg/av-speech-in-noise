#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_TARGETPLAYERIMPL_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_TARGETPLAYERIMPL_HPP_

#include "AudioReader.hpp"
#include <recognition-test/RecognitionTestModel.hpp>
#include <gsl/gsl>
#include <vector>
#include <string>
#include <atomic>

namespace stimulus_players {
class VideoPlayer {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void playbackComplete() = 0;
        virtual void fillAudioBuffer(
            const std::vector<gsl::span<float>> &audio) = 0;
    };

    virtual ~VideoPlayer() = default;
    virtual void subscribe(EventListener *) = 0;
    virtual void subscribeToPlaybackCompletion() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void loadFile(std::string) = 0;
    virtual void play() = 0;
    virtual void playAt(const av_speech_in_noise::TargetTimeWithDelay &) = 0;
    virtual auto playing() -> bool = 0;
    virtual auto deviceCount() -> int = 0;
    virtual auto deviceDescription(int index) -> std::string = 0;
    virtual void setDevice(int index) = 0;
    virtual auto durationSeconds() -> double = 0;
};

class TargetPlayerImpl : public av_speech_in_noise::TargetPlayer,
                         public VideoPlayer::EventListener {
  public:
    TargetPlayerImpl(VideoPlayer *, AudioReader *);
    void subscribe(TargetPlayer::EventListener *) override;
    void play() override;
    void playAt(const av_speech_in_noise::TargetTimeWithDelay &) override;
    void loadFile(std::string filePath) override;
    void hideVideo() override;
    void showVideo() override;
    auto rms() -> double override;
    void setLevel_dB(double) override;
    void setAudioDevice(std::string) override;
    auto playing() -> bool override;
    void subscribeToPlaybackCompletion() override;
    auto durationSeconds() -> double override;
    void playbackComplete() override;
    void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) override;
    auto audioDevices() -> std::vector<std::string>;
    void useFirstChannelOnly() override;
    void useAllChannels() override;

  private:
    auto readAudio_() -> audio_type;

    std::string filePath_{};
    VideoPlayer *player;
    AudioReader *reader;
    TargetPlayer::EventListener *listener_{};
    std::atomic<double> audioScale{1};
    std::atomic<bool> useFirstChannelOnly_{};
};
}

#endif
