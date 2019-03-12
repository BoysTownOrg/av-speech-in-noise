#include "AvFoundationPlayers.h"

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

AvFoundationStimulusPlayer::AvFoundationStimulusPlayer() {
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
