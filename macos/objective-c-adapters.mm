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
    av_speech_in_noise::CorrectKeywordsControl::Observer *observer;
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

ChooseKeywordsUIImpl::ChooseKeywordsUIImpl(NSObject<ChooseKeywordsUI> *ui)
    : ui{ui} {}

void ChooseKeywordsUIImpl::attach(Observer *a) {
    const auto adapted{[[ChooseKeywordsUIObserverImpl alloc] init]};
    adapted->observer = a;
    [ui attach:adapted];
}

auto ChooseKeywordsUIImpl::firstKeywordCorrect() -> bool {
    return [ui firstKeywordCorrect] == YES;
}

auto ChooseKeywordsUIImpl::secondKeywordCorrect() -> bool {
    return [ui secondKeywordCorrect] == YES;
}

auto ChooseKeywordsUIImpl::thirdKeywordCorrect() -> bool {
    return [ui thirdKeywordCorrect] == YES;
}

auto ChooseKeywordsUIImpl::flagged() -> bool { return [ui flagged] == YES; }

void ChooseKeywordsUIImpl::clearFlag() { [ui clearFlag]; }

void ChooseKeywordsUIImpl::markFirstKeywordIncorrect() {
    [ui markFirstKeywordIncorrect];
}

void ChooseKeywordsUIImpl::markSecondKeywordIncorrect() {
    [ui markSecondKeywordIncorrect];
}

void ChooseKeywordsUIImpl::markThirdKeywordIncorrect() {
    [ui markThirdKeywordIncorrect];
}

void ChooseKeywordsUIImpl::markFirstKeywordCorrect() {
    [ui markFirstKeywordCorrect];
}

void ChooseKeywordsUIImpl::markSecondKeywordCorrect() {
    [ui markSecondKeywordCorrect];
}

void ChooseKeywordsUIImpl::markThirdKeywordCorrect() {
    [ui markThirdKeywordCorrect];
}

void ChooseKeywordsUIImpl::hideResponseSubmission() {
    [ui hideResponseSubmission];
}

void ChooseKeywordsUIImpl::showResponseSubmission() {
    [ui showResponseSubmission];
}

void ChooseKeywordsUIImpl::setFirstKeywordButtonText(const std::string &s) {
    [ui setFirstKeywordButtonText:nsString(s)];
}

void ChooseKeywordsUIImpl::setSecondKeywordButtonText(const std::string &s) {
    [ui setSecondKeywordButtonText:nsString(s)];
}

void ChooseKeywordsUIImpl::setThirdKeywordButtonText(const std::string &s) {
    [ui setThirdKeywordButtonText:nsString(s)];
}

void ChooseKeywordsUIImpl::setTextPrecedingFirstKeywordButton(
    const std::string &s) {
    [ui setTextPrecedingFirstKeywordButton:nsString(s)];
}

void ChooseKeywordsUIImpl::setTextFollowingFirstKeywordButton(
    const std::string &s) {
    [ui setTextFollowingFirstKeywordButton:nsString(s)];
}

void ChooseKeywordsUIImpl::setTextFollowingSecondKeywordButton(
    const std::string &s) {
    [ui setTextFollowingSecondKeywordButton:nsString(s)];
}

void ChooseKeywordsUIImpl::setTextFollowingThirdKeywordButton(
    const std::string &s) {
    [ui setTextFollowingThirdKeywordButton:nsString(s)];
}

SyllablesUIImpl::SyllablesUIImpl(NSObject<SyllablesUI> *syllablesUI)
    : syllablesUI{syllablesUI} {}

void SyllablesUIImpl::attach(Observer *a) {
    const auto adapted{[[SyllablesUIObserverImpl alloc] init]};
    adapted->observer = a;
    [syllablesUI attach:adapted];
}

void SyllablesUIImpl::hide() { [syllablesUI hide]; }

void SyllablesUIImpl::show() { [syllablesUI show]; }

auto SyllablesUIImpl::syllable() -> std::string {
    return [syllablesUI syllable].UTF8String;
}

auto SyllablesUIImpl::flagged() -> bool { return [syllablesUI flagged] == YES; }

void SyllablesUIImpl::clearFlag() { [syllablesUI clearFlag]; }

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
    id nsstrings = [NSMutableArray new];
    for_each(v.begin(), v.end(), [&nsstrings](const std::string &str) {
        id nsstr = [NSString stringWithUTF8String:str.c_str()];
        [nsstrings addObject:nsstr];
    });
    [testSetupUI populateTransducerMenu:nsstrings];
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
    id nsstrings = [NSMutableArray new];
    for_each(v.begin(), v.end(), [&nsstrings](const std::string &str) {
        if (str.c_str() == nullptr)
            return;
        id nsstr = [NSString stringWithUTF8String:str.c_str()];
        [nsstrings addObject:nsstr];
    });
    [sessionUI populateAudioDeviceMenu:nsstrings];
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

CorrectKeywordsUIImpl::CorrectKeywordsUIImpl(NSObject<CorrectKeywordsUI> *ui)
    : ui{ui} {}

void CorrectKeywordsUIImpl::attach(Observer *a) {
    const auto adapted{[[CorrectKeywordsUIObserverImpl alloc] init]};
    adapted->observer = a;
    [ui attach:adapted];
}

auto CorrectKeywordsUIImpl::correctKeywords() -> std::string {
    return [ui correctKeywords].UTF8String;
}

void CorrectKeywordsUIImpl::showCorrectKeywordsSubmission() {
    [ui showCorrectKeywordsSubmission];
}

void CorrectKeywordsUIImpl::hideCorrectKeywordsSubmission() {
    [ui hideCorrectKeywordsSubmission];
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
