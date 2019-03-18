#include "AvFoundationPlayers.h"
#include <gsl/gsl>

CoreAudioDevice::CoreAudioDevice() {
    auto count = deviceCount_();
    devices.resize(count);
    UInt32 dataSize = count * sizeof(AudioDeviceID);
    auto address = propertyAddress(kAudioHardwarePropertyDevices);
    if (kAudioHardwareNoError !=
        AudioObjectGetPropertyData(
            kAudioObjectSystemObject,
            &address,
            0,
            nullptr,
            &dataSize,
            &devices[0]
        )
    )
        throw std::runtime_error{"Cannot determine audio device IDs."};
}

UInt32 CoreAudioDevice::deviceCount_() {
    auto address = propertyAddress(kAudioHardwarePropertyDevices);
    UInt32 dataSize{};
    if (kAudioHardwareNoError !=
        AudioObjectGetPropertyDataSize(
            kAudioObjectSystemObject,
            &address,
            0,
            nullptr,
            &dataSize
        )
    )
        throw std::runtime_error{"Cannot determine number of audio devices."};
    return dataSize / sizeof(AudioDeviceID);
}

AudioObjectPropertyAddress CoreAudioDevice::propertyAddress(AudioObjectPropertySelector s) {
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

std::string CoreAudioDevice::stringProperty(AudioObjectPropertySelector s, int device) {
    auto address = propertyAddress(s);
    CFStringRef deviceName{};
    UInt32 dataSize = sizeof(CFStringRef);
    if (kAudioHardwareNoError !=
        AudioObjectGetPropertyData(
            objectId(device),
            &address,
            0,
            nullptr,
            &dataSize,
            &deviceName
        )
    )
        throw std::runtime_error{"Cannot do something..."};
    char buffer[128];
    CFStringGetCString(deviceName, buffer, sizeof(buffer), kCFStringEncodingUTF8);
    return buffer;
}

AudioObjectID CoreAudioDevice::objectId(int device) {
    return devices.at(gsl::narrow<decltype(devices)::size_type>(device));
}

std::string CoreAudioDevice::uid(int device) {
    return stringProperty(kAudioDevicePropertyDeviceUID, device);
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

AvFoundationStimulusPlayer::AvFoundationStimulusPlayer() :
    actions{[StimulusPlayerActions alloc]},
    videoWindow{
        [[NSWindow alloc]
            initWithContentRect: NSMakeRect(400, 400, 0, 0)
            styleMask:NSWindowStyleMaskBorderless
            backing:NSBackingStoreBuffered
            defer:YES
        ]
    },
    player{[AVPlayer playerWithPlayerItem:nil]},
    playerLayer{[AVPlayerLayer playerLayerWithPlayer:player]}
{
    [videoWindow.contentView setWantsLayer:YES];
    [videoWindow.contentView.layer addSublayer:playerLayer];
    [videoWindow makeKeyAndOrderFront:nil];
    actions.controller = this;
}

void AvFoundationStimulusPlayer::subscribe(EventListener *listener_) {
    listener = listener_;
}

void AvFoundationStimulusPlayer::playbackComplete() {
    listener->playbackComplete();
}

void AvFoundationStimulusPlayer::play() {
    [player play];
}

void AvFoundationStimulusPlayer::loadFile(std::string filePath) {
    const auto asset = makeAvAsset(filePath);
    [player replaceCurrentItemWithPlayerItem:
        [AVPlayerItem playerItemWithAsset:asset]];
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

void AvFoundationStimulusPlayer::setDevice(int index) {
    auto uid_ = device.uid(index);
    player.audioOutputDeviceUniqueID = [
        NSString stringWithCString:uid_.c_str()
        encoding:[NSString defaultCStringEncoding]
    ];
}

@implementation StimulusPlayerActions
@synthesize controller;
- (void)playbackComplete {
    controller->playbackComplete();
}
@end


void AvFoundationAudioPlayer::init(
    MTAudioProcessingTapRef,
    void *clientInfo,
    void **tapStorageOut
) {
    *tapStorageOut = clientInfo;
}

void AvFoundationAudioPlayer::finalize(MTAudioProcessingTapRef)
{
}

void AvFoundationAudioPlayer::prepare(
    MTAudioProcessingTapRef tap,
    CMItemCount,
    const AudioStreamBasicDescription *description
) {
    auto self = static_cast<AvFoundationAudioPlayer *>(
        MTAudioProcessingTapGetStorage(tap)
    );
    self->audio.resize(description->mChannelsPerFrame);
    self->sampleRate_ = description->mSampleRate;
}

void AvFoundationAudioPlayer::unprepare(MTAudioProcessingTapRef)
{
}

void AvFoundationAudioPlayer::process(
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

    auto self = static_cast<AvFoundationAudioPlayer *>(
        MTAudioProcessingTapGetStorage(tap)
    );
    if (self->audio.size() != bufferListInOut->mNumberBuffers)
        return;
    
    for (UInt32 j = 0; j < bufferListInOut->mNumberBuffers; ++j)
        self->audio[j] = {
            static_cast<float *>(bufferListInOut->mBuffers[j].mData),
            numberFrames
        };
    self->listener->fillAudioBuffer(self->audio);
}

void AvFoundationAudioPlayer::subscribe(EventListener *listener_) {
    listener = listener_;
}

void AvFoundationAudioPlayer::loadFile(std::string filePath) {
    const auto asset = makeAvAsset(filePath);
    const auto processing =
        [AVMutableAudioMixInputParameters audioMixInputParametersWithTrack:
            [asset tracksWithMediaType:AVMediaTypeAudio].firstObject];
    processing.audioTapProcessor = tap;
    const auto audioMix = [AVMutableAudioMix audioMix];
    audioMix.inputParameters = @[processing];
    const auto playerItem = [AVPlayerItem playerItemWithAsset:asset];
    playerItem.audioMix = audioMix;
    [player replaceCurrentItemWithPlayerItem:
        [AVPlayerItem playerItemWithAsset:asset]];
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
    player{[AVPlayer playerWithPlayerItem:nil]}
{
    MTAudioProcessingTapCallbacks callbacks;
    callbacks.version = kMTAudioProcessingTapCallbacksVersion_0;
    callbacks.clientInfo = this;
    callbacks.init = init;
    callbacks.prepare = prepare;
    callbacks.process = process;
    callbacks.unprepare = unprepare;
    callbacks.finalize = finalize;

    if (
        MTAudioProcessingTapCreate(
            kCFAllocatorDefault,
            &callbacks,
            kMTAudioProcessingTapCreationFlag_PostEffects,
            &tap
        ) ||
        !tap
    )
        throw std::runtime_error{"Unable to create the AudioProcessingTap"};
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




