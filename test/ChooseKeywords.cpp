#include "assert-utility.hpp"
#include "TestViewStub.hpp"
#include <presentation/ChooseKeywords.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
class ChooseKeywordsViewStub : public ChooseKeywordsView {
  public:
    [[nodiscard]] auto responseSubmissionHidden() const -> bool {
        return responseSubmissionHidden_;
    }

    void hideResponseSubmission() { responseSubmissionHidden_ = true; }

  private:
    bool responseSubmissionHidden_{};
};

// class ChooseKeywordsControllerTests : public ::testing::Test {
//   protected:
//     ModelStub model;
//     ChooseKeywordsControlStub control;
//     ChooseKeywordsController controller{model, control};
//     TestControllerStub testController;
//     TaskControllerObserverStub taskController;

//     ChooseKeywordsControllerTests() {
//         controller.attach(&testController);
//         controller.attach(&taskController);
//     }
// };

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

CHOOSE_KEYWORDS_PRESENTER_TEST(presenterHidesReadyButtonWhenTaskStarts) {
    presenter.notifyThatTaskHasStarted();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonHidden());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(
    presenterHidesResponseSubmissionAfterUserIsDoneResponding) {
    presenter.notifyThatUserIsDoneResponding();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.responseSubmissionHidden());
}

CHOOSE_KEYWORDS_PRESENTER_TEST(presenterHidesResponseButtonsWhenStopped) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.responseSubmissionHidden());
}

// CHOOSE_KEYWORDS_PRESENTER_TEST(presenterShowsReadyButtonWhenStarted) {
//     start(presenter);
//     AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonShown());
// }

// CHOOSE_KEYWORDS_PRESENTER_TEST(
//     presenterShowsResponseButtonWhenShowingResponseSubmission) {
//     presenter.showResponseSubmission();
//     AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.freeResponseSubmissionShown());
// }

// CHOOSE_KEYWORDS_PRESENTER_TEST(
//     presenterClearsFreeResponseWhenShowingResponseSubmission) {
//     presenter.showResponseSubmission();
//     AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.freeResponseCleared());
// }

// CHOOSE_KEYWORDS_PRESENTER_TEST(
//     presenterClearsFlagWhenShowingResponseSubmission) {
//     presenter.showResponseSubmission();
//     AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.flagCleared());
// }

// CHOOSE_KEYWORDS_CONTROLLER_TEST(
//     responderSubmitsFreeResponseAfterResponseButtonIsClicked) {
//     control.setFreeResponse("a");
//     notifyThatSubmitButtonHasBeenClicked(control);
//     AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
//         std::string{"a"}, model.freeResponse().response);
// }

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