#import "objective-c-adapters.h"
#include "Foundation-utility.h"

#include <algorithm>
#include <string>

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

- (void)notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked {
    observer->notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked();
}

- (void)notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked {
    observer->notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked();
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
    av_speech_in_noise::submitting_free_response::Control::Observer *observer;
}

- (void)notifyThatSubmitButtonHasBeenClicked {
    observer->notifyThatSubmitButtonHasBeenClicked();
}
@end

@interface CorrectKeywordsUIObserverImpl : NSObject <CorrectKeywordsUIObserver>
@end

@implementation CorrectKeywordsUIObserverImpl {
  @public
    av_speech_in_noise::submitting_number_keywords::Control::Observer *observer;
}

- (void)notifyThatSubmitButtonHasBeenClicked {
    observer->notifyThatSubmitButtonHasBeenClicked();
}
@end

@interface PassFailUIObserverImpl : NSObject <PassFailUIObserver>
@end

@implementation PassFailUIObserverImpl {
  @public
    av_speech_in_noise::submitting_pass_fail::Control::Observer *observer;
}

- (void)notifyThatCorrectButtonHasBeenClicked {
    observer->notifyThatCorrectButtonHasBeenClicked();
}

- (void)notifyThatIncorrectButtonHasBeenClicked {
    observer->notifyThatIncorrectButtonHasBeenClicked();
}
@end

@interface SyllablesUIObserverImpl : NSObject <SyllablesUIObserver>
@end

@implementation SyllablesUIObserverImpl {
  @public
    av_speech_in_noise::submitting_syllable::Control::Observer *observer;
}

- (void)notifyThatResponseButtonHasBeenClicked {
    observer->notifyThatResponseButtonHasBeenClicked();
}
@end

@interface ChooseKeywordsUIObserverImpl : NSObject <ChooseKeywordsUIObserver>
@end

@implementation ChooseKeywordsUIObserverImpl {
  @public
    av_speech_in_noise::submitting_keywords::Control::Observer *observer;
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
namespace submitting_keywords {
UIImpl::UIImpl(NSObject<ChooseKeywordsUI> *ui) : ui{ui} {}

void UIImpl::attach(Observer *a) {
    const auto adapted{[[ChooseKeywordsUIObserverImpl alloc] init]};
    adapted->observer = a;
    [ui attach:adapted];
}

auto UIImpl::firstKeywordCorrect() -> bool {
    return [ui firstKeywordCorrect] == YES;
}

auto UIImpl::secondKeywordCorrect() -> bool {
    return [ui secondKeywordCorrect] == YES;
}

auto UIImpl::thirdKeywordCorrect() -> bool {
    return [ui thirdKeywordCorrect] == YES;
}

auto UIImpl::flagged() -> bool { return [ui flagged] == YES; }

void UIImpl::clearFlag() { [ui clearFlag]; }

void UIImpl::markFirstKeywordIncorrect() { [ui markFirstKeywordIncorrect]; }

void UIImpl::markSecondKeywordIncorrect() { [ui markSecondKeywordIncorrect]; }

void UIImpl::markThirdKeywordIncorrect() { [ui markThirdKeywordIncorrect]; }

void UIImpl::markFirstKeywordCorrect() { [ui markFirstKeywordCorrect]; }

void UIImpl::markSecondKeywordCorrect() { [ui markSecondKeywordCorrect]; }

void UIImpl::markThirdKeywordCorrect() { [ui markThirdKeywordCorrect]; }

void UIImpl::hideResponseSubmission() { [ui hideResponseSubmission]; }

void UIImpl::showResponseSubmission() { [ui showResponseSubmission]; }

void UIImpl::setFirstKeywordButtonText(const std::string &s) {
    [ui setFirstKeywordButtonText:nsString(s)];
}

void UIImpl::setSecondKeywordButtonText(const std::string &s) {
    [ui setSecondKeywordButtonText:nsString(s)];
}

void UIImpl::setThirdKeywordButtonText(const std::string &s) {
    [ui setThirdKeywordButtonText:nsString(s)];
}

void UIImpl::setTextPrecedingFirstKeywordButton(const std::string &s) {
    [ui setTextPrecedingFirstKeywordButton:nsString(s)];
}

void UIImpl::setTextFollowingFirstKeywordButton(const std::string &s) {
    [ui setTextFollowingFirstKeywordButton:nsString(s)];
}

void UIImpl::setTextFollowingSecondKeywordButton(const std::string &s) {
    [ui setTextFollowingSecondKeywordButton:nsString(s)];
}

void UIImpl::setTextFollowingThirdKeywordButton(const std::string &s) {
    [ui setTextFollowingThirdKeywordButton:nsString(s)];
}
}

namespace submitting_syllable {
UIImpl::UIImpl(NSObject<SyllablesUI> *syllablesUI) : syllablesUI{syllablesUI} {}

void UIImpl::attach(Observer *a) {
    const auto adapted{[[SyllablesUIObserverImpl alloc] init]};
    adapted->observer = a;
    [syllablesUI attach:adapted];
}

void UIImpl::hide() { [syllablesUI hide]; }

void UIImpl::show() { [syllablesUI show]; }

auto UIImpl::syllable() -> std::string {
    return [syllablesUI syllable].UTF8String;
}

auto UIImpl::flagged() -> bool { return [syllablesUI flagged] == YES; }

void UIImpl::clearFlag() { [syllablesUI clearFlag]; }
}

TestSetupUIImpl::TestSetupUIImpl(NSObject<TestSetupUI> *testSetupUI)
    : testSetupUI{testSetupUI} {}

void TestSetupUIImpl::show() { return [testSetupUI show]; }

void TestSetupUIImpl::hide() { return [testSetupUI hide]; }

auto TestSetupUIImpl::testerId() -> std::string {
    return [testSetupUI testerId].UTF8String;
}

auto TestSetupUIImpl::subjectId() -> std::string {
    return [testSetupUI subjectId].UTF8String;
}

auto TestSetupUIImpl::session() -> std::string {
    return [testSetupUI session].UTF8String;
}

auto TestSetupUIImpl::testSettingsFile() -> std::string {
    return [testSetupUI testSettingsFile].UTF8String;
}

auto TestSetupUIImpl::startingSnr() -> std::string {
    return [testSetupUI startingSnr].UTF8String;
}

auto TestSetupUIImpl::transducer() -> std::string {
    return [testSetupUI transducer].UTF8String;
}

auto TestSetupUIImpl::rmeSetting() -> std::string {
    return [testSetupUI rmeSetting].UTF8String;
}

void TestSetupUIImpl::populateTransducerMenu(std::vector<std::string> v) {
    const auto adapted{[NSMutableArray<NSString *> arrayWithCapacity:v.size()]};
    for_each(v.begin(), v.end(), [&adapted](const std::string &str) {
        [adapted addObject:nsString(str)];
    });
    [testSetupUI populateTransducerMenu:adapted];
}

void TestSetupUIImpl::attach(Observer *a) {
    const auto adapted{[[TestSetupUIObserverImpl alloc] init]};
    adapted->observer = a;
    [testSetupUI attach:adapted];
}

TestSetupUIFactoryImpl::TestSetupUIFactoryImpl(
    NSObject<TestSetupUIFactory> *testSetupUIFactory)
    : testSetupUIFactory{testSetupUIFactory} {}

auto TestSetupUIFactoryImpl::make(NSViewController *c)
    -> std::unique_ptr<TestSetupUI> {
    return std::make_unique<TestSetupUIImpl>([testSetupUIFactory make:c]);
}

TestUIImpl::TestUIImpl(NSObject<TestUI> *testUI) : testUI{testUI} {}

void TestUIImpl::attach(TestControl::Observer *a) {
    const auto adapted{[[TestUIObserverImpl alloc] init]};
    adapted->observer = a;
    [testUI attach:adapted];
}

void TestUIImpl::showExitTestButton() { [testUI showExitTestButton]; }

void TestUIImpl::hideExitTestButton() { [testUI hideExitTestButton]; }

void TestUIImpl::show() { [testUI show]; }

void TestUIImpl::hide() { [testUI hide]; }

void TestUIImpl::display(std::string s) { [testUI display:nsString(s)]; }

void TestUIImpl::secondaryDisplay(std::string s) {
    [testUI secondaryDisplay:nsString(s)];
}

void TestUIImpl::showNextTrialButton() { [testUI showNextTrialButton]; }

void TestUIImpl::hideNextTrialButton() { [testUI hideNextTrialButton]; }

void TestUIImpl::showContinueTestingDialog() {
    [testUI showContinueTestingDialog];
}

void TestUIImpl::hideContinueTestingDialog() {
    [testUI hideContinueTestingDialog];
}

void TestUIImpl::setContinueTestingDialogMessage(const std::string &s) {
    [testUI setContinueTestingDialogMessage:nsString(s)];
}

void TestUIImpl::showSheet(std::string_view s) {
    [testUI showSheet:nsString(std::string{s})];
}

SessionUIImpl::SessionUIImpl(NSObject<SessionUI> *sessionUI)
    : sessionUI{sessionUI} {}

void SessionUIImpl::showErrorMessage(std::string_view s) {
    [sessionUI showErrorMessage:nsString(std::string{s})];
}

auto SessionUIImpl::audioDevice() -> std::string {
    return [sessionUI audioDevice].UTF8String;
}

void SessionUIImpl::populateAudioDeviceMenu(std::vector<std::string> v) {
    const auto adapted{[NSMutableArray<NSString *> arrayWithCapacity:v.size()]};
    for_each(v.begin(), v.end(), [&adapted](const std::string &str) {
        [adapted addObject:nsString(str)];
    });
    [sessionUI populateAudioDeviceMenu:adapted];
}

void SessionUIImpl::populateSubjectScreenMenu(const std::vector<Screen> &v) {
    const auto adapted{[NSMutableArray<NSString *> arrayWithCapacity:v.size()]};
    for_each(v.begin(), v.end(), [&adapted](const Screen &screen) {
        [adapted addObject:nsString(screen.name)];
    });
    [sessionUI populateAudioDeviceMenu:adapted];
}

auto SessionUIImpl::screens() -> std::vector<Screen> {
    std::vector<Screen> screens;
    for (NSScreen *item in [NSScreen screens]) {
        screens.push_back(Screen{item.localizedName.UTF8String});
    }
    return screens;
}

auto SessionUIImpl::subjectScreen() -> Screen {
    return Screen{[sessionUI subjectScreen].UTF8String};
}

namespace submitting_free_response {
UIImpl::UIImpl(NSObject<FreeResponseUI> *freeResponseUI)
    : freeResponseUI{freeResponseUI} {}

void UIImpl::attach(Observer *a) {
    const auto adapted{[[FreeResponseUIObserverImpl alloc] init]};
    adapted->observer = a;
    [freeResponseUI attach:adapted];
}

void UIImpl::show() { [freeResponseUI showFreeResponseSubmission]; }

void UIImpl::hide() { [freeResponseUI hideFreeResponseSubmission]; }

auto UIImpl::response() -> std::string {
    return [freeResponseUI freeResponse].UTF8String;
}

auto UIImpl::flagged() -> bool { return [freeResponseUI flagged] == YES; }

void UIImpl::clearResponse() { [freeResponseUI clearFreeResponse]; }

void UIImpl::clearFlag() { [freeResponseUI clearFlag]; }
}

namespace submitting_number_keywords {
UIImpl::UIImpl(NSObject<CorrectKeywordsUI> *ui) : ui{ui} {}

void UIImpl::attach(Observer *a) {
    const auto adapted{[[CorrectKeywordsUIObserverImpl alloc] init]};
    adapted->observer = a;
    [ui attach:adapted];
}

auto UIImpl::correctKeywords() -> std::string {
    return [ui correctKeywords].UTF8String;
}

void UIImpl::show() { [ui showCorrectKeywordsSubmission]; }

void UIImpl::hide() { [ui hideCorrectKeywordsSubmission]; }
}

namespace submitting_pass_fail {
UIImpl::UIImpl(NSObject<PassFailUI> *ui) : ui{ui} {}

void UIImpl::attach(Observer *a) {
    const auto adapted{[[PassFailUIObserverImpl alloc] init]};
    adapted->observer = a;
    [ui attach:adapted];
}

void UIImpl::show() { [ui showEvaluationButtons]; }

void UIImpl::hide() { [ui hideEvaluationButtons]; }
}
}
