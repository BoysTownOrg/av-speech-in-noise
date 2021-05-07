#include "AppKitTestSetupUIFactory.h"
#import <Foundation/Foundation.h>
#import "HelloWorldObjc.h"
#include "run.h"
#include "AppKitView.h"
#include "AppKit-utility.h"
#include "EyeTrackerStub.hpp"
#include <algorithm>
#include <string>
#include <sstream>

@interface TestSetupUIObserverImpl : NSObject <TestSetupUIObserver>
@end

@implementation TestSetupUIObserverImpl {
  @public
    av_speech_in_noise::TestSetupControl::Observer *observer;
}
- (void)notifyThatConfirmButtonHasBeenClicked {
    observer->notifyThatConfirmButtonHasBeenClicked();
}
- (void)notifyThatPlayCalibrationButtonHasBeenClicked {
    observer->notifyThatPlayCalibrationButtonHasBeenClicked();
}
@end

namespace av_speech_in_noise {
class TestSetupUIImpl : public TestSetupUI {
  public:
    explicit TestSetupUIImpl(NSObject<TestSetupUI> *testSetupUI)
        : testSetupUI{testSetupUI} {}
    void show() override { return [testSetupUI show]; }
    void hide() override { return [testSetupUI hide]; }
    auto testerId() -> std::string override {
        return [testSetupUI testerId].UTF8String;
    }
    auto subjectId() -> std::string override {
        return [testSetupUI subjectId].UTF8String;
    }
    auto session() -> std::string override {
        return [testSetupUI session].UTF8String;
    }
    auto testSettingsFile() -> std::string override {
        return [testSetupUI testSettingsFile].UTF8String;
    }
    auto startingSnr() -> std::string override {
        return [testSetupUI startingSnr].UTF8String;
    }
    auto transducer() -> std::string override {
        return [testSetupUI transducer].UTF8String;
    }
    auto rmeSetting() -> std::string override {
        return [testSetupUI rmeSetting].UTF8String;
    }
    void populateTransducerMenu(std::vector<std::string> v) override {
        id nsstrings = [NSMutableArray new];
        std::for_each(v.begin(), v.end(),
            [&nsstrings](std::string str) {
                id nsstr = [NSString stringWithUTF8String:str.c_str()];
                [nsstrings addObject:nsstr];
            });
        [testSetupUI populateTransducerMenu:nsstrings];
    }
    void attach(Observer *a) override {
        const auto adapted{[[TestSetupUIObserverImpl alloc] init]};
        adapted->observer = a;
        [testSetupUI attach:adapted];
    }

  private:
    NSObject<TestSetupUI> *testSetupUI;
};

class TestSetupUIFactoryImpl : public AppKitTestSetupUIFactory {
  public:
    explicit TestSetupUIFactoryImpl(
        NSObject<TestSetupUIFactory> *testSetupUIFactory)
        : testSetupUIFactory{testSetupUIFactory} {}

    auto make(NSViewController *c) -> std::unique_ptr<TestSetupUI> override {
        return std::make_unique<TestSetupUIImpl>([testSetupUIFactory make:c]);
    }

  private:
    NSObject<TestSetupUIFactory> *testSetupUIFactory;
};

static void main(NSObject<TestSetupUIFactory> *testSetupUIFactory) {
    static EyeTrackerStub eyeTracker;
    static TestSetupUIFactoryImpl testSetupViewFactory{testSetupUIFactory};
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
+ (void)doEverything:(NSObject<TestSetupUIFactory> *)testSetupUIFactory {
    av_speech_in_noise::main(testSetupUIFactory);
}
@end
