#include "assert-utility.hpp"
#include "ModelStub.hpp"
#include "SessionViewStub.hpp"
#include <presentation/TestSetupImpl.hpp>
#include <av-speech-in-noise/name.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <utility>

namespace av_speech_in_noise {
namespace {
class TestSetupViewStub : public TestSetupView {
  public:
    void setTestSettingsFile(std::string s) override {
        testSettingsFile_ = std::move(s);
    }

    void populateTransducerMenu(std::vector<std::string> v) override {
        transducers_ = std::move(v);
    }

    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const { return hidden_; }

    auto transducers() -> std::vector<std::string> { return transducers_; }

    auto testSettingsFile() -> std::string { return testSettingsFile_; }

  private:
    std::vector<std::string> transducers_;
    std::string testSettingsFile_;
    bool shown_{};
    bool hidden_{};
};

class TestSetupControlStub : public TestSetupControl {
  public:
    auto testSettingsFile() -> std::string override {
        return testSettingsFile_;
    }

    auto startingSnr() -> std::string override { return startingSnr_; }

    void browseForTestSettingsFile() {
        listener_->notifyThatBrowseForTestSettingsButtonHasBeenClicked();
    }

    void confirmTestSetup() {
        listener_->notifyThatConfirmButtonHasBeenClicked();
    }

    void playCalibration() {
        listener_->notifyThatPlayCalibrationButtonHasBeenClicked();
    }

    void playLeftSpeakerCalibration() {
        listener_->notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked();
    }

    void playRightSpeakerCalibration() {
        listener_->notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked();
    }

    auto session() -> std::string override { return session_; }

    void setSession(std::string s) { session_ = std::move(s); }

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

    void setTestSettingsFile(std::string s) {
        testSettingsFile_ = std::move(s);
    }

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

class LevelUseCase : public virtual UseCase {
  public:
    virtual auto calibration(ModelStub &) -> Calibration = 0;
};

class PlayingCalibration : public LevelUseCase {
  public:
    explicit PlayingCalibration(TestSetupControlStub &control)
        : control{control} {}

    void run() override { control.playCalibration(); }

    auto calibration(ModelStub &m) -> Calibration override {
        return m.calibration();
    }

  private:
    TestSetupControlStub &control;
};

class PlayingLeftSpeakerCalibration : public LevelUseCase {
  public:
    explicit PlayingLeftSpeakerCalibration(TestSetupControlStub &control)
        : control{control} {}

    void run() override { control.playLeftSpeakerCalibration(); }

    auto calibration(ModelStub &m) -> Calibration override {
        return m.leftSpeakerCalibration();
    }

  private:
    TestSetupControlStub &control;
};

class PlayingRightSpeakerCalibration : public LevelUseCase {
  public:
    explicit PlayingRightSpeakerCalibration(TestSetupControlStub &control)
        : control{control} {}

    void run() override { control.playRightSpeakerCalibration(); }

    auto calibration(ModelStub &m) -> Calibration override {
        return m.rightSpeakerCalibration();
    }

  private:
    TestSetupControlStub &control;
};

void confirmTestSetup(TestSetupControlStub &control) {
    control.confirmTestSetup();
}

class ConfirmingTestSetupImpl : public UseCase {
  public:
    explicit ConfirmingTestSetupImpl(TestSetupControlStub &control)
        : control{control} {}

    void run() override { confirmTestSetup(control); }

  private:
    TestSetupControlStub &control;
};

auto errorMessage(SessionViewStub &view) -> std::string {
    return view.errorMessage();
}

auto calibration(ModelStub &model) -> const Calibration & {
    return model.calibration();
}

void setAudioDevice(SessionViewStub &view, std::string s) {
    view.setAudioDevice(std::move(s));
}

void setTestComplete(ModelStub &model) { model.setTestComplete(); }

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
        notifiedThatUserHasSelectedTestSettingsFile_ = true;
    }

    auto testSettingsFile() -> std::string { return testSettingsFile_; }

    [[nodiscard]] auto notifiedThatUserHasSelectedTestSettingsFile() const
        -> bool {
        return notifiedThatUserHasSelectedTestSettingsFile_;
    }

  private:
    std::string testSettingsFile_;
    bool notifiedThatUserHasSelectedTestSettingsFile_{};
};

class TestSetupControllerTests : public ::testing::Test {
  protected:
    ModelStub model;
    SessionViewStub sessionView;
    TestSetupControlStub control;
    Calibration calibration;
    TestSettingsInterpreterStub testSettingsInterpreter{calibration};
    TextFileReaderStub textFileReader;
    TestSetupControllerImpl controller{
        model, sessionView, control, testSettingsInterpreter, textFileReader};
    PlayingCalibration playingCalibration{control};
    PlayingLeftSpeakerCalibration playingLeftSpeakerCalibration{control};
    PlayingRightSpeakerCalibration playingRightSpeakerCalibration{control};
    SessionControllerStub sessionController;
    TestSetupControllerObserverStub testSetupControllerObserver;
    ConfirmingTestSetupImpl confirmingTestSetup{control};

    TestSetupControllerTests() {
        controller.attach(&sessionController);
        controller.attach(&testSetupControllerObserver);
    }

    void assertPassesTestSettingsFileToTextFileReader(UseCase &useCase) {
        control.setTestSettingsFile("a");
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

    void assertPassesCalibrationLevel(LevelUseCase &useCase) {
        calibration.level.dB_SPL = 1;
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            1, useCase.calibration(model).level.dB_SPL);
    }

    void assertPassesCalibrationAudioUrl(LevelUseCase &useCase) {
        calibration.fileUrl.path = "a";
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"a"}, useCase.calibration(model).fileUrl.path);
    }
};

class TestSetupPresenterTests : public ::testing::Test {
  protected:
    TestSetupViewStub view;
    TestSetupPresenterImpl presenter{view};
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

    void playLeftSpeakerCalibration(const Calibration &) override {
        throw RequestFailure{errorMessage};
    }

    void playRightSpeakerCalibration(const Calibration &) override {
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
    RequestFailingModel failingModel;
    ModelStub defaultModel;
    Model *model{&defaultModel};
    SessionViewStub sessionView;
    TestSetupViewStub view;
    TestSetupControlStub control;
    TestSettingsInterpreterStub testSettingsInterpreter;
    TestSetupPresenterImpl testSetupPresenterRefactored{view};
    TextFileReaderStub textFileReader;

    void useFailingModel(std::string s = {}) {
        failingModel.setErrorMessage(std::move(s));
        model = &failingModel;
        testSettingsInterpreter.initializeAnyTestOnApply();
    }

    void confirmTestSetup() {
        TestSetupControllerImpl controller{*model, sessionView, control,
            testSettingsInterpreter, textFileReader};
        control.confirmTestSetup();
    }

    void assertConfirmTestSetupShowsErrorMessage(const std::string &s) {
        confirmTestSetup();
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, errorMessage(sessionView));
    }

    void assertConfirmTestSetupDoesNotHideSetupView() {
        confirmTestSetup();
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(view.hidden());
    }
};

#define TEST_SETUP_CONTROLLER_TEST(a) TEST_F(TestSetupControllerTests, a)

#define TEST_SETUP_PRESENTER_TEST(a) TEST_F(TestSetupPresenterTests, a)

TEST_SETUP_CONTROLLER_TEST(controllerPreparesTestAfterConfirmButtonIsClicked) {
    testSettingsInterpreter.setMethod(Method::adaptivePassFail);
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Method::adaptivePassFail, sessionController.method());
}

TEST_SETUP_CONTROLLER_TEST(
    controllerDoesNotPrepareTestAfterConfirmButtonIsClickedWhenTestWouldAlreadyBeComplete) {
    setTestComplete(model);
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(sessionController.prepareCalled());
}

TEST_SETUP_CONTROLLER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesSubjectId) {
    control.setSubjectId("b");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, testSettingsInterpreter.identity().subjectId);
}

TEST_SETUP_CONTROLLER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesStartingSnr) {
    control.setStartingSnr("1");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, testSettingsInterpreter.startingSnr());
}

TEST_SETUP_CONTROLLER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestWithInvalidStartingSnrShowsMessage) {
    control.setStartingSnr("a");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"\"a\" is not a valid starting SNR."},
        errorMessage(sessionView));
}

TEST_SETUP_CONTROLLER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTesterId) {
    control.setTesterId("c");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c"}, testSettingsInterpreter.identity().testerId);
}

TEST_SETUP_CONTROLLER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesSession) {
    control.setSession("e");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"e"}, testSettingsInterpreter.identity().session);
}

TEST_SETUP_CONTROLLER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesRmeSetting) {
    control.setRmeSetting("e");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"e"}, testSettingsInterpreter.identity().rmeSetting);
}

TEST_SETUP_CONTROLLER_TEST(
    confirmingFixedLevelCoordinateResponseMeasureTestWithTargetReplacementPassesTransducer) {
    control.setTransducer("a");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, testSettingsInterpreter.identity().transducer);
}

TEST_SETUP_CONTROLLER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsTextToTestSettingsInterpreterForMethodQuery) {
    textFileReader.setRead("a");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, testSettingsInterpreter.textForMethodQuery());
}

TEST_SETUP_CONTROLLER_TEST(playCalibrationPassesLevel) {
    assertPassesCalibrationLevel(playingCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playLeftSpeakerCalibrationPassesLevel) {
    assertPassesCalibrationLevel(playingLeftSpeakerCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playRightSpeakerCalibrationPassesLevel) {
    assertPassesCalibrationLevel(playingRightSpeakerCalibration);
}

TEST_SETUP_CONTROLLER_TEST(
    playingCalibrationPassesTestSettingsFileToTextFileReader) {
    assertPassesTestSettingsFileToTextFileReader(playingCalibration);
}

TEST_SETUP_CONTROLLER_TEST(
    playingLeftSpeakerCalibrationPassesTestSettingsFileToTextFileReader) {
    assertPassesTestSettingsFileToTextFileReader(playingLeftSpeakerCalibration);
}

TEST_SETUP_CONTROLLER_TEST(
    playingRightSpeakerCalibrationPassesTestSettingsFileToTextFileReader) {
    assertPassesTestSettingsFileToTextFileReader(
        playingRightSpeakerCalibration);
}

TEST_SETUP_CONTROLLER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsFileToTextFileReader) {
    assertPassesTestSettingsFileToTextFileReader(confirmingTestSetup);
}

TEST_SETUP_CONTROLLER_TEST(
    confirmingAdaptiveCoordinateResponseMeasureTestPassesTestSettingsTextToTestSettingsInterpreter) {
    assertPassesTestSettingsTextToTestSettingsInterpreter(confirmingTestSetup);
}

TEST_SETUP_CONTROLLER_TEST(
    playingCalibrationPassesTestSettingsTextToTestSettingsInterpreter) {
    assertPassesTestSettingsTextToTestSettingsInterpreter(playingCalibration);
}

TEST_SETUP_CONTROLLER_TEST(
    playingLeftSpeakerCalibrationPassesTestSettingsTextToTestSettingsInterpreter) {
    assertPassesTestSettingsTextToTestSettingsInterpreter(
        playingLeftSpeakerCalibration);
}

TEST_SETUP_CONTROLLER_TEST(
    playingRightSpeakerCalibrationPassesTestSettingsTextToTestSettingsInterpreter) {
    assertPassesTestSettingsTextToTestSettingsInterpreter(
        playingRightSpeakerCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playCalibrationPassesFilePath) {
    assertPassesCalibrationAudioUrl(playingCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playLeftSpeakerCalibrationPassesFilePath) {
    assertPassesCalibrationAudioUrl(playingLeftSpeakerCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playCalibrationPassesAudioDevice) {
    setAudioDevice(sessionView, "b");
    run(playingCalibration);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, av_speech_in_noise::calibration(model).audioDevice);
}

TEST_SETUP_CONTROLLER_TEST(browseForTestSettingsFileUpdatesTestSettingsFile) {
    sessionView.setBrowseForOpeningFileResult("a");
    control.browseForTestSettingsFile();
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "a", testSetupControllerObserver.testSettingsFile());
}

TEST_SETUP_CONTROLLER_TEST(
    browseForTestSettingsCancelDoesNotChangeTestSettingsFile) {
    sessionView.setBrowseCancelled();
    control.browseForTestSettingsFile();
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(
        testSetupControllerObserver
            .notifiedThatUserHasSelectedTestSettingsFile());
}

TEST_SETUP_CONTROLLER_TEST(playCalibrationPassesFullScaleLevel) {
    calibration.fullScaleLevel.dB_SPL = 1;
    run(playingCalibration);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        1, av_speech_in_noise::calibration(model).fullScaleLevel.dB_SPL);
}

TEST_SETUP_CONTROLLER_TEST(playLeftSpeakerCalibrationPassesFullScaleLevel) {
    calibration.fullScaleLevel.dB_SPL = 1;
    run(playingLeftSpeakerCalibration);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        1, model.leftSpeakerCalibration().fullScaleLevel.dB_SPL);
}

TEST_SETUP_PRESENTER_TEST(presenterShowsViewWhenStarted) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

TEST_SETUP_PRESENTER_TEST(presenterHidesViewWhenStopped) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

TEST_SETUP_PRESENTER_TEST(presenterSetsTestSettingsFileWhenNotified) {
    presenter.notifyThatUserHasSelectedTestSettingsFile("a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("a", view.testSettingsFile());
}
auto contains(const std::vector<std::string> &items, const std::string &item)
    -> bool {
    return std::find(items.begin(), items.end(), item) != items.end();
}

TEST_SETUP_PRESENTER_TEST(presenterPopulatesTransducerMenu) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(view.transducers(), name(Transducer::headphone)));
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(view.transducers(), name(Transducer::oneSpeaker)));
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(view.transducers(), name(Transducer::twoSpeakers)));
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
