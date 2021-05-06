#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

@protocol PrintProtocolDelegate
- (NSInteger)favoriteNumber;
@end

@protocol TestSetupUIObserver
- (void)notifyThatConfirmButtonHasBeenClicked;
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
- (void)populateTransducerMenu;
- (void)attach:(id<TestSetupUIObserver>)observer;
@end

@protocol TestSetupUIFactory
- (id<TestSetupUI>)make:(NSViewController *)viewController;
@end

@interface HelloWorldObjc : NSObject
- (NSString *)sayHello:(NSObject<PrintProtocolDelegate> *)me;
+ (void)doEverything:(NSObject<TestSetupUIFactory> *)testSetupUIFactory;
@end