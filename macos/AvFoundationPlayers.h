#ifndef AV_SPEECH_IN_NOISE_MACOS_MAIN_AVFOUNDATIONPLAYERS_H_
#define AV_SPEECH_IN_NOISE_MACOS_MAIN_AVFOUNDATIONPLAYERS_H_

#include <av-speech-in-noise/core/RunningATest.hpp>
#include <av-speech-in-noise/core/AudioRecording.hpp>
#include <av-speech-in-noise/player/MaskerPlayerImpl.hpp>
#include <av-speech-in-noise/player/TargetPlayerImpl.hpp>
#include <av-speech-in-noise/player/AudioReaderSimplified.hpp>

#import <CoreMedia/CoreMedia.h>
#import <MediaToolbox/MediaToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <AppKit/AppKit.h>

#include <vector>

@class VideoPlayerActions;

namespace av_speech_in_noise {
auto loadAudioDevices() -> std::vector<AudioObjectID>;

class AvFoundationAudioRecorder : public AudioRecorder {
  public:
    void initialize(const LocalUrl &url) override;
    void start() override;
    void stop() override;

  private:
    AVAudioRecorder *audioRecorder;
};

class AvFoundationBufferedAudioReader : public BufferedAudioReader {
  public:
    explicit AvFoundationBufferedAudioReader(const LocalUrl &);
    auto channel(gsl::index) -> std::vector<float> override;
    auto channels() -> gsl::index override;

  private:
    // order dependent initialization
    AVAudioFile *file;
    AVAudioPCMBuffer *buffer;
};

class AvFoundationBufferedAudioReaderFactory
    : public BufferedAudioReader::Factory {
  public:
    auto make(const LocalUrl &)
        -> std::shared_ptr<BufferedAudioReader> override;
};

class AvFoundationVideoPlayer : public VideoPlayer {
  public:
    AvFoundationVideoPlayer(NSView *, std::vector<AudioObjectID> audioDevices);
    void playbackComplete();
    void play() override;
    void playAt(const PlayerTimeWithDelay &) override;
    void loadFile(std::string filePath) override;
    void setDevice(int index) override;
    void hide() override;
    void show() override;
    void attach(Observer *) override;
    auto deviceCount() -> int override;
    auto deviceDescription(int index) -> std::string override;
    auto playing() -> bool override;
    void subscribeToPlaybackCompletion() override;
    auto durationSeconds() -> double override;
    void preRoll() override;

  private:
    void addPlayerLayer();
    void showWindow();
    void prepareWindow();
    void schedulePlaybackCompletion();
    void centerVideo();
    void resizeVideo();
    void prepareVideo();
    static void prepareTap(MTAudioProcessingTapRef tap, CMItemCount maxFrames,
        const AudioStreamBasicDescription *processingFormat);
    void prepareTap_(MTAudioProcessingTapRef tap, CMItemCount maxFrames,
        const AudioStreamBasicDescription *processingFormat);
    static void processTap(MTAudioProcessingTapRef tap,
        CMItemCount numberFrames, MTAudioProcessingTapFlags flags,
        AudioBufferList *bufferListInOut, CMItemCount *numberFramesOut,
        MTAudioProcessingTapFlags *flagsOut);
    void processTap_(CMItemCount numberFrames, MTAudioProcessingTapFlags flags,
        AudioBufferList *bufferListInOut, CMItemCount *numberFramesOut,
        MTAudioProcessingTapFlags *flagsOut);

    std::vector<gsl::span<float>> audio;
    std::vector<AudioObjectID> audioDevices;
    MTAudioProcessingTapRef tap{};
    VideoPlayerActions *actions;
    NSView *view;
    AVPlayer *player;
    AVPlayerLayer *playerLayer;
    NSLayoutConstraint *widthConstraint;
    NSLayoutConstraint *heightConstraint;
    Observer *listener_{};
};

class AvFoundationAudioPlayer : public AudioPlayer {
  public:
    AvFoundationAudioPlayer(std::vector<AudioObjectID> audioDevices);
    AvFoundationAudioPlayer(const AvFoundationAudioPlayer &) = delete;
    auto operator=(const AvFoundationAudioPlayer &)
        -> AvFoundationAudioPlayer & = delete;
    AvFoundationAudioPlayer(AvFoundationAudioPlayer &&) = delete;
    auto operator=(AvFoundationAudioPlayer &&)
        -> AvFoundationAudioPlayer & = delete;
    ~AvFoundationAudioPlayer() override;
    void attach(Observer *) override;
    void loadFile(std::string filePath) override;
    auto deviceCount() -> int override;
    auto deviceDescription(int index) -> std::string override;
    void setDevice(int index) override;
    auto playing() -> bool override;
    void play() override;
    auto sampleRateHz() -> double override;
    void stop() override;
    void timerCallback();
    auto outputDevice(int index) -> bool override;
    auto nanoseconds(PlayerTime) -> std::uintmax_t override;
    auto currentSystemTime() -> PlayerTime override;

  private:
    static auto AU_RenderCallback(void *inRefCon,
        AudioUnitRenderActionFlags *ioActionFlags,
        const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
        UInt32 inNumberFrames, AudioBufferList *ioData) -> OSStatus;
    auto audioBufferReady(AudioUnitRenderActionFlags *ioActionFlags,
        const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
        UInt32 inNumberFrames, AudioBufferList *ioData) -> OSStatus;

    std::vector<gsl::span<float>> audio;
    std::vector<AudioObjectID> audioDevices;
    Observer *listener_{};
    AudioUnit audioUnit{};
    double sampleRate_{};
};
}

#endif
