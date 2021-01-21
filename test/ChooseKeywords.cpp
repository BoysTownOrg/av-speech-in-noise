#include "assert-utility.hpp"
#include "TestViewStub.hpp"
#include "TestControllerStub.hpp"
#include "ModelStub.hpp"
#include <presentation/ChooseKeywords.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
class ChooseKeywordsViewStub : public ChooseKeywordsView {
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

  private:
    std::string textFollowingThirdKeywordButton_;
    std::string textFollowingSecondKeywordButton_;
    std::string textFollowingFirstKeywordButton_;
    std::string textPrecedingFirstKeywordButton_;
    std::string firstKeywordButtonText_;
    std::string secondKeywordButtonText_;
    std::string thirdKeywordButtonText_;
    bool responseSubmissionHidden_{};
    bool responseSubmissionShown_{};
    bool markFirstKeywordIncorrectCalled_{};
    bool markSecondKeywordIncorrectCalled_{};
    bool markThirdKeywordIncorrectCalled_{};
    bool markFirstKeywordCorrectCalled_{};
    bool markSecondKeywordCorrectCalled_{};
    bool markThirdKeywordCorrectCalled_{};
};

class ChooseKeywordsControlStub : public ChooseKeywordsControl {
  public:
    void setFirstKeywordCorrect() { firstKeywordCorrect_ = true; }

    void setThirdKeywordCorrect() { thirdKeywordCorrect_ = true; }

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
    bool firstKeywordCorrect_{};
    bool secondKeywordCorrect_{};
    bool thirdKeywordCorrect_{};
};

class ChooseKeywordsPresenterStub : public ChooseKeywordsPresenter {
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

    void markFirstKeywordCorrect() override {
        markFirstKeywordCorrectCalled_ = true;
    }

    void markSecondKeywordCorrect() override {
        markSecondKeywordCorrectCalled_ = true;
    }

    void markThirdKeywordCorrect() override {
        markThirdKeywordCorrectCalled_ = true;
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

    void hideResponseSubmission() override { responseSubmissionHidden_ = true; }

    [[nodiscard]] auto responseSubmissionHidden() const -> bool {
        return responseSubmissionHidden_;
    }

  private:
    bool responseSubmissionHidden_{};
    bool markFirstKeywordIncorrectCalled_{};
    bool markSecondKeywordIncorrectCalled_{};
    bool markThirdKeywordIncorrectCalled_{};
    bool markFirstKeywordCorrectCalled_{};
    bool markSecondKeywordCorrectCalled_{};
    bool markThirdKeywordCorrectCalled_{};
};

auto markFirstKeywordIncorrectCalled(ChooseKeywordsPresenterStub &control)
    -> bool {
    return control.markFirstKeywordIncorrectCalled();
}

auto markSecondKeywordIncorrectCalled(ChooseKeywordsPresenterStub &control)
    -> bool {
    return control.markSecondKeywordIncorrectCalled();
}

auto markThirdKeywordIncorrectCalled(ChooseKeywordsPresenterStub &control)
    -> bool {
    return control.markThirdKeywordIncorrectCalled();
}

void notifyThatSubmitButtonHasBeenClicked(ChooseKeywordsControlStub &control) {
    control.notifyThatSubmitButtonHasBeenClicked();
}

class ChooseKeywordsControllerTests : public ::testing::Test {
  protected:
    ModelStub model;
    ChooseKeywordsControlStub control;
    ChooseKeywordsPresenterStub presenter;
    ChooseKeywordsController controller{model, control, presenter};
    TestControllerStub testController;

    ChooseKeywordsControllerTests() { controller.attach(&testController); }
};

class ChooseKeywordsPresenterTests : public ::testing::Test {
  protected:
    TestViewStub testView;
    ChooseKeywordsViewStub view;
    ChooseKeywordsPresenterImpl presenter{testView, view};
};

#define CHOOSE_KEYWORDS_CONTROLLER_TEST(a)                                     \
    TEST_F(ChooseKeywordsControllerTests, a)

#define CHOOSE_KEYWORDS_PRESENTER_TEST(a)                                      \
    TEST_F(ChooseKeywordsPresenterTests, a)

#define AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(a)                   \
    AV_SPEECH_IN_NOISE_EXPECT_TRUE((a).responseSubmissionHidden())

CHOOSE_KEYWORDS_PRESENTER_TEST(hidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonHidden());
}

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

CHOOSE_KEYWORDS_PRESENTER_TEST(setsEachKeywordButtonText) {
    presenter.set(
        SentenceWithThreeKeywords{"The visitors stretched before dinner.",
            "visitors", "stretched", "dinner"});
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
    presenter.set(SentenceWithThreeKeywords{
        "Daddy's mouth is turning yellow.", "Daddy's", "mouth", "turning"});
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

CHOOSE_KEYWORDS_PRESENTER_TEST(marksFirstKeywordCorrect) {
    presenter.markFirstKeywordCorrect();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markFirstKeywordCorrectCalled());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(marksSecondKeywordCorrect) {
    presenter.markSecondKeywordCorrect();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markSecondKeywordCorrectCalled());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(marksThirdKeywordCorrect) {
    presenter.markThirdKeywordCorrect();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.markThirdKeywordCorrectCalled());
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    submitsKeywordResponseAfterSubmitButtonIsClicked) {
    control.setFirstKeywordCorrect();
    control.setThirdKeywordCorrect();
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.threeKeywords().firstCorrect);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(model.threeKeywords().secondCorrect);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.threeKeywords().thirdCorrect);
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    marksEachIncorrectAfterAllWrongButtonIsClicked) {
    control.notifyThatAllWrongButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(markFirstKeywordIncorrectCalled(presenter));
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(markSecondKeywordIncorrectCalled(presenter));
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(markThirdKeywordIncorrectCalled(presenter));
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

CHOOSE_KEYWORDS_CONTROLLER_TEST(marksEachCorrectAfterResetButtonIsClicked) {
    control.notifyThatResetButtonIsClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(presenter.markFirstKeywordCorrectCalled());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(presenter.markSecondKeywordCorrectCalled());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(presenter.markThirdKeywordCorrectCalled());
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    notifiesThatUserIsReadyForNextTrialAfterSubmitButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    hidesResponseSubmissionAfterSubmitButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(presenter.responseSubmissionHidden());
}
}
}
