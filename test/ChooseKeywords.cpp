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

    void attach(Observer *a) override { observer = a; }

  private:
    Observer *observer{};
    bool firstKeywordCorrect_{};
    bool secondKeywordCorrect_{};
    bool thirdKeywordCorrect_{};
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

CHOOSE_KEYWORDS_PRESENTER_TEST(presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonHidden());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(
    presenterHidesResponseSubmissionAfterUserIsDoneResponding) {
    presenter.notifyThatUserIsDoneResponding();
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

CHOOSE_KEYWORDS_PRESENTER_TEST(presenterHidesResponseButtonsWhenStopped) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_RESPONSE_BUTTONS_HIDDEN(view);
}

CHOOSE_KEYWORDS_PRESENTER_TEST(presenterShowsReadyButtonWhenStarted) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonShown());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(
    presenterShowsResponseButtonWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.responseSubmissionShown());
}

CHOOSE_KEYWORDS_CONTROLLER_TEST(
    controllerSubmitsKeywordResponseAfterResponseButtonIsClicked) {
    control.setFirstKeywordCorrect();
    control.setThirdKeywordCorrect();
    control.notifyThatSubmitButtonHasBeenClicked();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.threeKeywords().firstCorrect);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(model.threeKeywords().secondCorrect);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.threeKeywords().thirdCorrect);
}

// CHOOSE_KEYWORDS_CONTROLLER_TEST(
//     responderSubmitsFlaggedFreeResponseAfterResponseButtonIsClicked) {
//     control.setFlagged();
//     notifyThatSubmitButtonHasBeenClicked(control);
//     AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.freeResponse().flagged);
// }

// CHOOSE_KEYWORDS_CONTROLLER_TEST(
//     responderNotifiesThatUserIsReadyForNextTrialAfterResponseButtonIsClicked)
//     { notifyThatSubmitButtonHasBeenClicked(control);
//     AV_SPEECH_IN_NOISE_EXPECT_TRUE(
//         testController.notifiedThatUserIsDoneResponding());
// }

// CHOOSE_KEYWORDS_CONTROLLER_TEST(
//     responderNotifiesThatUserIsDoneRespondingAfterResponseButtonIsClicked) {
//     notifyThatSubmitButtonHasBeenClicked(control);
//     AV_SPEECH_IN_NOISE_EXPECT_TRUE(
//         taskController.notifiedThatUserIsDoneResponding());
// }
}
}
