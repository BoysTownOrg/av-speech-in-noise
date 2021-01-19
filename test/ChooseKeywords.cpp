#include "assert-utility.hpp"
#include "TestViewStub.hpp"
#include "TestControllerStub.hpp"
#include "TaskControllerObserverStub.hpp"
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

  private:
    bool responseSubmissionHidden_{};
    bool responseSubmissionShown_{};
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
    bool markFirstKeywordIncorrectCalled_{};
    bool markSecondKeywordIncorrectCalled_{};
    bool markThirdKeywordIncorrectCalled_{};
    bool markFirstKeywordCorrectCalled_{};
    bool markSecondKeywordCorrectCalled_{};
    bool markThirdKeywordCorrectCalled_{};
};

class ChooseKeywordsControllerTests : public ::testing::Test {
  protected:
    ModelStub model;
    ChooseKeywordsControlStub control;
    ChooseKeywordsController controller{model, control};
    TestControllerStub testController;
    TaskControllerObserverStub taskController;

    ChooseKeywordsControllerTests() {
        controller.attach(&testController);
        controller.attach(&taskController);
    }
};

class ChooseKeywordsPresenterTests : public ::testing::Test {
  protected:
    TestViewStub testView;
    ChooseKeywordsViewStub view;
    ChooseKeywordsPresenter presenter{testView, view};
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

CHOOSE_KEYWORDS_PRESENTER_TEST(
    hidesResponseSubmissionAfterUserIsDoneResponding) {
    presenter.notifyThatUserIsDoneResponding();
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

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    submitsKeywordResponseAfterSubmitButtonIsClicked) {
    control.setFirstKeywordCorrect();
    control.setThirdKeywordCorrect();
    control.notifyThatSubmitButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.threeKeywords().firstCorrect);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(model.threeKeywords().secondCorrect);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.threeKeywords().thirdCorrect);
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    marksEachIncorrectAfterAllWrongButtonIsClicked) {
    control.notifyThatAllWrongButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(control.markFirstKeywordIncorrectCalled());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(control.markSecondKeywordIncorrectCalled());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(control.markThirdKeywordIncorrectCalled());
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    marksFirstIncorrectAfterFirstKeywordButtonIsClicked) {
    control.notifyThatFirstKeywordButtonIsClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(control.markFirstKeywordIncorrectCalled());
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    marksSecondIncorrectAfterSecondKeywordButtonIsClicked) {
    control.notifyThatSecondKeywordButtonIsClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(control.markSecondKeywordIncorrectCalled());
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    marksThirdIncorrectAfterThirdKeywordButtonIsClicked) {
    control.notifyThatThirdKeywordButtonIsClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(control.markThirdKeywordIncorrectCalled());
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(marksEachCorrectAfterResetButtonIsClicked) {
    control.notifyThatResetButtonIsClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(control.markFirstKeywordCorrectCalled());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(control.markSecondKeywordCorrectCalled());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(control.markThirdKeywordCorrectCalled());
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    notifiesThatUserIsReadyForNextTrialAfterSubmitButtonIsClicked) {
    control.notifyThatSubmitButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    notifiesThatUserIsDoneRespondingAfterSubmitButtonIsClicked) {
    control.notifyThatSubmitButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        taskController.notifiedThatUserIsDoneResponding());
}
}
}
