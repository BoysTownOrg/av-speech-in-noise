#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

@protocol PrintProtocolDelegate
- (NSInteger)favoriteNumber;
@end

@protocol TestSetupUIObserver
- (void)notifyThatConfirmButtonHasBeenClicked;
- (void)notifyThatPlayCalibrationButtonHasBeenClicked;
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

@protocol TestSetupUIFactory
- (id<TestSetupUI>)make:(NSViewController *)viewController;
@end

@interface HelloWorldObjc : NSObject
- (NSString *)sayHello:(NSObject<PrintProtocolDelegate> *)me;
+ (void)doEverything:(NSObject<TestSetupUIFactory> *)testSetupUIFactory;
@end
