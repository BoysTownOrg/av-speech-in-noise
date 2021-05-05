#import <Foundation/Foundation.h>
#include <string>
#include <sstream>
#import "HelloWorldObjc.h"

@implementation HelloWorldObjc
- (NSString *)sayHello:(NSObject<PrintProtocolDelegate> *)me {
    std::stringstream stream;
    stream << "sup dudes " << me.favoriteNumber;
    return [NSString stringWithCString:stream.str().c_str()
                              encoding:NSUTF8StringEncoding];
}
@end