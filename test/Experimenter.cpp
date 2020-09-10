#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include <presentation/ExperimenterImpl.hpp>
#include <av-speech-in-noise/name.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <utility>

namespace av_speech_in_noise {
namespace {
class ExperimenterViewStub : public ExperimenterView {
  public:
    void declineContinuingTesting() { listener_->declineContinuingTesting(); }

    [[nodiscard]] auto continueTestingDialogMessage() const -> std::string {
        return continueTestingDialogMessage_;
    }

    void setContinueTestingDialogMessage(const std::string &s) override {
        continueTestingDialogMessage_ = s;
    }

    void acceptContinuingTesting() { listener_->acceptContinuingTesting(); }

    void showContinueTestingDialog() override {
        continueTestingDialogShown_ = true;
    }

    [[nodiscard]] auto continueTestingDialogShown() const -> bool {
        return continueTestingDialogShown_;
    }

    void hideContinueTestingDialog() override {
        continueTestingDialogHidden_ = true;
    }

    [[nodiscard]] auto responseSubmissionShown() const {
        return responseSubmissionShown_;
    }

    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const { return hidden_; }

    void subscribe(ExperimenterView::EventListener *e) override {
        listener_ = e;
    }

    void exitTest() { listener_->exitTest(); }

    void playTrial() { listener_->playTrial(); }

    void display(std::string s) override { displayed_ = std::move(s); }

    [[nodiscard]] auto displayed() const { return displayed_; }

    void secondaryDisplay(std::string s) override {
        secondaryDisplayed_ = std::move(s);
    }

    [[nodiscard]] auto secondaryDisplayed() const {
        return secondaryDisplayed_;
    }

    void showNextTrialButton() override { nextTrialButtonShown_ = true; }

    [[nodiscard]] auto nextTrialButtonShown() const {
        return nextTrialButtonShown_;
    }

    void hideNextTrialButton() override { nextTrialButtonHidden_ = true; }

    [[nodiscard]] auto nextTrialButtonHidden() const {
        return nextTrialButtonHidden_;
    }

    void showExitTestButton() override { exitTestButtonShown_ = true; }

    [[nodiscard]] auto exitTestButtonShown() const {
        return exitTestButtonShown_;
    }

    void hideExitTestButton() override { exitTestButtonHidden_ = true; }

    [[nodiscard]] auto exitTestButtonHidden() const {
        return exitTestButtonHidden_;
    }

  private:
    std::string displayed_;
    std::string secondaryDisplayed_;
    std::string continueTestingDialogMessage_;
    std::string response_;
    std::string correctKeywords_{"0"};
    ExperimenterView::EventListener *listener_{};
    bool exitTestButtonHidden_{};
    bool exitTestButtonShown_{};
    bool nextTrialButtonShown_{};
    bool nextTrialButtonHidden_{};
    bool responseSubmissionShown_{};
    bool continueTestingDialogShown_{};
    bool continueTestingDialogHidden_{};
    bool shown_{};
    bool hidden_{};
};

class ViewStub : public View {
  public:
    void setAudioDevice(std::string s) { audioDevice_ = std::move(s); }

    auto audioDevice() -> std::string override { return audioDevice_; }

    void showErrorMessage(std::string s) override {
        errorMessage_ = std::move(s);
    }

    auto errorMessage() { return errorMessage_; }

    void eventLoop() override { eventLoopCalled_ = true; }

    [[nodiscard]] auto eventLoopCalled() const { return eventLoopCalled_; }

    auto browseForDirectory() -> std::string override {
        return browseForDirectoryResult_;
    }

    auto browseCancelled() -> bool override { return browseCancelled_; }

    auto browseForOpeningFile() -> std::string override {
        return browseForOpeningFileResult_;
    }

    void setBrowseForOpeningFileResult(std::string s) {
        browseForOpeningFileResult_ = std::move(s);
    }

    void setBrowseCancelled() { browseCancelled_ = true; }

    void populateAudioDeviceMenu(std::vector<std::string> v) override {
        audioDevices_ = std::move(v);
    }

    [[nodiscard]] auto audioDevices() const { return audioDevices_; }

  private:
    std::vector<std::string> audioDevices_;
    std::string errorMessage_;
    std::string browseForDirectoryResult_;
    std::string browseForOpeningFileResult_;
    std::string audioDevice_;
    bool eventLoopCalled_{};
    bool browseCancelled_{};
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(ExperimenterPresenter &) = 0;
};

void run(UseCase &useCase, ExperimenterPresenter &presenter) {
    useCase.run(presenter);
}

class ConditionUseCase : public virtual UseCase {
  public:
    virtual auto condition(ModelStub &) -> Condition = 0;
};

class LevelUseCase : public virtual UseCase {
  public:
    virtual auto fullScaleLevel(ModelStub &) -> int = 0;
};

class InitializingExperimenterPresenter : public virtual UseCase {};

class InitializingAdaptiveCoordinateResponseMeasureMethod
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(Method::adaptiveCoordinateResponseMeasure);
    }
};

class InitializingAdaptiveCoordinateResponseMeasureMethodWithSingleSpeaker
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(
            Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
    }
};

class InitializingAdaptiveCoordinateResponseMeasureMethodWithEyeTracking
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(
            Method::adaptiveCoordinateResponseMeasureWithEyeTracking);
    }
};

class InitializingAdaptiveCoordinateResponseMeasureMethodWithDelayedMasker
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(
            Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
    }
};

class InitializingAdaptivePassFailMethod
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(Method::adaptivePassFail);
    }
};

class InitializingAdaptivePassFailMethodWithEyeTracking
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(Method::adaptivePassFailWithEyeTracking);
    }
};

class InitializingAdaptiveCorrectKeywordsMethod
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(Method::adaptiveCorrectKeywords);
    }
};

class InitializingAdaptiveCorrectKeywordsMethodWithEyeTracking
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(Method::adaptiveCorrectKeywordsWithEyeTracking);
    }
};

class InitializingFixedLevelFreeResponseWithTargetReplacementMethod
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(
            Method::fixedLevelFreeResponseWithTargetReplacement);
    }
};

class InitializingFixedLevelConsonantMethod
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(Method::fixedLevelConsonants);
    }
};

class InitializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacement
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(
            Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
    }
};

class
    InitializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementAndEyeTracking
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(Method::
                fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking);
    }
};

class
    InitializingFixedLevelCoordinateResponseMeasureMethodWithSilentIntervalTargets
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(Method::
                fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
    }
};

class InitializingFixedLevelFreeResponseWithSilentIntervalTargetsMethod
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(
            Method::fixedLevelFreeResponseWithSilentIntervalTargets);
    }
};

class InitializingFixedLevelFreeResponseMethodWithAllTargets
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(Method::fixedLevelFreeResponseWithAllTargets);
    }
};

class InitializingFixedLevelFreeResponseMethodWithAllTargetsAndEyeTracking
    : public InitializingExperimenterPresenter {
  public:
    void run(ExperimenterPresenter &presenter) override {
        presenter.initialize(
            Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking);
    }
};

class ExitingTest : public UseCase {
    ExperimenterViewStub *view;

  public:
    explicit ExitingTest(ExperimenterViewStub *view) : view{view} {}

    void run(ExperimenterPresenter &) override { view->exitTest(); }
};

class DecliningContinuingTesting : public UseCase {
  public:
    explicit DecliningContinuingTesting(ExperimenterViewStub &view)
        : view{view} {}

    void run(ExperimenterPresenter &) override {
        view.declineContinuingTesting();
    }

  private:
    ExperimenterViewStub &view;
};

class AcceptingContinuingTesting : public UseCase {
  public:
    explicit AcceptingContinuingTesting(ExperimenterViewStub &view)
        : view{view} {}

    void run(ExperimenterPresenter &) override {
        view.acceptContinuingTesting();
    }

  private:
    ExperimenterViewStub &view;
};

void exitTest(ExperimenterViewStub &view) { view.exitTest(); }

auto hidden(ExperimenterViewStub &view) -> bool { return view.hidden(); }

void completeTrial(ModelStub &model) { model.completeTrial(); }

auto errorMessage(ViewStub &view) -> std::string { return view.errorMessage(); }

void setAudioDevice(ViewStub &view, std::string s) {
    view.setAudioDevice(std::move(s));
}

void setTestComplete(ModelStub &model) { model.setTestComplete(); }

auto trialPlayed(ModelStub &model) -> bool { return model.trialPlayed(); }

class TaskResponderStub : public TaskResponder {
  public:
    void subscribe(EventListener *) override {}
    void subscribe(ExperimenterResponder *) override {}
};

class TaskPresenterStub : public TaskPresenter {
  public:
    void showResponseSubmission() override {}
    void notifyThatTaskHasStarted() override {}
    void notifyThatUserIsDoneResponding() override {}
    void notifyThatTrialHasStarted() override {}
    void start() override {}
    void stop() override {}
};

class PresenterStub : public IPresenter {
  public:
    void switchToTestSetupView() override { switchedToTestSetupView_ = true; }

    [[nodiscard]] auto switchedToTestSetupView() const -> bool {
        return switchedToTestSetupView_;
    }

  private:
    bool switchedToTestSetupView_{};
};

class ExperimenterTests : public ::testing::Test {
  protected:
    ModelStub model;
    ViewStub view;
    ExperimenterViewStub experimenterView;
    TaskResponderStub consonantResponder;
    TaskResponderStub coordinateResponseMeasureResponder;
    TaskResponderStub freeResponseResponder;
    TaskResponderStub passFailResponder;
    TaskResponderStub correctKeywordsResponder;
    TaskPresenterStub consonantPresenter;
    TaskPresenterStub coordinateResponseMeasurePresenter;
    TaskPresenterStub freeResponsePresenter;
    TaskPresenterStub correctKeywordsPresenter;
    TaskPresenterStub passFailPresenter;
    ExperimenterResponderImpl experimenterResponder{model, view,
        experimenterView, &consonantResponder, &consonantPresenter,
        &coordinateResponseMeasureResponder,
        &coordinateResponseMeasurePresenter, &freeResponseResponder,
        &freeResponsePresenter, &correctKeywordsResponder,
        &correctKeywordsPresenter, &passFailResponder, &passFailPresenter};
    ExperimenterPresenterImpl experimenterPresenterRefactored{model,
        experimenterView, &consonantPresenter,
        &coordinateResponseMeasurePresenter, &freeResponsePresenter,
        &correctKeywordsPresenter, &passFailPresenter};
    InitializingAdaptiveCoordinateResponseMeasureMethod
        initializingAdaptiveCoordinateResponseMeasureMethod;
    InitializingAdaptiveCoordinateResponseMeasureMethodWithSingleSpeaker
        initializingAdaptiveCoordinateResponseMeasureMethodWithSingleSpeaker;
    InitializingAdaptiveCoordinateResponseMeasureMethodWithDelayedMasker
        initializingAdaptiveCoordinateResponseMeasureMethodWithDelayedMasker;
    InitializingAdaptiveCoordinateResponseMeasureMethodWithEyeTracking
        initializingAdaptiveCoordinateResponseMeasureMethodWithEyeTracking;
    InitializingAdaptivePassFailMethod initializingAdaptivePassFailMethod;
    InitializingAdaptivePassFailMethodWithEyeTracking
        initializingAdaptivePassFailMethodWithEyeTracking;
    InitializingFixedLevelFreeResponseWithTargetReplacementMethod
        initializingFixedLevelFreeResponseWithTargetReplacementMethod;
    InitializingFixedLevelConsonantMethod initializingFixedLevelConsonantMethod;
    InitializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacement
        initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacement;
    InitializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementAndEyeTracking
        initializingFixedLevelCoordinateResponseMeasureMethodWithTargetReplacementAndEyeTracking;
    InitializingAdaptiveCorrectKeywordsMethod
        initializingAdaptiveCorrectKeywordsMethod;
    InitializingAdaptiveCorrectKeywordsMethodWithEyeTracking
        initializingAdaptiveCorrectKeywordsMethodWithEyeTracking;
    InitializingFixedLevelCoordinateResponseMeasureMethodWithSilentIntervalTargets
        initializingFixedLevelCoordinateResponseMeasureSilentIntervalsMethod;
    InitializingFixedLevelFreeResponseWithSilentIntervalTargetsMethod
        initializingFixedLevelFreeResponseWithSilentIntervalTargetsMethod;
    InitializingFixedLevelFreeResponseMethodWithAllTargets
        initializingFixedLevelFreeResponseMethodWithAllTargets;
    InitializingFixedLevelFreeResponseMethodWithAllTargetsAndEyeTracking
        initializingFixedLevelFreeResponseMethodWithAllTargetsAndEyeTracking;
    DecliningContinuingTesting decliningContinuingTesting{experimenterView};
    AcceptingContinuingTesting acceptingContinuingTesting{experimenterView};
    ExitingTest exitingTest{&experimenterView};
    PresenterStub presenter;

    ExperimenterTests() { experimenterResponder.subscribe(&presenter); }
};

#define EXPERIMENTER_TEST(a) TEST_F(ExperimenterTests, a)

EXPERIMENTER_TEST(responderSwitchesToTestSetupViewWhenExitTestButtonClicked) {
    exitTest(experimenterView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(presenter.switchedToTestSetupView());
}
}
}
