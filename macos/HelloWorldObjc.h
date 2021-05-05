#import <Foundation/Foundation.h>

@protocol PrintProtocolDelegate
- (NSInteger)favoriteNumber;
@end

@interface HelloWorldObjc : NSObject
- (NSString *)sayHello:(NSObject<PrintProtocolDelegate> *)me;
@end