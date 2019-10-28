#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_TARGETPLAYERIMPL_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_TARGETPLAYERIMPL_HPP_

#include "AudioReader.hpp"
#include <recognition-test/RecognitionTestModel.hpp>
#include <recognition-test/RecognitionTestModel_.hpp>
#include <gsl/gsl>
#include <vector>
#include <string>

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
    virtual bool playing() = 0;
    virtual int deviceCount() = 0;
    virtual std::string deviceDescription(int index) = 0;
    virtual void setDevice(int index) = 0;
    virtual double durationSeconds() = 0;
};

class TargetPlayerImpl : public av_speech_in_noise::TargetPlayer,
                         public VideoPlayer::EventListener {
    std::string filePath_{};
    std::atomic<double> audioScale{};
    VideoPlayer *player;
    AudioReader *reader;
    TargetPlayer::EventListener *listener_{};

  public:
    TargetPlayerImpl(VideoPlayer *, AudioReader *);
    void subscribe(TargetPlayer::EventListener *) override;
    void play() override;
    void loadFile(std::string filePath) override;
    void hideVideo() override;
    void showVideo() override;
    double rms() override;
    void setLevel_dB(double) override;
    void setAudioDevice(std::string) override;
    bool playing() override;
    void subscribeToPlaybackCompletion() override;
    double durationSeconds() override;
    void playbackComplete() override;
    void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) override;
    std::vector<std::string> audioDevices();

  private:
    std::vector<std::vector<float>> readAudio_();
};
}

#endif
