#include "AvFoundationPlayers.h"
#include "common-objc.h"
#include <gsl/gsl>
#include <limits>

static auto getPropertyDataSize(AudioObjectID id_,
    const AudioObjectPropertyAddress *address, UInt32 *outDataSize)
    -> OSStatus {
    return AudioObjectGetPropertyDataSize(
        id_, address, 0, nullptr, outDataSize);
}

static auto getPropertyData(AudioObjectID id_,
    const AudioObjectPropertyAddress *address, UInt32 *dataSize, void *out_)
    -> OSStatus {
    return AudioObjectGetPropertyData(id_, address, 0, nullptr, dataSize, out_);
}

template <typename T>
auto loadPropertyData(AudioObjectID id_,
    const AudioObjectPropertyAddress *address) -> std::vector<T> {
    UInt32 dataSize{};
    getPropertyDataSize(id_, address, &dataSize);
    std::vector<T> data(dataSize / sizeof(T));
    if (!data.empty())
        getPropertyData(id_, address, &dataSize, &data.front());
    return data;
}

// https://stackoverflow.com/questions/4575408/audioobjectgetpropertydata-to-get-a-list-of-input-devices
// http://fdiv.net/2008/08/12/nssound-setplaybackdeviceidentifier-coreaudio-output-device-enumeration
CoreAudioDevices::CoreAudioDevices() { loadDevices(); }

void CoreAudioDevices::loadDevices() {
    auto address = globalAddress(kAudioHardwarePropertyDevices);
    devices =
        loadPropertyData<AudioDeviceID>(kAudioObjectSystemObject, &address);
}

auto CoreAudioDevices::globalAddress(AudioObjectPropertySelector s)
    -> AudioObjectPropertyAddress {
    return masterAddress(s, kAudioObjectPropertyScopeGlobal);
}

auto CoreAudioDevices::masterAddress(AudioObjectPropertySelector selector,
    AudioObjectPropertyScope scope) -> AudioObjectPropertyAddress {
    return {selector, scope, kAudioObjectPropertyElementMaster};
}

auto CoreAudioDevices::deviceCount() -> int {
    return gsl::narrow<int>(devices.size());
}

auto CoreAudioDevices::description(int device) -> std::string {
    return stringProperty(kAudioObjectPropertyName, device);
}

static auto toString(CFStringRef deviceName) -> std::string {
    std::string buffer(128, '\0');
    CFStringGetCString(
        deviceName, buffer.data(), buffer.size(), kCFStringEncodingUTF8);
    buffer.shrink_to_fit();
    return buffer;
}

auto CoreAudioDevices::stringProperty(AudioObjectPropertySelector s, int device)
    -> std::string {
    auto address = globalAddress(s);
    auto data = loadPropertyData<CFStringRef>(objectId(device), &address);
    if (data.empty())
        return {};

    return toString(data.front());
}

auto CoreAudioDevices::objectId(int device) -> AudioObjectID {
    return devices.at(device);
}

auto CoreAudioDevices::uid(int device) -> std::string {
    return stringProperty(kAudioDevicePropertyDeviceUID, device);
}

auto CoreAudioDevices::outputDevice(int device) -> bool {
    auto address = masterAddress(kAudioDevicePropertyStreamConfiguration,
        kAudioObjectPropertyScopeOutput);
    auto bufferLists =
        loadPropertyData<AudioBufferList>(objectId(device), &address);
    for (auto list : bufferLists)
        for (UInt32 j = 0; j < list.mNumberBuffers; ++j)
            if (list.mBuffers[j].mNumberChannels != 0)
                return true;
    return false;
}

class AvAssetFacade {
    AVAsset *asset;

  public:
    explicit AvAssetFacade(std::string filePath)
        : asset{makeAvAsset(std::move(filePath))} {}

    explicit AvAssetFacade(AVAsset *asset) : asset{asset} {}

    auto audioTrack() -> AVAssetTrack * { return firstTrack(AVMediaTypeAudio); }

    auto videoTrack() -> AVAssetTrack * { return firstTrack(AVMediaTypeVideo); }

    auto get() -> AVAsset * { return asset; }

  private:
    auto firstTrack(AVMediaType mediaType) -> AVAssetTrack * {
        return [asset tracksWithMediaType:mediaType].firstObject;
    }

    static auto makeAvAsset(std::string filePath) -> AVURLAsset * {
        const auto withPercents = [asNsString(std::move(filePath))
            stringByAddingPercentEncodingWithAllowedCharacters:
                NSCharacterSet.URLQueryAllowedCharacterSet];
        const auto url =
            [NSURL URLWithString:[NSString stringWithFormat:@"file://%@/",
                                           withPercents]];
        return [AVURLAsset URLAssetWithURL:url options:nil];
    }
};

// https://stackoverflow.com/questions/4972677/reading-audio-samples-via-avassetreader
CoreAudioBuffer::CoreAudioBuffer(AVAssetReaderTrackOutput *trackOutput)
    : sampleBuffer{[trackOutput copyNextSampleBuffer]} {
    frames = CMSampleBufferGetNumSamples(sampleBuffer);
    CMSampleBufferGetAudioBufferListWithRetainedBlockBuffer(sampleBuffer,
        nullptr, &audioBufferList, sizeof(audioBufferList), nullptr, nullptr,
        kCMSampleBufferFlag_AudioBufferList_Assure16ByteAlignment,
        &blockBuffer);
}

// A better design could throw if constructor cannot allocate buffer(s)...
CoreAudioBuffer::~CoreAudioBuffer() {
    if (blockBuffer != nullptr)
        CFRelease(blockBuffer);
    if (sampleBuffer != nullptr)
        CFRelease(sampleBuffer);
}

auto CoreAudioBuffer::channels() -> int {
    return audioBufferList.mNumberBuffers;
}

auto CoreAudioBuffer::channel(int n) -> std::vector<int> {
    std::vector<int> channel_{};
    auto data = static_cast<SInt16 *>(audioBufferList.mBuffers[n].mData);
    for (int i{}; i < frames; ++i)
        channel_.push_back(data[i]);
    return channel_;
}

auto CoreAudioBuffer::empty() -> bool { return frames == 0; }

void CoreAudioBufferedReader::loadFile(std::string filePath) {
    AvAssetFacade asset{std::move(filePath)};
    auto reader = [[AVAssetReader alloc] initWithAsset:asset.get() error:nil];
    auto track = asset.audioTrack();

    // assetReaderTrackOutputWithTrack throws if track is nil...
    // I do not handle the error here but by querying failed method.
    trackOutput = track == nil
        ? nil
        : [AVAssetReaderTrackOutput
              assetReaderTrackOutputWithTrack:track
                               outputSettings:@{
                                   AVFormatIDKey : [NSNumber
                                       numberWithInt:kAudioFormatLinearPCM]
                               }];

    [reader addOutput:trackOutput];
    [reader startReading];
}

auto CoreAudioBufferedReader::failed() -> bool { return trackOutput == nil; }

auto CoreAudioBufferedReader::readNextBuffer()
    -> std::shared_ptr<stimulus_players::AudioBuffer> {
    return std::make_shared<CoreAudioBuffer>(trackOutput);
}

auto CoreAudioBufferedReader::minimumPossibleSample() -> int {
    return std::numeric_limits<SInt16>::min();
}

static void init(
    MTAudioProcessingTapRef, void *clientInfo, void **tapStorageOut) {
    *tapStorageOut = clientInfo;
}

static void finalize(MTAudioProcessingTapRef) {}

template <typename T>
static void prepare(MTAudioProcessingTapRef tap, CMItemCount,
    const AudioStreamBasicDescription *description) {
    auto self = static_cast<T *>(MTAudioProcessingTapGetStorage(tap));
    self->audio().resize(description->mChannelsPerFrame);
    self->setSampleRate(description->mSampleRate);
}

static void unprepare(MTAudioProcessingTapRef) {}

template <typename T>
static void process(MTAudioProcessingTapRef tap, CMItemCount numberFrames,
    MTAudioProcessingTapFlags, AudioBufferList *bufferListInOut,
    CMItemCount *numberFramesOut, MTAudioProcessingTapFlags *flagsOut) {
    MTAudioProcessingTapGetSourceAudio(
        tap, numberFrames, bufferListInOut, flagsOut, nullptr, numberFramesOut);

    auto self = static_cast<T *>(MTAudioProcessingTapGetStorage(tap));
    if (self->audio().size() != bufferListInOut->mNumberBuffers)
        return;

    for (UInt32 j = 0; j < bufferListInOut->mNumberBuffers; ++j)
        self->audio()[j] = {
            static_cast<float *>(bufferListInOut->mBuffers[j].mData),
            numberFrames};
    self->fillAudioBuffer();
}

template <typename T>
static void createAudioProcessingTap(
    void *CM_NULLABLE clientInfo, MTAudioProcessingTapRef *tap) {
    MTAudioProcessingTapCallbacks callbacks;
    callbacks.version = kMTAudioProcessingTapCallbacksVersion_0;
    callbacks.clientInfo = clientInfo;
    callbacks.init = init;
    callbacks.prepare = prepare<T>;
    callbacks.process = process<T>;
    callbacks.unprepare = unprepare;
    callbacks.finalize = finalize;

    MTAudioProcessingTapCreate(kCFAllocatorDefault, &callbacks,
        kMTAudioProcessingTapCreationFlag_PostEffects, tap);
}

static auto playerItemWithAudioProcessing(
    std::string filePath, MTAudioProcessingTapRef tap) -> AVPlayerItem * {
    AvAssetFacade asset{std::move(filePath)};
    const auto playerItem = [AVPlayerItem playerItemWithAsset:asset.get()];
    const auto audioMix = [AVMutableAudioMix audioMix];
    const auto processing = [AVMutableAudioMixInputParameters
        audioMixInputParametersWithTrack:asset.audioTrack()];
    processing.audioTapProcessor = tap;
    audioMix.inputParameters = @[ processing ];
    playerItem.audioMix = audioMix;
    return playerItem;
}

static void loadItemFromFileWithAudioProcessing(
    std::string filePath, AVPlayer *player, MTAudioProcessingTapRef tap) {
    const auto playerItem =
        playerItemWithAudioProcessing(std::move(filePath), tap);
    [player replaceCurrentItemWithPlayerItem:playerItem];
}

AvFoundationVideoPlayer::AvFoundationVideoPlayer(NSScreen *screen)
    : actions{[VideoPlayerActions alloc]},
      videoWindow{
          [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 0, 0)
                                      styleMask:NSWindowStyleMaskBorderless
                                        backing:NSBackingStoreBuffered
                                          defer:YES]},
      player{[AVPlayer playerWithPlayerItem:nil]},
      playerLayer{[AVPlayerLayer playerLayerWithPlayer:player]}, screen{
                                                                     screen} {
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

void AvFoundationVideoPlayer::subscribe(EventListener *e) { listener_ = e; }

void AvFoundationVideoPlayer::play() { [player play]; }

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
    auto size = asset.videoTrack().naturalSize;
    size.height *= 2;
    size.height /= 3;
    size.width *= 2;
    size.width /= 3;
    [videoWindow setContentSize:NSSizeFromCGSize(size)];
    [playerLayer setFrame:videoWindow.contentView.bounds];
}

void AvFoundationVideoPlayer::centerVideo() {
    auto screenFrame = [screen frame];
    auto screenOrigin = screenFrame.origin;
    auto screenSize = screenFrame.size;
    auto windowSize = videoWindow.frame.size;
    auto videoLeadingEdge =
        screenOrigin.x + (screenSize.width - windowSize.width) / 2;
    auto videoBottomEdge =
        screenOrigin.y + (screenSize.height - windowSize.height) / 2;
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
             object:player.currentItem];
}

void AvFoundationVideoPlayer::playbackComplete() {
    listener_->playbackComplete();
}

void AvFoundationVideoPlayer::setDevice(int index) {
    player.audioOutputDeviceUniqueID = asNsString(device.uid(index));
}

void AvFoundationVideoPlayer::hide() { [videoWindow setIsVisible:NO]; }

void AvFoundationVideoPlayer::show() { showWindow(); }

auto AvFoundationVideoPlayer::deviceCount() -> int {
    return device.deviceCount();
}

auto AvFoundationVideoPlayer::deviceDescription(int index) -> std::string {
    return device.description(index);
}

void AvFoundationVideoPlayer::fillAudioBuffer() {
    listener_->fillAudioBuffer(audio_);
}

static auto playing(AVPlayer *player) -> bool {
    return player.timeControlStatus == AVPlayerTimeControlStatusPlaying;
}

auto AvFoundationVideoPlayer::playing() -> bool { return ::playing(player); }

// https://stackoverflow.com/questions/19059321/ios-7-avplayer-avplayeritem-duration-incorrect-in-ios-7
// "It appears the duration value isn't always immediately available
// from an AVPlayerItem but it seems to work fine with an AVAsset immediately."
static auto durationSeconds_(AVPlayer *player) -> Float64 {
    return CMTimeGetSeconds(player.currentItem.asset.duration);
}

auto AvFoundationVideoPlayer::durationSeconds() -> double {
    return durationSeconds_(player);
}

@implementation VideoPlayerActions
@synthesize controller;
- (void)playbackComplete {
    controller->playbackComplete();
}
@end

AvFoundationAudioPlayer::AvFoundationAudioPlayer()
    : player{[AVPlayer playerWithPlayerItem:nil]} {
    createAudioProcessingTap<AvFoundationAudioPlayer>(this, &tap);
}

void AvFoundationAudioPlayer::subscribe(EventListener *e) { listener_ = e; }

void AvFoundationAudioPlayer::loadFile(std::string filePath) {
    loadItemFromFileWithAudioProcessing(filePath, player, tap);
}

auto AvFoundationAudioPlayer::deviceCount() -> int {
    return device.deviceCount();
}

auto AvFoundationAudioPlayer::deviceDescription(int index) -> std::string {
    return device.description(index);
}

void AvFoundationAudioPlayer::setDevice(int index) {
    player.audioOutputDeviceUniqueID = asNsString(device.uid(index));
}

auto AvFoundationAudioPlayer::playing() -> bool { return ::playing(player); }

void AvFoundationAudioPlayer::play() { [player play]; }

auto AvFoundationAudioPlayer::sampleRateHz() -> double { return sampleRate_; }

void AvFoundationAudioPlayer::stop() { [player pause]; }

auto AvFoundationAudioPlayer::outputDevice(int index) -> bool {
    return device.outputDevice(index);
}

void AvFoundationAudioPlayer::fillAudioBuffer() {
    listener_->fillAudioBuffer(audio_);
}

auto AvFoundationAudioPlayer::durationSeconds() -> double {
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
