#include "assert-utility.hpp"
#include "TestViewStub.hpp"
#include "TestControllerStub.hpp"
#include "ModelStub.hpp"
#include <presentation/Syllables.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise {
namespace {
class SyllablesViewStub : public SyllablesView {
  public:
    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void show() override { shown_ = true; }

    [[nodiscard]] auto flagCleared() const -> bool { return flagCleared_; }

    void clearFlag() override { flagCleared_ = true; }

  private:
    bool hidden_{};
    bool shown_{};
    bool flagCleared_{};
};

class SyllablesControlStub : public SyllablesControl {
  public:
    void notifyThatResponseButtonHasBeenClicked() {
        observer->notifyThatResponseButtonHasBeenClicked();
    }

    void attach(Observer *a) override { observer = a; }

    void setSyllable(std::string s) { syllable_ = std::move(s); }

    auto syllable() -> std::string override { return syllable_; }

    void setFlagged() { flagged_ = true; }

    auto flagged() -> bool override { return flagged_; }

  private:
    std::string syllable_;
    Observer *observer{};
    bool flagged_{};
};

void notifyThatResponseButtonHasBeenClicked(SyllablesControlStub &control) {
    control.notifyThatResponseButtonHasBeenClicked();
}

class SyllablesPresenterStub : public SyllablesPresenter {};

class SyllablesControllerTests : public ::testing::Test {
  protected:
    SyllablesControlStub control;
    TestControllerStub testController;
    ModelStub model;
    SyllablesController controller{
        control, testController, model, {{"Ghee", Syllable::gi}}};
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

SYLLABLES_PRESENTER_TEST(clearsViewWhenShown) {
    presenter.showResponseSubmission();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.flagCleared());
}

SYLLABLES_CONTROLLER_TEST(submitsKeywordResponseAfterSubmitButtonIsClicked) {
    control.setSyllable("Ghee");
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Syllable::gi, model.syllableResponse().syllable);
}

SYLLABLES_CONTROLLER_TEST(submitsFlaggedAfterSubmitButtonIsClicked) {
    control.setFlagged();
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.syllableResponse().flagged);
}

SYLLABLES_CONTROLLER_TEST(
    notifiesThatUserIsReadyForNextTrialAfterSubmitButtonIsClicked) {
    notifyThatResponseButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}
}
}
