#include "AppKitTestSetupUIFactory.h"
#include "Foundation-utility.h"
#import "HelloWorldObjc.h"
#include "run.h"
#include "AppKit-utility.h"
#include "EyeTrackerStub.hpp"
#import <Foundation/Foundation.h>
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

@interface TestUIObserverImpl : NSObject <TestUIObserver>
@end

@implementation TestUIObserverImpl {
  @public
    av_speech_in_noise::TestControl::Observer *observer;
}

- (void)playTrial {
    observer->playTrial();
}

- (void)exitTest {
    observer->exitTest();
}

- (void)declineContinuingTesting {
    observer->declineContinuingTesting();
}

- (void)acceptContinuingTesting {
    observer->acceptContinuingTesting();
}
@end

@interface FreeResponseUIObserverImpl : NSObject <FreeResponseUIObserver>
@end

@implementation FreeResponseUIObserverImpl {
  @public
    av_speech_in_noise::FreeResponseControl::Observer *observer;
}

- (void)notifyThatSubmitButtonHasBeenClicked {
    observer->notifyThatSubmitButtonHasBeenClicked();
}
@end

@interface SyllablesUIObserverImpl : NSObject <SyllablesUIObserver>
@end

@implementation SyllablesUIObserverImpl {
  @public
    av_speech_in_noise::SyllablesControl::Observer *observer;
}

- (void)notifyThatResponseButtonHasBeenClicked {
    observer->notifyThatResponseButtonHasBeenClicked();
}
@end

@interface ChooseKeywordsUIObserverImpl : NSObject <ChooseKeywordsUIObserver>
@end

@implementation ChooseKeywordsUIObserverImpl {
  @public
    av_speech_in_noise::ChooseKeywordsControl::Observer *observer;
}

- (void)notifyThatFirstKeywordButtonIsClicked {
    observer->notifyThatFirstKeywordButtonIsClicked();
}

- (void)notifyThatSecondKeywordButtonIsClicked {
    observer->notifyThatSecondKeywordButtonIsClicked();
}

- (void)notifyThatThirdKeywordButtonIsClicked {
    observer->notifyThatThirdKeywordButtonIsClicked();
}

- (void)notifyThatAllWrongButtonHasBeenClicked {
    observer->notifyThatAllWrongButtonHasBeenClicked();
}

- (void)notifyThatResetButtonIsClicked {
    observer->notifyThatResetButtonIsClicked();
}

- (void)notifyThatSubmitButtonHasBeenClicked {
    observer->notifyThatSubmitButtonHasBeenClicked();
}
@end

namespace av_speech_in_noise {
class ChooseKeywordsUIImpl : public ChooseKeywordsUI_ {
  public:
    explicit ChooseKeywordsUIImpl(NSObject<ChooseKeywordsUI> *ui) : ui{ui} {}

    void attach(Observer *a) override {
        const auto adapted{[[ChooseKeywordsUIObserverImpl alloc] init]};
        adapted->observer = a;
        [ui attach:adapted];
    }

    auto firstKeywordCorrect() -> bool override {
        return [ui firstKeywordCorrect] == YES;
    }

    auto secondKeywordCorrect() -> bool override {
        return [ui secondKeywordCorrect] == YES;
    }

    auto thirdKeywordCorrect() -> bool override {
        return [ui thirdKeywordCorrect] == YES;
    }

    auto flagged() -> bool override { return [ui flagged] == YES; }

    void clearFlag() override { [ui clearFlag]; }

    void markFirstKeywordIncorrect() override {
        [ui markFirstKeywordIncorrect];
    }

    void markSecondKeywordIncorrect() override {
        [ui markSecondKeywordIncorrect];
    }

    void markThirdKeywordIncorrect() override {
        [ui markThirdKeywordIncorrect];
    }

    void markFirstKeywordCorrect() override { [ui markFirstKeywordCorrect]; }

    void markSecondKeywordCorrect() override { [ui markSecondKeywordCorrect]; }

    void markThirdKeywordCorrect() override { [ui markThirdKeywordCorrect]; }

    void hideResponseSubmission() override { [ui hideResponseSubmission]; }

    void showResponseSubmission() override { [ui showResponseSubmission]; }

    void setFirstKeywordButtonText(const std::string &s) override {
        [ui setFirstKeywordButtonText:nsString(s)];
    }

    void setSecondKeywordButtonText(const std::string &s) override {
        [ui setSecondKeywordButtonText:nsString(s)];
    }

    void setThirdKeywordButtonText(const std::string &s) override {
        [ui setThirdKeywordButtonText:nsString(s)];
    }

    void setTextPrecedingFirstKeywordButton(const std::string &s) override {
        [ui setTextPrecedingFirstKeywordButton:nsString(s)];
    }

    void setTextFollowingFirstKeywordButton(const std::string &s) override {
        [ui setTextFollowingFirstKeywordButton:nsString(s)];
    }

    void setTextFollowingSecondKeywordButton(const std::string &s) override {
        [ui setTextFollowingSecondKeywordButton:nsString(s)];
    }

    void setTextFollowingThirdKeywordButton(const std::string &s) override {
        [ui setTextFollowingThirdKeywordButton:nsString(s)];
    }

  private:
    NSObject<ChooseKeywordsUI> *ui;
};

class SyllablesUIImpl : public SyllablesUI_ {
  public:
    explicit SyllablesUIImpl(NSObject<SyllablesUI> *syllablesUI)
        : syllablesUI{syllablesUI} {}

    void attach(Observer *a) override {
        const auto adapted{[[SyllablesUIObserverImpl alloc] init]};
        adapted->observer = a;
        [syllablesUI attach:adapted];
    }

    void hide() override { [syllablesUI hide]; }

    void show() override { [syllablesUI show]; }

    auto syllable() -> std::string override {
        return [syllablesUI syllable].UTF8String;
    }

    auto flagged() -> bool override { return [syllablesUI flagged] == YES; }

    void clearFlag() override { [syllablesUI clearFlag]; }

  private:
    NSObject<SyllablesUI> *syllablesUI;
};

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
        for_each(v.begin(), v.end(), [&nsstrings](const std::string &str) {
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

class TestUIImpl : public TestUI {
  public:
    explicit TestUIImpl(NSObject<TestUI> *testUI) : testUI{testUI} {}

    void attach(TestControl::Observer *a) override {
        const auto adapted{[[TestUIObserverImpl alloc] init]};
        adapted->observer = a;
        [testUI attach:adapted];
    }

    void showExitTestButton() override { [testUI showExitTestButton]; }

    void hideExitTestButton() override { [testUI hideExitTestButton]; }

    void show() override { [testUI show]; }

    void hide() override { [testUI hide]; }

    void display(std::string s) override { [testUI display:nsString(s)]; }

    void secondaryDisplay(std::string s) override {
        [testUI secondaryDisplay:nsString(s)];
    }

    void showNextTrialButton() override { [testUI showNextTrialButton]; }

    void hideNextTrialButton() override { [testUI hideNextTrialButton]; }

    void showContinueTestingDialog() override {
        [testUI showContinueTestingDialog];
    }

    void hideContinueTestingDialog() override {
        [testUI hideContinueTestingDialog];
    }

    void setContinueTestingDialogMessage(const std::string &s) override {
        [testUI setContinueTestingDialogMessage:nsString(s)];
    }

    void showSheet(std::string_view s) override {
        [testUI showSheet:nsString(std::string{s})];
    }

  private:
    NSObject<TestUI> *testUI;
};

class SessionUIImpl : public SessionUI {
  public:
    explicit SessionUIImpl(NSObject<SessionUI> *sessionUI)
        : sessionUI{sessionUI} {}

    void eventLoop() override { [sessionUI eventLoop]; }

    void showErrorMessage(std::string_view s) override {
        [sessionUI showErrorMessage:nsString(std::string{s})];
    }

    auto audioDevice() -> std::string override {
        return [sessionUI audioDevice].UTF8String;
    }

    void populateAudioDeviceMenu(std::vector<std::string> v) override {
        id nsstrings = [NSMutableArray new];
        for_each(v.begin(), v.end(), [&nsstrings](const std::string &str) {
            id nsstr = [NSString stringWithUTF8String:str.c_str()];
            [nsstrings addObject:nsstr];
        });
        [sessionUI populateAudioDeviceMenu:nsstrings];
    }

  private:
    NSObject<SessionUI> *sessionUI;
};

class FreeResponseUIImpl : public FreeResponseUI {
  public:
    explicit FreeResponseUIImpl(NSObject<FreeResponseUI> *freeResponseUI)
        : freeResponseUI{freeResponseUI} {}

    void attach(Observer *a) override {
        const auto adapted{[[FreeResponseUIObserverImpl alloc] init]};
        adapted->observer = a;
        [freeResponseUI attach:adapted];
    }

    void showFreeResponseSubmission() override {
        [freeResponseUI showFreeResponseSubmission];
    }

    void hideFreeResponseSubmission() override {
        [freeResponseUI hideFreeResponseSubmission];
    }

    auto freeResponse() -> std::string override {
        return [freeResponseUI freeResponse].UTF8String;
    }

    auto flagged() -> bool override { return [freeResponseUI flagged] == YES; }

    void clearFreeResponse() override { [freeResponseUI clearFreeResponse]; }

    void clearFlag() override { [freeResponseUI clearFlag]; }

  private:
    NSObject<FreeResponseUI> *freeResponseUI;
};

static void main(NSObject<TestSetupUIFactory> *testSetupUIFactory,
    NSObject<SessionUI> *sessionUI, NSObject<TestUI> *testUI,
    NSObject<FreeResponseUI> *freeResponseUI,
    NSObject<SyllablesUI> *syllablesUI,
    NSObject<ChooseKeywordsUI> *chooseKeywordsUI) {
    static EyeTrackerStub eyeTracker;
    static TestSetupUIFactoryImpl testSetupViewFactory{testSetupUIFactory};
    static DefaultOutputFileNameFactory outputFileNameFactory;
    const auto aboutViewController{nsTabViewControllerWithoutTabControl()};
    static SessionUIImpl sessionUIAdapted{sessionUI};
    static TestUIImpl testUIAdapted{testUI};
    static FreeResponseUIImpl freeResponseUIAdapted{freeResponseUI};
    static SyllablesUIImpl syllablesUIAdapted{syllablesUI};
    static ChooseKeywordsUIImpl chooseKeywordsUIAdapted{chooseKeywordsUI};
    initializeAppAndRunEventLoop(eyeTracker, testSetupViewFactory,
        outputFileNameFactory, aboutViewController, nullptr,
        "Documents/AvSpeechInNoise Data", &sessionUIAdapted, &testUIAdapted,
        &freeResponseUIAdapted, &syllablesUIAdapted, &chooseKeywordsUIAdapted);
}
}

@implementation HelloWorldObjc
+ (void)doEverything:(NSObject<TestSetupUIFactory> *)testSetupUIFactory
           withSessionUI:(NSObject<SessionUI> *)sessionUI
              withTestUI:(NSObject<TestUI> *)testUI
      withFreeResponseUI:(NSObject<FreeResponseUI> *)freeResponseUI
         withSyllablesUI:(NSObject<SyllablesUI> *)syllablesUI
    withChooseKeywordsUI:(NSObject<ChooseKeywordsUI> *)chooseKeywordsUI {
    av_speech_in_noise::main(testSetupUIFactory, sessionUI, testUI,
        freeResponseUI, syllablesUI, chooseKeywordsUI);
}
@end
