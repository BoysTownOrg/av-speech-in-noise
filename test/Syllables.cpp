#include "assert-utility.hpp"
#include <presentation/Syllables.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
class SyllablesViewStub : public SyllablesView {
  public:
    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void hide() { hidden_ = true; }

  private:
    bool hidden_{};
};

class SyllablesControlStub : public SyllablesControl {};

class SyllablesPresenterStub : public SyllablesPresenter {};

class SyllablesControllerTests : public ::testing::Test {
  protected:
    SyllablesControlStub control;
    SyllablesController controller{control};
};

class SyllablesPresenterTests : public ::testing::Test {
  protected:
    SyllablesViewStub view;
    SyllablesPresenterImpl presenter{view};
};

#define SYLLABLES_CONTROLLER_TEST(a) TEST_F(SyllablesControllerTests, a)

#define SYLLABLES_PRESENTER_TEST(a) TEST_F(SyllablesPresenterTests, a)

SYLLABLES_PRESENTER_TEST(hidesResponseSubmission) {
    presenter.hideResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

SYLLABLES_PRESENTER_TEST(hidesResponseButtonsWhenStopped) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

// SYLLABLES_PRESENTER_TEST(showsReadyButtonWhenStarted) {
//     presenter.start();
//     AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonShown());
// }

// SYLLABLES_PRESENTER_TEST(showsResponseButtonWhenShowingResponseSubmission) {
//     presenter.showResponseSubmission();
//     AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.responseSubmissionShown());
// }

// SYLLABLES_CONTROLLER_TEST(submitsKeywordResponseAfterSubmitButtonIsClicked) {
//     control.setFirstKeywordCorrect();
//     control.setThirdKeywordCorrect();
//     notifyThatSubmitButtonHasBeenClicked(control);
//     AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.threeKeywords().firstCorrect);
//     AV_SPEECH_IN_NOISE_EXPECT_FALSE(model.threeKeywords().secondCorrect);
//     AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.threeKeywords().thirdCorrect);
// }

// SYLLABLES_CONTROLLER_TEST(
//     notifiesThatUserIsReadyForNextTrialAfterSubmitButtonIsClicked) {
//     notifyThatSubmitButtonHasBeenClicked(control);
//     AV_SPEECH_IN_NOISE_EXPECT_TRUE(
//         testController.notifiedThatUserIsDoneResponding());
// }
}
}
