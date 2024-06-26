#include "../run.h"
#include "../objective-c-adapters.h"
#include "../objective-c-bridge.h"
#include "../EyeTrackerStub.hpp"
#include "../AppKit-utility.h"
#include "../AppKitView.h"

#include <av-speech-in-noise/ui/SubjectImpl.hpp>

#import <AppKit/AppKit.h>

namespace av_speech_in_noise {
class MetaConditionOutputFileNameFactory : public OutputFileNameFactory {
  public:
    auto make(TimeStamp &timeStamp)
        -> std::unique_ptr<OutputFileName> override {
        return std::make_unique<MetaConditionOutputFileName>(timeStamp);
    }
};

class CongratulatesUserWhenTestCompletes : public SessionController::Observer {
  public:
    void notifyThatTestIsComplete() override {
        const auto alert{[[NSAlert alloc] init]};
        [alert setMessageText:@""];
        [alert setInformativeText:@"Condition complete, great work!"];
        [alert addButtonWithTitle:@"Continue"];
        [alert runModal];
    }
};

static void main(NSObject<TestSetupUIFactory> *testSetupUIFactory,
    NSObject<SessionUI> *sessionUI, NSObject<TestUI> *testUI,
    NSObject<FreeResponseUI> *freeResponseUI,
    NSObject<SyllablesUI> *syllablesUI,
    NSObject<ChooseKeywordsUI> *chooseKeywordsUI,
    NSObject<CorrectKeywordsUI> *correctKeywordsUI,
    NSObject<PassFailUI> *passFailUI) {
    static EyeTrackerStub eyeTracker;
    static TestSetupUIFactoryImpl testSetupViewFactory{testSetupUIFactory};
    static MetaConditionOutputFileNameFactory outputFileNameFactory;
    static CongratulatesUserWhenTestCompletes
        congratulatesUserWhenTestCompletes;
    static SessionUIImpl sessionUIAdapted{sessionUI};
    static TestUIImpl testUIAdapted{testUI};
    static submitting_free_response::UIImpl freeResponseUIAdapted{
        freeResponseUI};
    static submitting_syllable::UIImpl syllablesUIAdapted{syllablesUI};
    static submitting_keywords::UIImpl chooseKeywordsUIAdapted{
        chooseKeywordsUI};
    static submitting_number_keywords::UIImpl correctKeywordsUIAdapted{
        correctKeywordsUI};
    static submitting_pass_fail::UIImpl passFailUIAdapted{passFailUI};
    const auto subjectWindow{av_speech_in_noise::subjectWindow()};
    static SubjectAppKitView subjectView{subjectWindow};
    static av_speech_in_noise::SubjectPresenterImpl subjectPresenter{
        subjectView, sessionUIAdapted};
    initializeAppAndRunEventLoop(eyeTracker, outputFileNameFactory,
        testSetupViewFactory, sessionUIAdapted, testUIAdapted,
        freeResponseUIAdapted, syllablesUIAdapted, chooseKeywordsUIAdapted,
        correctKeywordsUIAdapted, passFailUIAdapted, subjectPresenter,
        subjectWindow, &congratulatesUserWhenTestCompletes);
}
}

@implementation AvSpeechInNoiseMain
+ (void)facemaskStudy:(NSObject<TestSetupUIFactory> *)testSetupUIFactory
            withSessionUI:(NSObject<SessionUI> *)sessionUI
               withTestUI:(NSObject<TestUI> *)testUI
       withFreeResponseUI:(NSObject<FreeResponseUI> *)freeResponseUI
          withSyllablesUI:(NSObject<SyllablesUI> *)syllablesUI
     withChooseKeywordsUI:(NSObject<ChooseKeywordsUI> *)chooseKeywordsUI
    withCorrectKeywordsUI:(NSObject<CorrectKeywordsUI> *)correctKeywordsUI
           withPassFailUI:(NSObject<PassFailUI> *)passFailUI {
    av_speech_in_noise::main(testSetupUIFactory, sessionUI, testUI,
        freeResponseUI, syllablesUI, chooseKeywordsUI, correctKeywordsUI,
        passFailUI);
}
@end
