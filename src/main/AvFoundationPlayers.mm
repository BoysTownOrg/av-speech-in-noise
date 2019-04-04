#include "AvFoundationPlayers.h"
#include <gsl/gsl>
#include <limits>

CoreAudioDevice::CoreAudioDevice() {
    loadDevices();
}

void CoreAudioDevice::loadDevices() {
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

UInt32 CoreAudioDevice::deviceCount_() {
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

AudioObjectPropertyAddress CoreAudioDevice::globalAddress(
    AudioObjectPropertySelector s
) {
    return {
        s,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };
}

int CoreAudioDevice::deviceCount() {
    return gsl::narrow<int>(devices.size());
}

std::string CoreAudioDevice::description(int device) {
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

std::string CoreAudioDevice::stringProperty(
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

AudioObjectID CoreAudioDevice::objectId(int device) {
    return devices[device];
}

std::string CoreAudioDevice::uid(int device) {
    return stringProperty(kAudioDevicePropertyDeviceUID, device);
}

bool CoreAudioDevice::outputDevice(int device) {
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


static AVURLAsset *makeAvAsset(std::string filePath) {
    const auto url = [NSURL URLWithString:
        [NSString stringWithFormat:@"file://%@/",
            [
                [NSString stringWithCString:
                    filePath.c_str()
                    encoding:[NSString defaultCStringEncoding]
                ]
                stringByAddingPercentEncodingWithAllowedCharacters:
                    [NSCharacterSet URLQueryAllowedCharacterSet]
            ]
        ]
    ];
    return [AVURLAsset URLAssetWithURL:url options:nil];
}

//https://stackoverflow.com/questions/4972677/reading-audio-samples-via-avassetreader
static std::vector<std::vector<float>> readAudio(std::string filePath) {
    const auto asset = makeAvAsset(filePath);
    auto reader = [[AVAssetReader alloc]
        initWithAsset:asset
        error:nil
    ];
    auto track = [
        [asset tracksWithMediaType:AVMediaTypeAudio] firstObject
    ];
    auto trackOutput = [AVAssetReaderTrackOutput
        assetReaderTrackOutputWithTrack:track
        outputSettings:@{
            AVFormatIDKey : [NSNumber numberWithInt:kAudioFormatLinearPCM]
        }
    ];
    [reader addOutput:trackOutput];
    [reader startReading];
    auto sampleBuffer = [trackOutput copyNextSampleBuffer];
    std::vector<std::vector<float>> audio{};
    while (sampleBuffer) {
        auto frames = CMSampleBufferGetNumSamples(sampleBuffer);
        AudioBufferList audioBufferList;
        CMBlockBufferRef blockBuffer{};
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

        audio.resize(audioBufferList.mNumberBuffers);
        for (UInt32 channel{}; channel < audioBufferList.mNumberBuffers; ++channel) {
            auto data = static_cast<SInt16 *>(audioBufferList.mBuffers[channel].mData);
            constexpr auto minSample = std::numeric_limits<SInt16>::min();
            for (int i{}; i < frames; ++i) {
                float x = data[i];
                audio.at(channel).push_back(-x/minSample);
            }
        }
        
        CFRelease(blockBuffer);
        CFRelease(sampleBuffer);
        sampleBuffer = [trackOutput copyNextSampleBuffer];
    }
    return audio;
}


static void init(
    MTAudioProcessingTapRef,
    void *clientInfo,
    void **tapStorageOut
) {
    *tapStorageOut = clientInfo;
}

static void finalize(MTAudioProcessingTapRef)
{
}

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

static void unprepare(MTAudioProcessingTapRef)
{
}

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

    if (
        MTAudioProcessingTapCreate(
            kCFAllocatorDefault,
            &callbacks,
            kMTAudioProcessingTapCreationFlag_PostEffects,
            tap
        ) ||
        !*tap
    )
        throw std::runtime_error{"Unable to create the AudioProcessingTap"};
}

static void loadItemFromFileWithAudioProcessing(
    std::string filePath,
    AVPlayer *player,
    MTAudioProcessingTapRef tap
) {
    const auto asset = makeAvAsset(filePath);
    const auto processing =
        [AVMutableAudioMixInputParameters audioMixInputParametersWithTrack:
            [asset tracksWithMediaType:AVMediaTypeAudio].firstObject];
    processing.audioTapProcessor = tap;
    const auto audioMix = [AVMutableAudioMix audioMix];
    audioMix.inputParameters = @[processing];
    const auto playerItem = [AVPlayerItem playerItemWithAsset:asset];
    playerItem.audioMix = audioMix;
    [player replaceCurrentItemWithPlayerItem: playerItem];
}

AvFoundationVideoPlayer::AvFoundationVideoPlayer() :
    actions{[StimulusPlayerActions alloc]},
    videoWindow{
        [[NSWindow alloc]
            initWithContentRect: NSMakeRect(400, 450, 0, 0)
            styleMask:NSWindowStyleMaskBorderless
            backing:NSBackingStoreBuffered
            defer:YES
        ]
    },
    player{[AVPlayer playerWithPlayerItem:nil]},
    playerLayer{[AVPlayerLayer playerLayerWithPlayer:player]}
{
    createAudioProcessingTap<AvFoundationVideoPlayer>(this, &tap);
    [videoWindow.contentView setWantsLayer:YES];
    [videoWindow.contentView.layer addSublayer:playerLayer];
    [videoWindow makeKeyAndOrderFront:nil];
    actions.controller = this;
}

void AvFoundationVideoPlayer::playbackComplete() {
    listener_->playbackComplete();
}

void AvFoundationVideoPlayer::play() {
    [player play];
}

void AvFoundationVideoPlayer::loadFile(std::string filePath) {
    loadItemFromFileWithAudioProcessing(filePath, player, tap);
    auto asset = [[player currentItem] asset];
    [videoWindow setContentSize:NSSizeFromCGSize(
        [asset tracksWithMediaType:AVMediaTypeVideo].firstObject.naturalSize)];
    [playerLayer setFrame:videoWindow.contentView.bounds];
    [NSNotificationCenter.defaultCenter addObserver:
        actions
        selector:@selector(playbackComplete)
        name:AVPlayerItemDidPlayToEndTimeNotification
        object:player.currentItem
    ];
}

void AvFoundationVideoPlayer::setDevice(int index) {
    auto uid_ = device.uid(index);
    player.audioOutputDeviceUniqueID = [
        NSString stringWithCString:uid_.c_str()
        encoding:[NSString defaultCStringEncoding]
    ];
}

void AvFoundationVideoPlayer::hide() {
    [videoWindow setIsVisible:NO];
}

void AvFoundationVideoPlayer::show() {
    [videoWindow setIsVisible:YES];
}

void AvFoundationVideoPlayer::subscribe(EventListener *e) {
    listener_ = e;
}

int AvFoundationVideoPlayer::deviceCount() {
    return device.deviceCount();
}

std::string AvFoundationVideoPlayer::deviceDescription(int index) {
    return device.description(index);
}


std::vector<std::vector<float>>
    AvFoundationVideoPlayer::readAudio(std::string filePath
) {
    return ::readAudio(filePath);
}

@implementation StimulusPlayerActions
@synthesize controller;
- (void)playbackComplete {
    controller->playbackComplete();
}
@end

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
    auto uid_ = device.uid(index);
    player.audioOutputDeviceUniqueID = [
        NSString stringWithCString:uid_.c_str()
        encoding:[NSString defaultCStringEncoding]
    ];
}

AvFoundationAudioPlayer::AvFoundationAudioPlayer() :
    player{[AVPlayer playerWithPlayerItem:nil]},
    scheduler{[CallbackScheduler alloc]}
{
    scheduler.controller = this;
    createAudioProcessingTap<AvFoundationAudioPlayer>(this, &tap);
}

bool AvFoundationAudioPlayer::playing() {
    return player.timeControlStatus == AVPlayerTimeControlStatusPlaying;
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

std::vector<std::vector<float>>
    AvFoundationAudioPlayer::readAudio(std::string filePath
) {
    return ::readAudio(filePath);
}

@implementation CallbackScheduler
@synthesize controller;

- (void)scheduleCallbackAfterSeconds:(double)x {
    [NSTimer scheduledTimerWithTimeInterval:
        x
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

bool AvFoundationAudioPlayer::outputDevice(int index) { 
    return device.outputDevice(index);
}
