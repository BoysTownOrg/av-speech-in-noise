#include "assert-utility.hpp"
#include "TestViewStub.hpp"
#include <presentation/Syllables.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
class SyllablesViewStub : public SyllablesView {
  public:
    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void show() { shown_ = true; }

  private:
    bool hidden_{};
    bool shown_{};
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
    TestViewStub testView;
    SyllablesPresenterImpl presenter{view, testView};
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

SYLLABLES_PRESENTER_TEST(showsNextTrialButtonWhenStarted) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testView.nextTrialButtonShown());
}

SYLLABLES_PRESENTER_TEST(showsResponseButtonsWhenShowingResponseSubmission) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

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
