#include "AvFoundationPlayers.h"
#include "common-objc.h"
#include <gsl/gsl>
#include <limits>

static auto sampleRateHz(AVAssetTrack *track) -> double {
    auto description = CMAudioFormatDescriptionGetStreamBasicDescription(
        static_cast<CMAudioFormatDescriptionRef>(
            track.formatDescriptions.firstObject));
    return description->mSampleRate;
}

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
    if (data.empty())
        return {};
    getPropertyData(id_, address, &dataSize, &data.front());
    return data;
}

static auto masterAddress(AudioObjectPropertySelector selector,
    AudioObjectPropertyScope scope) -> AudioObjectPropertyAddress {
    return {selector, scope, kAudioObjectPropertyElementMaster};
}

static auto globalAddress(AudioObjectPropertySelector s)
    -> AudioObjectPropertyAddress {
    return masterAddress(s, kAudioObjectPropertyScopeGlobal);
}

// https://stackoverflow.com/questions/4575408/audioobjectgetpropertydata-to-get-a-list-of-input-devices
// http://fdiv.net/2008/08/12/nssound-setplaybackdeviceidentifier-coreaudio-output-device-enumeration
CoreAudioDevices::CoreAudioDevices() { loadDevices(); }

void CoreAudioDevices::loadDevices() {
    auto address = globalAddress(kAudioHardwarePropertyDevices);
    devices =
        loadPropertyData<AudioDeviceID>(kAudioObjectSystemObject, &address);
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
    buffer.erase(std::find(buffer.begin(), buffer.end(), '\0'), buffer.end());
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

    AVAsset *asset;
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

auto CoreAudioBufferedReader::sampleRateHz() -> double {
    return ::sampleRateHz(trackOutput.track);
}

static void init(
    MTAudioProcessingTapRef, void *clientInfo, void **tapStorageOut) {
    *tapStorageOut = clientInfo;
}

static void finalize(MTAudioProcessingTapRef) {}

static void unprepare(MTAudioProcessingTapRef) {}

static auto currentAsset(AVPlayer *player) -> AVAsset * {
    return player.currentItem.asset;
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
    MTAudioProcessingTapCallbacks callbacks;
    callbacks.version = kMTAudioProcessingTapCallbacksVersion_0;
    callbacks.clientInfo = this;
    callbacks.init = init;
    callbacks.prepare = prepareTap;
    callbacks.process = processTap;
    callbacks.unprepare = unprepare;
    callbacks.finalize = finalize;

    MTAudioProcessingTapCreate(kCFAllocatorDefault, &callbacks,
        kMTAudioProcessingTapCreationFlag_PostEffects, &tap);
    prepareWindow();
    actions.controller = this;
}

void AvFoundationVideoPlayer::prepareTap(MTAudioProcessingTapRef tap,
    CMItemCount maxFrames,
    const AudioStreamBasicDescription *processingFormat) {
    return static_cast<AvFoundationVideoPlayer *>(
        MTAudioProcessingTapGetStorage(tap))
        ->prepareTap_(tap, maxFrames, processingFormat);
}

void AvFoundationVideoPlayer::prepareTap_(MTAudioProcessingTapRef, CMItemCount,
    const AudioStreamBasicDescription *processingFormat) {
    audio_.resize(processingFormat->mChannelsPerFrame);
}

void AvFoundationVideoPlayer::processTap(MTAudioProcessingTapRef tap,
    CMItemCount numberFrames, MTAudioProcessingTapFlags flags,
    AudioBufferList *bufferListInOut, CMItemCount *numberFramesOut,
    MTAudioProcessingTapFlags *flagsOut) {
    return static_cast<AvFoundationVideoPlayer *>(
        MTAudioProcessingTapGetStorage(tap))
        ->processTap_(tap, numberFrames, flags, bufferListInOut,
            numberFramesOut, flagsOut);
}

void AvFoundationVideoPlayer::processTap_(MTAudioProcessingTapRef tap,
    CMItemCount numberFrames, MTAudioProcessingTapFlags flags,
    AudioBufferList *bufferListInOut, CMItemCount *numberFramesOut,
    MTAudioProcessingTapFlags *flagsOut) {
    MTAudioProcessingTapGetSourceAudio(
        tap, numberFrames, bufferListInOut, flagsOut, nullptr, numberFramesOut);

    if (audio_.size() != bufferListInOut->mNumberBuffers)
        return;

    for (UInt32 j = 0; j < bufferListInOut->mNumberBuffers; ++j)
        audio_[j] = {static_cast<float *>(bufferListInOut->mBuffers[j].mData),
            numberFrames};
    listener_->fillAudioBuffer(audio_);
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
    AvAssetFacade asset{std::move(filePath)};
    const auto playerItem = [AVPlayerItem playerItemWithAsset:asset.get()];
    const auto audioMix = [AVMutableAudioMix audioMix];
    const auto processing = [AVMutableAudioMixInputParameters
        audioMixInputParametersWithTrack:asset.audioTrack()];
    processing.audioTapProcessor = tap;
    audioMix.inputParameters = @[ processing ];
    playerItem.audioMix = audioMix;
    [player replaceCurrentItemWithPlayerItem:playerItem];
    prepareVideo();
}

void AvFoundationVideoPlayer::prepareVideo() {
    resizeVideo();
    centerVideo();
}

void AvFoundationVideoPlayer::resizeVideo() {
    AvAssetFacade asset{currentAsset(player)};
    auto size = asset.videoTrack().naturalSize;
    // Kaylah requested that the video be reduced in size.
    // We landed on 2/3 scale.
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

static auto playing(AVPlayer *player) -> bool {
    return player.timeControlStatus == AVPlayerTimeControlStatusPlaying;
}

auto AvFoundationVideoPlayer::playing() -> bool { return ::playing(player); }

static auto durationSeconds_(AVAsset *asset) -> Float64 {
    return CMTimeGetSeconds(asset.duration);
}

// https://stackoverflow.com/questions/19059321/ios-7-avplayer-avplayeritem-duration-incorrect-in-ios-7
// "It appears the duration value isn't always immediately available
// from an AVPlayerItem but it seems to work fine with an AVAsset immediately."
static auto durationSeconds_(AVPlayer *player) -> Float64 {
    return durationSeconds_(currentAsset(player));
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

AvFoundationAudioPlayer::AvFoundationAudioPlayer() {
    AudioComponentDescription audioComponentDescription;
    audioComponentDescription.componentType = kAudioUnitType_Output;
    audioComponentDescription.componentSubType = kAudioUnitSubType_HALOutput;
    audioComponentDescription.componentManufacturer =
        kAudioUnitManufacturer_Apple;
    audioComponentDescription.componentFlags = 0;
    audioComponentDescription.componentFlagsMask = 0;

    auto audioComponent =
        AudioComponentFindNext(nullptr, &audioComponentDescription);
    AudioComponentInstanceNew(audioComponent, &audioUnit);
    AudioUnitInitialize(audioUnit);
    // enable output
    {
        UInt32 enable = 1;
        AudioUnitSetProperty(audioUnit, kAudioOutputUnitProperty_EnableIO,
            kAudioUnitScope_Output, 0, &enable, sizeof(enable));
    }

    // disable input
    UInt32 enable = 0;
    AudioUnitSetProperty(audioUnit, kAudioOutputUnitProperty_EnableIO,
        kAudioUnitScope_Input, 1, &enable, sizeof(enable));

    // Set audio unit render callback.
    AURenderCallbackStruct renderCallbackStruct;
    renderCallbackStruct.inputProc = AU_RenderCallback;
    renderCallbackStruct.inputProcRefCon = this;
    AudioUnitSetProperty(audioUnit, kAudioUnitProperty_SetRenderCallback,
        kAudioUnitScope_Output, 0, &renderCallbackStruct,
        sizeof(AURenderCallbackStruct));
}

AvFoundationAudioPlayer::~AvFoundationAudioPlayer() {
    AudioUnitUninitialize(audioUnit);
    AudioComponentInstanceDispose(audioUnit);
}

auto AvFoundationAudioPlayer::AU_RenderCallback(void *inRefCon,
    AudioUnitRenderActionFlags *ioActionFlags,
    const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
    UInt32 inNumberFrames, AudioBufferList *ioData) -> OSStatus {
    return static_cast<AvFoundationAudioPlayer *>(inRefCon)->audioBufferReady(
        ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData);
}

auto AvFoundationAudioPlayer::audioBufferReady(
    AudioUnitRenderActionFlags *ioActionFlags,
    const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
    UInt32 inNumberFrames, AudioBufferList *ioData) -> OSStatus {
    if (audio_.size() != ioData->mNumberBuffers)
        return -1;
    for (UInt32 j = 0; j < ioData->mNumberBuffers; ++j)
        audio_[j] = {
            static_cast<float *>(ioData->mBuffers[j].mData), inNumberFrames};
    listener_->fillAudioBuffer(audio_);
    return noErr;
}

void AvFoundationAudioPlayer::subscribe(EventListener *e) { listener_ = e; }

void AvFoundationAudioPlayer::loadFile(std::string filePath) {
    filePath_ = std::move(filePath);

    AvAssetFacade asset{filePath_};

    AudioStreamBasicDescription streamFormat{};

    streamFormat.mSampleRate = ::sampleRateHz(asset.audioTrack());
    streamFormat.mFormatID = kAudioFormatLinearPCM;
    streamFormat.mFramesPerPacket = 1;
    streamFormat.mBytesPerPacket = 4;
    streamFormat.mBytesPerFrame = sizeof(float);
    streamFormat.mChannelsPerFrame = 2;
    streamFormat.mBitsPerChannel = 8 * sizeof(float);
    streamFormat.mFormatFlags =
        kAudioFormatFlagIsFloat | kAudioFormatFlagIsNonInterleaved;
    AudioUnitSetProperty(audioUnit, kAudioUnitProperty_StreamFormat,
        kAudioUnitScope_Input, 0, &streamFormat,
        sizeof(AudioStreamBasicDescription));

    audio_.resize(2);
}

auto AvFoundationAudioPlayer::deviceCount() -> int {
    return device.deviceCount();
}

auto AvFoundationAudioPlayer::deviceDescription(int index) -> std::string {
    return device.description(index);
}

void AvFoundationAudioPlayer::setDevice(int index) {
    auto deviceId = device.objectId(index);
    AudioUnitSetProperty(audioUnit, kAudioOutputUnitProperty_CurrentDevice,
        kAudioUnitScope_Global, 0, &deviceId, sizeof(deviceId));
}

auto AvFoundationAudioPlayer::playing() -> bool {
    UInt32 auhalRunning = 0;
    UInt32 size = sizeof(auhalRunning);
    AudioUnitGetProperty(audioUnit, kAudioOutputUnitProperty_IsRunning,
        kAudioUnitScope_Global, 0, &auhalRunning, &size);
    return auhalRunning != 0U;
}

void AvFoundationAudioPlayer::play() { AudioOutputUnitStart(audioUnit); }

auto AvFoundationAudioPlayer::sampleRateHz() -> double {
    AudioStreamBasicDescription streamFormat{};
    UInt32 size = sizeof(AudioStreamBasicDescription);
    AudioUnitGetProperty(audioUnit, kAudioUnitProperty_StreamFormat,
        kAudioUnitScope_Input, 0, &streamFormat, &size);
    return streamFormat.mSampleRate;
}

void AvFoundationAudioPlayer::stop() { AudioOutputUnitStop(audioUnit); }

auto AvFoundationAudioPlayer::outputDevice(int index) -> bool {
    return device.outputDevice(index);
}

auto AvFoundationAudioPlayer::durationSeconds() -> double {
    AvAssetFacade asset{filePath_};
    return durationSeconds_(asset.get());
}
