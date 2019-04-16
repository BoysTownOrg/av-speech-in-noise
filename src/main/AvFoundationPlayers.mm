#include "AvFoundationPlayers.h"
#include "common-objc.h"
#include <gsl/gsl>
#include <limits>

CoreAudioDevices::CoreAudioDevices() {
    loadDevices();
}

void CoreAudioDevices::loadDevices() {
    auto count = deviceCount_();
    devices.resize(count);
    UInt32 dataSize = count * sizeof(AudioDeviceID);
    auto address = globalAddress(kAudioHardwarePropertyDevices);
    AudioObjectGetPropertyData(
        kAudioObjectSystemObject,
        &address,
        0,
        nullptr,
        &dataSize,
        &devices[0]
    );
}

UInt32 CoreAudioDevices::deviceCount_() {
    auto address = globalAddress(kAudioHardwarePropertyDevices);
    UInt32 dataSize{};
    AudioObjectGetPropertyDataSize(
        kAudioObjectSystemObject,
        &address,
        0,
        nullptr,
        &dataSize
    );
    return dataSize / sizeof(AudioDeviceID);
}

AudioObjectPropertyAddress CoreAudioDevices::globalAddress(
    AudioObjectPropertySelector s
) {
    return {
        s,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };
}

int CoreAudioDevices::deviceCount() {
    return gsl::narrow<int>(devices.size());
}

std::string CoreAudioDevices::description(int device) {
    return stringProperty(kAudioObjectPropertyName, device);
}

static std::string toString(CFStringRef deviceName) {
    char buffer[128];
    CFStringGetCString(
        deviceName,
        buffer,
        sizeof(buffer),
        kCFStringEncodingUTF8
    );
    return buffer;
}

std::string CoreAudioDevices::stringProperty(
    AudioObjectPropertySelector s,
    int device
) {
    auto address = globalAddress(s);
    CFStringRef deviceName{};
    UInt32 dataSize = sizeof(CFStringRef);
    AudioObjectGetPropertyData(
        objectId(device),
        &address,
        0,
        nullptr,
        &dataSize,
        &deviceName
    );
    return toString(deviceName);
}

AudioObjectID CoreAudioDevices::objectId(int device) {
    return devices[device];
}

std::string CoreAudioDevices::uid(int device) {
    return stringProperty(kAudioDevicePropertyDeviceUID, device);
}

bool CoreAudioDevices::outputDevice(int device) {
    AudioObjectPropertyAddress address {
        kAudioDevicePropertyStreamConfiguration,
        kAudioObjectPropertyScopeOutput,
        kAudioObjectPropertyElementMaster
    };
    AudioBufferList bufferList{};
    UInt32 dataSize = sizeof(AudioBufferList);
    AudioObjectGetPropertyData(
        objectId(device),
        &address,
        0,
        nullptr,
        &dataSize,
        &bufferList
    );
    return bufferList.mNumberBuffers != 0;
}

class AvAssetFacade {
    AVAsset *asset;
public:
    explicit AvAssetFacade(std::string filePath) :
        asset{makeAvAsset(std::move(filePath))} {}
    
    explicit AvAssetFacade(AVAsset *asset) : asset{asset} {}
    
    AVAssetTrack *audioTrack() {
        return [asset tracksWithMediaType:AVMediaTypeAudio].firstObject;
    }
    
    AVAssetTrack *videoTrack() {
        return [asset tracksWithMediaType:AVMediaTypeVideo].firstObject;
    }
    
    AVAsset *get() {
        return asset;
    }
    
private:
    AVURLAsset *makeAvAsset(std::string filePath) {
        const auto withPercents = [asNsString(std::move(filePath))
            stringByAddingPercentEncodingWithAllowedCharacters:
                NSCharacterSet.URLQueryAllowedCharacterSet
        ];
        const auto url = [NSURL URLWithString:
            [NSString stringWithFormat:@"file://%@/", withPercents]
        ];
        return [AVURLAsset URLAssetWithURL:url options:nil];
    }
};

//https://stackoverflow.com/questions/4972677/reading-audio-samples-via-avassetreader
CoreAudioBuffer::CoreAudioBuffer(AVAssetReaderTrackOutput *trackOutput) :
    sampleBuffer{[trackOutput copyNextSampleBuffer]}
{
    frames = CMSampleBufferGetNumSamples(sampleBuffer);
    CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(
        sampleBuffer,
        nullptr,
        &audioBufferList,
        sizeof(audioBufferList),
        nullptr,
        nullptr,
        kCMSampleBufferFlag_AudioBufferList_Assure16ByteAlignment,
        &blockBuffer
    );
}

CoreAudioBuffer::~CoreAudioBuffer() {
    if (blockBuffer)
        CFRelease(blockBuffer);
    if (sampleBuffer)
        CFRelease(sampleBuffer);
}

int CoreAudioBuffer::channels() {
    return audioBufferList.mNumberBuffers;
}

std::vector<int> CoreAudioBuffer::channel(int n) {
    std::vector<int> channel_{};
    auto data = static_cast<SInt16 *>(audioBufferList.mBuffers[n].mData);
    for (int i{}; i < frames; ++i)
        channel_.push_back(data[i]);
    return channel_;
}

bool CoreAudioBuffer::empty() {
    return frames == 0;
}

void CoreAudioBufferedReader::loadFile(std::string filePath) {
    AvAssetFacade asset{std::move(filePath)};
    auto reader = [[AVAssetReader alloc]
        initWithAsset:asset.get()
        error:nil
    ];
    auto track = asset.audioTrack();
    trackOutput = track == nil ? nil : [AVAssetReaderTrackOutput
        assetReaderTrackOutputWithTrack:track
        outputSettings:@{
            AVFormatIDKey : [NSNumber numberWithInt:kAudioFormatLinearPCM]
        }
    ];
    [reader addOutput:trackOutput];
    [reader startReading];
}

bool CoreAudioBufferedReader::failed() {
    return trackOutput == nil;
}

std::shared_ptr<stimulus_players::AudioBuffer> CoreAudioBufferedReader::readNextBuffer() {
    return std::make_shared<CoreAudioBuffer>(trackOutput);
}

int CoreAudioBufferedReader::minimumPossibleSample() {
    return std::numeric_limits<SInt16>::min();
}

static void init(
    MTAudioProcessingTapRef,
    void *clientInfo,
    void **tapStorageOut
) {
    *tapStorageOut = clientInfo;
}

static void finalize(MTAudioProcessingTapRef) {}

template<typename T>
static void prepare(
    MTAudioProcessingTapRef tap,
    CMItemCount,
    const AudioStreamBasicDescription *description
) {
    auto self = static_cast<T *>(
        MTAudioProcessingTapGetStorage(tap)
    );
    self->audio().resize(description->mChannelsPerFrame);
    self->setSampleRate(description->mSampleRate);
}

static void unprepare(MTAudioProcessingTapRef) {}

template<typename T>
static void process(
    MTAudioProcessingTapRef tap,
    CMItemCount numberFrames,
    MTAudioProcessingTapFlags,
    AudioBufferList *bufferListInOut,
    CMItemCount *numberFramesOut,
    MTAudioProcessingTapFlags *flagsOut
) {
    MTAudioProcessingTapGetSourceAudio(
        tap,
        numberFrames,
        bufferListInOut,
        flagsOut,
        nullptr,
        numberFramesOut
    );

    auto self = static_cast<T *>(
        MTAudioProcessingTapGetStorage(tap)
    );
    if (self->audio().size() != bufferListInOut->mNumberBuffers)
        return;
    
    for (UInt32 j = 0; j < bufferListInOut->mNumberBuffers; ++j)
        self->audio()[j] = {
            static_cast<float *>(bufferListInOut->mBuffers[j].mData),
            numberFrames
        };
    self->fillAudioBuffer();
}

template<typename T>
static void createAudioProcessingTap(
    void* CM_NULLABLE clientInfo,
    MTAudioProcessingTapRef *tap
) {
    MTAudioProcessingTapCallbacks callbacks;
    callbacks.version = kMTAudioProcessingTapCallbacksVersion_0;
    callbacks.clientInfo = clientInfo;
    callbacks.init = init;
    callbacks.prepare = prepare<T>;
    callbacks.process = process<T>;
    callbacks.unprepare = unprepare;
    callbacks.finalize = finalize;

    MTAudioProcessingTapCreate(
        kCFAllocatorDefault,
        &callbacks,
        kMTAudioProcessingTapCreationFlag_PostEffects,
        tap
    );
}

static AVPlayerItem *playerItemWithAudioProcessing(
    std::string filePath,
    MTAudioProcessingTapRef tap
) {
    AvAssetFacade asset{std::move(filePath)};
    const auto playerItem = [AVPlayerItem playerItemWithAsset:asset.get()];
    const auto audioMix = [AVMutableAudioMix audioMix];
    const auto processing = [AVMutableAudioMixInputParameters
        audioMixInputParametersWithTrack:asset.audioTrack()
    ];
    processing.audioTapProcessor = tap;
    audioMix.inputParameters = @[processing];
    playerItem.audioMix = audioMix;
    return playerItem;
}

static void loadItemFromFileWithAudioProcessing(
    std::string filePath,
    AVPlayer *player,
    MTAudioProcessingTapRef tap
) {
    const auto playerItem = playerItemWithAudioProcessing(
        std::move(filePath),
        tap
    );
    [player replaceCurrentItemWithPlayerItem:playerItem];
}

AvFoundationVideoPlayer::AvFoundationVideoPlayer() :
    actions{[VideoPlayerActions alloc]},
    videoWindow{[[NSWindow alloc]
        initWithContentRect: NSMakeRect(400, 450, 0, 0)
        styleMask:NSWindowStyleMaskBorderless
        backing:NSBackingStoreBuffered
        defer:YES
    ]},
    player{[AVPlayer playerWithPlayerItem:nil]},
    playerLayer{[AVPlayerLayer playerLayerWithPlayer:player]}
{
    createAudioProcessingTap<AvFoundationVideoPlayer>(this, &tap);
    [videoWindow.contentView setWantsLayer:YES];
    [videoWindow.contentView.layer addSublayer:playerLayer];
    [videoWindow makeKeyAndOrderFront:nil];
    actions.controller = this;
}

void AvFoundationVideoPlayer::subscribe(EventListener *e) {
    listener_ = e;
}

void AvFoundationVideoPlayer::play() {
    [player play];
}

void AvFoundationVideoPlayer::loadFile(std::string filePath) {
    loadItemFromFileWithAudioProcessing(std::move(filePath), player, tap);
    prepareVideo();
}

void AvFoundationVideoPlayer::prepareVideo() {
    resizeVideo();
}

void AvFoundationVideoPlayer::resizeVideo() {
    AvAssetFacade asset{player.currentItem.asset};
    [videoWindow setContentSize:NSSizeFromCGSize(asset.videoTrack().naturalSize)];
    [playerLayer setFrame:videoWindow.contentView.bounds];
}

void AvFoundationVideoPlayer::subscribeToPlaybackCompletion() {
    schedulePlaybackCompletion();
}

void AvFoundationVideoPlayer::schedulePlaybackCompletion() {
    [NSNotificationCenter.defaultCenter
        addObserver:actions
        selector:@selector(playbackComplete)
        name:AVPlayerItemDidPlayToEndTimeNotification
        object:player.currentItem
    ];
}

void AvFoundationVideoPlayer::playbackComplete() {
    listener_->playbackComplete();
}

void AvFoundationVideoPlayer::setDevice(int index) {
    player.audioOutputDeviceUniqueID = asNsString(device.uid(index));
}

void AvFoundationVideoPlayer::hide() {
    [videoWindow setIsVisible:NO];
}

void AvFoundationVideoPlayer::show() {
    [videoWindow setIsVisible:YES];
}

int AvFoundationVideoPlayer::deviceCount() {
    return device.deviceCount();
}

std::string AvFoundationVideoPlayer::deviceDescription(int index) {
    return device.description(index);
}

static bool playing(AVPlayer *player) {
    return player.timeControlStatus == AVPlayerTimeControlStatusPlaying;
}

bool AvFoundationVideoPlayer::playing() {
    return ::playing(player);
}

@implementation VideoPlayerActions
@synthesize controller;
- (void)playbackComplete {
    controller->playbackComplete();
}
@end


AvFoundationAudioPlayer::AvFoundationAudioPlayer() :
    player{[AVPlayer playerWithPlayerItem:nil]},
    scheduler{[CallbackScheduler alloc]}
{
    scheduler.controller = this;
    createAudioProcessingTap<AvFoundationAudioPlayer>(this, &tap);
}

void AvFoundationAudioPlayer::subscribe(EventListener *e) {
    listener_ = e;
}

void AvFoundationAudioPlayer::loadFile(std::string filePath) {
    loadItemFromFileWithAudioProcessing(filePath, player, tap);
}

int AvFoundationAudioPlayer::deviceCount() {
    return device.deviceCount();
}

std::string AvFoundationAudioPlayer::deviceDescription(int index) {
    return device.description(index);
}

void AvFoundationAudioPlayer::setDevice(int index) {
    player.audioOutputDeviceUniqueID = asNsString(device.uid(index));
}

bool AvFoundationAudioPlayer::playing() {
    return ::playing(player);
}

void AvFoundationAudioPlayer::play() {
    [player play];
}

double AvFoundationAudioPlayer::sampleRateHz() { 
    return sampleRate_;
}

void AvFoundationAudioPlayer::stop() { 
    [player pause];
}

void AvFoundationAudioPlayer::scheduleCallbackAfterSeconds(double x) {
    [scheduler scheduleCallbackAfterSeconds:x];
}

void AvFoundationAudioPlayer::timerCallback() {
    listener_->timerCallback();
}

bool AvFoundationAudioPlayer::outputDevice(int index) {
    return device.outputDevice(index);
}

@implementation CallbackScheduler
@synthesize controller;

- (void)scheduleCallbackAfterSeconds:(double)x {
    [NSTimer
        scheduledTimerWithTimeInterval:x
        target:self
        selector: @selector(timerCallback)
        userInfo:nil
        repeats:NO
    ];
}

- (void)timerCallback {
    controller->timerCallback();
}
@end
