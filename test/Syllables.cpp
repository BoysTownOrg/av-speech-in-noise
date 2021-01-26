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

  private:
    bool hidden_{};
    bool shown_{};
};

class SyllablesControlStub : public SyllablesControl {
  public:
    void notifyThatSubmitButtonHasBeenClicked() {
        observer->notifyThatSubmitButtonHasBeenClicked();
    }

    void attach(Observer *a) override { observer = a; }

    void setSyllable(std::string s) { syllable_ = std::move(s); }

    auto syllable() -> std::string { return syllable_; }

  private:
    std::string syllable_;
    Observer *observer{};
};

void notifyThatSubmitButtonHasBeenClicked(SyllablesControlStub &control) {
    control.notifyThatSubmitButtonHasBeenClicked();
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

SYLLABLES_CONTROLLER_TEST(submitsKeywordResponseAfterSubmitButtonIsClicked) {
    control.setSyllable("Ghee");
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Syllable::gi, model.syllableResponse().syllable);
}

SYLLABLES_CONTROLLER_TEST(
    notifiesThatUserIsReadyForNextTrialAfterSubmitButtonIsClicked) {
    notifyThatSubmitButtonHasBeenClicked(control);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        testController.notifiedThatUserIsDoneResponding());
}
}
}
