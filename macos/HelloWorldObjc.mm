#include "AppKitTestSetupUIFactory.h"
#import <Foundation/Foundation.h>
#include <string>
#include <sstream>
#import "HelloWorldObjc.h"
#include "run.h"
#include "AppKitView.h"
#include "AppKit-utility.h"
#include "EyeTrackerStub.hpp"

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
    void populateTransducerMenu(std::vector<std::string>) override {}
    void attach(Observer *) override {}

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