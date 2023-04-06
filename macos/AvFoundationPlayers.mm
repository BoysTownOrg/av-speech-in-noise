#include "AvFoundationPlayers.h"
#include "Foundation-utility.h"

#include <Foundation/Foundation.h>
#include <mach/mach_time.h>

#include <gsl/gsl>

#include <limits>
#include <stdexcept>

@interface VideoPlayerActions : NSObject
@end

@implementation VideoPlayerActions {
  @public
    av_speech_in_noise::AvFoundationVideoPlayer *controller;
}

- (void)playbackComplete {
    controller->playbackComplete();
}
@end

namespace av_speech_in_noise {
static auto sampleRateHz(AVAssetTrack *track) -> double {
    const auto *const description{
        CMAudioFormatDescriptionGetStreamBasicDescription(
            static_cast<CMAudioFormatDescriptionRef>(
                track.formatDescriptions.firstObject))};
    return description != nullptr ? description->mSampleRate : 0;
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
    if (getPropertyDataSize(id_, address, &dataSize) != 0)
        NSLog(@"Some error calling AudioObjectGetPropertyDataSize encountered");
    std::vector<T> data(dataSize / sizeof(T));
    if (data.empty())
        return {};
    if (getPropertyData(id_, address, &dataSize, &data.front()) != 0)
        NSLog(@"Some error calling AudioObjectGetPropertyData encountered");
    return data;
}

static auto masterAddress(AudioObjectPropertySelector selector,
    AudioObjectPropertyScope scope) -> AudioObjectPropertyAddress {
    return {selector, scope, kAudioObjectPropertyElementMain};
}

static auto globalAddress(AudioObjectPropertySelector s)
    -> AudioObjectPropertyAddress {
    return masterAddress(s, kAudioObjectPropertyScopeGlobal);
}

// https://stackoverflow.com/questions/4575408/audioobjectgetpropertydata-to-get-a-list-of-input-devices
// http://fdiv.net/2008/08/12/nssound-setplaybackdeviceidentifier-coreaudio-output-device-enumeration
static auto loadDevices() -> std::vector<AudioObjectID> {
    const auto address{globalAddress(kAudioHardwarePropertyDevices)};
    return loadPropertyData<AudioDeviceID>(kAudioObjectSystemObject, &address);
}

static std::vector<AudioDeviceID> globalAudioDevices{loadDevices()};

static auto deviceCount() -> int {
    return gsl::narrow<int>(globalAudioDevices.size());
}

static auto objectId(gsl::index device) -> AudioObjectID {
    return globalAudioDevices.at(device);
}

// https://chromium.googlesource.com/chromium/src/media/+/7479f0acde23267d810b8e58c07b342719d9a923/audio/mac/audio_manager_mac.cc#68
static auto stringProperty(AudioObjectPropertySelector s, int device)
    -> std::string {
    const auto address{globalAddress(s)};
    const auto deviceID{objectId(device)};
    CFStringRef name;
    UInt32 size = sizeof(CFStringRef);
    OSStatus result = AudioObjectGetPropertyData(
        deviceID, &address, 0, nullptr, &size, &name);
    if (result != kAudioHardwareNoError) {
        return "";
    }
    const auto property{std::string{[(__bridge NSString *)name UTF8String]}};
    CFRelease(name);

    return property;
}

static auto description(int device) -> std::string {
    return stringProperty(kAudioObjectPropertyName, device);
}

static auto uid(int device) -> std::string {
    return stringProperty(kAudioDevicePropertyDeviceUID, device);
}

// https://stackoverflow.com/a/4577271
static auto outputDevice(int device) -> bool {
    const auto address = masterAddress(kAudioDevicePropertyStreamConfiguration,
        kAudioObjectPropertyScopeOutput);

    const auto deviceID{objectId(device)};
    UInt32 dataSize = 0;
    OSStatus result = AudioObjectGetPropertyDataSize(
        deviceID, &address, 0, nullptr, &dataSize);
    if (result != kAudioHardwareNoError) {
        return false;
    }

    AudioBufferList *bufferList =
        static_cast<AudioBufferList *>(malloc(dataSize));
    if (!bufferList) {
        return false;
    }

    result = AudioObjectGetPropertyData(
        deviceID, &address, 0, nullptr, &dataSize, bufferList);
    if (result != kAudioHardwareNoError) {
        free(bufferList);
        return false;
    }

    const auto answer = bufferList->mNumberBuffers > 0;
    free(bufferList);

    return answer;
}

static auto firstTrack(AVAsset *asset, AVMediaType mediaType)
    -> AVAssetTrack * {
    return [asset tracksWithMediaType:mediaType].firstObject;
}

static auto audioTrack(AVAsset *asset) -> AVAssetTrack * {
    return firstTrack(asset, AVMediaTypeAudio);
}

static auto videoTrack(AVAsset *asset) -> AVAssetTrack * {
    return firstTrack(asset, AVMediaTypeVideo);
}

static auto makeAvAsset(const std::string &filePath) -> AVURLAsset * {
    const auto url =
        [NSURL fileURLWithPath:nsString(filePath).stringByExpandingTildeInPath
                   isDirectory:NO];
    return [AVURLAsset URLAssetWithURL:url options:nil];
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

AvFoundationVideoPlayer::AvFoundationVideoPlayer(NSView *view)
    : actions{[[VideoPlayerActions alloc] init]}, view{view},
      player{[AVPlayer playerWithPlayerItem:nil]},
      playerLayer{[AVPlayerLayer playerLayerWithPlayer:player]},
      widthConstraint{[view.widthAnchor constraintEqualToConstant:0]},
      heightConstraint{[view.heightAnchor constraintEqualToConstant:0]} {
    MTAudioProcessingTapCallbacks callbacks;
    callbacks.version = kMTAudioProcessingTapCallbacksVersion_0;
    callbacks.clientInfo = this;
    callbacks.init = init;
    callbacks.prepare = prepareTap;
    callbacks.process = processTap;
    callbacks.unprepare = unprepare;
    callbacks.finalize = finalize;

    if (MTAudioProcessingTapCreate(kCFAllocatorDefault, &callbacks,
            kMTAudioProcessingTapCreationFlag_PostEffects, &tap) != 0)
        NSLog(@"Some error with MTAudioProcessingTapCreate encountered.");
    prepareWindow();
    if (view.superview != nullptr)
        [NSLayoutConstraint activateConstraints:@[
            [view.centerXAnchor
                constraintEqualToAnchor:view.superview.centerXAnchor],
            [view.centerYAnchor
                constraintEqualToAnchor:view.superview.centerYAnchor],
            widthConstraint, heightConstraint
        ]];
    actions->controller = this;
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
    audio.resize(processingFormat->mChannelsPerFrame);
}

void AvFoundationVideoPlayer::processTap(MTAudioProcessingTapRef tap,
    CMItemCount numberFrames, MTAudioProcessingTapFlags flags,
    AudioBufferList *bufferListInOut, CMItemCount *numberFramesOut,
    MTAudioProcessingTapFlags *flagsOut) {
    return static_cast<AvFoundationVideoPlayer *>(
        MTAudioProcessingTapGetStorage(tap))
        ->processTap_(
            numberFrames, flags, bufferListInOut, numberFramesOut, flagsOut);
}

void AvFoundationVideoPlayer::processTap_(CMItemCount numberFrames,
    MTAudioProcessingTapFlags, AudioBufferList *bufferListInOut,
    CMItemCount *numberFramesOut, MTAudioProcessingTapFlags *flagsOut) {
    if (MTAudioProcessingTapGetSourceAudio(tap, numberFrames, bufferListInOut,
            flagsOut, nullptr, numberFramesOut) != 0)
        ; // Something failed, but how to respond is unclear given the high
          // priority context.

    if (audio.size() != bufferListInOut->mNumberBuffers)
        return;

    for (UInt32 j{0}; j < bufferListInOut->mNumberBuffers; ++j)
        audio.at(j) = {static_cast<float *>(bufferListInOut->mBuffers[j].mData),
            gsl::narrow<gsl::span<float>::size_type>(numberFrames)};
    listener_->fillAudioBuffer(audio);
}

void AvFoundationVideoPlayer::prepareWindow() { addPlayerLayer(); }

void AvFoundationVideoPlayer::addPlayerLayer() {
    [view setWantsLayer:YES];
    [view.layer addSublayer:playerLayer];
}

void AvFoundationVideoPlayer::attach(Observer *e) { listener_ = e; }

void AvFoundationVideoPlayer::play() { [player play]; }

void AvFoundationVideoPlayer::playAt(const PlayerTimeWithDelay &t) {
    // https://developer.apple.com/documentation/avfoundation/avplayer/1386591-setrate?language=objc
    // "For clients linked against iOS 10.0 and later or macOS 10.12 and later,
    // invoking [[setRate:time:atHostTime:]] when
    // automaticallyWaitsToMinimizeStalling is YES will raise an
    // NSInvalidArgument exception."
    player.automaticallyWaitsToMinimizeStalling = NO;
    auto hostTime{CMClockMakeHostTimeFromSystemUnits(t.playerTime.system)};
    [player setRate:1.0
               time:kCMTimeInvalid
         atHostTime:CMTimeAdd(hostTime,
                        CMTimeMakeWithSeconds(
                            t.delay.seconds, hostTime.timescale))];
}

void AvFoundationVideoPlayer::loadFile(std::string filePath) {
    const auto asset{makeAvAsset(filePath)};
    // It seems if AVPlayer's replaceCurrentItemWithPlayerItem is called with an
    // unplayable asset the player does not recover even when a subsequent call
    // passes one that is playable.
    if (asset.playable == 0) {
        NSLog(@"Unplayable AVURLAsset encountered.");
        return;
    }
    const auto playerItem{[AVPlayerItem playerItemWithAsset:asset]};
    const auto audioMix{[AVMutableAudioMix audioMix]};
    const auto processing = [AVMutableAudioMixInputParameters
        audioMixInputParametersWithTrack:audioTrack(asset)];
    processing.audioTapProcessor = tap;
    audioMix.inputParameters = @[ processing ];
    playerItem.audioMix = audioMix;
    [player pause];
    [player replaceCurrentItemWithPlayerItem:playerItem];
    prepareVideo();
}

void AvFoundationVideoPlayer::prepareVideo() { resizeVideo(); }

void AvFoundationVideoPlayer::resizeVideo() {
    const auto asset{currentAsset(player)};
    auto size{videoTrack(asset).naturalSize};
    // Kaylah requested that the video be reduced in size.
    // We landed on 2/3 scale.
    size.height *= 2;
    size.height /= 3;
    size.width *= 2;
    size.width /= 3;
    widthConstraint.constant = size.width;
    heightConstraint.constant = size.height;
    const auto nsSize{NSSizeFromCGSize(size)};
    [playerLayer setFrame:NSMakeRect(0, 0, nsSize.width, nsSize.height)];
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
    player.audioOutputDeviceUniqueID = nsString(uid(index));
}

void AvFoundationVideoPlayer::hide() { [view setHidden:YES]; }

void AvFoundationVideoPlayer::show() { [view setHidden:NO]; }

auto AvFoundationVideoPlayer::deviceCount() -> int {
    return av_speech_in_noise::deviceCount();
}

auto AvFoundationVideoPlayer::deviceDescription(int index) -> std::string {
    return description(index);
}

static auto playing(AVPlayer *player) -> bool {
    return player.timeControlStatus == AVPlayerTimeControlStatusPlaying;
}

auto AvFoundationVideoPlayer::playing() -> bool {
    return av_speech_in_noise::playing(player);
}

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

AvFoundationAudioPlayer::AvFoundationAudioPlayer() {
    AudioComponentDescription audioComponentDescription;
    audioComponentDescription.componentType = kAudioUnitType_Output;
    audioComponentDescription.componentSubType = kAudioUnitSubType_HALOutput;
    audioComponentDescription.componentManufacturer =
        kAudioUnitManufacturer_Apple;
    audioComponentDescription.componentFlags = 0;
    audioComponentDescription.componentFlagsMask = 0;

    auto *const audioComponent =
        AudioComponentFindNext(nullptr, &audioComponentDescription);
    if (audioComponent == nullptr)
        throw std::runtime_error{"NULL returned from AudioComponentFindNext."};
    if (AudioComponentInstanceNew(audioComponent, &audioUnit) != 0)
        NSLog(@"AudioComponentInstanceNew failed.");
    if (AudioUnitInitialize(audioUnit) != 0)
        throw std::runtime_error{"AudioUnitInitialize failed."};
    // enable output
    {
        UInt32 enable{1};
        if (AudioUnitSetProperty(audioUnit, kAudioOutputUnitProperty_EnableIO,
                kAudioUnitScope_Output, 0, &enable, sizeof(enable)) != 0)
            NSLog(@"AudioUnitSetProperty failed to enable output.");
    }

    // disable input
    UInt32 enable{0};
    if (AudioUnitSetProperty(audioUnit, kAudioOutputUnitProperty_EnableIO,
            kAudioUnitScope_Input, 1, &enable, sizeof(enable)) != 0)
        NSLog(@"AudioUnitSetProperty failed to disable input.");

    // Set audio unit render callback.
    AURenderCallbackStruct renderCallbackStruct;
    renderCallbackStruct.inputProc = AU_RenderCallback;
    renderCallbackStruct.inputProcRefCon = this;
    if (AudioUnitSetProperty(audioUnit, kAudioUnitProperty_SetRenderCallback,
            kAudioUnitScope_Output, 0, &renderCallbackStruct,
            sizeof(AURenderCallbackStruct)) != 0)
        NSLog(@"AudioUnitSetProperty failed to set render callback.");
}

AvFoundationAudioPlayer::~AvFoundationAudioPlayer() {
    if (AudioUnitUninitialize(audioUnit) != 0)
        NSLog(@"AudioUnitUninitialize failed.");
    if (AudioComponentInstanceDispose(audioUnit) != 0)
        NSLog(@"AudioComponentInstanceDispose failed.");
}

auto AvFoundationAudioPlayer::AU_RenderCallback(void *inRefCon,
    AudioUnitRenderActionFlags *ioActionFlags,
    const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber,
    UInt32 inNumberFrames, AudioBufferList *ioData) -> OSStatus {
    return static_cast<AvFoundationAudioPlayer *>(inRefCon)->audioBufferReady(
        ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, ioData);
}

auto AvFoundationAudioPlayer::audioBufferReady(AudioUnitRenderActionFlags *,
    const AudioTimeStamp *inTimeStamp, UInt32, UInt32 inNumberFrames,
    AudioBufferList *ioData) -> OSStatus {
    if (audio.size() != ioData->mNumberBuffers)
        return -1;
    for (UInt32 j{0}; j < ioData->mNumberBuffers; ++j)
        audio.at(j) = {
            static_cast<float *>(ioData->mBuffers[j].mData), inNumberFrames};
    listener_->fillAudioBuffer(audio, inTimeStamp->mHostTime);
    return noErr;
}

void AvFoundationAudioPlayer::attach(Observer *e) { listener_ = e; }

void AvFoundationAudioPlayer::loadFile(std::string filePath) {
    const auto asset{makeAvAsset(filePath)};

    if (asset.playable == 0) {
        NSLog(@"Unplayable AVURLAsset encountered.");
    }

    AudioStreamBasicDescription streamFormat{};

    streamFormat.mSampleRate =
        av_speech_in_noise::sampleRateHz(audioTrack(asset));
    streamFormat.mFormatID = kAudioFormatLinearPCM;
    streamFormat.mFramesPerPacket = 1;
    streamFormat.mBytesPerPacket = 4;
    streamFormat.mBytesPerFrame = sizeof(float);
    streamFormat.mChannelsPerFrame = 2;
    streamFormat.mBitsPerChannel = 8 * sizeof(float);
    streamFormat.mFormatFlags =
        kAudioFormatFlagIsFloat | kAudioFormatFlagIsNonInterleaved;
    if (AudioUnitSetProperty(audioUnit, kAudioUnitProperty_StreamFormat,
            kAudioUnitScope_Input, 0, &streamFormat,
            sizeof(AudioStreamBasicDescription)) != 0)
        NSLog(@"AudioUnitSetProperty failed to set stream format.");

    audio.resize(2);
}

auto AvFoundationAudioPlayer::deviceCount() -> int {
    return av_speech_in_noise::deviceCount();
}

auto AvFoundationAudioPlayer::deviceDescription(int index) -> std::string {
    return description(index);
}

void AvFoundationAudioPlayer::setDevice(int index) {
    const auto deviceId{objectId(index)};
    if (AudioUnitSetProperty(audioUnit, kAudioOutputUnitProperty_CurrentDevice,
            kAudioUnitScope_Global, 0, &deviceId, sizeof(deviceId)) != 0)
        NSLog(@"AudioUnitSetProperty failed to set device.");
}

auto AvFoundationAudioPlayer::playing() -> bool {
    UInt32 auhalRunning{0};
    UInt32 size{sizeof(auhalRunning)};
    if (AudioUnitGetProperty(audioUnit, kAudioOutputUnitProperty_IsRunning,
            kAudioUnitScope_Global, 0, &auhalRunning, &size) != 0)
        NSLog(@"AudioUnitGetProperty failed to check whether playing.");
    return auhalRunning != 0U;
}

void AvFoundationAudioPlayer::play() { AudioOutputUnitStart(audioUnit); }

auto AvFoundationAudioPlayer::sampleRateHz() -> double {
    AudioStreamBasicDescription streamFormat{};
    UInt32 size{sizeof(AudioStreamBasicDescription)};
    if (AudioUnitGetProperty(audioUnit, kAudioUnitProperty_StreamFormat,
            kAudioUnitScope_Input, 0, &streamFormat, &size) != 0)
        NSLog(@"AudioUnitGetProperty failed to query sample rate.");
    return streamFormat.mSampleRate;
}

void AvFoundationAudioPlayer::stop() {
    if (AudioOutputUnitStop(audioUnit) != 0)
        NSLog(@"AudioOutputUnitStop failed.");
}

auto AvFoundationAudioPlayer::outputDevice(int index) -> bool {
    return av_speech_in_noise::outputDevice(index);
}

static auto numerator(const mach_timebase_info_data_t &t) -> uint32_t {
    return t.numer;
}

static auto denominator(const mach_timebase_info_data_t &t) -> uint32_t {
    return t.denom;
}

auto AvFoundationAudioPlayer::nanoseconds(PlayerTime t) -> std::uintmax_t {
    // https://stackoverflow.com/questions/23378063/how-can-i-use-mach-absolute-time-without-overflowing
    mach_timebase_info_data_t tb;
    mach_timebase_info(&tb);
    auto high{(t.system >> 32) * numerator(tb)};
    auto low{(t.system & 0xffffffffULL) * numerator(tb) / denominator(tb)};
    auto highRem{((high % denominator(tb)) << 32) / denominator(tb)};
    high /= denominator(tb);
    return (high << 32) + highRem + low;
}

auto AvFoundationAudioPlayer::currentSystemTime() -> PlayerTime {
    return PlayerTime{mach_absolute_time()};
}

void AvFoundationVideoPlayer::preRoll() {
    [player prerollAtRate:1.
        completionHandler:^(BOOL finished) {
          if (finished == NO)
              NSLog(@"prerollAtRate failed.");
          listener_->notifyThatPreRollHasCompleted();
        }];
}

AvFoundationBufferedAudioReader::AvFoundationBufferedAudioReader(
    const LocalUrl &url) {
    // https://developer.apple.com/documentation/foundation/nsurl/1414650-fileurlwithpath?language=objc
    // "path should be a valid system path, and must not be an empty path."
    if (url.path.empty())
        throw CannotReadFile{};
    const auto fileURL{
        [NSURL fileURLWithPath:nsString(url.path).stringByExpandingTildeInPath
                   isDirectory:NO]};
    if (fileURL == nil)
        throw CannotReadFile{};
    {
        NSError *error{nil};
        file = [[AVAudioFile alloc] initForReading:fileURL
                                      commonFormat:AVAudioPCMFormatFloat32
                                       interleaved:NO
                                             error:&error];
        if (file == nil)
            throw CannotReadFile{};
    }
    buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:file.processingFormat
                                           frameCapacity:file.length];
    NSError *error{nil};
    if ([file readIntoBuffer:buffer error:&error] == NO)
        throw CannotReadFile{};
}

auto AvFoundationBufferedAudioReader::channel(gsl::index n)
    -> std::vector<float> {
    auto *const p{buffer.floatChannelData[n]};
    return {p, p + buffer.frameLength};
}

auto AvFoundationBufferedAudioReader::channels() -> gsl::index {
    return buffer.format.channelCount;
}

auto AvFoundationBufferedAudioReaderFactory::make(const LocalUrl &url)
    -> std::shared_ptr<BufferedAudioReader> {
    return std::make_shared<AvFoundationBufferedAudioReader>(url);
}

void AvFoundationAudioRecorder::initialize(const LocalUrl &url) {
    NSError *error;
    const auto format =
        [[AVAudioFormat alloc] initStandardFormatWithSampleRate:44100
                                                       channels:1];
    if (format != nullptr)
        audioRecorder = [[AVAudioRecorder alloc]
            initWithURL:[NSURL
                            fileURLWithPath:nsString(url.path)
                                                .stringByExpandingTildeInPath]
                 format:format
                  error:&error];
    if (audioRecorder == nullptr)
        NSLog(@"Failed to initialize audio recorder: %@",
            error.localizedDescription);
    [audioRecorder prepareToRecord];
}

void AvFoundationAudioRecorder::start() { [audioRecorder record]; }

void AvFoundationAudioRecorder::stop() { [audioRecorder stop]; }
}
