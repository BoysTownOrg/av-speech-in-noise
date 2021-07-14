#include "../objective-c-adapters.h"
#include "../objective-c-bridge.h"
#include "../AppKit-utility.h"
#include "../AppKitView.h"
#include "../EyeTrackerStub.hpp"
#include "../run.h"

#include <av-speech-in-noise/ui/SubjectImpl.hpp>

namespace av_speech_in_noise {
static void main(NSObject<TestSetupUIFactory> *testSetupUIFactory,
    NSObject<SessionUI> *sessionUI, NSObject<TestUI> *testUI,
    NSObject<FreeResponseUI> *freeResponseUI,
    NSObject<SyllablesUI> *syllablesUI,
    NSObject<ChooseKeywordsUI> *chooseKeywordsUI,
    NSObject<CorrectKeywordsUI> *correctKeywordsUI,
    NSObject<PassFailUI> *passFailUI) {
    static EyeTrackerStub eyeTracker;
    static TestSetupUIFactoryImpl testSetupViewFactory{testSetupUIFactory};
    static DefaultOutputFileNameFactory outputFileNameFactory;
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
        subjectWindow);
}
}

@implementation AvSpeechInNoiseMain
+ (void)default:(NSObject<TestSetupUIFactory> *)testSetupUIFactory
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