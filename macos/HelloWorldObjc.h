#import <Foundation/Foundation.h>

@protocol PrintProtocolDelegate
- (NSInteger)favoriteNumber;
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
- (void)attach;
@end

@interface HelloWorldObjc : NSObject
- (NSString *)sayHello:(NSObject<PrintProtocolDelegate> *)me;
+ (void)doEverything;
@end