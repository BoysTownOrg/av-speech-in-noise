#include "assert-utility.h"
#include "ModelStub.hpp"
#include <presentation/Presenter.hpp>
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

class ViewStub : public View {
  public:
    void setAudioDevice(std::string s) { audioDevice_ = std::move(s); }

    void showErrorMessage(std::string s) override {
        errorMessage_ = std::move(s);
    }

    auto errorMessage() { return errorMessage_; }

    void eventLoop() override { eventLoopCalled_ = true; }

    auto browseForDirectory() -> std::string override {
        return browseForDirectoryResult_;
    }

    auto browseCancelled() -> bool override { return browseCancelled_; }

    auto browseForOpeningFile() -> std::string override {
        return browseForOpeningFileResult_;
    }

    auto audioDevice() -> std::string override { return audioDevice_; }

    [[nodiscard]] auto eventLoopCalled() const { return eventLoopCalled_; }

    void setBrowseForOpeningFileResult(std::string s) {
        browseForOpeningFileResult_ = std::move(s);
    }

    void setBrowseCancelled() { browseCancelled_ = true; }

    void populateAudioDeviceMenu(std::vector<std::string> v) override {
        audioDevices_ = std::move(v);
    }

    [[nodiscard]] auto audioDevices() const { return audioDevices_; }

    class TestSetupViewStub : public TestSetup {
      public:
        auto testSettingsFile() -> std::string override {
            return testSettingsFile_;
        }

        void confirmTestSetup() { listener_->confirmTestSetup(); }

        void playCalibration() { listener_->playCalibration(); }

        auto session() -> std::string override { return session_; }

        [[nodiscard]] auto shown() const { return shown_; }

        void show() override { shown_ = true; }

        void hide() override { hidden_ = true; }

        [[nodiscard]] auto hidden() const { return hidden_; }

        void setTestSettingsFile(std::string s) override {
            testSettingsFile_ = std::move(s);
        }

        void setSession(std::string s) { session_ = std::move(s); }

        void setSubjectId(std::string s) { subjectId_ = std::move(s); }

        void setTesterId(std::string s) { testerId_ = std::move(s); }

        auto testerId() -> std::string override { return testerId_; }

        auto subjectId() -> std::string override { return subjectId_; }

        void subscribe(EventListener *listener) override {
            listener_ = listener;
        }

        void browseForTestSettingsFile() {
            listener_->browseForTestSettingsFile();
        }

      private:
        std::string subjectId_;
        std::string testerId_;
        std::string session_;
        std::string testSettingsFile_;
        EventListener *listener_{};
        bool shown_{};
        bool hidden_{};
    };

    class SubjectViewStub : public CoordinateResponseMeasure {
      public:
        void show() override { shown_ = true; }

        [[nodiscard]] auto shown() const { return shown_; }

        [[nodiscard]] auto hidden() const { return hidden_; }

        auto whiteResponse() -> bool override { return grayResponse_; }

        void setGrayResponse() { grayResponse_ = true; }

        auto blueResponse() -> bool override { return blueResponse_; }

        void setBlueResponse() { blueResponse_ = true; }

        void setRedResponse() { redResponse_ = true; }

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

        [[nodiscard]] auto responseButtonsShown() const {
            return responseButtonsShown_;
        }

        void setGreenResponse() { greenResponse_ = true; }

        void setNumberResponse(std::string s) {
            numberResponse_ = std::move(s);
        }

        auto numberResponse() -> std::string override {
            return numberResponse_;
        }

        auto greenResponse() -> bool override { return greenResponse_; }

        void showResponseButtons() override { responseButtonsShown_ = true; }

        void subscribe(EventListener *e) override { listener_ = e; }

        void hide() override { hidden_ = true; }

        void submitResponse() { listener_->submitResponse(); }

        void playTrial() { listener_->playTrial(); }

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

    class ExperimenterViewStub : public Experimenter {
      public:
        void declineContinuingTesting() {
            listener_->declineContinuingTesting();
        }

        void acceptContinuingTesting() {
            listener_->acceptContinuingTesting();
        }

        [[nodiscard]] auto continueTestingDialogShown() const -> bool {
            return continueTestingDialogShown_;
        }

        [[nodiscard]] auto continueTestingDialogHidden() const -> bool {
            return continueTestingDialogHidden_;
        }

        void showContinueTestingDialog() override {
            continueTestingDialogShown_ = true;
        }

        void hideContinueTestingDialog() override {
            continueTestingDialogHidden_ = true;
        }

        void submitFailedTrial() { listener_->submitFailedTrial(); }

        [[nodiscard]] auto responseSubmissionHidden() const {
            return responseSubmissionHidden_;
        }

        [[nodiscard]] auto evaluationButtonsHidden() const {
            return evaluationButtonsHidden_;
        }

        [[nodiscard]] auto correctKeywordsEntryShown() const {
            return correctKeywordsEntryShown_;
        }

        [[nodiscard]] auto correctKeywordsEntryHidden() const {
            return correctKeywordsEntryHidden_;
        }

        [[nodiscard]] auto evaluationButtonsShown() const {
            return evaluationButtonsShown_;
        }

        [[nodiscard]] auto responseSubmissionShown() const {
            return responseSubmissionShown_;
        }

        auto correctKeywords() -> std::string override {
            return correctKeywords_;
        }

        void showCorrectKeywordsSubmission() override {
            correctKeywordsEntryShown_ = true;
        }

        void hideCorrectKeywordsSubmission() override {
            correctKeywordsEntryHidden_ = true;
        }

        [[nodiscard]] auto shown() const { return shown_; }

        [[nodiscard]] auto hidden() const { return hidden_; }

        void show() override { shown_ = true; }

        void subscribe(EventListener *e) override { listener_ = e; }

        void hide() override { hidden_ = true; }

        void showEvaluationButtons() override {
            evaluationButtonsShown_ = true;
        }

        auto freeResponse() -> std::string override { return response_; }

        void showFreeResponseSubmission() override {
            responseSubmissionShown_ = true;
        }

        void hideFreeResponseSubmission() override {
            responseSubmissionHidden_ = true;
        }

        void hideEvaluationButtons() override {
            evaluationButtonsHidden_ = true;
        }

        void submitPassedTrial() { listener_->submitPassedTrial(); }

        void submitCorrectKeywords() { listener_->submitCorrectKeywords(); }

        void setResponse(std::string s) { response_ = std::move(s); }

        void setCorrectKeywords(std::string s) {
            correctKeywords_ = std::move(s);
        }

        void flagResponse() { flagged_ = true; }

        auto flagged() -> bool override { return flagged_; }

        void submitFreeResponse() { listener_->submitFreeResponse(); }

        void display(std::string s) override { displayed_ = std::move(s); }

        void secondaryDisplay(std::string s) override {
            secondaryDisplayed_ = std::move(s);
        }

        void playTrial() { listener_->playTrial(); }

        [[nodiscard]] auto secondaryDisplayed() const {
            return secondaryDisplayed_;
        }

        [[nodiscard]] auto displayed() const { return displayed_; }

        void showNextTrialButton() override { nextTrialButtonShown_ = true; }

        void hideNextTrialButton() override { nextTrialButtonHidden_ = true; }

        void showExitTestButton() override { exitTestButtonShown_ = true; }

        void hideExitTestButton() override { exitTestButtonHidden_ = true; }

        [[nodiscard]] auto exitTestButtonShown() const {
            return exitTestButtonShown_;
        }

        [[nodiscard]] auto exitTestButtonHidden() const {
            return exitTestButtonHidden_;
        }

        [[nodiscard]] auto nextTrialButtonShown() const {
            return nextTrialButtonShown_;
        }

        [[nodiscard]] auto nextTrialButtonHidden() const {
            return nextTrialButtonHidden_;
        }

        void exitTest() { listener_->exitTest(); }

      private:
        std::string displayed_;
        std::string secondaryDisplayed_;
        std::string response_;
        std::string correctKeywords_{"0"};
        EventListener *listener_{};
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

  private:
    std::vector<std::string> audioDevices_;
    std::string errorMessage_;
    std::string browseForDirectoryResult_;
    std::string browseForOpeningFileResult_;
    std::string audioDevice_;
    bool eventLoopCalled_{};
    bool browseCancelled_{};
};

class TestSettingsInterpreterStub : public TestSettingsInterpreter {
  public:
    explicit TestSettingsInterpreterStub(const Calibration &calibration_ = {})
        : calibration_{calibration_} {}

    auto calibration(const std::string &t) -> Calibration override {
        text_ = t;
        return calibration_;
    }

    [[nodiscard]] auto text() const -> std::string { return text_; }

    [[nodiscard]] auto identity() const -> TestIdentity { return identity_; }

    [[nodiscard]] auto textForMethodQuery() const -> std::string {
        return textForMethodQuery_;
    }

    void initialize(
        Model &m, const std::string &t, const TestIdentity &id) override {
        text_ = t;
        identity_ = id;
        if (initializeAnyTestOnApply_)
            m.initialize(AdaptiveTest{});
    }

    void setMethod(Method m) { method_ = m; }

    auto method(const std::string &t) -> Method override {
        textForMethodQuery_ = t;
        return method_;
    }

    void initializeAnyTestOnApply() { initializeAnyTestOnApply_ = true; }

  private:
    std::string text_;
    std::string textForMethodQuery_;
    TestIdentity identity_;
    const Calibration &calibration_;
    Method method_{};
    bool initializeAnyTestOnApply_{};
};

class TextFileReaderStub : public TextFileReader {
  public:
    [[nodiscard]] auto filePath() const -> std::string { return filePath_; }

    auto read(const std::string &s) -> std::string override {
        filePath_ = s;
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
    ViewStub::TestSetupViewStub *view;

  public:
    explicit PlayingCalibration(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override { view->playCalibration(); }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return m.calibration().fullScaleLevel_dB_SPL;
    }
};

class ConfirmingTestSetup : public virtual UseCase {};

void confirmTestSetup(ViewStub::TestSetupViewStub *view) {
    view->confirmTestSetup();
}

void setMethod(TestSettingsInterpreterStub &interpeter, Method m) {
    interpeter.setMethod(m);
}

class ConfirmingDefaultAdaptiveCoordinateResponseMeasureTest
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingDefaultAdaptiveCoordinateResponseMeasureTest(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(
            interpreter, Method::defaultAdaptiveCoordinateResponseMeasure);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    explicit ConfirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::adaptiveCoordinateResponseMeasureWithDelayedMasker);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptivePassFailTest : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptivePassFailTest(ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::adaptivePassFail);
        confirmTestSetup(view);
    }
};

class ConfirmingAdaptiveCorrectKeywordsTest : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingAdaptiveCorrectKeywordsTest(ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::adaptiveCorrectKeywords);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelFreeResponseWithTargetReplacementTest
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelFreeResponseWithTargetReplacementTest(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(
            interpreter, Method::fixedLevelFreeResponseWithTargetReplacement);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::fixedLevelCoordinateResponseMeasureWithTargetReplacement);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargets
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargets(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
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
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter,
            Method::fixedLevelFreeResponseWithSilentIntervalTargets);
        confirmTestSetup(view);
    }
};

class ConfirmingFixedLevelFreeResponseTestWithAllTargets
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;
    TestSettingsInterpreterStub &interpreter;

  public:
    ConfirmingFixedLevelFreeResponseTestWithAllTargets(
        ViewStub::TestSetupViewStub *view,
        TestSettingsInterpreterStub &interpreter)
        : view{view}, interpreter{interpreter} {}

    void run() override {
        setMethod(interpreter, Method::fixedLevelFreeResponseWithAllTargets);
        confirmTestSetup(view);
    }
};

class TrialSubmission : public virtual UseCase {
  public:
    virtual auto nextTrialButtonShown() -> bool = 0;
    virtual auto responseViewShown() -> bool = 0;
    virtual auto responseViewHidden() -> bool = 0;
};

class RespondingFromSubject : public TrialSubmission {
    ViewStub::SubjectViewStub *view;

  public:
    explicit RespondingFromSubject(ViewStub::SubjectViewStub *view)
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

class SubmittingFreeResponse : public TrialSubmission {
    ViewStub::ExperimenterViewStub &view;

  public:
    explicit SubmittingFreeResponse(ViewStub::ExperimenterViewStub &view)
        : view{view} {}

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
    ViewStub::ExperimenterViewStub *view;

  public:
    explicit ExitingTest(ViewStub::ExperimenterViewStub *view) : view{view} {}

    void run() override { view->exitTest(); }
};

class SubmittingPassedTrial : public TrialSubmission {
    ViewStub::ExperimenterViewStub &view;

  public:
    explicit SubmittingPassedTrial(ViewStub::ExperimenterViewStub &view)
        : view{view} {}

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
    ViewStub::ExperimenterViewStub &view;

  public:
    explicit SubmittingFailedTrial(ViewStub::ExperimenterViewStub &view)
        : view{view} {}

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
    ViewStub::ExperimenterViewStub &view;

  public:
    explicit SubmittingCorrectKeywords(ViewStub::ExperimenterViewStub &view)
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
    explicit DecliningContinuingTesting(ViewStub::ExperimenterViewStub &view)
        : view{view} {}

    void run() override { view.declineContinuingTesting(); }

  private:
    ViewStub::ExperimenterViewStub &view;
};

class AcceptingContinuingTesting : public UseCase {
  public:
    explicit AcceptingContinuingTesting(ViewStub::ExperimenterViewStub &view)
        : view{view} {}

    void run() override { view.acceptContinuingTesting(); }

  private:
    ViewStub::ExperimenterViewStub &view;
};

class PlayingTrial : public virtual UseCase {
  public:
    virtual auto nextTrialButtonHidden() -> bool = 0;
    virtual auto nextTrialButtonShown() -> bool = 0;
};

class PlayingTrialFromSubject : public PlayingTrial {
    ViewStub::SubjectViewStub *view;

  public:
    explicit PlayingTrialFromSubject(ViewStub::SubjectViewStub *view)
        : view{view} {}

    void run() override { view->playTrial(); }

    auto nextTrialButtonHidden() -> bool override {
        return view->nextTrialButtonHidden();
    }

    auto nextTrialButtonShown() -> bool override {
        return view->nextTrialButtonShown();
    }
};

class PlayingTrialFromExperimenter : public PlayingTrial {
    ViewStub::ExperimenterViewStub &view;

  public:
    explicit PlayingTrialFromExperimenter(ViewStub::ExperimenterViewStub &view)
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
    ViewStub::TestSetupViewStub *view;

  public:
    explicit BrowsingForTestSettingsFile(ViewStub::TestSetupViewStub *view)
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
    ViewStub::TestSetupViewStub setupView;
    ViewStub::SubjectViewStub subjectView;
    ViewStub::ExperimenterViewStub experimenterView;
    ViewStub view;
    Presenter::TestSetup testSetup{&setupView};
    Presenter::CoordinateResponseMeasure subject{&subjectView};
    Presenter::Experimenter experimenter{&experimenterView};
    TestSettingsInterpreterStub testSettingsInterpreter;
    TextFileReaderStub textFileReader;

    auto construct() -> Presenter {
        return {model, view, testSetup, subject, experimenter,
            testSettingsInterpreter, textFileReader};
    }
};

class PresenterTests : public ::testing::Test {
  protected:
    ModelStub model;
    ViewStub view;
    ViewStub::TestSetupViewStub setupView;
    ViewStub::SubjectViewStub subjectView;
    ViewStub::ExperimenterViewStub experimenterView;
    Presenter::TestSetup testSetup{&setupView};
    Presenter::Experimenter experimenter{&experimenterView};
    Presenter::CoordinateResponseMeasure subject{&subjectView};
    Calibration interpretedCalibration;
    TestSettingsInterpreterStub testSettingsInterpreter{interpretedCalibration};
    TextFileReaderStub textFileReader;
    Presenter presenter{model, view, testSetup, subject, experimenter,
        testSettingsInterpreter, textFileReader};
    BrowsingForTestSettingsFile browsingForTestSettingsFile{&setupView};
    ConfirmingDefaultAdaptiveCoordinateResponseMeasureTest
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptivePassFailTest confirmingAdaptivePassFailTest{
        &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelFreeResponseWithTargetReplacementTest
        confirmingFixedLevelFreeResponseWithTargetReplacementTest{
            &setupView, testSettingsInterpreter};
    ConfirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest{
            &setupView, testSettingsInterpreter};
    ConfirmingAdaptiveCorrectKeywordsTest confirmingAdaptiveCorrectKeywordsTest{
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
    PlayingCalibration playingCalibration{&setupView};
    PlayingTrialFromSubject playingTrialFromSubject{&subjectView};
    PlayingTrialFromExperimenter playingTrialFromExperimenter{experimenterView};
    RespondingFromSubject respondingFromSubject{&subjectView};
    SubmittingFreeResponse submittingFreeResponse{experimenterView};
    SubmittingPassedTrial submittingPassedTrial{experimenterView};
    SubmittingCorrectKeywords submittingCorrectKeywords{experimenterView};
    SubmittingFailedTrial submittingFailedTrial{experimenterView};
    DecliningContinuingTesting decliningContinuingTesting{experimenterView};
    AcceptingContinuingTesting acceptingContinuingTesting{experimenterView};
    ExitingTest exitingTest{&experimenterView};

    void respondFromSubject() { subjectView.submitResponse(); }

    void respondFromExperimenter() { experimenterView.submitFreeResponse(); }

    void exitTest() { experimenterView.exitTest(); }

    void playCalibration() { setupView.playCalibration(); }

    void assertSetupViewShown() { assertTrue(setupViewShown()); }

    auto setupViewShown() -> bool { return setupView.shown(); }

    void assertSetupViewNotShown() { assertFalse(setupViewShown()); }

    void assertSetupViewHidden() { assertTrue(setupViewHidden()); }

    auto setupViewHidden() -> bool { return setupView.hidden(); }

    void assertSetupViewNotHidden() { assertFalse(setupViewHidden()); }

    void assertExperimenterViewShown() { assertTrue(experimenterViewShown()); }

    void assertTestingViewShown() { assertTrue(testingViewShown()); }

    auto experimenterViewShown() -> bool { return experimenterView.shown(); }

    auto testingViewShown() -> bool { return experimenterView.shown(); }

    void assertExperimenterViewHidden() {
        assertTrue(experimenterViewHidden());
    }

    void assertTestingViewHidden() { assertTrue(testingViewHidden()); }

    auto experimenterViewHidden() -> bool { return experimenterView.hidden(); }

    auto testingViewHidden() -> bool { return experimenterView.hidden(); }

    void assertExperimenterViewNotHidden() {
        assertFalse(experimenterViewHidden());
    }

    void assertTestingViewNotHidden() { assertFalse(testingViewHidden()); }

    void assertSubjectViewShown() { assertTrue(subjectViewShown()); }

    auto subjectViewShown() -> bool { return subjectView.shown(); }

    void assertSubjectViewNotShown() { assertFalse(subjectViewShown()); }

    void assertSubjectViewHidden() { assertTrue(subjectView.hidden()); }

    void assertBrowseResultPassedToEntry(BrowsingEnteredPathUseCase &useCase) {
        setBrowsingResult(useCase, "a");
        run(useCase);
        assertEntryEquals(useCase, "a");
    }

    void setBrowsingResult(BrowsingEnteredPathUseCase &useCase, std::string s) {
        useCase.setResult(view, std::move(s));
    }

    static void assertEntryEquals(
        BrowsingEnteredPathUseCase &useCase, const std::string &s) {
        assertEqual(s, entry(useCase));
    }

    static auto entry(BrowsingEnteredPathUseCase &useCase) -> std::string {
        return useCase.entry();
    }

    void assertCancellingBrowseDoesNotChangePath(
        BrowsingEnteredPathUseCase &useCase) {
        useCase.setEntry("a");
        setBrowsingResult(useCase, "b");
        view.setBrowseCancelled();
        run(useCase);
        assertEntryEquals(useCase, "a");
    }

    void completeTrial() { model.completeTrial(); }

    auto errorMessage() -> std::string { return view.errorMessage(); }

    void assertModelPassedColor(coordinate_response_measure::Color c) {
        assertEqual(c, model.responseParameters().color);
    }

    auto calibration() -> const Calibration & { return model.calibration(); }

    void assertErrorMessageEquals(const std::string &s) {
        assertEqual(s, errorMessage());
    }

    void setAudioDevice(std::string s) { view.setAudioDevice(std::move(s)); }

    void setCalibrationLevel(int s) { interpretedCalibration.level_dB_SPL = s; }

    void setTestComplete() { model.setTestComplete(); }

    void assertAudioDevicePassedToTrial(PlayingTrial &useCase) {
        setAudioDevice("a");
        run(useCase);
        assertEqual("a", model.trialParameters().audioDevice);
    }

    void assertPlaysTrial(UseCase &useCase) {
        run(useCase);
        assertTrue(trialPlayed());
    }

    auto trialPlayed() -> bool { return model.trialPlayed(); }

    static void assertHidesPlayTrialButton(PlayingTrial &useCase) {
        run(useCase);
        assertTrue(useCase.nextTrialButtonHidden());
    }

    void assertHidesExitTestButton(PlayingTrial &useCase) {
        run(useCase);
        assertTrue(exitTestButtonHidden());
    }

    auto exitTestButtonHidden() -> bool {
        return experimenterView.exitTestButtonHidden();
    }

    auto exitTestButtonShown() -> bool {
        return experimenterView.exitTestButtonShown();
    }

    static void assertConfirmTestSetupShowsNextTrialButton(
        ConfirmingTestSetup &confirmingTest, PlayingTrial &playingTrial) {
        run(confirmingTest);
        assertTrue(playingTrial.nextTrialButtonShown());
    }

    void assertCompleteTestShowsSetupView(TrialSubmission &useCase) {
        setTestComplete();
        run(useCase);
        assertSetupViewShown();
    }

    void assertShowsSetupView(UseCase &useCase) {
        run(useCase);
        assertSetupViewShown();
    }

    void assertCompleteTestShowsContinueTestingDialog(
        TrialSubmission &useCase) {
        setTestComplete();
        run(useCase);
        assertTrue(experimenterView.continueTestingDialogShown());
    }

    void assertHidesContinueTestingDialog(UseCase &useCase) {
        run(useCase);
        assertTrue(experimenterView.continueTestingDialogHidden());
    }

    void assertIncompleteTestDoesNotShowSetupView(TrialSubmission &useCase) {
        run(useCase);
        assertSetupViewNotShown();
    }

    void assertCompleteTestHidesExperimenterView(UseCase &useCase) {
        setTestComplete();
        assertHidesExperimenterView(useCase);
    }

    void assertCompleteTestHidesTestingView(TrialSubmission &useCase) {
        setTestComplete();
        assertHidesTestingView(useCase);
    }

    void assertHidesExperimenterView(UseCase &useCase) {
        run(useCase);
        assertExperimenterViewHidden();
    }

    void assertHidesTestingView(UseCase &useCase) {
        run(useCase);
        assertTestingViewHidden();
    }

    void assertCompleteTestDoesNotPlayTrial(UseCase &useCase) {
        setTestComplete();
        run(useCase);
        assertFalse(trialPlayed());
    }

    void assertDoesNotHideExperimenterView(TrialSubmission &useCase) {
        run(useCase);
        assertExperimenterViewNotHidden();
    }

    void assertDoesNotHideTestingView(TrialSubmission &useCase) {
        run(useCase);
        assertTestingViewNotHidden();
    }

    static void assertShowsNextTrialButton(TrialSubmission &useCase) {
        run(useCase);
        assertTrue(useCase.nextTrialButtonShown());
    }

    void assertHidesTestSetupView(UseCase &useCase) {
        run(useCase);
        assertSetupViewHidden();
    }

    void assertDoesNotHideTestSetupView(UseCase &useCase) {
        run(useCase);
        assertSetupViewNotHidden();
    }

    void assertShowsExperimenterView(UseCase &useCase) {
        run(useCase);
        assertExperimenterViewShown();
    }

    void assertShowsTestingView(UseCase &useCase) {
        run(useCase);
        assertTestingViewShown();
    }

    void assertDoesNotShowSubjectView(UseCase &useCase) {
        run(useCase);
        assertSubjectViewNotShown();
    }

    void assertPassesTestSettingsFileToTextFileReader(UseCase &useCase) {
        setupView.setTestSettingsFile("a");
        run(useCase);
        assertEqual("a", textFileReader.filePath());
    }

    void assertPassesTestSettingsTextToTestSettingsInterpreter(
        UseCase &useCase) {
        textFileReader.setRead("a");
        run(useCase);
        assertEqual("a", testSettingsInterpreter.text());
    }

    void assertPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery(
        ConfirmingTestSetup &useCase) {
        textFileReader.setRead("a");
        run(useCase);
        assertEqual("a", testSettingsInterpreter.textForMethodQuery());
    }

    void assertPassesSubjectId(ConfirmingTestSetup &useCase) {
        setupView.setSubjectId("b");
        run(useCase);
        assertEqual("b", testSettingsInterpreter.identity().subjectId);
    }

    void assertPassesTesterId(ConfirmingTestSetup &useCase) {
        setupView.setTesterId("c");
        run(useCase);
        assertEqual("c", testSettingsInterpreter.identity().testerId);
    }

    void assertPassesSession(ConfirmingTestSetup &useCase) {
        setupView.setSession("e");
        run(useCase);
        assertEqual("e", testSettingsInterpreter.identity().session);
    }

    void assertCompleteTrialShowsResponseView(
        ConfirmingTestSetup &useCase, TrialSubmission &trialSubmission) {
        run(useCase);
        completeTrial();
        assertTrue(trialSubmission.responseViewShown());
    }

    void assertShowsTrialNumber(UseCase &useCase) {
        setTrialNumber(1);
        run(useCase);
        assertDisplayedToExperimenter("Trial 1");
    }

    void assertShowsTargetFileName(UseCase &useCase) {
        setTargetFileName("a");
        run(useCase);
        assertSecondaryDisplayedToExperimenter("a");
    }

    void setTrialNumber(int n) { model.setTrialNumber(n); }

    void setTargetFileName(std::string s) {
        model.setTargetFileName(std::move(s));
    }

    void assertDisplayedToExperimenter(const std::string &s) {
        assertEqual(s, experimenterView.displayed());
    }

    void assertSecondaryDisplayedToExperimenter(const std::string &s) {
        assertEqual(s, experimenterView.secondaryDisplayed());
    }

    static void assertResponseViewHidden(TrialSubmission &useCase) {
        run(useCase);
        assertTrue(useCase.responseViewHidden());
    }

    void assertShowsSubjectView(UseCase &useCase) {
        run(useCase);
        assertSubjectViewShown();
    }

    void setCorrectKeywords(std::string s) {
        experimenterView.setCorrectKeywords(std::move(s));
    }

    void assertExitTestAfterCompletingTrialHidesResponseSubmission(
        UseCase &useCase, TrialSubmission &submission) {
        run(useCase);
        completeTrial();
        exitTest();
        assertTrue(submission.responseViewHidden());
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

    void initializeWithTargetReplacement(const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithSilentIntervalTargets(const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithAllTargets(const FixedLevelTest &) override {
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

    void playTrial(const AudioSettings &) override {
        throw RequestFailure{errorMessage};
    }

    void submit(const coordinate_response_measure::Response &) override {
        throw RequestFailure{errorMessage};
    }

    void submit(const open_set::FreeResponse &) override {
        throw RequestFailure{errorMessage};
    }

    void submit(const open_set::CorrectKeywords &) override {
        throw RequestFailure{errorMessage};
    }

    void playCalibration(const Calibration &) override {
        throw RequestFailure{errorMessage};
    }

    auto testComplete() -> bool override { return {}; }
    auto audioDevices() -> std::vector<std::string> override { return {}; }
    void subscribe(EventListener *) override {}
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
};

class PresenterFailureTests : public ::testing::Test {
  protected:
    RequestFailingModel failure;
    ModelStub defaultModel;
    Model *model{&defaultModel};
    ViewStub view;
    ViewStub::TestSetupViewStub setupView;
    ViewStub::SubjectViewStub subjectView;
    ViewStub::ExperimenterViewStub experimenterView;
    Presenter::TestSetup testSetup{&setupView};
    Presenter::CoordinateResponseMeasure subject{&subjectView};
    Presenter::Experimenter experimenter{&experimenterView};
    TestSettingsInterpreterStub testSettingsInterpreter;
    TextFileReaderStub textFileReader;

    void useFailingModel(std::string s = {}) {
        failure.setErrorMessage(std::move(s));
        model = &failure;
        testSettingsInterpreter.initializeAnyTestOnApply();
    }

    void confirmTestSetup() {
        Presenter presenter{*model, view, testSetup, subject, experimenter,
            testSettingsInterpreter, textFileReader};
        setupView.confirmTestSetup();
    }

    void assertConfirmTestSetupShowsErrorMessage(const std::string &s) {
        confirmTestSetup();
        assertEqual(s, view.errorMessage());
    }

    void assertConfirmTestSetupDoesNotHideSetupView() {
        confirmTestSetup();
        assertFalse(setupView.hidden());
    }
};
TEST_F(PresenterConstructionTests, populatesAudioDeviceMenu) {
    model.setAudioDevices({"a", "b", "c"});
    construct();
    assertEqual({"a", "b", "c"}, view.audioDevices());
}

#define PRESENTER_TEST(a) TEST_F(PresenterTests, a)

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestShowsTestingView) {
    assertShowsTestingView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCorrectKeywordsTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptiveCorrectKeywordsTest, playingTrialFromExperimenter);
}

PRESENTER_TEST(submittingCorrectKeywordsPassesCorrectKeywords) {
    setCorrectKeywords("1");
    run(submittingCorrectKeywords);
    assertEqual(1, model.correctKeywords());
}

PRESENTER_TEST(submittingInvalidCorrectKeywordsShowsErrorMessage) {
    setCorrectKeywords("a");
    run(submittingCorrectKeywords);
    assertErrorMessageEquals("'a' is not a valid number.");
}

PRESENTER_TEST(submittingInvalidCorrectKeywordsDoesNotHideEntry) {
    setCorrectKeywords("a");
    run(submittingCorrectKeywords);
    assertFalse(submittingCorrectKeywords.responseViewHidden());
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

PRESENTER_TEST(submittingCorrectKeywordsShowsContinueTestingDialog) {
    assertCompleteTestShowsContinueTestingDialog(submittingCorrectKeywords);
}

PRESENTER_TEST(
    submittingCorrectKeywordsDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(submittingCorrectKeywords);
}

PRESENTER_TEST(decliningContinuingTestingHidesExperimenterView) {
    assertHidesExperimenterView(decliningContinuingTesting);
}

PRESENTER_TEST(decliningContinuingTestingHidesTestingView) {
    assertHidesTestingView(decliningContinuingTesting);
}

PRESENTER_TEST(
    submittingCorrectKeywordsDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(submittingCorrectKeywords);
}

PRESENTER_TEST(
    submittingCorrectKeywordsDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingCorrectKeywordsShowsNextTrialButton) {
    assertShowsNextTrialButton(submittingCorrectKeywords);
}

PRESENTER_TEST(submittingCorrectKeywordsHidesCorrectKeywordsEntry) {
    assertResponseViewHidden(submittingCorrectKeywords);
}

PRESENTER_TEST(confirmingAdaptiveCorrectKeywordsTestShowsTrialNumber) {
    assertShowsTrialNumber(confirmingAdaptiveCorrectKeywordsTest);
}

PRESENTER_TEST(submittingCorrectKeywordsShowsTrialNumber) {
    assertShowsTrialNumber(submittingCorrectKeywords);
}

PRESENTER_TEST(acceptingContinuingTestingDialogShowsTrialNumber) {
    assertShowsTrialNumber(acceptingContinuingTesting);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterCorrectKeywordsEntryForAdaptiveCorrectKeywordsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCorrectKeywordsTest, submittingCorrectKeywords);
}

PRESENTER_TEST(callsEventLoopWhenRun) {
    presenter.run();
    assertTrue(view.eventLoopCalled());
}

PRESENTER_TEST(
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
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

PRESENTER_TEST(confirmingAdaptivePassFailTestHidesTestSetupView) {
    assertHidesTestSetupView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsHidesTestSetupView) {
    assertHidesTestSetupView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestShowsExperimenterView) {
    assertShowsExperimenterView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestShowsTestingView) {
    assertShowsTestingView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementShowsTestingView) {
    assertShowsTestingView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestShowsSubjectView) {
    assertShowsSubjectView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeakerShowsSubjectView) {
    assertShowsSubjectView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMaskerShowsSubjectView) {
    assertShowsSubjectView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker);
}

PRESENTER_TEST(
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestDoesNotShowSubjectViewWhenTestComplete) {
    setTestComplete();
    assertDoesNotShowSubjectView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestDoesNotHideSetupViewWhenTestComplete) {
    setTestComplete();
    assertDoesNotHideTestSetupView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsSubjectView) {
    assertShowsSubjectView(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsSubjectView) {
    assertShowsSubjectView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithAllTargetsDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelFreeResponseTestWithAllTargets);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithSilentIntervalTargetsDoesNotShowSubjectView) {
    assertDoesNotShowSubjectView(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest);
}

PRESENTER_TEST(
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsExperimenterView) {
    assertShowsExperimenterView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(playCalibrationPassesLevel) {
    setCalibrationLevel(1);
    playCalibration();
    assertEqual(1, calibration().level_dB_SPL);
}

PRESENTER_TEST(playingCalibrationPassesTestSettingsFileToTextFileReader) {
    assertPassesTestSettingsFileToTextFileReader(playingCalibration);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsFileToTextFileReader) {
    assertPassesTestSettingsFileToTextFileReader(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery) {
    assertPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsTextToTestSettingsInterpreter) {
    assertPassesTestSettingsTextToTestSettingsInterpreter(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(
    playingCalibrationPassesTestSettingsTextToTestSettingsInterpreter) {
    assertPassesTestSettingsTextToTestSettingsInterpreter(playingCalibration);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesSubjectId) {
    assertPassesSubjectId(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(confirmingAdaptivePassFailTestPassesSubjectId) {
    assertPassesSubjectId(confirmingAdaptivePassFailTest);
}

PRESENTER_TEST(
    confirmingFixedLevelFreeResponseTestWithTargetReplacementPassesSubjectId) {
    assertPassesSubjectId(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesSubjectId) {
    assertPassesSubjectId(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesTesterId) {
    assertPassesTesterId(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
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
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(playCalibrationPassesFilePath) {
    interpretedCalibration.filePath = "a";
    playCalibration();
    assertEqual("a", calibration().filePath);
}

PRESENTER_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesSession) {
    assertPassesSession(confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
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
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(exitTestAfterCompletingTrialHidesCorrectKeywordsSubmission) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingAdaptiveCorrectKeywordsTest, submittingCorrectKeywords);
}

PRESENTER_TEST(exitTestAfterCompletingTrialHidesFreeResponseSubmission) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingFixedLevelFreeResponseTestWithAllTargets,
        submittingFreeResponse);
}

PRESENTER_TEST(exitTestAfterCompletingTrialHidesPassFailSubmission) {
    assertExitTestAfterCompletingTrialHidesResponseSubmission(
        confirmingAdaptivePassFailTest, submittingPassedTrial);
}

PRESENTER_TEST(
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest,
        playingTrialFromSubject);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest,
        playingTrialFromSubject);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsNextTrialButtonForSubject) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest,
        playingTrialFromSubject);
}

PRESENTER_TEST(
    confirmingAdaptivePassFailTestShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingAdaptivePassFailTest, playingTrialFromExperimenter);
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
    confirmingFixedLevelFreeResponseTestWithSilentIntervalTargetsShowsNextTrialButtonForExperimenter) {
    assertConfirmTestSetupShowsNextTrialButton(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest,
        playingTrialFromExperimenter);
}

PRESENTER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestDoesNotShowExperimentersNextTrialButton) {
    run(confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
    assertFalse(experimenterView.nextTrialButtonShown());
}

PRESENTER_TEST(respondingFromSubjectPlaysTrial) {
    assertPlaysTrial(respondingFromSubject);
}

PRESENTER_TEST(playingTrialFromSubjectPlaysTrial) {
    assertPlaysTrial(playingTrialFromSubject);
}

PRESENTER_TEST(playingTrialFromExperimenterPlaysTrial) {
    assertPlaysTrial(playingTrialFromExperimenter);
}

PRESENTER_TEST(playingTrialHidesNextTrialButton) {
    assertHidesPlayTrialButton(playingTrialFromSubject);
}

PRESENTER_TEST(playingTrialHidesNextTrialButtonForExperimenter) {
    assertHidesPlayTrialButton(playingTrialFromExperimenter);
}

PRESENTER_TEST(playingTrialFromSubjectHidesExitTestButton) {
    assertHidesExitTestButton(playingTrialFromSubject);
}

PRESENTER_TEST(playingTrialFromSubjectPassesAudioDevice) {
    assertAudioDevicePassedToTrial(playingTrialFromSubject);
}

PRESENTER_TEST(playingTrialFromExperimenterPassesAudioDevice) {
    assertAudioDevicePassedToTrial(playingTrialFromExperimenter);
}

PRESENTER_TEST(playCalibrationPassesAudioDevice) {
    setAudioDevice("b");
    playCalibration();
    assertEqual("b", calibration().audioSettings.audioDevice);
}

PRESENTER_TEST(subjectResponsePassesNumberResponse) {
    subjectView.setNumberResponse("1");
    respondFromSubject();
    assertEqual(1, model.responseParameters().number);
}

PRESENTER_TEST(subjectResponsePassesGreenColor) {
    subjectView.setGreenResponse();
    respondFromSubject();
    assertModelPassedColor(coordinate_response_measure::Color::green);
}

PRESENTER_TEST(subjectResponsePassesRedColor) {
    subjectView.setRedResponse();
    respondFromSubject();
    assertModelPassedColor(coordinate_response_measure::Color::red);
}

PRESENTER_TEST(subjectResponsePassesBlueColor) {
    subjectView.setBlueResponse();
    respondFromSubject();
    assertModelPassedColor(coordinate_response_measure::Color::blue);
}

PRESENTER_TEST(subjectResponsePassesWhiteColor) {
    subjectView.setGrayResponse();
    respondFromSubject();
    assertModelPassedColor(coordinate_response_measure::Color::white);
}

PRESENTER_TEST(experimenterResponsePassesResponse) {
    experimenterView.setResponse("a");
    respondFromExperimenter();
    assertEqual("a", model.freeResponse().response);
}

PRESENTER_TEST(experimenterResponseFlagsResponse) {
    experimenterView.flagResponse();
    respondFromExperimenter();
    assertTrue(model.freeResponse().flagged);
}

PRESENTER_TEST(passedTrialSubmitsCorrectResponse) {
    run(submittingPassedTrial);
    assertTrue(model.correctResponseSubmitted());
}

PRESENTER_TEST(failedTrialSubmitsIncorrectResponse) {
    run(submittingFailedTrial);
    assertTrue(model.incorrectResponseSubmitted());
}

PRESENTER_TEST(respondFromSubjectShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(respondingFromSubject);
}

PRESENTER_TEST(respondFromExperimenterShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(submittingFreeResponse);
}

PRESENTER_TEST(submitPassedTrialShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialShowsSetupViewWhenTestComplete) {
    assertCompleteTestShowsSetupView(submittingFailedTrial);
}

PRESENTER_TEST(respondFromSubjectDoesNotShowSetupViewWhenTestIncomplete) {
    assertIncompleteTestDoesNotShowSetupView(respondingFromSubject);
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

PRESENTER_TEST(respondFromSubjectHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(respondingFromSubject);
}

PRESENTER_TEST(respondFromExperimenterHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(submittingFreeResponse);
}

PRESENTER_TEST(submitPassedTrialHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialHidesExperimenterViewWhenTestComplete) {
    assertCompleteTestHidesExperimenterView(submittingFailedTrial);
}

PRESENTER_TEST(respondFromExperimenterHidesTestingViewWhenTestComplete) {
    assertCompleteTestHidesTestingView(submittingFreeResponse);
}

PRESENTER_TEST(submitPassedTrialHidesTestingViewWhenTestComplete) {
    assertCompleteTestHidesTestingView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialHidesTestingViewWhenTestComplete) {
    assertCompleteTestHidesTestingView(submittingFailedTrial);
}

PRESENTER_TEST(submitCoordinateResponseDoesNotPlayTrialWhenTestComplete) {
    assertCompleteTestDoesNotPlayTrial(respondingFromSubject);
}

PRESENTER_TEST(
    respondFromSubjectDoesNotHideExperimenterViewWhenTestIncomplete) {
    assertDoesNotHideExperimenterView(respondingFromSubject);
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

PRESENTER_TEST(
    respondFromExperimenterDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(submittingFreeResponse);
}

PRESENTER_TEST(submitPassedTrialDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(submittingPassedTrial);
}

PRESENTER_TEST(submitFailedTrialDoesNotHideTestingViewWhenTestIncomplete) {
    assertDoesNotHideTestingView(submittingFailedTrial);
}

PRESENTER_TEST(experimenterResponseShowsNextTrialButton) {
    assertShowsNextTrialButton(submittingFreeResponse);
}

PRESENTER_TEST(subjectPassedTrialShowsNextTrialButtonForExperimenter) {
    assertShowsNextTrialButton(submittingPassedTrial);
}

PRESENTER_TEST(subjectFailedTrialShowsNextTrialButtonForExperimenter) {
    assertShowsNextTrialButton(submittingFailedTrial);
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

PRESENTER_TEST(subjectResponseHidesResponseButtons) {
    assertResponseViewHidden(respondingFromSubject);
}

PRESENTER_TEST(subjectResponseHidesSubjectViewWhenTestComplete) {
    setTestComplete();
    respondFromSubject();
    assertSubjectViewHidden();
}

PRESENTER_TEST(exitTestHidesSubjectView) {
    exitTest();
    assertSubjectViewHidden();
}

PRESENTER_TEST(exitTestHidesExperimenterView) {
    assertHidesExperimenterView(exitingTest);
}

PRESENTER_TEST(exitTestHidesTestingView) {
    assertHidesTestingView(exitingTest);
}

PRESENTER_TEST(exitTestHidesResponseButtons) {
    run(exitingTest);
    assertTrue(respondingFromSubject.responseViewHidden());
}

PRESENTER_TEST(exitTestShowsTestSetupView) {
    exitTest();
    assertSetupViewShown();
}

PRESENTER_TEST(browseForTestSettingsFileUpdatesTestSettingsFile) {
    assertBrowseResultPassedToEntry(browsingForTestSettingsFile);
}

PRESENTER_TEST(browseForTestSettingsCancelDoesNotChangeTestSettingsFile) {
    assertCancellingBrowseDoesNotChangePath(browsingForTestSettingsFile);
}

PRESENTER_TEST(completingTrialShowsExitTestButton) {
    completeTrial();
    assertTrue(exitTestButtonShown());
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
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest);
}

PRESENTER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithSilentIntervalTargetsShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest);
}

PRESENTER_TEST(
    confirmingDefaultAdaptiveCoordinateResponseMeasureTestShowsTrialNumber) {
    assertShowsTrialNumber(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

PRESENTER_TEST(submittingResponseFromExperimenterShowsTrialNumber) {
    assertShowsTrialNumber(submittingFreeResponse);
}

PRESENTER_TEST(submittingResponseFromSubjectShowsTrialNumber) {
    assertShowsTrialNumber(respondingFromSubject);
}

PRESENTER_TEST(submittingPassedTrialShowsTrialNumber) {
    assertShowsTrialNumber(submittingPassedTrial);
}

PRESENTER_TEST(submittingFailedTrialShowsTrialNumber) {
    assertShowsTrialNumber(submittingFailedTrial);
}

TEST_F(
    PresenterTests, confirmingAdaptiveCorrectKeywordsTestShowsTargetFileName) {
    assertShowsTargetFileName(confirmingAdaptiveCorrectKeywordsTest);
}

TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestShowsTargetFileName) {
    assertShowsTargetFileName(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest);
}

TEST_F(PresenterTests, submittingCorrectKeywordsShowsTargetFileName) {
    assertShowsTargetFileName(submittingCorrectKeywords);
}

TEST_F(PresenterTests, submittingCoordinateResponseShowsTargetFileName) {
    assertShowsTargetFileName(respondingFromSubject);
}

TEST_F(PresenterTests, submittingFreeResponseShowsTargetFileName) {
    assertShowsTargetFileName(submittingFreeResponse);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveCoordinateResponseMeasureTest) {
    assertCompleteTrialShowsResponseView(
        confirmingDefaultAdaptiveCoordinateResponseMeasureTest,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveCoordinateResponseMeasureSingleSpeakerTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithSingleSpeaker,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForAdaptiveCoordinateResponseMeasureTestWithDelayedMasker) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptiveCoordinateResponseMeasureTestWithDelayedMasker,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForFixedLevelCoordinateResponseMeasureWithTargetReplacementTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelCoordinateResponseMeasureWithTargetReplacementTest,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsSubjectResponseButtonsForFixedLevelCoordinateResponseMeasureSilentIntervalsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelCoordinateResponseMeasureSilentIntervalsTest,
        respondingFromSubject);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterEvaluationButtonsForAdaptivePassFailTest) {
    assertCompleteTrialShowsResponseView(
        confirmingAdaptivePassFailTest, submittingPassedTrial);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseWithTargetReplacementTest,
        submittingFreeResponse);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseAllStimuliTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseTestWithAllTargets,
        submittingFreeResponse);
}

PRESENTER_TEST(
    completingTrialShowsExperimenterResponseSubmissionForFixedLevelFreeResponseSilentIntervalsTest) {
    assertCompleteTrialShowsResponseView(
        confirmingFixedLevelFreeResponseWithSilentIntervalTargetsTest,
        submittingFreeResponse);
}

PRESENTER_TEST(playCalibrationPassesFullScaleLevel) {
    interpretedCalibration.fullScaleLevel_dB_SPL = 1;
    run(playingCalibration);
    assertEqual(1, calibration().fullScaleLevel_dB_SPL);
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