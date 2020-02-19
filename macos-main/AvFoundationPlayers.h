#ifndef MACOS_MAIN_AVFOUNDATIONPLAYERS_H_
#define MACOS_MAIN_AVFOUNDATIONPLAYERS_H_

#include <recognition-test/RecognitionTestModel.hpp>
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <stimulus-players/TargetPlayerImpl.hpp>
#include <stimulus-players/AudioReaderImpl.hpp>
#import <CoreMedia/CoreMedia.h>
#import <MediaToolbox/MediaToolbox.h>
#import <AVFoundation/AVFoundation.h>
#import <AppKit/AppKit.h>
#include <vector>

@class VideoPlayerActions;

namespace stimulus_players {
class CoreAudioBuffer : public AudioBuffer {
  public:
    explicit CoreAudioBuffer(AVAssetReaderTrackOutput *);
    ~CoreAudioBuffer() override;
    void set(CMSampleBufferRef);
    auto channels() -> int override;
    auto channel(int) -> std::vector<int> override;
    auto empty() -> bool override;

  private:
    AudioBufferList audioBufferList{};
    CMItemCount frames{};
    CMSampleBufferRef sampleBuffer;
    // possibly critical: initialize blockBuffer to null
    CMBlockBufferRef blockBuffer{};
};

class CoreAudioBufferedReader : public BufferedAudioReader {
  public:
    void loadFile(std::string) override;
    auto failed() -> bool override;
    auto readNextBuffer() -> std::shared_ptr<AudioBuffer> override;
    auto minimumPossibleSample() -> int override;
    auto sampleRateHz() -> double;

  private:
    AVAssetReaderTrackOutput *trackOutput{};
};

class AvFoundationVideoPlayer : public VideoPlayer {
  public:
    explicit AvFoundationVideoPlayer(NSScreen *);
    void playbackComplete();
    void play() override;
    void playAt(const av_speech_in_noise::SystemTimeWithDelay &) override;
    void loadFile(std::string filePath) override;
    void setDevice(int index) override;
    void hide() override;
    void show() override;
    void subscribe(EventListener *) override;
    auto deviceCount() -> int override;
    auto deviceDescription(int index) -> std::string override;
    auto playing() -> bool override;
    void subscribeToPlaybackCompletion() override;
    auto durationSeconds() -> double override;

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
    MTAudioProcessingTapRef tap{};
    VideoPlayerActions *actions;
    NSWindow *videoWindow;
    AVPlayer *player;
    AVPlayerLayer *playerLayer;
    NSScreen *screen;
    EventListener *listener_{};
};

class AvFoundationAudioPlayer : public AudioPlayer {
  public:
    AvFoundationAudioPlayer();
    ~AvFoundationAudioPlayer() override;
    void subscribe(EventListener *) override;
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
    auto nanoseconds(av_speech_in_noise::system_time) -> std::uintmax_t override;

  private:
    static auto AU_RenderCallback(void *inRefCon,
        AudioUnitRenderActionFlags *ioActionFlags,
        const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
        UInt32 inNumberFrames, AudioBufferList *ioData) -> OSStatus;
    auto audioBufferReady(AudioUnitRenderActionFlags *ioActionFlags,
        const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
        UInt32 inNumberFrames, AudioBufferList *ioData) -> OSStatus;

    std::vector<gsl::span<float>> audio;
    EventListener *listener_{};
    AudioUnit audioUnit{};
    double sampleRate_{};
};
}

#endif
