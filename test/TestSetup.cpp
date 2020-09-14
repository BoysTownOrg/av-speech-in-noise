#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include <presentation/Consonant.hpp>
#include <presentation/CoordinateResponseMeasure.hpp>
#include <presentation/FreeResponse.hpp>
#include <presentation/PassFail.hpp>
#include <presentation/CorrectKeywords.hpp>
#include <presentation/TestSetupImpl.hpp>
#include <presentation/ExperimenterImpl.hpp>
#include <presentation/SessionControllerImpl.hpp>
#include <av-speech-in-noise/name.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <utility>

namespace av_speech_in_noise {
namespace {
class TestSetupViewStub : public TestSetupView, public TestSetupControl {
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

    void populateTransducerMenu(std::vector<std::string> v) override {
        transducers_ = std::move(v);
    }

    void confirmTestSetup() {
        listener_->notifyThatConfirmButtonHasBeenClicked();
    }

    void playCalibration() {
        listener_->notifyThatPlayCalibrationButtonHasBeenClicked();
    }

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

    void attach(Observer *listener) override { listener_ = listener; }

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
    Observer *listener_{};
    bool shown_{};
    bool hidden_{};
};

class ViewStub : public SessionView {
  public:
    void setAudioDevice(std::string s) { audioDevice_ = std::move(s); }

    auto audioDevice() -> std::string override { return audioDevice_; }

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

    void setBrowseForOpeningFileResult(std::string s) {
        browseForOpeningFileResult_ = std::move(s);
    }

    void setBrowseCancelled() { browseCancelled_ = true; }

    void populateAudioDeviceMenu(std::vector<std::string> v) override {
        audioDevices_ = std::move(v);
    }

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

class PlayingTrial : public virtual UseCase {
  public:
    virtual auto nextTrialButtonHidden() -> bool = 0;
    virtual auto nextTrialButtonShown() -> bool = 0;
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

auto entry(BrowsingEnteredPathUseCase &useCase) -> std::string {
    return useCase.entry();
}

void assertEntryEquals(
    BrowsingEnteredPathUseCase &useCase, const std::string &s) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, entry(useCase));
}

void playCalibration(TestSetupViewStub &view) { view.playCalibration(); }

auto shown(TestSetupViewStub &view) -> bool { return view.shown(); }

void assertShown(TestSetupViewStub &view) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(shown(view));
}

auto hidden(TestSetupViewStub &view) -> bool { return view.hidden(); }

void completeTrial(ModelStub &model) { model.completeTrial(); }

auto errorMessage(ViewStub &view) -> std::string { return view.errorMessage(); }

auto calibration(ModelStub &model) -> const Calibration & {
    return model.calibration();
}

void setAudioDevice(ViewStub &view, std::string s) {
    view.setAudioDevice(std::move(s));
}

void setTestComplete(ModelStub &model) { model.setTestComplete(); }

auto trialPlayed(ModelStub &model) -> bool { return model.trialPlayed(); }

class SessionControllerStub : public SessionController {
  public:
    void notifyThatTestIsComplete() override {}
    void prepare(Method m) override {
        method_ = m;
        prepareCalled_ = true;
    }
    auto method() -> Method { return method_; }
    [[nodiscard]] auto prepareCalled() const -> bool { return prepareCalled_; }

  private:
    Method method_{};
    bool prepareCalled_{};
};

class TestSetupControllerObserverStub : public TestSetupController::Observer {
  public:
    void notifyThatUserHasSelectedTestSettingsFile(
        const std::string &s) override {
        testSettingsFile_ = s;
    }
    auto testSettingsFile() -> std::string { return testSettingsFile_; }

  private:
    std::string testSettingsFile_;
};

class TestSetupTests : public ::testing::Test {
  protected:
    ModelStub model;
    ViewStub view;
    TestSetupViewStub setupView;
    Calibration interpretedCalibration;
    TestSettingsInterpreterStub testSettingsInterpreter{interpretedCalibration};
    TextFileReaderStub textFileReader;
    TestSetupControllerImpl testSetupControllerImpl{
        model, view, setupView, testSettingsInterpreter, textFileReader};
    TestSetupPresenterImpl testSetupPresenterRefactored{setupView};
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
    SessionControllerStub sessionController;
    TestSetupControllerObserverStub testSetupControllerObserver;

    TestSetupTests() {
        testSetupControllerImpl.attach(&sessionController);
        testSetupControllerImpl.attach(&testSetupControllerObserver);
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

    void assertDoesNotHideTestSetupView(UseCase &useCase) {
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(hidden(setupView));
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
    void attach(Observer *) override {}
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void restartAdaptiveTestWhilePreservingTargets() override {}
};

class TestSetupFailureTests : public ::testing::Test {
  protected:
    RequestFailingModel failure;
    ModelStub defaultModel;
    Model *model{&defaultModel};
    ViewStub view;
    TestSetupViewStub setupView;
    TestSettingsInterpreterStub testSettingsInterpreter;
    TestSetupPresenterImpl testSetupPresenterRefactored{setupView};
    TextFileReaderStub textFileReader;

    void useFailingModel(std::string s = {}) {
        failure.setErrorMessage(std::move(s));
        model = &failure;
        testSettingsInterpreter.initializeAnyTestOnApply();
    }

    void confirmTestSetup() {
        TestSetupControllerImpl testSetupControllerImpl{
            *model, view, setupView, testSettingsInterpreter, textFileReader};
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

#define TEST_SETUP_TEST(a) TEST_F(TestSetupTests, a)

TEST_SETUP_TEST(controllerPreparesTestAfterConfirmButtonIsClicked) {
    testSettingsInterpreter.setMethod(Method::adaptivePassFail);
    confirmTestSetup(&setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Method::adaptivePassFail, sessionController.method());
}

TEST_SETUP_TEST(
    controllerDoesNotPrepareTestAfterConfirmButtonIsClickedWhenTestWouldAlreadyBeComplete) {
    setTestComplete(model);
    confirmTestSetup(&setupView);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(sessionController.prepareCalled());
}

TEST_SETUP_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesSubjectId) {
    setupView.setSubjectId("b");
    confirmTestSetup(&setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, testSettingsInterpreter.identity().subjectId);
}

TEST_SETUP_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesStartingSnr) {
    setupView.setStartingSnr("1");
    confirmTestSetup(&setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, testSettingsInterpreter.startingSnr());
}

TEST_SETUP_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithInvalidStartingSnrShowsMessage) {
    setupView.setStartingSnr("a");
    confirmTestSetup(&setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"\"a\" is not a valid starting SNR."}, errorMessage(view));
}

TEST_SETUP_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesTesterId) {
    setupView.setTesterId("c");
    confirmTestSetup(&setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c"}, testSettingsInterpreter.identity().testerId);
}

TEST_SETUP_TEST(confirmingAdaptiveCoordinateResponseMeasureTestPassesSession) {
    setupView.setSession("e");
    confirmTestSetup(&setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"e"}, testSettingsInterpreter.identity().session);
}

TEST_SETUP_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesRmeSetting) {
    setupView.setRmeSetting("e");
    confirmTestSetup(&setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"e"}, testSettingsInterpreter.identity().rmeSetting);
}

TEST_SETUP_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesTransducer) {
    setupView.setTransducer("a");
    confirmTestSetup(&setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, testSettingsInterpreter.identity().transducer);
}

TEST_SETUP_TEST(playCalibrationPassesLevel) {
    interpretedCalibration.level.dB_SPL = 1;
    playCalibration(setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, calibration(model).level.dB_SPL);
}

TEST_SETUP_TEST(playingCalibrationPassesTestSettingsFileToTextFileReader) {
    assertPassesTestSettingsFileToTextFileReader(playingCalibration);
}

TEST_SETUP_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsFileToTextFileReader) {
    assertPassesTestSettingsFileToTextFileReader(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

TEST_SETUP_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery) {
    assertPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

TEST_SETUP_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsTextToTestSettingsInterpreter) {
    assertPassesTestSettingsTextToTestSettingsInterpreter(
        confirmingAdaptiveCoordinateResponseMeasureTest);
}

TEST_SETUP_TEST(
    playingCalibrationPassesTestSettingsTextToTestSettingsInterpreter) {
    assertPassesTestSettingsTextToTestSettingsInterpreter(playingCalibration);
}

TEST_SETUP_TEST(playCalibrationPassesFilePath) {
    interpretedCalibration.fileUrl.path = "a";
    playCalibration(setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, calibration(model).fileUrl.path);
}

TEST_SETUP_TEST(playCalibrationPassesAudioDevice) {
    setAudioDevice(view, "b");
    playCalibration(setupView);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, calibration(model).audioDevice);
}

TEST_SETUP_TEST(browseForTestSettingsFileUpdatesTestSettingsFile) {
    browsingForTestSettingsFile.setResult(view, "a");
    run(browsingForTestSettingsFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "a", testSetupControllerObserver.testSettingsFile());
}

TEST_SETUP_TEST(browseForTestSettingsCancelDoesNotChangeTestSettingsFile) {
    assertCancellingBrowseDoesNotChangePath(browsingForTestSettingsFile);
}

TEST_SETUP_TEST(playCalibrationPassesFullScaleLevel) {
    interpretedCalibration.fullScaleLevel.dB_SPL = 1;
    run(playingCalibration);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        1, calibration(model).fullScaleLevel.dB_SPL);
}

TEST_F(TestSetupFailureTests,
    initializeTestShowsErrorMessageWhenModelFailsRequest) {
    useFailingModel("a");
    assertConfirmTestSetupShowsErrorMessage("a");
}

TEST_F(TestSetupFailureTests,
    initializeTestDoesNotHideSetupViewWhenModelFailsRequest) {
    useFailingModel();
    assertConfirmTestSetupDoesNotHideSetupView();
}
}
}
