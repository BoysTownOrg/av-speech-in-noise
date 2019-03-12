#include "CocoaView.h"

CocoaTesterView::CocoaTesterView() {
    [view_ setHidden:YES];
    [view_ addSubview:deviceMenu];
}

NSView *CocoaTesterView::view() {
    return view_;
}

void CocoaTesterView::show() {
    [view_ setHidden:NO];
}

void CocoaTesterView::hide() {
    [view_ setHidden:YES];
}

std::string CocoaTesterView::audioDevice() {
    return [deviceMenu.titleOfSelectedItem UTF8String];
}

void CocoaTesterView::populateAudioDeviceMenu(std::vector<std::string> items) {
    for (const auto &item : items) {
        auto title = [NSString stringWithCString:
            item.c_str()
            encoding:[NSString defaultCStringEncoding]
        ];
        [deviceMenu addItemWithTitle: title];
    }
}

CocoaTestSetupView::CocoaTestSetupView() {
    [view_ setHidden:YES];
    [view_ addSubview:subjectIdLabel];
    [view_ addSubview:subjectId_];
    [view_ addSubview:testerIdLabel];
    [view_ addSubview:testerId_];
    [view_ addSubview:signalLevel_dB_SPL_label];
    [view_ addSubview:signalLevel_dB_SPL_];
    [view_ addSubview:maskerLevel_dB_SPL_label];
    [view_ addSubview:maskerLevel_dB_SPL_];
    [view_ addSubview:stimulusListDirectoryLabel];
    [view_ addSubview:stimulusListDirectory_];
    [view_ addSubview:maskerFilePath_label];
    [view_ addSubview:maskerFilePath_];
    stimulusListDirectory_.stringValue =
        @"/Users/basset/Documents/maxdetection/Stimuli/Video/List_Detection";
    maskerFilePath_.stringValue =
        @"/Users/basset/Documents/maxdetection/Stimuli/Masker/L1L2_EngEng.wav";
}

NSView *CocoaTestSetupView::view() {
    return view_;
}

void CocoaTestSetupView::show() {
    [view_ setHidden:NO];
}

void CocoaTestSetupView::hide() {
    [view_ setHidden:YES];
}

std::string CocoaTestSetupView::maskerLevel_dB_SPL() {
    return [maskerLevel_dB_SPL_.stringValue UTF8String];
}

std::string CocoaTestSetupView::signalLevel_dB_SPL() {
    return [signalLevel_dB_SPL_.stringValue UTF8String];
}

std::string CocoaTestSetupView::maskerFilePath() {
    return [maskerFilePath_.stringValue UTF8String];
}

std::string CocoaTestSetupView::stimulusListDirectory() {
    return [stimulusListDirectory_.stringValue UTF8String];
}

std::string CocoaTestSetupView::testerId() {
    return [testerId_.stringValue UTF8String];
}

std::string CocoaTestSetupView::subjectId() {
    return [subjectId_.stringValue UTF8String];
}

std::string CocoaTestSetupView::condition() {
    return "";
}

@implementation ViewActions
@synthesize controller;
- (void)newTest {
    controller->newTest();
}

- (void)openTest {
    controller->openTest();
}

- (void)confirmTestSetup {
    controller->confirmTestSetup();
}

- (void)playTrial {
    controller->playTrial();
}
@end
