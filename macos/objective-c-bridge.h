#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

@protocol TestSetupUIObserver
- (void)notifyThatConfirmButtonHasBeenClicked;
- (void)notifyThatPlayCalibrationButtonHasBeenClicked;
- (void)notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked;
- (void)notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked;
@end

@protocol TestUIObserver
- (void)exitTest;
- (void)playTrial;
- (void)declineContinuingTesting;
- (void)acceptContinuingTesting;
@end

@protocol FreeResponseUIObserver
- (void)notifyThatSubmitButtonHasBeenClicked;
@end

@protocol SyllablesUIObserver
- (void)notifyThatResponseButtonHasBeenClicked;
@end

@protocol ChooseKeywordsUIObserver
- (void)notifyThatFirstKeywordButtonIsClicked;
- (void)notifyThatSecondKeywordButtonIsClicked;
- (void)notifyThatThirdKeywordButtonIsClicked;
- (void)notifyThatAllWrongButtonHasBeenClicked;
- (void)notifyThatResetButtonIsClicked;
- (void)notifyThatSubmitButtonHasBeenClicked;
@end

@protocol CorrectKeywordsUIObserver
- (void)notifyThatSubmitButtonHasBeenClicked;
@end

@protocol PassFailUIObserver
- (void)notifyThatCorrectButtonHasBeenClicked;
- (void)notifyThatIncorrectButtonHasBeenClicked;
@end

@protocol ChooseKeywordsUI
- (void)attach:(id<ChooseKeywordsUIObserver>)observer;
- (BOOL)firstKeywordCorrect;
- (BOOL)secondKeywordCorrect;
- (BOOL)thirdKeywordCorrect;
- (BOOL)flagged;
- (void)clearFlag;
- (void)markFirstKeywordIncorrect;
- (void)markSecondKeywordIncorrect;
- (void)markThirdKeywordIncorrect;
- (void)markFirstKeywordCorrect;
- (void)markSecondKeywordCorrect;
- (void)markThirdKeywordCorrect;
- (void)hideResponseSubmission;
- (void)showResponseSubmission;
- (void)setFirstKeywordButtonText:(NSString *)text;
- (void)setSecondKeywordButtonText:(NSString *)text;
- (void)setThirdKeywordButtonText:(NSString *)text;
- (void)setTextPrecedingFirstKeywordButton:(NSString *)text;
- (void)setTextFollowingFirstKeywordButton:(NSString *)text;
- (void)setTextFollowingSecondKeywordButton:(NSString *)text;
- (void)setTextFollowingThirdKeywordButton:(NSString *)text;
@end

@protocol SyllablesUI
- (void)hide;
- (void)show;
- (NSString *)syllable;
- (BOOL)flagged;
- (void)clearFlag;
- (void)attach:(id<SyllablesUIObserver>)observer;
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
- (void)populateTransducerMenu:(NSArray<NSString *> *)transducers;
- (void)attach:(id<TestSetupUIObserver>)observer;
@end

@protocol TestUI
- (void)attach:(id<TestUIObserver>)observer;
- (void)showExitTestButton;
- (void)hideExitTestButton;
- (void)show;
- (void)hide;
- (void)display:(NSString *)something;
- (void)secondaryDisplay:(NSString *)something;
- (void)showNextTrialButton;
- (void)hideNextTrialButton;
- (void)showContinueTestingDialog;
- (void)hideContinueTestingDialog;
- (void)setContinueTestingDialogMessage:(NSString *)something;
- (void)showSheet:(NSString *)something;
@end

@protocol SessionUI
- (void)eventLoop;
- (void)showErrorMessage:(NSString *)something;
- (NSString *)audioDevice;
- (void)populateAudioDeviceMenu:(NSArray<NSString *> *)devices;
@end

@protocol FreeResponseUI
- (void)attach:(id<FreeResponseUIObserver>)observer;
- (void)showFreeResponseSubmission;
- (void)hideFreeResponseSubmission;
- (NSString *)freeResponse;
- (BOOL)flagged;
- (void)clearFreeResponse;
- (void)clearFlag;
@end

@protocol CorrectKeywordsUI
- (void)attach:(id<CorrectKeywordsUIObserver>)observer;
- (void)hideCorrectKeywordsSubmission;
- (void)showCorrectKeywordsSubmission;
- (NSString *)correctKeywords;
@end

@protocol PassFailUI
- (void)attach:(id<PassFailUIObserver>)observer;
- (void)hideEvaluationButtons;
- (void)showEvaluationButtons;
@end

@protocol TestSetupUIFactory
- (id<TestSetupUI>)make:(NSViewController *)viewController;
@end

@interface HelloWorldObjc : NSObject
+ (void)doEverything:(NSObject<TestSetupUIFactory> *)testSetupUIFactory
            withSessionUI:(NSObject<SessionUI> *)sessionUI
               withTestUI:(NSObject<TestUI> *)testUI
       withFreeResponseUI:(NSObject<FreeResponseUI> *)freeResponseUI
          withSyllablesUI:(NSObject<SyllablesUI> *)syllablesUI
     withChooseKeywordsUI:(NSObject<ChooseKeywordsUI> *)chooseKeywordsUI
    withCorrectKeywordsUI:(NSObject<CorrectKeywordsUI> *)correctKeywordsUI
           withPassFailUI:(NSObject<PassFailUI> *)passFailUI;
@end

@interface AvSpeechInNoiseUtility : NSObject
+ (NSString *)meta:(NSString *)stem withExtension:(NSString *)extension;
+ (NSString *)resourcePath:(NSString *)stem withExtension:(NSString *)extension;
@end