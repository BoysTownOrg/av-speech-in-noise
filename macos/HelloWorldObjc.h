#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

@protocol TestSetupUIObserver
- (void)notifyThatConfirmButtonHasBeenClicked;
- (void)notifyThatPlayCalibrationButtonHasBeenClicked;
@end

@protocol TestUIObserver
- (void)exitTest;
- (void)playTrial;
- (void)declineContinuingTesting;
- (void)acceptContinuingTesting;
@end

@protocol TestSetupUI
- (void)show;
- (void)hide;
- (NSString *)testerId;
- (NSString *)subjectId;
- (NSString *)session;
- (NSString *)testSettingsFile;
- (NSString *)startingSnr;
- (NSString *)transducer;
- (NSString *)rmeSetting;
- (void)populateTransducerMenu:(NSArray<NSString *> *)transducers;
- (void)attach:(id<TestSetupUIObserver>)observer;
@end

@protocol TestUI
- (void)attach:(id<TestUIObserver>)observer;
- (void)showExitTestButton;
- (void)hideExitTestButton;
- (void)show;
- (void)hide;
- (void)display:(NSString *)something;
- (void)secondaryDisplay:(NSString *)something;
- (void)showNextTrialButton;
- (void)hideNextTrialButton;
- (void)showContinueTestingDialog;
- (void)hideContinueTestingDialog;
- (void)setContinueTestingDialogMessage:(NSString *)something;
- (void)showSheet:(NSString *)something;
@end

@protocol SessionUI
- (void)eventLoop;
- (void)showErrorMessage:(NSString *)something;
- (NSString *)audioDevice;
- (void)populateAudioDeviceMenu:(NSArray<NSString *> *)devices;
@end

@protocol TestSetupUIFactory
- (id<TestSetupUI>)make:(NSViewController *)viewController;
@end

@interface HelloWorldObjc : NSObject
+ (void)doEverything:(NSObject<TestSetupUIFactory> *)testSetupUIFactory;
@end
