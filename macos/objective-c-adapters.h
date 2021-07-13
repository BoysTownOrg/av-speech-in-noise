#ifndef AV_SPEECH_IN_NOISE_MACOS_OBJECTIVE_C_ADAPTERS_H_
#define AV_SPEECH_IN_NOISE_MACOS_OBJECTIVE_C_ADAPTERS_H_

#import "objective-c-bridge.h"

#include "run.h"

namespace av_speech_in_noise {
namespace submitting_keywords {
class UIImpl : public UI {
  public:
    explicit UIImpl(NSObject<ChooseKeywordsUI> *);
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
}

namespace submitting_syllable {
class UIImpl : public UI {
  public:
    explicit UIImpl(NSObject<SyllablesUI> *);
    void attach(Observer *a) override;
    void hide() override;
    void show() override;
    auto syllable() -> std::string override;
    auto flagged() -> bool override;
    void clearFlag() override;

  private:
    NSObject<SyllablesUI> *syllablesUI;
};
}

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
    void populateSubjectScreenMenu(const std::vector<Screen> &) override;
    auto screens() -> std::vector<Screen> override;
    auto subjectScreen() -> Screen override;

  private:
    NSObject<SessionUI> *sessionUI;
};

namespace submitting_free_response {
class UIImpl : public UI {
  public:
    explicit UIImpl(NSObject<FreeResponseUI> *);
    void attach(Observer *a) override;
    void show() override;
    void hide() override;
    auto response() -> std::string override;
    auto flagged() -> bool override;
    void clearResponse() override;
    void clearFlag() override;

  private:
    NSObject<FreeResponseUI> *freeResponseUI;
};
}

namespace submitting_number_keywords {
class UIImpl : public UI {
  public:
    explicit UIImpl(NSObject<CorrectKeywordsUI> *);
    void attach(Observer *a) override;
    auto correctKeywords() -> std::string override;
    void show() override;
    void hide() override;

  private:
    NSObject<CorrectKeywordsUI> *ui;
};
}

namespace submitting_pass_fail {
class UIImpl : public UI {
  public:
    explicit UIImpl(NSObject<PassFailUI> *);
    void attach(Observer *a) override;
    void show() override;
    void hide() override;

  private:
    NSObject<PassFailUI> *ui;
};
}
}

#endif
