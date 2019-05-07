#include "AvFoundationPlayers.h"
#include "common-objc.h"
#include <gsl/gsl>
#include <limits>

static OSStatus getPropertyDataSize(
    AudioObjectID id_,
    const AudioObjectPropertyAddress *address,
    UInt32 *outDataSize
) {
    return AudioObjectGetPropertyDataSize(
        id_,
        address,
        0,
        nullptr,
        outDataSize
    );
}

static OSStatus getPropertyData(
    AudioObjectID id_,
    const AudioObjectPropertyAddress *address,
    UInt32 *dataSize,
    void *out_
) {
    return AudioObjectGetPropertyData(
        id_,
        address,
        0,
        nullptr,
        dataSize,
        out_
    );
}

template<typename T>
std::vector<T> loadPropertyData(
    AudioObjectID id_,
    const AudioObjectPropertyAddress *address
) {
    UInt32 dataSize{};
    getPropertyDataSize(
        id_,
        address,
        &dataSize
    );
    std::vector<T> data(dataSize / sizeof(T));
    if (!data.empty())
        getPropertyData(
            id_,
            address,
            &dataSize,
            &data.front()
        );
    return data;
}

// https://stackoverflow.com/questions/4575408/audioobjectgetpropertydata-to-get-a-list-of-input-devices
// http://fdiv.net/2008/08/12/nssound-setplaybackdeviceidentifier-coreaudio-output-device-enumeration
CoreAudioDevices::CoreAudioDevices() {
    loadDevices();
}

void CoreAudioDevices::loadDevices() {
    auto address = globalAddress(kAudioHardwarePropertyDevices);
    devices = loadPropertyData<AudioDeviceID>(kAudioObjectSystemObject, &address);
}

AudioObjectPropertyAddress CoreAudioDevices::globalAddress(
    AudioObjectPropertySelector s
) {
    return masterAddress(s, kAudioObjectPropertyScopeGlobal);
}

AudioObjectPropertyAddress CoreAudioDevices::masterAddress(
    AudioObjectPropertySelector selector,
    AudioObjectPropertyScope scope
) {
    return {
        selector,
        scope,
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
    ::getPropertyData(
        objectId(device),
        &address,
        &dataSize,
        &deviceName
    );
    return toString(deviceName);
}

AudioObjectID CoreAudioDevices::objectId(int device) {
    return devices.at(device);
}

std::string CoreAudioDevices::uid(int device) {
    return stringProperty(kAudioDevicePropertyDeviceUID, device);
}

bool CoreAudioDevices::outputDevice(int device) {
    auto address = masterAddress(
        kAudioDevicePropertyStreamConfiguration,
        kAudioObjectPropertyScopeOutput
    );
    auto bufferLists = loadPropertyData<AudioBufferList>(objectId(device), &address);
    for (auto list : bufferLists)
        for(UInt32 j = 0; j < list.mNumberBuffers; ++j)
            if (list.mBuffers[j].mNumberChannels != 0)
                return true;
    return false;
}


class AvAssetFacade {
    AVAsset *asset;
public:
    explicit AvAssetFacade(std::string filePath) :
        asset{makeAvAsset(std::move(filePath))} {}
    
    explicit AvAssetFacade(AVAsset *asset) : asset{asset} {}
    
    AVAssetTrack *audioTrack() {
        return firstTrack(AVMediaTypeAudio);
    }
    
    AVAssetTrack *videoTrack() {
        return firstTrack(AVMediaTypeVideo);
    }
    
    AVAsset *get() {
        return asset;
    }
    
private:
    AVAssetTrack *firstTrack(AVMediaType mediaType) {
        return [asset tracksWithMediaType:mediaType].firstObject;
    }
    
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

// A better design could throw if constructor cannot allocate buffer(s)...
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
    
    // assetReaderTrackOutputWithTrack throws if track is nil...
    // I do not handle the error here but by querying failed method.
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

std::shared_ptr<stimulus_players::AudioBuffer>
    CoreAudioBufferedReader::readNextBuffer()
{
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

AvFoundationVideoPlayer::AvFoundationVideoPlayer(NSScreen *screen) :
    actions{[VideoPlayerActions alloc]},
    videoWindow{[[NSWindow alloc]
        initWithContentRect: NSMakeRect(0, 0, 0, 0)
        styleMask:NSWindowStyleMaskBorderless
        backing:NSBackingStoreBuffered
        defer:YES
    ]},
    player{[AVPlayer playerWithPlayerItem:nil]},
    playerLayer{[AVPlayerLayer playerLayerWithPlayer:player]},
    screen{screen}
{
    createAudioProcessingTap<AvFoundationVideoPlayer>(this, &tap);
    prepareWindow();
    actions.controller = this;
}

void AvFoundationVideoPlayer::prepareWindow() {
    addPlayerLayer();
    showWindow();
}

void AvFoundationVideoPlayer::addPlayerLayer() {
    [videoWindow.contentView setWantsLayer:YES];
    [videoWindow.contentView.layer addSublayer:playerLayer];
}

void AvFoundationVideoPlayer::showWindow() {
    [videoWindow makeKeyAndOrderFront:nil];
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
    centerVideo();
}

void AvFoundationVideoPlayer::resizeVideo() {
    AvAssetFacade asset{player.currentItem.asset};
    [videoWindow setContentSize:
        NSSizeFromCGSize(asset.videoTrack().naturalSize)
    ];
    [playerLayer setFrame:videoWindow.contentView.bounds];
}

void AvFoundationVideoPlayer::centerVideo() {
    auto screenFrame = [screen frame];
    auto screenOrigin = screenFrame.origin;
    auto screenSize = screenFrame.size;
    auto windowSize = videoWindow.frame.size;
    auto videoLeadingEdge =
        screenOrigin.x +
        (screenSize.width - windowSize.width) / 2;
    auto videoBottomEdge =
        screenOrigin.y +
        (screenSize.height - windowSize.height) / 2;
    [videoWindow setFrameOrigin:NSMakePoint(videoLeadingEdge, videoBottomEdge)];
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

void AvFoundationVideoPlayer::fillAudioBuffer()  {
    listener_->fillAudioBuffer(audio_);
}

static bool playing(AVPlayer *player) {
    return player.timeControlStatus == AVPlayerTimeControlStatusPlaying;
}

bool AvFoundationVideoPlayer::playing() {
    return ::playing(player);
}

// https://stackoverflow.com/questions/19059321/ios-7-avplayer-avplayeritem-duration-incorrect-in-ios-7
// "It appears the duration value isn't always immediately available
// from an AVPlayerItem but it seems to work fine with an AVAsset immediately."
static Float64 durationSeconds_(AVPlayer *player) {
    return CMTimeGetSeconds(player.currentItem.asset.duration);
}

double AvFoundationVideoPlayer::durationSeconds() {
    return durationSeconds_(player);
}

@implementation VideoPlayerActions
@synthesize controller;
- (void)playbackComplete {
    controller->playbackComplete();
}
@end


AvFoundationAudioPlayer::AvFoundationAudioPlayer() :
    player{[AVPlayer playerWithPlayerItem:nil]}
{
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

bool AvFoundationAudioPlayer::outputDevice(int index) {
    return device.outputDevice(index);
}

void AvFoundationAudioPlayer::fillAudioBuffer() {
    listener_->fillAudioBuffer(audio_);
}

double AvFoundationAudioPlayer::durationSeconds() {
    return durationSeconds_(player);
}

// https://warrenmoore.net/understanding-cmtime
// "Apple recommends a timescale of 600 for video,
// with the explanation that 600 is a multiple of the
// common video framerates (24, 25, and 30 FPS)."
void AvFoundationAudioPlayer::seekSeconds(double x) {
    auto timescale = 600;
    [player seekToTime:CMTimeMakeWithSeconds(x, timescale)];
}
