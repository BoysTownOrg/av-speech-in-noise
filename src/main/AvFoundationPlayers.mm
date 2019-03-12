#include "AvFoundationPlayers.h"

@implementation StimulusPlayerActions
@synthesize controller;
- (void)playbackComplete {
    controller->playbackComplete();
}
@end

void AvFoundationMaskerPlayer::subscribe(EventListener *listener_) {
    listener = listener_;
}

void AvFoundationMaskerPlayer::fadeIn() {
    [player play];
    listener->fadeInComplete();
}

void AvFoundationMaskerPlayer::fadeOut() {
    [player pause];
}

void AvFoundationMaskerPlayer::loadFile(std::string filePath) {
    const auto asset = makeAvAsset(filePath);
    [player replaceCurrentItemWithPlayerItem:
        [AVPlayerItem playerItemWithAsset:asset]];
}

int AvFoundationMaskerPlayer::deviceCount() {
    return device.deviceCount();
}

std::string AvFoundationMaskerPlayer::deviceDescription(int index) {
    return device.description(index);
}

void AvFoundationMaskerPlayer::setDevice(int index) {
    auto uid_ = device.uid(index);
    player.audioOutputDeviceUniqueID = [
        NSString stringWithCString:uid_.c_str()
        encoding:[NSString defaultCStringEncoding]
    ];
}
