#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_TARGETPLAYERIMPL_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_TARGETPLAYERIMPL_HPP_

#include "AudioReader.hpp"
#include <recognition-test/RecognitionTestModel.hpp>
#include <gsl/gsl>
#include <vector>
#include <string>
#include <atomic>

namespace av_speech_in_noise {
class VideoPlayer {
  public:
    class Observer {
      public:
        virtual ~Observer() = default;
        virtual void playbackComplete() = 0;
        virtual void fillAudioBuffer(
            const std::vector<gsl::span<float>> &audio) = 0;
    };

    virtual ~VideoPlayer() = default;
    virtual void attach(Observer *) = 0;
    virtual void subscribeToPlaybackCompletion() = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void loadFile(std::string) = 0;
    virtual void play() = 0;
    virtual void playAt(const PlayerTimeWithDelay &) = 0;
    virtual auto playing() -> bool = 0;
    virtual auto deviceCount() -> int = 0;
    virtual auto deviceDescription(int index) -> std::string = 0;
    virtual void setDevice(int index) = 0;
    virtual auto durationSeconds() -> double = 0;
};

class SignalProcessor {
  public:
    virtual ~SignalProcessor() = default;
    virtual void process(const std::vector<gsl::span<float>> &) = 0;
    virtual void initialize(const audio_type &) = 0;
};

class TargetPlayerImpl : public TargetPlayer, public VideoPlayer::Observer {
  public:
    TargetPlayerImpl(VideoPlayer *, AudioReader *, SignalProcessor * = {});
    void attach(TargetPlayer::Observer *) override;
    void play() override;
    void playAt(const PlayerTimeWithDelay &) override;
    void loadFile(const LocalUrl &) override;
    void hideVideo() override;
    void showVideo() override;
    auto digitalLevel() -> DigitalLevel override;
    void apply(LevelAmplification) override;
    void setAudioDevice(std::string) override;
    auto playing() -> bool override;
    void subscribeToPlaybackCompletion() override;
    auto duration() -> Duration override;
    void playbackComplete() override;
    void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) override;
    auto audioDevices() -> std::vector<std::string>;
    void useFirstChannelOnly() override;
    void useAllChannels() override;
    void initializeProcessor(const LocalUrl &);

  private:
    auto readAudio_() -> audio_type;

    std::string filePath_{};
    VideoPlayer *player;
    AudioReader *reader;
    SignalProcessor *signalProcessor;
    TargetPlayer::Observer *listener_{};
    std::atomic<double> audioScale{1};
    std::atomic<bool> useFirstChannelOnly_{};
};
}

#endif
