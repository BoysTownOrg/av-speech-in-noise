#import <Foundation/Foundation.h>
#include <string>
#include <sstream>
#import "HelloWorldObjc.h"
#include "run.h"
#include "AppKitView.h"
#include "AppKit-utility.h"
#include "EyeTrackerStub.hpp"

namespace av_speech_in_noise {
static void main() {
    static EyeTrackerStub eyeTracker;
    static AppKitTestSetupUIFactoryImpl testSetupViewFactory;
    static DefaultOutputFileNameFactory outputFileNameFactory;
    const auto aboutViewController{nsTabViewControllerWithoutTabControl()};
    initializeAppAndRunEventLoop(eyeTracker, testSetupViewFactory,
        outputFileNameFactory, aboutViewController);
}
}

@implementation HelloWorldObjc
- (NSString *)sayHello:(NSObject<PrintProtocolDelegate> *)me {
    std::stringstream stream;
    stream << "sup dudes " << me.favoriteNumber;
    return [NSString stringWithCString:stream.str().c_str()
                              encoding:NSUTF8StringEncoding];
}
+ (void)doEverything {
    av_speech_in_noise::main();
}
@end