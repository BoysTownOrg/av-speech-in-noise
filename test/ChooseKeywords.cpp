#include "FixedLevelMethodStub.hpp"
#include "assert-utility.hpp"
#include "TestViewStub.hpp"
#include "TestControllerStub.hpp"
#include "RunningATestStub.hpp"

#include <av-speech-in-noise/ui/ChooseKeywords.hpp>

#include <gtest/gtest.h>

namespace av_speech_in_noise::submitting_keywords {
namespace {
class ViewStub : public View {
  public:
    [[nodiscard]] auto responseSubmissionHidden() const -> bool {
        return responseSubmissionHidden_;
    }

    void hideResponseSubmission() override { responseSubmissionHidden_ = true; }

    [[nodiscard]] auto responseSubmissionShown() const -> bool {
        return responseSubmissionShown_;
    }

    void showResponseSubmission() override { responseSubmissionShown_ = true; }

    auto firstKeywordButtonText() -> std::string {
        return firstKeywordButtonText_;
    }

    auto secondKeywordButtonText() -> std::string {
        return secondKeywordButtonText_;
    }

    auto thirdKeywordButtonText() -> std::string {
        return thirdKeywordButtonText_;
    }

    void setFirstKeywordButtonText(const std::string &s) override {
        firstKeywordButtonText_ = s;
    }

    void setSecondKeywordButtonText(const std::string &s) override {
        secondKeywordButtonText_ = s;
    }

    void setThirdKeywordButtonText(const std::string &s) override {
        thirdKeywordButtonText_ = s;
    }

    void setTextPrecedingFirstKeywordButton(const std::string &s) override {
        textPrecedingFirstKeywordButton_ = s;
    }

    auto textPrecedingFirstKeywordButton() -> std::string {
        return textPrecedingFirstKeywordButton_;
    }

    void setTextFollowingFirstKeywordButton(const std::string &s) override {
        textFollowingFirstKeywordButton_ = s;
    }

    auto textFollowingFirstKeywordButton() -> std::string {
        return textFollowingFirstKeywordButton_;
    }

    void setTextFollowingSecondKeywordButton(const std::string &s) override {
        textFollowingSecondKeywordButton_ = s;
    }

    auto textFollowingSecondKeywordButton() -> std::string {
        return textFollowingSecondKeywordButton_;
    }

    void setTextFollowingThirdKeywordButton(const std::string &s) override {
        textFollowingThirdKeywordButton_ = s;
    }

    auto textFollowingThirdKeywordButton() -> std::string {
        return textFollowingThirdKeywordButton_;
    }

    [[nodiscard]] auto markFirstKeywordIncorrectCalled() const -> bool {
        return markFirstKeywordIncorrectCalled_;
    }

    [[nodiscard]] auto markSecondKeywordIncorrectCalled() const -> bool {
        return markSecondKeywordIncorrectCalled_;
    }

    [[nodiscard]] auto markThirdKeywordIncorrectCalled() const -> bool {
        return markThirdKeywordIncorrectCalled_;
    }

    [[nodiscard]] auto markFirstKeywordCorrectCalled() const -> bool {
        return markFirstKeywordCorrectCalled_;
    }

    [[nodiscard]] auto markSecondKeywordCorrectCalled() const -> bool {
        return markSecondKeywordCorrectCalled_;
    }

    [[nodiscard]] auto markThirdKeywordCorrectCalled() const -> bool {
        return markThirdKeywordCorrectCalled_;
    }

    void markFirstKeywordIncorrect() override {
        markFirstKeywordIncorrectCalled_ = true;
    }

    void markSecondKeywordIncorrect() override {
        markSecondKeywordIncorrectCalled_ = true;
    }

    void markThirdKeywordIncorrect() override {
        markThirdKeywordIncorrectCalled_ = true;
    }

    void markFirstKeywordCorrect() override {
        markFirstKeywordCorrectCalled_ = true;
    }

    void markSecondKeywordCorrect() override {
        markSecondKeywordCorrectCalled_ = true;
    }

    void markThirdKeywordCorrect() override {
        markThirdKeywordCorrectCalled_ = true;
    }

    [[nodiscard]] auto flagCleared() const -> bool { return flagCleared_; }

    void clearFlag() override { flagCleared_ = true; }

  private:
    std::string textFollowingThirdKeywordButton_;
    std::string textFollowingSecondKeywordButton_;
    std::string textFollowingFirstKeywordButton_;
    std::string textPrecedingFirstKeywordButton_;
    std::string firstKeywordButtonText_;
    std::string secondKeywordButtonText_;
    std::string thirdKeywordButtonText_;
    bool flagCleared_{};
    bool responseSubmissionHidden_{};
    bool responseSubmissionShown_{};
    bool markFirstKeywordIncorrectCalled_{};
    bool markSecondKeywordIncorrectCalled_{};
    bool markThirdKeywordIncorrectCalled_{};
    bool markFirstKeywordCorrectCalled_{};
    bool markSecondKeywordCorrectCalled_{};
    bool markThirdKeywordCorrectCalled_{};
};

class ControlStub : public Control {
  public:
    void setFlagged() { flagged_ = true; }

    void setFirstKeywordCorrect() { firstKeywordCorrect_ = true; }

    void setThirdKeywordCorrect() { thirdKeywordCorrect_ = true; }

    auto flagged() -> bool override { return flagged_; }

    auto thirdKeywordCorrect() -> bool override { return thirdKeywordCorrect_; }

    auto secondKeywordCorrect() -> bool override {
        return secondKeywordCorrect_;
    }

    auto firstKeywordCorrect() -> bool override { return firstKeywordCorrect_; }

    void notifyThatSubmitButtonHasBeenClicked() {
        observer->notifyThatSubmitButtonHasBeenClicked();
    }

    void attach(Observer *a) override { observer = a; }

    void notifyThatAllWrongButtonHasBeenClicked() {
        observer->notifyThatAllWrongButtonHasBeenClicked();
    }

    void notifyThatResetButtonIsClicked() {
        observer->notifyThatResetButtonIsClicked();
    }

    void notifyThatFirstKeywordButtonIsClicked() {
        observer->notifyThatFirstKeywordButtonIsClicked();
    }

    void notifyThatSecondKeywordButtonIsClicked() {
        observer->notifyThatSecondKeywordButtonIsClicked();
    }

    void notifyThatThirdKeywordButtonIsClicked() {
        observer->notifyThatThirdKeywordButtonIsClicked();
    }

  private:
    Observer *observer{};
    bool flagged_{};
    bool firstKeywordCorrect_{};
    bool secondKeywordCorrect_{};
    bool thirdKeywordCorrect_{};
};

class PresenterStub : public Presenter {
  public:
    void markFirstKeywordIncorrect() override {
        markFirstKeywordIncorrectCalled_ = true;
    }

    void markSecondKeywordIncorrect() override {
        markSecondKeywordIncorrectCalled_ = true;
    }

    void markThirdKeywordIncorrect() override {
        markThirdKeywordIncorrectCalled_ = true;
    }

    void markAllKeywordsIncorrect() override {
        allKeywordsMarkedIncorrect_ = true;
    }

    void markAllKeywordsCorrect() override { allKeywordsMarkedCorrect_ = true; }

    [[nodiscard]] auto markFirstKeywordIncorrectCalled() const -> bool {
        return markFirstKeywordIncorrectCalled_;
    }

    [[nodiscard]] auto markSecondKeywordIncorrectCalled() const -> bool {
        return markSecondKeywordIncorrectCalled_;
    }

    [[nodiscard]] auto markThirdKeywordIncorrectCalled() const -> bool {
        return markThirdKeywordIncorrectCalled_;
    }

    [[nodiscard]] auto allKeywordsMarkedIncorrect() const -> bool {
        return allKeywordsMarkedIncorrect_;
    }

    [[nodiscard]] auto allKeywordsMarkedCorrect() const -> bool {
        return allKeywordsMarkedCorrect_;
    }

  private:
    bool markFirstKeywordIncorrectCalled_{};
    bool markSecondKeywordIncorrectCalled_{};
    bool markThirdKeywordIncorrectCalled_{};
    bool allKeywordsMarkedIncorrect_{};
    bool allKeywordsMarkedCorrect_{};
};

auto markFirstKeywordIncorrectCalled(PresenterStub &control) -> bool {
    return control.markFirstKeywordIncorrectCalled();
}

auto markSecondKeywordIncorrectCalled(PresenterStub &control) -> bool {
    return control.markSecondKeywordIncorrectCalled();
}

auto markThirdKeywordIncorrectCalled(PresenterStub &control) -> bool {
    return control.markThirdKeywordIncorrectCalled();
}

void notifyThatSubmitButtonHasBeenClicked(ControlStub &control) {
    control.notifyThatSubmitButtonHasBeenClicked();
}

class InteractorStub : public Interactor {
  public:
    void submit(const ThreeKeywordsResponse &r) override { threeKeywords_ = r; }

    auto threeKeywords() -> ThreeKeywordsResponse { return threeKeywords_; }

  private:
    ThreeKeywordsResponse threeKeywords_;
};

class ChooseKeywordsControllerTests : public ::testing::Test {
  protected:
    InteractorStub interactor;
    ControlStub control;
    PresenterStub presenter;
    TestControllerStub testController;
    Controller controller{testController, interactor, control, presenter};
};

class ChooseKeywordsPresenterTests : public ::testing::Test {
  protected:
    RunningATestStub model;
    FixedLevelMethodStub fixedLevelMethod;
    TestViewStub testView;
    ViewStub view;
    PresenterImpl presenter{model, fixedLevelMethod, testView, view,
        {{"The visitors stretched before dinner.", "visitors", "stretched",
             "dinner"},
            {"Daddy's mouth is turning yellow.", "Daddy's", "mouth", "turning"},
            {"Tom won't pull the oar.", "won't", "pull", "oar"},
            {"This is an example of a bad match.", "This", "exmple", "match"}}};
};

#define CHOOSE_KEYWORDS_CONTROLLER_TEST(a)                                     \
    TEST_F(ChooseKeywordsControllerTests, a)

#define CHOOSE_KEYWORDS_PRESENTER_TEST(a)                                      \
    TEST_F(ChooseKeywordsPresenterTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).responseSubmissionHidden())

CHOOSE_KEYWORDS_PRESENTER_TEST(hidesResponseSubmission) {
    presenter.hideResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

CHOOSE_KEYWORDS_PRESENTER_TEST(hidesResponseButtonsWhenStopped) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

CHOOSE_KEYWORDS_PRESENTER_TEST(showsReadyButtonWhenStarted) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonShown());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(
    showsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.responseSubmissionShown());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(clearsViewWhenShown) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markFirstKeywordCorrectCalled());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markSecondKeywordCorrectCalled());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markThirdKeywordCorrectCalled());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.flagCleared());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(setsEachKeywordButtonText) {
    model.targetFileName_ = "11 The visitors stretched before dinner.mov";
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "The", view.textPrecedingFirstKeywordButton());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("visitors", view.firstKeywordButtonText());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("", view.textFollowingFirstKeywordButton());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "stretched", view.secondKeywordButtonText());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "before", view.textFollowingSecondKeywordButton());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("dinner", view.thirdKeywordButtonText());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        ".", view.textFollowingThirdKeywordButton());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(setsEachKeywordButtonText2) {
    model.targetFileName_ = "3 Daddys mouth is turning yellow.mov";
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("", view.textPrecedingFirstKeywordButton());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("Daddy's", view.firstKeywordButtonText());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("", view.textFollowingFirstKeywordButton());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("mouth", view.secondKeywordButtonText());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "is", view.textFollowingSecondKeywordButton());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("turning", view.thirdKeywordButtonText());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "yellow.", view.textFollowingThirdKeywordButton());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(setsEachKeywordButtonText3) {
    model.targetFileName_ = "5 Tom wont pull the oar.mov";
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "Tom", view.textPrecedingFirstKeywordButton());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("won't", view.firstKeywordButtonText());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("", view.textFollowingFirstKeywordButton());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("pull", view.secondKeywordButtonText());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "the", view.textFollowingSecondKeywordButton());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("oar", view.thirdKeywordButtonText());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        ".", view.textFollowingThirdKeywordButton());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(setsEachKeywordButtonText4) {
    model.targetFileName_ = "5 Tom won't pull the oar.mov";
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "Tom", view.textPrecedingFirstKeywordButton());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("won't", view.firstKeywordButtonText());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("", view.textFollowingFirstKeywordButton());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("pull", view.secondKeywordButtonText());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "the", view.textFollowingSecondKeywordButton());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("oar", view.thirdKeywordButtonText());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        ".", view.textFollowingThirdKeywordButton());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(badMatchDoesNotThrow) {
    model.targetFileName_ = "2 This is an example of a bad match.mov";
    presenter.showResponseSubmission();
}

CHOOSE_KEYWORDS_PRESENTER_TEST(marksFirstKeywordIncorrect) {
    presenter.markFirstKeywordIncorrect();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markFirstKeywordIncorrectCalled());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(marksSecondKeywordIncorrect) {
    presenter.markSecondKeywordIncorrect();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markSecondKeywordIncorrectCalled());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(marksThirdKeywordIncorrect) {
    presenter.markThirdKeywordIncorrect();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markThirdKeywordIncorrectCalled());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(marksAllKeywordsCorrect) {
    presenter.markAllKeywordsCorrect();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markFirstKeywordCorrectCalled());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markSecondKeywordCorrectCalled());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markThirdKeywordCorrectCalled());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(marksAllKeywordsIncorrect) {
    presenter.markAllKeywordsIncorrect();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markFirstKeywordIncorrectCalled());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markSecondKeywordIncorrectCalled());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markThirdKeywordIncorrectCalled());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(completeShowsKeywordTestResults) {
    fixedLevelMethod.keywordsTestResults_ = {12.34, 5};
    presenter.complete();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"5 (12.3%) keywords correct"}, testView.sheetMessage());
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    submitsKeywordResponseAfterSubmitButtonIsClicked) {
    control.setFirstKeywordCorrect();
    control.setThirdKeywordCorrect();
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(interactor.threeKeywords().firstCorrect);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(interactor.threeKeywords().secondCorrect);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(interactor.threeKeywords().thirdCorrect);
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(submitsFlaggedAfterSubmitButtonIsClicked) {
    control.setFlagged();
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(interactor.threeKeywords().flagged);
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(marksAllIncorrectAfterAllWrongButtonIsClicked) {
    control.notifyThatAllWrongButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(presenter.allKeywordsMarkedIncorrect());
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    marksFirstIncorrectAfterFirstKeywordButtonIsClicked) {
    control.notifyThatFirstKeywordButtonIsClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(markFirstKeywordIncorrectCalled(presenter));
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    marksSecondIncorrectAfterSecondKeywordButtonIsClicked) {
    control.notifyThatSecondKeywordButtonIsClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(markSecondKeywordIncorrectCalled(presenter));
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    marksThirdIncorrectAfterThirdKeywordButtonIsClicked) {
    control.notifyThatThirdKeywordButtonIsClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(markThirdKeywordIncorrectCalled(presenter));
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(marksAllCorrectAfterResetButtonIsClicked) {
    control.notifyThatResetButtonIsClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(presenter.allKeywordsMarkedCorrect());
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    notifiesThatUserIsReadyForNextTrialAfterSubmitButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}
}
}
