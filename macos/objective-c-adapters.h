#ifndef AV_SPEECH_IN_NOISE_MACOS_OBJECTIVE_C_ADAPTERS_H_
#define AV_SPEECH_IN_NOISE_MACOS_OBJECTIVE_C_ADAPTERS_H_

#import "objective-c-bridge.h"
#include "run.h"

namespace av_speech_in_noise {
class ChooseKeywordsUIImpl : public ChooseKeywordsUI {
  public:
    explicit ChooseKeywordsUIImpl(NSObject<ChooseKeywordsUI> *);
    void attach(Observer *a) override;
    auto firstKeywordCorrect() -> bool override;
    auto secondKeywordCorrect() -> bool override;
    auto thirdKeywordCorrect() -> bool override;
    auto flagged() -> bool override;
    void clearFlag() override;
    void markFirstKeywordIncorrect() override;
    void markSecondKeywordIncorrect() override;
    void markThirdKeywordIncorrect() override;
    void markFirstKeywordCorrect() override;
    void markSecondKeywordCorrect() override;
    void markThirdKeywordCorrect() override;
    void hideResponseSubmission() override;
    void showResponseSubmission() override;
    void setFirstKeywordButtonText(const std::string &s) override;
    void setSecondKeywordButtonText(const std::string &s) override;
    void setThirdKeywordButtonText(const std::string &s) override;
    void setTextPrecedingFirstKeywordButton(const std::string &s) override;
    void setTextFollowingFirstKeywordButton(const std::string &s) override;
    void setTextFollowingSecondKeywordButton(const std::string &s) override;
    void setTextFollowingThirdKeywordButton(const std::string &s) override;

  private:
    NSObject<ChooseKeywordsUI> *ui;
};

class SyllablesUIImpl : public SyllablesUI {
  public:
    explicit SyllablesUIImpl(NSObject<SyllablesUI> *);
    void attach(Observer *a) override;
    void hide() override;
    void show() override;
    auto syllable() -> std::string override;
    auto flagged() -> bool override;
    void clearFlag() override;

  private:
    NSObject<SyllablesUI> *syllablesUI;
};

class TestSetupUIImpl : public TestSetupUI {
  public:
    explicit TestSetupUIImpl(NSObject<TestSetupUI> *);
    void show() override;
    void hide() override;
    auto testerId() -> std::string override;
    auto subjectId() -> std::string override;
    auto session() -> std::string override;
    auto testSettingsFile() -> std::string override;
    auto startingSnr() -> std::string override;
    auto transducer() -> std::string override;
    auto rmeSetting() -> std::string override;
    void populateTransducerMenu(std::vector<std::string> v) override;
    void attach(Observer *a) override;

  private:
    NSObject<TestSetupUI> *testSetupUI;
};

class TestSetupUIFactoryImpl : public AppKitTestSetupUIFactory {
  public:
    explicit TestSetupUIFactoryImpl(NSObject<TestSetupUIFactory> *);
    auto make(NSViewController *c) -> std::unique_ptr<TestSetupUI> override;

  private:
    NSObject<TestSetupUIFactory> *testSetupUIFactory;
};

class TestUIImpl : public TestUI {
  public:
    explicit TestUIImpl(NSObject<TestUI> *);
    void attach(TestControl::Observer *a) override;
    void showExitTestButton() override;
    void hideExitTestButton() override;
    void show() override;
    void hide() override;
    void display(std::string s) override;
    void secondaryDisplay(std::string s) override;
    void showNextTrialButton() override;
    void hideNextTrialButton() override;
    void showContinueTestingDialog() override;
    void hideContinueTestingDialog() override;
    void setContinueTestingDialogMessage(const std::string &s) override;
    void showSheet(std::string_view s) override;

  private:
    NSObject<TestUI> *testUI;
};

class SessionUIImpl : public SessionUI {
  public:
    explicit SessionUIImpl(NSObject<SessionUI> *);
    void showErrorMessage(std::string_view s) override;
    auto audioDevice() -> std::string override;
    void populateAudioDeviceMenu(std::vector<std::string> v) override;

  private:
    NSObject<SessionUI> *sessionUI;
};

class FreeResponseUIImpl : public FreeResponseUI {
  public:
    explicit FreeResponseUIImpl(NSObject<FreeResponseUI> *);
    void attach(Observer *a) override;
    void showFreeResponseSubmission() override;
    void hideFreeResponseSubmission() override;
    auto freeResponse() -> std::string override;
    auto flagged() -> bool override;
    void clearFreeResponse() override;
    void clearFlag() override;

  private:
    NSObject<FreeResponseUI> *freeResponseUI;
};

class CorrectKeywordsUIImpl : public CorrectKeywordsUI {
  public:
    explicit CorrectKeywordsUIImpl(NSObject<CorrectKeywordsUI> *);
    void attach(Observer *a) override;
    auto correctKeywords() -> std::string override;
    void showCorrectKeywordsSubmission() override;
    void hideCorrectKeywordsSubmission() override;

  private:
    NSObject<CorrectKeywordsUI> *ui;
};

class PassFailUIImpl : public PassFailUI {
  public:
    explicit PassFailUIImpl(NSObject<PassFailUI> *);
    void attach(Observer *a) override;
    void showEvaluationButtons() override;
    void hideEvaluationButtons() override;

  private:
    NSObject<PassFailUI> *ui;
};
}

#endif
