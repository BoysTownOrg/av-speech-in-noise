#ifndef APPS_AVFOUNDATIONPLAYERS_H_
#define APPS_AVFOUNDATIONPLAYERS_H_

#include <recognition-test/RecognitionTestModel.hpp>
#include <stimulus-players/MaskerPlayerImpl.hpp>
#include <stimulus-players/TargetPlayerImpl.hpp>
#include <stimulus-players/AudioReaderImpl.hpp>
#import <Cocoa/Cocoa.h>
#import <AVFoundation/AVFoundation.h>
#include <vector>

class CoreAudioDevices {
    std::vector<AudioObjectID> devices{};

  public:
    CoreAudioDevices();
    auto deviceCount() -> int;
    auto description(int device) -> std::string;
    auto uid(int device) -> std::string;
    auto outputDevice(int device) -> bool;
    auto objectId(int device) -> AudioObjectID;

  private:
    void loadDevices();
    auto stringProperty(AudioObjectPropertySelector, int device) -> std::string;
};

class CoreAudioBuffer : public stimulus_players::AudioBuffer {
    AudioBufferList audioBufferList{};
    CMItemCount frames{};
    CMSampleBufferRef sampleBuffer;
    // possibly critical: initialize blockBuffer to null
    CMBlockBufferRef blockBuffer{};

  public:
    explicit CoreAudioBuffer(AVAssetReaderTrackOutput *);
    ~CoreAudioBuffer() override;
    void set(CMSampleBufferRef);
    auto channels() -> int override;
    auto channel(int) -> std::vector<int> override;
    auto empty() -> bool override;
};

class CoreAudioBufferedReader : public stimulus_players::BufferedAudioReader {
    AVAssetReaderTrackOutput *trackOutput{};

  public:
    void loadFile(std::string) override;
    auto failed() -> bool override;
    auto readNextBuffer()
        -> std::shared_ptr<stimulus_players::AudioBuffer> override;
    auto minimumPossibleSample() -> int override;
    auto sampleRateHz() -> double;
};

class AvFoundationVideoPlayer;

@interface VideoPlayerActions : NSObject
@property AvFoundationVideoPlayer *controller;
- (void)playbackComplete;
@end

class AvFoundationVideoPlayer : public stimulus_players::VideoPlayer {
    std::vector<gsl::span<float>> audio_;
    MTAudioProcessingTapRef tap{};
    CoreAudioDevices device{};
    VideoPlayerActions *actions;
    NSWindow *videoWindow;
    AVPlayer *player;
    AVPlayerLayer *playerLayer;
    NSScreen *screen;
    EventListener *listener_{};

  public:
    explicit AvFoundationVideoPlayer(NSScreen *);
    void playbackComplete();
    void setSampleRate(double) {}
    auto audio() -> std::vector<gsl::span<float>> & { return audio_; }
    void fillAudioBuffer();
    void play() override;
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
};

class AvFoundationAudioPlayer : public stimulus_players::AudioPlayer {
    std::vector<gsl::span<float>> audio_;
    CoreAudioDevices device{};
    MTAudioProcessingTapRef tap{};
    EventListener *listener_{};
    AVPlayer *player;
    AudioUnit audioUnit{};
    double sampleRate_{};

  public:
    AvFoundationAudioPlayer();
    ~AvFoundationAudioPlayer() override;
    void setSampleRate(double x) { sampleRate_ = x; }
    auto audio() -> std::vector<gsl::span<float>> & { return audio_; }
    void fillAudioBuffer();
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
    auto durationSeconds() -> double override;
    void seekSeconds(double) override;
};

#endif
