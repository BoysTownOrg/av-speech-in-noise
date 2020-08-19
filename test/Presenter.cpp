#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include <presentation/Presenter.hpp>
#include <av-speech-in-noise/name.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <utility>

namespace av_speech_in_noise {
namespace {
template <typename T> class Collection {
  public:
    explicit Collection(std::vector<T> items = {}) : items{std::move(items)} {}

    [[nodiscard]] auto contains(const T &item) const -> bool {
        return std::find(items.begin(), items.end(), item) != items.end();
    }

  private:
    std::vector<T> items{};
};

class ConsonantViewStub : public View::Consonant {
  public:
    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const -> bool { return hidden_; }

    void playTrial() { listener_->notifyThatReadyButtonHasBeenClicked(); }

    void subscribe(EventListener *e) override { listener_ = e; }

    void showReadyButton() override { readyButtonShown_ = true; }

    [[nodiscard]] auto readyButtonShown() const -> bool {
        return readyButtonShown_;
    }

    void hideReadyButton() override { readyButtonHidden_ = true; }

    [[nodiscard]] auto readyButtonHidden() const -> bool {
        return readyButtonHidden_;
    }

    void submitResponse() {
        listener_->notifyThatResponseButtonHasBeenClicked();
    }

    void hideResponseButtons() override { responseButtonsHidden_ = true; }

    [[nodiscard]] auto responseButtonsHidden() const -> bool {
        return responseButtonsHidden_;
    }

    void showResponseButtons() override { responseButtonsShown_ = true; }

    [[nodiscard]] auto responseButtonsShown() const -> bool {
        return responseButtonsShown_;
    }

    void setConsonant(std::string c) { consonant_ = std::move(c); }

    auto consonant() -> std::string override { return consonant_; }

    [[nodiscard]] auto cursorHidden() const -> bool { return cursorHidden_; }

    void hideCursor() override { cursorHidden_ = true; }

  private:
    std::string consonant_;
    EventListener *listener_{};
    bool shown_{};
    bool hidden_{};
    bool responseButtonsShown_{};
    bool responseButtonsHidden_{};
    bool readyButtonShown_{};
    bool readyButtonHidden_{};
    bool cursorHidden_{};
};

class CoordinateResponseMeasureViewStub
    : public View::CoordinateResponseMeasure {
  public:
    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const { return hidden_; }

    auto whiteResponse() -> bool override { return grayResponse_; }

    void setGrayResponse() { grayResponse_ = true; }

    auto blueResponse() -> bool override { return blueResponse_; }

    void setBlueResponse() { blueResponse_ = true; }

    void setRedResponse() { redResponse_ = true; }

    void setGreenResponse() { greenResponse_ = true; }

    auto greenResponse() -> bool override { return greenResponse_; }

    void hideNextTrialButton() override { nextTrialButtonHidden_ = true; }

    [[nodiscard]] auto nextTrialButtonHidden() const {
        return nextTrialButtonHidden_;
    }

    void hideResponseButtons() override { responseButtonsHidden_ = true; }

    [[nodiscard]] auto responseButtonsHidden() const {
        return responseButtonsHidden_;
    }

    void showNextTrialButton() override { nextTrialButtonShown_ = true; }

    [[nodiscard]] auto nextTrialButtonShown() const {
        return nextTrialButtonShown_;
    }

    void showResponseButtons() override { responseButtonsShown_ = true; }

    [[nodiscard]] auto responseButtonsShown() const {
        return responseButtonsShown_;
    }

    void setNumberResponse(std::string s) { numberResponse_ = std::move(s); }

    auto numberResponse() -> std::string override { return numberResponse_; }

    void subscribe(EventListener *e) override { listener_ = e; }

    void submitResponse() {
        listener_->notifyThatResponseButtonHasBeenClicked();
    }

    void playTrial() { listener_->notifyThatReadyButtonHasBeenClicked(); }

  private:
    std::string numberResponse_{"0"};
    EventListener *listener_{};
    bool responseButtonsShown_{};
    bool responseButtonsHidden_{};
    bool shown_{};
    bool hidden_{};
    bool greenResponse_{};
    bool redResponse_{};
    bool blueResponse_{};
    bool grayResponse_{};
    bool nextTrialButtonHidden_{};
    bool nextTrialButtonShown_{};
};

class TestSetupViewStub : public View::TestSetup {
  public:
    auto testSettingsFile() -> std::string override {
        return testSettingsFile_;
    }

    auto startingSnr() -> std::string override { return startingSnr_; }

    void setTestSettingsFile(std::string s) override {
        testSettingsFile_ = std::move(s);
    }

    void browseForTestSettingsFile() {
        listener_->notifyThatBrowseForTestSettingsButtonHasBeenClicked();
    }

    [[nodiscard]] auto transducers() const -> std::vector<std::string> {
        return transducers_;
    }

    void populateTransducerMenu(std::vector<std::string> v) override {
        transducers_ = std::move(v);
    }

    void confirmTestSetup() { listener_->confirmTestSetup(); }

    void playCalibration() { listener_->playCalibration(); }

    auto session() -> std::string override { return session_; }

    void setSession(std::string s) { session_ = std::move(s); }

    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const { return hidden_; }

    void setRmeSetting(std::string s) { rmeSetting_ = std::move(s); }

    auto rmeSetting() -> std::string override { return rmeSetting_; }

    void setTransducer(std::string s) { transducer_ = std::move(s); }

    void setSubjectId(std::string s) { subjectId_ = std::move(s); }

    auto subjectId() -> std::string override { return subjectId_; }

    void setTesterId(std::string s) { testerId_ = std::move(s); }

    auto testerId() -> std::string override { return testerId_; }

    auto transducer() -> std::string override { return transducer_; }

    void subscribe(EventListener *listener) override { listener_ = listener; }

    void setStartingSnr(std::string s) { startingSnr_ = std::move(s); }

  private:
    std::vector<std::string> transducers_;
    std::string startingSnr_{"0"};
    std::string subjectId_;
    std::string testerId_;
    std::string session_;
    std::string rmeSetting_;
    std::string transducer_;
    std::string testSettingsFile_;
    EventListener *listener_{};
    bool shown_{};
    bool hidden_{};
};

class ExperimenterViewStub : public View::Experimenter {
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

    [[nodiscard]] auto continueTestingDialogHidden() const -> bool {
        return continueTestingDialogHidden_;
    }

    void submitFailedTrial() { listener_->submitFailedTrial(); }

    void hideFreeResponseSubmission() override {
        responseSubmissionHidden_ = true;
    }

    [[nodiscard]] auto responseSubmissionHidden() const {
        return responseSubmissionHidden_;
    }

    void hideEvaluationButtons() override { evaluationButtonsHidden_ = true; }

    [[nodiscard]] auto evaluationButtonsHidden() const {
        return evaluationButtonsHidden_;
    }

    void showCorrectKeywordsSubmission() override {
        correctKeywordsEntryShown_ = true;
    }

    [[nodiscard]] auto correctKeywordsEntryShown() const {
        return correctKeywordsEntryShown_;
    }

    void hideCorrectKeywordsSubmission() override {
        correctKeywordsEntryHidden_ = true;
    }

    [[nodiscard]] auto correctKeywordsEntryHidden() const {
        return correctKeywordsEntryHidden_;
    }

    void showEvaluationButtons() override { evaluationButtonsShown_ = true; }

    [[nodiscard]] auto evaluationButtonsShown() const {
        return evaluationButtonsShown_;
    }

    void showFreeResponseSubmission() override {
        responseSubmissionShown_ = true;
    }

    [[nodiscard]] auto responseSubmissionShown() const {
        return responseSubmissionShown_;
    }

    void setCorrectKeywords(std::string s) { correctKeywords_ = std::move(s); }

    auto correctKeywords() -> std::string override { return correctKeywords_; }

    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const { return hidden_; }

    void subscribe(EventListener *e) override { listener_ = e; }

    void setResponse(std::string s) { response_ = std::move(s); }

    auto freeResponse() -> std::string override { return response_; }

    void submitPassedTrial() { listener_->submitPassedTrial(); }

    void submitFreeResponse() { listener_->submitFreeResponse(); }

    void submitCorrectKeywords() { listener_->submitCorrectKeywords(); }

    void exitTest() { listener_->exitTest(); }

    void playTrial() { listener_->playTrial(); }

    void flagResponse() { flagged_ = true; }

    auto flagged() -> bool override { return flagged_; }

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

    [[nodiscard]] auto freeResponseCleared() const -> bool {
        return freeResponseCleared_;
    }

    void clearFreeResponse() override { freeResponseCleared_ = true; }

  private:
    std::string displayed_;
    std::string secondaryDisplayed_;
    std::string continueTestingDialogMessage_;
    std::string response_;
    std::string correctKeywords_{"0"};
    EventListener *listener_{};
    bool freeResponseCleared_{};
    bool exitTestButtonHidden_{};
    bool exitTestButtonShown_{};
    bool nextTrialButtonShown_{};
    bool nextTrialButtonHidden_{};
    bool evaluationButtonsShown_{};
    bool responseSubmissionShown_{};
    bool responseSubmissionHidden_{};
    bool evaluationButtonsHidden_{};
    bool correctKeywordsEntryShown_{};
    bool correctKeywordsEntryHidden_{};
    bool continueTestingDialogShown_{};
    bool continueTestingDialogHidden_{};
    bool shown_{};
    bool hidden_{};
    bool flagged_{};
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

    [[nodiscard]] auto cursorShown() const -> bool { return cursorShown_; }

    void showCursor() override { cursorShown_ = true; }

  private:
    std::vector<std::string> audioDevices_;
    std::string errorMessage_;
    std::string browseForDirectoryResult_;
    std::string browseForOpeningFileResult_;
    std::string audioDevice_;
    bool eventLoopCalled_{};
    bool browseCancelled_{};
    bool cursorShown_{};
};

class TestSettingsInterpreterStub : public TestSettingsInterpreter {
  public:
    explicit TestSettingsInterpreterStub(const Calibration &calibration_ = {})
        : calibration_{calibration_} {}

    auto calibration(const std::string &t) -> Calibration override {
        text_ = t;
        return calibration_;
    }

    void initialize(Model &m, const std::string &t, const TestIdentity &id,
        SNR snr) override {
        startingSnr_ = snr.dB;
        text_ = t;
        identity_ = id;
        if (initializeAnyTestOnApply_)
            m.initialize(AdaptiveTest{});
    }

    [[nodiscard]] auto text() const -> std::string { return text_; }

    [[nodiscard]] auto identity() const -> TestIdentity { return identity_; }

    [[nodiscard]] auto textForMethodQuery() const -> std::string {
        return textForMethodQuery_;
    }

    [[nodiscard]] auto startingSnr() const -> int { return startingSnr_; }

    void setMethod(Method m) { method_ = m; }

    auto method(const std::string &t) -> Method override {
        textForMethodQuery_ = t;
        return method_;
    }

    void initializeAnyTestOnApply() { initializeAnyTestOnApply_ = true; }

  private:
    std::string text_;
    std::string textForMethodQuery_;
    TestIdentity identity_{};
    int startingSnr_{};
    const Calibration &calibration_;
    Method method_{};
    bool initializeAnyTestOnApply_{};
};

class TextFileReaderStub : public TextFileReader {
  public:
    [[nodiscard]] auto filePath() const -> std::string { return filePath_; }

    auto read(const LocalUrl &s) -> std::string override {
        filePath_ = s.path;
        return read_;
    }

    void setRead(std::string s) { read_ = std::move(s); }

  private:
    std::string filePath_;
    std::string read_;
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run() = 0;
};

void run(UseCase &useCase) { useCase.run(); }

class ConditionUseCase : public virtual UseCase {
  public:
    virtual auto condition(ModelStub &) -> Condition = 0;
};

class LevelUseCase : public virtual UseCase {
  public:
    virtual auto fullScaleLevel(ModelStub &) -> int = 0;
};

class PlayingCalibration : public LevelUseCase {
    TestSetupViewStub *view;

  public:
    explicit PlayingCalibration(TestSetupViewStub *view) : view{view} {}

    void run() override { view->playCalibration(); }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return m.calibration().fullScaleLevel.dB_SPL;
    }
};

class ConfirmingTestSetup : public virtual UseCase {};

void confirmTestSetup(TestSetupViewStub *view) { view->confirmTestSetup(); }

void setMethod(TestSettingsInterpreterStub &interpeter, Method m) {
    interpeter.setMethod(m);
}

class ConfirmingAdaptiveCoordinateResponseMeasureTest
    : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptiveCoordinateResponseMeasureTest(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::adaptiveCoordinateResponseMeasure);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker
    : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    explicit ConfirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking
    : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    explicit ConfirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::adaptiveCoordinateResponseMeasureWithEyeTracking);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker
    : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptivePassFailTest : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptivePassFailTest(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::adaptivePassFail);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptivePassFailTestWithEyeTracking
    : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptivePassFailTestWithEyeTracking(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::adaptivePassFailWithEyeTracking);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptiveCorrectKeywordsTest : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptiveCorrectKeywordsTest(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::adaptiveCorrectKeywords);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptiveCorrectKeywordsTestWithEyeTracking
    : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptiveCorrectKeywordsTestWithEyeTracking(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::adaptiveCorrectKeywordsWithEyeTracking);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelFreeResponseWithTargetReplacementTest
    : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelFreeResponseWithTargetReplacementTest(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(
            interpreter, Method::fixedLevelFreeResponseWithTargetReplacement);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelConsonantTest : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelConsonantTest(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::fixedLevelConsonants);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement
    : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
        confirmTestSetup(view);
    }
};

class
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking
    : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::
                fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargets
    : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargets(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {

        setMethod(interpreter,
            Method::
                fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest
    : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::fixedLevelFreeResponseWithSilentIntervalTargets);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelFreeResponseTestWithAllTargets
    : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelFreeResponseTestWithAllTargets(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::fixedLevelFreeResponseWithAllTargets);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking
    : public ConfirmingTestSetup {
    TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking(
        TestSetupViewStub *view, TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::fixedLevelFreeResponseWithAllTargetsAndEyeTracking);
        confirmTestSetup(view);
    }
};

class TrialSubmission : public virtual UseCase {
  public:
    virtual auto nextTrialButtonShown() -> bool = 0;
    virtual auto responseViewShown() -> bool = 0;
    virtual auto responseViewHidden() -> bool = 0;
};

class SubmittingCoordinateResponseMeasure : public TrialSubmission {
    CoordinateResponseMeasureViewStub *view;

  public:
    explicit SubmittingCoordinateResponseMeasure(
        CoordinateResponseMeasureViewStub *view)
        : view{view} {}

    void run() override { view->submitResponse(); }

    auto nextTrialButtonShown() -> bool override {
        return view->nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view->responseButtonsShown();
    }

    auto responseViewHidden() -> bool override {
        return view->responseButtonsHidden();
    }
};

class SubmittingConsonant : public TrialSubmission {
    ConsonantViewStub *view;

  public:
    explicit SubmittingConsonant(ConsonantViewStub *view) : view{view} {}

    void run() override { view->submitResponse(); }

    auto nextTrialButtonShown() -> bool override {
        return view->readyButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view->responseButtonsShown();
    }

    auto responseViewHidden() -> bool override {
        return view->responseButtonsHidden();
    }
};

class SubmittingFreeResponse : public TrialSubmission {
    ExperimenterViewStub &view;

  public:
    explicit SubmittingFreeResponse(ExperimenterViewStub &view) : view{view} {}

    void run() override { view.submitFreeResponse(); }

    auto nextTrialButtonShown() -> bool override {
        return view.nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view.responseSubmissionShown();
    }

    auto responseViewHidden() -> bool override {
        return view.responseSubmissionHidden();
    }
};

class ExitingTest : public UseCase {
    ExperimenterViewStub *view;

  public:
    explicit ExitingTest(ExperimenterViewStub *view) : view{view} {}

    void run() override { view->exitTest(); }
};

class SubmittingPassedTrial : public TrialSubmission {
    ExperimenterViewStub &view;

  public:
    explicit SubmittingPassedTrial(ExperimenterViewStub &view) : view{view} {}

    void run() override { view.submitPassedTrial(); }

    auto nextTrialButtonShown() -> bool override {
        return view.nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view.evaluationButtonsShown();
    }

    auto responseViewHidden() -> bool override {
        return view.evaluationButtonsHidden();
    }
};

class SubmittingFailedTrial : public TrialSubmission {
    ExperimenterViewStub &view;

  public:
    explicit SubmittingFailedTrial(ExperimenterViewStub &view) : view{view} {}

    void run() override { view.submitFailedTrial(); }

    auto nextTrialButtonShown() -> bool override {
        return view.nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view.evaluationButtonsShown();
    }

    auto responseViewHidden() -> bool override {
        return view.evaluationButtonsHidden();
    }
};

class SubmittingCorrectKeywords : public TrialSubmission {
    ExperimenterViewStub &view;

  public:
    explicit SubmittingCorrectKeywords(ExperimenterViewStub &view)
        : view{view} {}

    void run() override { view.submitCorrectKeywords(); }

    auto nextTrialButtonShown() -> bool override {
        return view.nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view.correctKeywordsEntryShown();
    }

    auto responseViewHidden() -> bool override {
        return view.correctKeywordsEntryHidden();
    }
};

class DecliningContinuingTesting : public UseCase {
  public:
    explicit DecliningContinuingTesting(ExperimenterViewStub &view)
        : view{view} {}

    void run() override { view.declineContinuingTesting(); }

  private:
    ExperimenterViewStub &view;
};

class AcceptingContinuingTesting : public UseCase {
  public:
    explicit AcceptingContinuingTesting(ExperimenterViewStub &view)
        : view{view} {}

    void run() override { view.acceptContinuingTesting(); }

  private:
    ExperimenterViewStub &view;
};

class PlayingTrial : public virtual UseCase {
  public:
    virtual auto nextTrialButtonHidden() -> bool = 0;
    virtual auto nextTrialButtonShown() -> bool = 0;
};

class PlayingCoordinateResponseMeasureTrial : public PlayingTrial {
    CoordinateResponseMeasureViewStub *view;

  public:
    explicit PlayingCoordinateResponseMeasureTrial(
        CoordinateResponseMeasureViewStub *view)
        : view{view} {}

    void run() override { view->playTrial(); }

    auto nextTrialButtonHidden() -> bool override {
        return view->nextTrialButtonHidden();
    }

    auto nextTrialButtonShown() -> bool override {
        return view->nextTrialButtonShown();
    }
};

class PlayingConsonantTrial : public PlayingTrial {
    ConsonantViewStub *view;

  public:
    explicit PlayingConsonantTrial(ConsonantViewStub *view) : view{view} {}

    void run() override { view->playTrial(); }

    auto nextTrialButtonHidden() -> bool override {
        return view->readyButtonHidden();
    }

    auto nextTrialButtonShown() -> bool override {
        return view->readyButtonShown();
    }
};

class PlayingTrialFromExperimenter : public PlayingTrial {
    ExperimenterViewStub &view;

  public:
    explicit PlayingTrialFromExperimenter(ExperimenterViewStub &view)
        : view{view} {}

    void run() override { view.playTrial(); }

    auto nextTrialButtonHidden() -> bool override {
        return view.nextTrialButtonHidden();
    }

    auto nextTrialButtonShown() -> bool override {
        return view.nextTrialButtonShown();
    }
};

class BrowsingUseCase : public virtual UseCase {
  public:
    virtual void setResult(ViewStub &, std::string) = 0;
};

class BrowsingEnteredPathUseCase : public virtual BrowsingUseCase {
  public:
    virtual auto entry() -> std::string = 0;
    virtual void setEntry(std::string) = 0;
};

class BrowsingForTestSettingsFile : public BrowsingEnteredPathUseCase {
    TestSetupViewStub *view;

  public:
    explicit BrowsingForTestSettingsFile(TestSetupViewStub *view)
        : view{view} {}

    void run() override { view->browseForTestSettingsFile(); }

    void setResult(ViewStub &view_, std::string s) override {
        view_.setBrowseForOpeningFileResult(s);
    }

    auto entry() -> std::string override { return view->testSettingsFile(); }

    void setEntry(std::string s) override {
        view->setTestSettingsFile(std::move(s));
    }
};

class PresenterConstructionTests : public ::testing::Test {
  protected:
    ModelStub model;
    TestSetupViewStub setupView;
    CoordinateResponseMeasureViewStub coordinateResponseMeasureView;
    ConsonantViewStub consonantView;
    ExperimenterViewStub experimenterView;
    ViewStub view;
    Presenter::TestSetup testSetup{&setupView};
    Presenter::CoordinateResponseMeasure coordinateResponseMeasure{
        &coordinateResponseMeasureView};
    Presenter::Consonant consonant{&consonantView};
    Presenter::Experimenter experimenter{&experimenterView};
    TestSettingsInterpreterStub testSettingsInterpreter;
    TextFileReaderStub textFileReader;

    auto construct() -> Presenter {
        return {model, view, testSetup, coordinateResponseMeasure, consonant,
            experimenter, testSettingsInterpreter, textFileReader};
    }
};

auto entry(BrowsingEnteredPathUseCase &useCase) -> std::string {
    return useCase.entry();
}

void assertEntryEquals(
    BrowsingEnteredPathUseCase &useCase, const std::string &s) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, entry(useCase));
}

void assertHidesPlayTrialButton(PlayingTrial &useCase) {
    run(useCase);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(useCase.nextTrialButtonHidden());
}

void assertConfirmTestSetupShowsNextTrialButton(
    ConfirmingTestSetup &confirmingTest, PlayingTrial &playingTrial) {
    run(confirmingTest);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(playingTrial.nextTrialButtonShown());
}

void assertShowsNextTrialButton(TrialSubmission &useCase) {
    run(useCase);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(useCase.nextTrialButtonShown());
}

void assertResponseViewHidden(TrialSubmission &useCase) {
    run(useCase);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(useCase.responseViewHidden());
}

void submitResponse(CoordinateResponseMeasureViewStub &view) {
    view.submitResponse();
}

void submitFreeResponse(ExperimenterViewStub &view) {
    view.submitFreeResponse();
}

void exitTest(ExperimenterViewStub &view) { view.exitTest(); }

void playCalibration(TestSetupViewStub &view) { view.playCalibration(); }

auto shown(TestSetupViewStub &view) -> bool { return view.shown(); }

void assertShown(TestSetupViewStub &view) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(shown(view));
}

auto hidden(TestSetupViewStub &view) -> bool { return view.hidden(); }

auto hidden(ExperimenterViewStub &view) -> bool { return view.hidden(); }

auto shown(CoordinateResponseMeasureViewStub &view) -> bool {
    return view.shown();
}

auto shown(ConsonantViewStub &view) -> bool { return view.shown(); }

void assertHidden(CoordinateResponseMeasureViewStub &view) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

void assertHidden(ConsonantViewStub &view) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

void completeTrial(ModelStub &model) { model.completeTrial(); }

auto errorMessage(ViewStub &view) -> std::string { return view.errorMessage(); }

void assertPassedColor(ModelStub &model, coordinate_response_measure::Color c) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(c, model.responseParameters().color);
}

auto calibration(ModelStub &model) -> const Calibration & {
    return model.calibration();
}

void setAudioDevice(ViewStub &view, std::string s) {
    view.setAudioDevice(std::move(s));
}

void setTestComplete(ModelStub &model) { model.setTestComplete(); }

auto trialPlayed(ModelStub &model) -> bool { return model.trialPlayed(); }

void setCorrectKeywords(ExperimenterViewStub &view, std::string s) {
    view.setCorrectKeywords(std::move(s));
}

class PresenterTests : public ::testing::Test {
  protected:
    ModelStub model;
    ViewStub view;
    TestSetupViewStub setupView;
    CoordinateResponseMeasureViewStub coordinateResponseMeasureView;
    ConsonantViewStub consonantView;
    ExperimenterViewStub experimenterView;
    Presenter::TestSetup testSetup{&setupView};
    Presenter::Experimenter experimenter{&experimenterView};
    Presenter::CoordinateResponseMeasure coordinateResponseMeasure{
        &coordinateResponseMeasureView};
    Presenter::Consonant consonant{&consonantView};
    Calibration interpretedCalibration;
    TestSettingsInterpreterStub testSettingsInterpreter{interpretedCalibration};
    TextFileReaderStub textFileReader;
    Presenter presenter{model, view, testSetup, coordinateResponseMeasure,
        consonant, experimenter, testSettingsInterpreter, textFileReader};
    BrowsingForTestSettingsFile browsingForTestSettingsFile{&setupView};
    ConfirmingAdaptiveCoordinateResponseMeasureTest
        confirmingAdaptiveCoordinateResponseMeasureTest{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptivePassFailTest confirmingAdaptivePassFailTest{
        &setupView, testSettingsInterpreter};
    ConfirmingAdaptivePassFailTestWithEyeTracking
        confirmingAdaptivePassFailTestWithEyeTracking{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelFreeResponseWithTargetReplacementTest
        confirmingFixedLevelFreeResponseWithTargetReplacementTest{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelConsonantTest confirmingFixedLevelConsonantTest{
        &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptiveCorrectKeywordsTest confirmingAdaptiveCorrectKeywordsTest{
        &setupView, testSettingsInterpreter};
    ConfirmingAdaptiveCorrectKeywordsTestWithEyeTracking
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargets
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelFreeResponseTestWithAllTargets
        confirmingFixedLevelFreeResponseTestWithAllTargets{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking
        confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking{
            &setupView, testSettingsInterpreter};
    PlayingCalibration playingCalibration{&setupView};
    PlayingCoordinateResponseMeasureTrial playingCoordinateResponseMeasureTrial{
        &coordinateResponseMeasureView};
    PlayingConsonantTrial playingConsonantTrial{&consonantView};
    PlayingTrialFromExperimenter playingTrialFromExperimenter{experimenterView};
    SubmittingCoordinateResponseMeasure submittingCoordinateResponseMeasure{
        &coordinateResponseMeasureView};
    SubmittingConsonant submittingConsonant{&consonantView};
    SubmittingFreeResponse submittingFreeResponse{experimenterView};
    SubmittingPassedTrial submittingPassedTrial{experimenterView};
    SubmittingCorrectKeywords submittingCorrectKeywords{experimenterView};
    SubmittingFailedTrial submittingFailedTrial{experimenterView};
    DecliningContinuingTesting decliningContinuingTesting{experimenterView};
    AcceptingContinuingTesting acceptingContinuingTesting{experimenterView};
    ExitingTest exitingTest{&experimenterView};

    void assertBrowseResultPassedToEntry(BrowsingEnteredPathUseCase &useCase) {
        setBrowsingResult(useCase, "a");
        run(useCase);
        assertEntryEquals(useCase, "a");
    }

    void setBrowsingResult(BrowsingEnteredPathUseCase &useCase, std::string s) {
        useCase.setResult(view, std::move(s));
    }

    void assertCancellingBrowseDoesNotChangePath(
        BrowsingEnteredPathUseCase &useCase) {
        useCase.setEntry("a");
        setBrowsingResult(useCase, "b");
        view.setBrowseCancelled();
        run(useCase);
        assertEntryEquals(useCase, "a");
    }

    void assertAudioDevicePassedToTrial(PlayingTrial &useCase) {
        setAudioDevice(view, "a");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"a"}, model.trialParameters().audioDevice);
    }

    void assertPlaysTrial(UseCase &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(trialPlayed(model));
    }

    void assertHidesExitTestButton(PlayingTrial &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.exitTestButtonHidden());
    }

    void assertCompleteTestShowsSetupView(TrialSubmission &useCase) {
        setTestComplete(model);
        run(useCase);
        assertShown(setupView);
    }

    void assertShowsSetupView(UseCase &useCase) {
        run(useCase);
        assertShown(setupView);
    }

    void assertHidesContinueTestingDialog(UseCase &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(
            experimenterView.continueTestingDialogHidden());
    }

    void assertIncompleteTestDoesNotShowSetupView(TrialSubmission &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(shown(setupView));
    }

    void assertCompleteTestHidesExperimenterView(UseCase &useCase) {
        setTestComplete(model);
        assertHidesExperimenterView(useCase);
    }

    void assertHidesExperimenterView(UseCase &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(hidden(experimenterView));
    }

    void assertCompleteTestDoesNotPlayTrial(UseCase &useCase) {
        setTestComplete(model);
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(trialPlayed(model));
    }

    void assertDoesNotHideExperimenterView(TrialSubmission &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(hidden(experimenterView));
    }

    void assertHidesTestSetupView(UseCase &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(hidden(setupView));
    }

    void assertDoesNotHideTestSetupView(UseCase &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(hidden(setupView));
    }

    void assertShowsExperimenterView(UseCase &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.shown());
    }

    void assertDoesNotShowCoordinateResponseMeasureView(UseCase &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(shown(coordinateResponseMeasureView));
    }

    void assertPassesTestSettingsFileToTextFileReader(UseCase &useCase) {
        setupView.setTestSettingsFile("a");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"a"}, textFileReader.filePath());
    }

    void assertPassesTestSettingsTextToTestSettingsInterpreter(
        UseCase &useCase) {
        textFileReader.setRead("a");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"a"}, testSettingsInterpreter.text());
    }

    void assertPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery(
        ConfirmingTestSetup &useCase) {
        textFileReader.setRead("a");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"a"}, testSettingsInterpreter.textForMethodQuery());
    }

    void assertInvalidSnrShowsMessage(ConfirmingTestSetup &useCase) {
        setupView.setStartingSnr("a");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"\"a\" is not a valid starting SNR."},
            errorMessage(view));
    }

    void assertPassesStartingSnr(ConfirmingTestSetup &useCase) {
        setupView.setStartingSnr("1");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            1, testSettingsInterpreter.startingSnr());
    }

    void assertPassesSubjectId(ConfirmingTestSetup &useCase) {
        setupView.setSubjectId("b");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"b"}, testSettingsInterpreter.identity().subjectId);
    }

    void assertPassesTesterId(ConfirmingTestSetup &useCase) {
        setupView.setTesterId("c");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"c"}, testSettingsInterpreter.identity().testerId);
    }

    void assertPassesSession(ConfirmingTestSetup &useCase) {
        setupView.setSession("e");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"e"}, testSettingsInterpreter.identity().session);
    }

    void assertPassesRmeSetting(ConfirmingTestSetup &useCase) {
        setupView.setRmeSetting("e");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"e"}, testSettingsInterpreter.identity().rmeSetting);
    }

    void assertPassesTransducer(ConfirmingTestSetup &useCase) {
        setupView.setTransducer("a");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"a"}, testSettingsInterpreter.identity().transducer);
    }

    void assertCompleteTrialShowsResponseView(
        ConfirmingTestSetup &useCase, TrialSubmission &trialSubmission) {
        run(useCase);
        completeTrial(model);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(trialSubmission.responseViewShown());
    }

    void assertCompleteTrialShowsCursor(ConfirmingTestSetup &useCase) {
        run(useCase);
        completeTrial(model);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.cursorShown());
    }

    void assertShowsTrialNumber(UseCase &useCase) {
        model.setTrialNumber(1);
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"Trial 1"}, experimenterView.displayed());
    }

    void assertShowsTargetFileName(UseCase &useCase) {
        model.setTargetFileName("a");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"a"}, experimenterView.secondaryDisplayed());
    }

    void assertShowsCoordinateResponseMeasureView(UseCase &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(shown(coordinateResponseMeasureView));
    }

    void assertShowsConsonantView(UseCase &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(shown(consonantView));
    }

    void assertExitTestAfterCompletingTrialHidesResponseSubmission(
        UseCase &useCase, TrialSubmission &submission) {
        run(useCase);
        completeTrial(model);
        exitTest(experimenterView);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(submission.responseViewHidden());
    }

    void assertCompleteTestShowsContinueTestingDialog(UseCase &useCase) {
        setTestComplete(model);
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(
            experimenterView.continueTestingDialogShown());
    }

    void assertCompleteTestShowsThresholds(UseCase &useCase) {
        setTestComplete(model);
        model.setAdaptiveTestResults({{{"a"}, 1.}, {{"b"}, 2.}, {{"c"}, 3.}});
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"thresholds (targets: dB SNR)\na: 1\nb: 2\nc: 3"},
            experimenterView.continueTestingDialogMessage());
    }

    void assertCompleteTestHidesResponse(TrialSubmission &useCase) {
        setTestComplete(model);
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(useCase.responseViewHidden());
    }
};

class RequestFailingModel : public Model {
    std::string errorMessage{};

  public:
    auto trialNumber() -> int override { return 0; }

    auto targetFileName() -> std::string override { return {}; }

    void setErrorMessage(std::string s) { errorMessage = std::move(s); }

    void initialize(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initialize(const FixedLevelTestWithEachTargetNTimes &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithTargetReplacement(
        const FixedLevelFixedTrialsTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithTargetReplacementAndEyeTracking(
        const FixedLevelFixedTrialsTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithSilentIntervalTargets(const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithAllTargets(const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithAllTargetsAndEyeTracking(
        const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithSingleSpeaker(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithDelayedMasker(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithCyclicTargets(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithCyclicTargetsAndEyeTracking(
        const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithEyeTracking(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void playTrial(const AudioSettings &) override {
        throw RequestFailure{errorMessage};
    }

    void submit(const coordinate_response_measure::Response &) override {
        throw RequestFailure{errorMessage};
    }

    void submit(const FreeResponse &) override {
        throw RequestFailure{errorMessage};
    }

    void submit(const CorrectKeywords &) override {
        throw RequestFailure{errorMessage};
    }

    void submit(const ConsonantResponse &) override {
        throw RequestFailure{errorMessage};
    }

    void playCalibration(const Calibration &) override {
        throw RequestFailure{errorMessage};
    }

    auto testComplete() -> bool override { return {}; }
    auto audioDevices() -> AudioDevices override { return {}; }
    auto adaptiveTestResults() -> AdaptiveTestResults override { return {}; }
    void subscribe(EventListener *) override {}
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void restartAdaptiveTestWhilePreservingTargets() override {}
};

class PresenterFailureTests : public ::testing::Test {
  protected:
    RequestFailingModel failure;
    ModelStub defaultModel;
    Model *model{&defaultModel};
    ViewStub view;
    TestSetupViewStub setupView;
    CoordinateResponseMeasureViewStub coordinateResponseMeasureView;
    ConsonantViewStub consonantView;
    ExperimenterViewStub experimenterView;
    Presenter::TestSetup testSetup{&setupView};
    Presenter::CoordinateResponseMeasure coordinateResponseMeasure{
        &coordinateResponseMeasureView};
    Presenter::Consonant consonant{&consonantView};
    Presenter::Experimenter experimenter{&experimenterView};
    TestSettingsInterpreterStub testSettingsInterpreter;
    TextFileReaderStub textFileReader;

    void useFailingModel(std::string s = {}) {
        failure.setErrorMessage(std::move(s));
        model = &failure;
        testSettingsInterpreter.initializeAnyTestOnApply();
    }

    void confirmTestSetup() {
        Presenter presenter{*model, view, testSetup, coordinateResponseMeasure,
            consonant, experimenter, testSettingsInterpreter, textFileReader};
        setupView.confirmTestSetup();
    }

    void assertConfirmTestSetupShowsErrorMessage(const std::string &s) {
        confirmTestSetup();
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, errorMessage(view));
    }

    void assertConfirmTestSetupDoesNotHideSetupView() {
        confirmTestSetup();
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(setupView.hidden());
    }
};

TEST_F(PresenterConstructionTests, populatesAudioDeviceMenu) {
    model.setAudioDevices({"a", "b", "c"});
    construct();
    assertEqual({"a", "b", "c"}, view.audioDevices());
}

TEST_F(PresenterConstructionTests, populatesTransducerMenu) {
    construct();
    assertEqual({name(Transducer::headphone), name(Transducer::oneSpeaker),
                    name(Transducer::twoSpeakers)},
        setupView.transducers());
}

#define PRESENTER_TEST(a) TEST_F(PresenterTests, a)

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestDoesNotShowExperimentersNextTrialButton) {
    run(confirmingAdaptiveCoordinateResponseMeasureTest);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(experimenterView.nextTrialButtonShown());
}

PRESENTER_TEST(
    exitTestAfterCompletingTrialHidesCorrectKeywordsSubmissionForAdaptiveCorrectKeywordsTest) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingAdaptiveCorrectKeywordsTest, submittingCorrectKeywords);
}

PRESENTER_TEST(
    exitTestAfterCompletingTrialHidesCorrectKeywordsSubmissionForAdaptiveCorrectKeywordsTestWithEyeTracking) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking,
        submittingCorrectKeywords);
}

PRESENTER_TEST(
    exitTestAfterCompletingTrialHidesFreeResponseSubmissionForFixedLevelFreeResponseTestWithAllTargets) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingFixedLevelFreeResponseTestWithAllTargets,
        submittingFreeResponse);
}

PRESENTER_TEST(
    exitTestAfterCompletingTrialHidesFreeResponseSubmissionForFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking,
        submittingFreeResponse);
}

PRESENTER_TEST(
    exitTestAfterCompletingTrialHidesPassFailSubmissionForAdaptivePassFailTest) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingAdaptivePassFailTest, submittingPassedTrial);
}

PRESENTER_TEST(
    exitTestAfterCompletingTrialHidesPassFailSubmissionForAdaptivePassFailTestWithEyeTracking) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingAdaptivePassFailTestWithEyeTracking, submittingPassedTrial);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestShowsTargetFileName) {
    assertShowsTargetFileName(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithEyeTrackingShowsTargetFileName) {
    assertShowsTargetFileName(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking);
}

PRESENTER_TEST(confirmingAdaptiveClosedSetTestShowsTargetFileName) {
    assertShowsTargetFileName(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(submittingCorrectKeywordsShowsTargetFileName) {
    assertShowsTargetFileName(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingCoordinateResponseShowsTargetFileName) {
    assertShowsTargetFileName(submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(submittingFreeResponseShowsTargetFileName) {
    assertShowsTargetFileName(submittingFreeResponse);
}

PRESENTER_TEST(submittingCorrectKeywordsPassesCorrectKeywords) {
    setCorrectKeywords(experimenterView, "1");
    run(submittingCorrectKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, model.correctKeywords());
}

PRESENTER_TEST(submittingInvalidCorrectKeywordsShowsErrorMessage) {
    setCorrectKeywords(experimenterView, "a");
    run(submittingCorrectKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"\"a\" is not a valid number."}, errorMessage(view));
}

PRESENTER_TEST(submittingInvalidCorrectKeywordsDoesNotHideEntry) {
    setCorrectKeywords(experimenterView, "a");
    run(submittingCorrectKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(
        submittingCorrectKeywords.responseViewHidden());
}

PRESENTER_TEST(
    acceptingContinuingTestingRestartsAdaptiveTestWhilePreservingCyclicTargets) {
    run(acceptingContinuingTesting);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        model.adaptiveTestRestartedWhilePreservingCyclicTargets());
}

PRESENTER_TEST(acceptingContinuingTestingHidesContinueTestingDialog) {
    assertHidesContinueTestingDialog(acceptingContinuingTesting);
}

PRESENTER_TEST(decliningContinuingTestingHidesContinueTestingDialog) {
    assertHidesContinueTestingDialog(decliningContinuingTesting);
}

PRESENTER_TEST(decliningContinuingTestingShowsSetupView) {
    assertShowsSetupView(decliningContinuingTesting);
}

PRESENTER_TEST(
    submittingCorrectKeywordsShowsContinueTestingDialogWhenComplete) {
    assertCompleteTestShowsContinueTestingDialog(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingPassedTrialShowsContinueTestingDialogWhenComplete) {
    assertCompleteTestShowsContinueTestingDialog(submittingPassedTrial);
}

PRESENTER_TEST(submittingFailedTrialShowsContinueTestingDialogWhenComplete) {
    assertCompleteTestShowsContinueTestingDialog(submittingFailedTrial);
}

PRESENTER_TEST(submittingCorrectKeywordsShowsThresholdsWhenTestingComplete) {
    assertCompleteTestShowsThresholds(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingPassedTrialShowsThresholdsWhenTestingComplete) {
    assertCompleteTestShowsThresholds(submittingPassedTrial);
}

PRESENTER_TEST(submittingFailedTrialShowsThresholdsWhenTestingComplete) {
    assertCompleteTestShowsThresholds(submittingFailedTrial);
}

PRESENTER_TEST(submittingCorrectKeywordsHidesSubmissionEvenWhenTestComplete) {
    assertCompleteTestHidesResponse(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingPassedTrialHidesSubmissionEvenWhenTestComplete) {
    assertCompleteTestHidesResponse(submittingPassedTrial);
}

PRESENTER_TEST(submittingFailedTrialHidesSubmissionEvenWhenTestComplete) {
    assertCompleteTestHidesResponse(submittingFailedTrial);
}

PRESENTER_TEST(completingTrialShowsCursorForAdaptiveCorrectKeywordsTest) {
    assertCompleteTrialShowsCursor(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterCorrectKeywordsEntryForAdaptiveCorrectKeywordsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCorrectKeywordsTest, submittingCorrectKeywords);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterCorrectKeywordsEntryForAdaptiveCorrectKeywordsTestWithEyeTracking) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking,
        submittingCorrectKeywords);
}

PRESENTER_TEST(callsEventLoopWhenRun) {
    presenter.run();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.eventLoopCalled());
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithEyeTrackingHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeakerHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMaskerHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTrackingHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestWithEyeTrackingHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptivePassFailTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(confirmingFixedLevelConsonantTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingFixedLevelConsonantTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithEyeTrackingShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestWithEyeTrackingShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptivePassFailTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(confirmingFixedLevelConsonantTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingFixedLevelConsonantTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestDoesNotHideSetupViewWhenTestComplete) {
    setTestComplete(model);
    assertDoesNotHideTestSetupView(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestShowsCoordinateResponseMeasureView) {
    assertShowsCoordinateResponseMeasureView(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeakerShowsCoordinateResponseMeasureView) {
    assertShowsCoordinateResponseMeasureView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMaskerShowsCoordinateResponseMeasureView) {
    assertShowsCoordinateResponseMeasureView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTrackingShowsSubjectView) {
    assertShowsCoordinateResponseMeasureView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsCoordinateResponseMeasureView) {
    assertShowsCoordinateResponseMeasureView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingShowsSubjectView) {
    assertShowsCoordinateResponseMeasureView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelConsonantTestWithTargetReplacementShowsCoordinateResponseMeasureView) {
    assertShowsConsonantView(confirmingFixedLevelConsonantTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsCoordinateResponseMeasureView) {
    assertShowsCoordinateResponseMeasureView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestDoesNotShowCoordinateResponseMeasureView) {
    assertDoesNotShowCoordinateResponseMeasureView(
        confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithEyeTrackingDoesNotShowCoordinateResponseMeasureView) {
    assertDoesNotShowCoordinateResponseMeasureView(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestDoesNotShowCoordinateResponseMeasureViewWhenTestComplete) {
    setTestComplete(model);
    assertDoesNotShowCoordinateResponseMeasureView(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestDoesNotShowCoordinateResponseMeasureView) {
    assertDoesNotShowCoordinateResponseMeasureView(
        confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestWithEyeTrackingDoesNotShowCoordinateResponseMeasureView) {
    assertDoesNotShowCoordinateResponseMeasureView(
        confirmingAdaptivePassFailTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementDoesNotShowCoordinateResponseMeasureView) {
    assertDoesNotShowCoordinateResponseMeasureView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsDoesNotShowCoordinateResponseMeasureView) {
    assertDoesNotShowCoordinateResponseMeasureView(
        confirmingFixedLevelFreeResponseTestWithAllTargets);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTrackingDoesNotShowCoordinateResponseMeasureView) {
    assertDoesNotShowCoordinateResponseMeasureView(
        confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithSilentIntervalTargetsDoesNotShowCoordinateResponseMeasureView) {
    assertDoesNotShowCoordinateResponseMeasureView(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(playCalibrationPassesLevel) {
    interpretedCalibration.level.dB_SPL = 1;
    playCalibration(setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, calibration(model).level.dB_SPL);
}

PRESENTER_TEST(playingCalibrationPassesTestSettingsFileToTextFileReader) {
    assertPassesTestSettingsFileToTextFileReader(playingCalibration);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsFileToTextFileReader) {
    assertPassesTestSettingsFileToTextFileReader(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery) {
    assertPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsTextToTestSettingsInterpreter) {
    assertPassesTestSettingsTextToTestSettingsInterpreter(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    playingCalibrationPassesTestSettingsTextToTestSettingsInterpreter) {
    assertPassesTestSettingsTextToTestSettingsInterpreter(playingCalibration);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesStartingSnr) {
    assertPassesStartingSnr(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithInvalidStartingSnrShowsMessage) {
    assertInvalidSnrShowsMessage(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesSubjectId) {
    assertPassesSubjectId(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesSubjectId) {
    assertPassesSubjectId(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestWithEyeTrackingPassesSubjectId) {
    assertPassesSubjectId(confirmingAdaptivePassFailTestWithEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesSubjectId) {
    assertPassesSubjectId(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesSubjectId) {
    assertPassesSubjectId(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingPassesSubjectId) {
    assertPassesSubjectId(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking);
}

PRESENTER_TEST(confirmingFixedLevelConsonantTestPassesSubjectId) {
    assertPassesSubjectId(confirmingFixedLevelConsonantTest);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesTesterId) {
    assertPassesTesterId(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesTesterId) {
    assertPassesTesterId(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesTesterId) {
    assertPassesTesterId(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesTesterId) {
    assertPassesTesterId(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingPassesTesterId) {
    assertPassesTesterId(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking);
}

PRESENTER_TEST(playCalibrationPassesFilePath) {
    interpretedCalibration.fileUrl.path = "a";
    playCalibration(setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, calibration(model).fileUrl.path);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesSession) {
    assertPassesSession(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesSession) {
    assertPassesSession(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesSession) {
    assertPassesSession(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesSession) {
    assertPassesSession(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesRmeSetting) {
    assertPassesRmeSetting(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesTransducer) {
    assertPassesTransducer(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveCorrectKeywordsTest, playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithEyeTrackingShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveCoordinateResponseMeasureTest,
        playingCoordinateResponseMeasureTrial);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestShowsNextTrialButtonForCoordinateResponseMeasure) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveCoordinateResponseMeasureTest,
        playingCoordinateResponseMeasureTrial);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsNextTrialButtonForCoordinateResponseMeasure) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement,
        playingCoordinateResponseMeasureTrial);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingShowsNextTrialButtonForCoordinateResponseMeasure) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking,
        playingCoordinateResponseMeasureTrial);
}

PRESENTER_TEST(
    confirmingFixedLevelConsonantTestShowsNextTrialButtonForConsonants) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelConsonantTest, playingConsonantTrial);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsNextTrialButtonForCoordinateResponseMeasure) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest,
        playingCoordinateResponseMeasureTrial);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptivePassFailTest, playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestWithEyeTrackingShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptivePassFailTestWithEyeTracking,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseTestWithAllTargets,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTrackingShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithSilentIntervalTargetsShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(playingConsonantTrialHidesCursor) {
    run(playingConsonantTrial);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(consonantView.cursorHidden());
}

PRESENTER_TEST(submittingConsonantTrialHidesCursor) {
    run(submittingConsonant);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(consonantView.cursorHidden());
}

PRESENTER_TEST(submittingCoordinateResponseMeasurePlaysTrial) {
    assertPlaysTrial(submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(submittingConsonantPlaysTrial) {
    assertPlaysTrial(submittingConsonant);
}

PRESENTER_TEST(playingCoordinateResponseMeasureTrialPlaysTrial) {
    assertPlaysTrial(playingCoordinateResponseMeasureTrial);
}

PRESENTER_TEST(playingConsonantTrialPlaysTrial) {
    assertPlaysTrial(playingConsonantTrial);
}

PRESENTER_TEST(playingTrialFromExperimenterPlaysTrial) {
    assertPlaysTrial(playingTrialFromExperimenter);
}

PRESENTER_TEST(playingTrialHidesNextTrialButton) {
    assertHidesPlayTrialButton(playingCoordinateResponseMeasureTrial);
}

PRESENTER_TEST(playingTrialFromConsonantViewHidesNextTrialButton) {
    assertHidesPlayTrialButton(playingConsonantTrial);
}

PRESENTER_TEST(playingTrialHidesNextTrialButtonForExperimenter) {
    assertHidesPlayTrialButton(playingTrialFromExperimenter);
}

PRESENTER_TEST(playingCoordinateResponseMeasureTrialHidesExitTestButton) {
    assertHidesExitTestButton(playingCoordinateResponseMeasureTrial);
}

PRESENTER_TEST(playingTrialFromConsonantViewHidesExitTestButton) {
    assertHidesExitTestButton(playingConsonantTrial);
}

PRESENTER_TEST(playingCoordinateResponseMeasureTrialPassesAudioDevice) {
    assertAudioDevicePassedToTrial(playingCoordinateResponseMeasureTrial);
}

PRESENTER_TEST(playingTrialFromConsonantViewPassesAudioDevice) {
    assertAudioDevicePassedToTrial(playingConsonantTrial);
}

PRESENTER_TEST(playingTrialFromExperimenterPassesAudioDevice) {
    assertAudioDevicePassedToTrial(playingTrialFromExperimenter);
}

PRESENTER_TEST(playCalibrationPassesAudioDevice) {
    setAudioDevice(view, "b");
    playCalibration(setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, calibration(model).audioDevice);
}

PRESENTER_TEST(coordinateResponsePassesNumberResponse) {
    coordinateResponseMeasureView.setNumberResponse("1");
    submitResponse(coordinateResponseMeasureView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, model.responseParameters().number);
}

PRESENTER_TEST(coordinateResponsePassesGreenColor) {
    coordinateResponseMeasureView.setGreenResponse();
    submitResponse(coordinateResponseMeasureView);
    assertPassedColor(model, coordinate_response_measure::Color::green);
}

PRESENTER_TEST(coordinateResponsePassesRedColor) {
    coordinateResponseMeasureView.setRedResponse();
    submitResponse(coordinateResponseMeasureView);
    assertPassedColor(model, coordinate_response_measure::Color::red);
}

PRESENTER_TEST(coordinateResponsePassesBlueColor) {
    coordinateResponseMeasureView.setBlueResponse();
    submitResponse(coordinateResponseMeasureView);
    assertPassedColor(model, coordinate_response_measure::Color::blue);
}

PRESENTER_TEST(coordinateResponsePassesWhiteColor) {
    coordinateResponseMeasureView.setGrayResponse();
    submitResponse(coordinateResponseMeasureView);
    assertPassedColor(model, coordinate_response_measure::Color::white);
}

PRESENTER_TEST(consonantResponsePassesConsonant) {
    consonantView.setConsonant("b");
    run(submittingConsonant);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL('b', model.consonantResponse().consonant);
}

PRESENTER_TEST(experimenterResponsePassesResponse) {
    experimenterView.setResponse("a");
    submitFreeResponse(experimenterView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, model.freeResponse().response);
}

PRESENTER_TEST(experimenterResponseFlagsResponse) {
    experimenterView.flagResponse();
    submitFreeResponse(experimenterView);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.freeResponse().flagged);
}

PRESENTER_TEST(passedTrialSubmitsCorrectResponse) {
    run(submittingPassedTrial);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.correctResponseSubmitted());
}

PRESENTER_TEST(failedTrialSubmitsIncorrectResponse) {
    run(submittingFailedTrial);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.incorrectResponseSubmitted());
}

PRESENTER_TEST(
    submittingCoordinateResponseMeasureShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(submittingConsonantShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(submittingConsonant);
}

PRESENTER_TEST(respondFromExperimenterShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(submittingFreeResponse);
}

PRESENTER_TEST(
    submittingCorrectKeywordsDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingCorrectKeywords);
}

PRESENTER_TEST(
    submittingCoordinateResponseMeasureDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(
        submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(submittingConsonantDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingConsonant);
}

PRESENTER_TEST(respondFromExperimenterDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingFreeResponse);
}

PRESENTER_TEST(submitPassedTrialDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingFailedTrial);
}

PRESENTER_TEST(
    submittingCoordinateResponseMeasureHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(
        submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(submittingConsonantHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(submittingConsonant);
}

PRESENTER_TEST(respondFromExperimenterHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(submittingFreeResponse);
}

PRESENTER_TEST(submittingCoordinateResponseDoesNotPlayTrialWhenTestComplete) {
    assertCompleteTestDoesNotPlayTrial(submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(submittingConsonantDoesNotPlayTrialWhenTestComplete) {
    assertCompleteTestDoesNotPlayTrial(submittingConsonant);
}

PRESENTER_TEST(
    submittingCorrectKeywordsDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingCorrectKeywords);
}

PRESENTER_TEST(
    submittingCoordinateResponseMeasureDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(
    submittingConsonantMeasureDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingConsonant);
}

PRESENTER_TEST(
    respondFromExperimenterDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingFreeResponse);
}

PRESENTER_TEST(submitPassedTrialDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingFailedTrial);
}

PRESENTER_TEST(submittingCorrectKeywordsShowsNextTrialButton) {
    assertShowsNextTrialButton(submittingCorrectKeywords);
}

PRESENTER_TEST(experimenterResponseShowsNextTrialButton) {
    assertShowsNextTrialButton(submittingFreeResponse);
}

PRESENTER_TEST(submittingPassedTrialShowsNextTrialButtonForExperimenter) {
    assertShowsNextTrialButton(submittingPassedTrial);
}

PRESENTER_TEST(submittingFailedTrialShowsNextTrialButtonForExperimenter) {
    assertShowsNextTrialButton(submittingFailedTrial);
}

PRESENTER_TEST(submittingCorrectKeywordsHidesCorrectKeywordsEntry) {
    assertResponseViewHidden(submittingCorrectKeywords);
}

PRESENTER_TEST(experimenterResponseHidesResponseSubmission) {
    assertResponseViewHidden(submittingFreeResponse);
}

PRESENTER_TEST(correctResponseHidesEvaluationButtons) {
    assertResponseViewHidden(submittingPassedTrial);
}

PRESENTER_TEST(incorrectResponseHidesEvaluationButtons) {
    assertResponseViewHidden(submittingFailedTrial);
}

PRESENTER_TEST(submittingCoordinateResponseHidesResponseButtons) {
    assertResponseViewHidden(submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(submittingConsonantHidesResponseButtons) {
    assertResponseViewHidden(submittingConsonant);
}

PRESENTER_TEST(
    submittingCoordinateResponseHidesCoordinateViewWhenTestComplete) {
    setTestComplete(model);
    run(submittingCoordinateResponseMeasure);
    assertHidden(coordinateResponseMeasureView);
}

PRESENTER_TEST(submittingConsonantHidesConsonantViewWhenTestComplete) {
    setTestComplete(model);
    run(submittingConsonant);
    assertHidden(consonantView);
}

PRESENTER_TEST(exitTestHidesCoordinateView) {
    exitTest(experimenterView);
    assertHidden(coordinateResponseMeasureView);
}

PRESENTER_TEST(exitTestHidesConsonantView) {
    exitTest(experimenterView);
    assertHidden(consonantView);
}

PRESENTER_TEST(decliningContinuingTestingHidesExperimenterView) {
    assertHidesExperimenterView(decliningContinuingTesting);
}

PRESENTER_TEST(exitTestHidesExperimenterView) {
    assertHidesExperimenterView(exitingTest);
}

PRESENTER_TEST(exitTestHidesResponseButtons) {
    run(exitingTest);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        submittingCoordinateResponseMeasure.responseViewHidden());
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(submittingConsonant.responseViewHidden());
}

PRESENTER_TEST(exitTestShowsTestSetupView) {
    exitTest(experimenterView);
    assertShown(setupView);
}

PRESENTER_TEST(browseForTestSettingsFileUpdatesTestSettingsFile) {
    assertBrowseResultPassedToEntry(browsingForTestSettingsFile);
}

PRESENTER_TEST(browseForTestSettingsCancelDoesNotChangeTestSettingsFile) {
    assertCancellingBrowseDoesNotChangePath(browsingForTestSettingsFile);
}

PRESENTER_TEST(completingTrialShowsExitTestButton) {
    completeTrial(model);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.exitTestButtonShown());
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(confirmingFixedLevelConsonantTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingFixedLevelConsonantTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestWithEyeTrackingShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingAdaptiveCorrectKeywordsTestWithEyeTracking);
}

PRESENTER_TEST(submittingCorrectKeywordsShowsTrialNumber) {
    assertShowsTrialNumber(submittingCorrectKeywords);
}

PRESENTER_TEST(acceptingContinuingTestingDialogShowsTrialNumber) {
    assertShowsTrialNumber(acceptingContinuingTesting);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsShowsTrialNumber) {
    assertShowsTrialNumber(confirmingFixedLevelFreeResponseTestWithAllTargets);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTrackingShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithSilentIntervalTargetsShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTrackingShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(submittingResponseFromExperimenterShowsTrialNumber) {
    assertShowsTrialNumber(submittingFreeResponse);
}

PRESENTER_TEST(submittingResponseFromSubjectShowsTrialNumber) {
    assertShowsTrialNumber(submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(submittingPassedTrialShowsTrialNumber) {
    assertShowsTrialNumber(submittingPassedTrial);
}

PRESENTER_TEST(submittingFailedTrialShowsTrialNumber) {
    assertShowsTrialNumber(submittingFailedTrial);
}

PRESENTER_TEST(confirmingFixedLevelConsonantTestShowsTargetFileName) {
    assertShowsTargetFileName(confirmingFixedLevelConsonantTest);
}

PRESENTER_TEST(submittingConsonantResponseShowsTargetFileName) {
    assertShowsTargetFileName(submittingConsonant);
}

PRESENTER_TEST(
    completingTrialShowsCoordinateResponseMeasureResponseButtonsForAdaptiveCoordinateResponseMeasureTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCoordinateResponseMeasureTest,
        submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(
    completingTrialShowsConsonantResponseButtonsForFixedLevelConsonantTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelConsonantTest, submittingConsonant);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveCoordinateResponseMeasureSingleSpeakerTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker,
        submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveCoordinateResponseMeasureTestWithDelayedMasker) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker,
        submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(
    completingTrialShowsCoordinateResponseMeasureResponseButtonsForAdaptiveCoordinateResponseMeasureTestWithEyeTracking) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithEyeTracking,
        submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(
    completingTrialShowsCoordinateResponseMeasureResponseButtonsForFixedLevelCoordinateResponseMeasureTestWithTargetReplacement) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacement,
        submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(
    completingTrialShowsCoordinateResponseMeasureResponseButtonsForFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementAndEyeTracking,
        submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(
    completingTrialShowsCoordinateResponseMeasureResponseButtonsForFixedLevelCoordinateResponseMeasureSilentIntervalsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest,
        submittingCoordinateResponseMeasure);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterEvaluationButtonsForAdaptivePassFailTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptivePassFailTest, submittingPassedTrial);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterEvaluationButtonsForAdaptivePassFailTestWithEyeTracking) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptivePassFailTestWithEyeTracking, submittingPassedTrial);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest,
        submittingFreeResponse);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseTestWithAllTargets) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseTestWithAllTargets,
        submittingFreeResponse);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseTestWithAllTargetsAndEyeTracking,
        submittingFreeResponse);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseSilentIntervalsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest,
        submittingFreeResponse);
}

PRESENTER_TEST(playCalibrationPassesFullScaleLevel) {
    interpretedCalibration.fullScaleLevel.dB_SPL = 1;
    run(playingCalibration);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        1, calibration(model).fullScaleLevel.dB_SPL);
}

PRESENTER_TEST(completingTrialClearsFreeResponseForFixedLevelFreeResponseTest) {
    run(confirmingFixedLevelFreeResponseWithTargetReplacementTest);
    completeTrial(model);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(experimenterView.freeResponseCleared());
}

TEST_F(PresenterFailureTests,
    initializeTestShowsErrorMessageWhenModelFailsRequest) {
    useFailingModel("a");
    assertConfirmTestSetupShowsErrorMessage("a");
}

TEST_F(PresenterFailureTests,
    initializeTestDoesNotHideSetupViewWhenModelFailsRequest) {
    useFailingModel();
    assertConfirmTestSetupDoesNotHideSetupView();
}
}
}
