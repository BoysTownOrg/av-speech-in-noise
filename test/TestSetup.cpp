#include "RunningATestStub.hpp"
#include "assert-utility.hpp"
#include "SessionViewStub.hpp"

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/ui/TestSetupImpl.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <utility>
#include <vector>

namespace av_speech_in_noise {
namespace {
class TestSetupViewStub : public TestSetupView {
  public:
    void populateTransducerMenu(std::vector<std::string> v) override {
        transducers_ = std::move(v);
    }

    void show() override { shown_ = true; }

    [[nodiscard]] auto shown() const -> bool { return shown_; }

    void hide() override { hidden_ = true; }

    [[nodiscard]] auto hidden() const { return hidden_; }

    auto transducers() -> std::vector<std::string> { return transducers_; }

  private:
    std::vector<std::string> transducers_;
    bool shown_{};
    bool hidden_{};
};

class TestSetupControlStub : public TestSetupControl {
  public:
    auto testSettingsFile() -> std::string override {
        return testSettingsFile_;
    }

    auto startingSnr() -> std::string override { return startingSnr_; }

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
    explicit TestSettingsInterpreterStub(
        RunningATest &runningATest, const Calibration &calibration_)
        : runningATest{runningATest}, calibration_{calibration_} {}

    auto calibration(const std::string &t) -> Calibration override {
        text_ = t;
        return calibration_;
    }

    void initializeTest(
        const std::string &t, const TestIdentity &id, SNR snr) override {
        startingSnr_ = snr.dB;
        text_ = t;
        identity_ = id;
        if (initializeAnyTestOnApply_)
            runningATest.initialize(nullptr, {}, {});
    }

    [[nodiscard]] auto text() const -> std::string { return text_; }

    [[nodiscard]] auto identity() const -> TestIdentity { return identity_; }

    [[nodiscard]] auto startingSnr() const -> int { return startingSnr_; }

    void initializeAnyTestOnApply() { initializeAnyTestOnApply_ = true; }

    auto sessionController() -> const SessionController * {
        return sessionController_;
    }

  private:
    RunningATest &runningATest;
    std::string text_;
    std::string textForMethodQuery_;
    TestIdentity identity_{};
    int startingSnr_{};
    const Calibration &calibration_;
    const SessionController *sessionController_{};
    bool initializeAnyTestOnApply_{};
};

class TextFileReaderStub : public TextFileReader {
  public:
    [[nodiscard]] auto filePath() const -> std::string { return filePath_; }

    auto read(const LocalUrl &s) -> std::string override {
        if (failOnRead_)
            throw FileDoesNotExist{};
        filePath_ = s.path;
        return read_;
    }

    void failOnRead() { failOnRead_ = true; }

    void setRead(std::string s) { read_ = std::move(s); }

  private:
    std::string filePath_;
    std::string read_;
    bool failOnRead_{};
};

class FailingFileReader : public TextFileReader {
  public:
    auto read(const LocalUrl &) -> std::string override {
        throw FileDoesNotExist{};
    }
};

class TestSetupPresenterStub : public TestSetupPresenter {
  public:
    void start() override {}

    void stop() override {}

    void updateErrorMessage(std::string_view s) override { errorMessage_ = s; }

    auto errorMessage() -> std::string { return errorMessage_; }

  private:
    std::string errorMessage_;
};

class UseCase {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(UseCase);
    virtual void run() = 0;
};

void run(UseCase &useCase) { useCase.run(); }

class CalibrationUseCase : public virtual UseCase {
  public:
    virtual auto calibration(RunningATestStub &) -> Calibration = 0;
};

class PlayingCalibration : public CalibrationUseCase {
  public:
    explicit PlayingCalibration(TestSetupControlStub &control)
        : control{control} {}

    void run() override { control.playCalibration(); }

    auto calibration(RunningATestStub &m) -> Calibration override {
        return m.calibration_;
    }

  private:
    TestSetupControlStub &control;
};

class PlayingLeftSpeakerCalibration : public CalibrationUseCase {
  public:
    explicit PlayingLeftSpeakerCalibration(TestSetupControlStub &control)
        : control{control} {}

    void run() override { control.playLeftSpeakerCalibration(); }

    auto calibration(RunningATestStub &m) -> Calibration override {
        return m.leftSpeakerCalibration_;
    }

  private:
    TestSetupControlStub &control;
};

class PlayingRightSpeakerCalibration : public CalibrationUseCase {
  public:
    explicit PlayingRightSpeakerCalibration(TestSetupControlStub &control)
        : control{control} {}

    void run() override { control.playRightSpeakerCalibration(); }

    auto calibration(RunningATestStub &m) -> Calibration override {
        return m.rightSpeakerCalibration_;
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

void setAudioDevice(SessionControlStub &view, std::string s) {
    view.setAudioDevice(std::move(s));
}

class SessionControllerStub : public SessionController {
  public:
    void notifyThatTestIsComplete() override {}

    void prepare(TaskPresenter &) override {}

  private:
};

class TestSetupControllerTests : public ::testing::Test {
  protected:
    RunningATestStub runningATest;
    SessionControlStub sessionView;
    TestSetupControlStub control;
    Calibration calibration;
    TestSettingsInterpreterStub testSettingsInterpreter{
        runningATest, calibration};
    TextFileReaderStub textFileReader;
    TestSetupPresenterStub presenter;
    TestSetupController controller{control, sessionView, presenter,
        runningATest, testSettingsInterpreter, textFileReader};
    PlayingCalibration playingCalibration{control};
    PlayingLeftSpeakerCalibration playingLeftSpeakerCalibration{control};
    PlayingRightSpeakerCalibration playingRightSpeakerCalibration{control};
    ConfirmingTestSetupImpl confirmingTestSetup{control};

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

    void assertPassesLevel(CalibrationUseCase &useCase) {
        calibration.level.dB_SPL = 1;
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            1, useCase.calibration(runningATest).level.dB_SPL);
    }

    void assertPassesAudioFileUrl(CalibrationUseCase &useCase) {
        calibration.fileUrl.path = "a";
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"a"}, useCase.calibration(runningATest).fileUrl.path);
    }

    void assertPassesAudioDevice(CalibrationUseCase &useCase) {
        setAudioDevice(sessionView, "b");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"b"}, useCase.calibration(runningATest).audioDevice);
    }

    void assertPassesFullScaleLevel(CalibrationUseCase &useCase) {
        calibration.fullScaleLevel.dB_SPL = 1;
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            1, useCase.calibration(runningATest).fullScaleLevel.dB_SPL);
    }
};

class TestSetupPresenterTests : public ::testing::Test {
  protected:
    TestSetupViewStub view;
    SessionViewStub sessionView;
    TestSetupPresenterImpl presenter{view, sessionView};
};

class TestSetupFailureTests : public ::testing::Test {
  protected:
    RunningATestStub runningATest;
    SessionControlStub sessionControl;
    SessionViewStub sessionView;
    TestSetupViewStub view;
    TestSetupControlStub control;
    Calibration calibration;
    TestSettingsInterpreterStub testSettingsInterpreter{
        runningATest, calibration};
    TestSetupPresenterImpl testSetupPresenter{view, sessionView};
    TextFileReaderStub textFileReader;
    TestSetupController controller{
        control,
        sessionControl,
        testSetupPresenter,
        runningATest,
        testSettingsInterpreter,
        textFileReader,
    };
};

#define TEST_SETUP_CONTROLLER_TEST(a) TEST_F(TestSetupControllerTests, a)

#define TEST_SETUP_PRESENTER_TEST(a) TEST_F(TestSetupPresenterTests, a)

#define TEST_SETUP_FAILURE_TEST(a) TEST_F(TestSetupFailureTests, a)

TEST_SETUP_CONTROLLER_TEST(confirmingTestPassesSubjectId) {
    control.setSubjectId("b");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, testSettingsInterpreter.identity().subjectId);
}

TEST_SETUP_CONTROLLER_TEST(confirmingTestPassesStartingSnr) {
    control.setStartingSnr("1");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, testSettingsInterpreter.startingSnr());
}

TEST_SETUP_CONTROLLER_TEST(confirmingTestSetupRoundsStartingSnr) {
    control.setStartingSnr("1.5");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(2, testSettingsInterpreter.startingSnr());
}

TEST_SETUP_CONTROLLER_TEST(confirmingTestWithInvalidStartingSnrShowsMessage) {
    control.setStartingSnr("a");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"\"a\" is not a valid starting SNR."},
        presenter.errorMessage());
}

TEST_SETUP_CONTROLLER_TEST(confirmingTestPassesTesterId) {
    control.setTesterId("c");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c"}, testSettingsInterpreter.identity().testerId);
}

TEST_SETUP_CONTROLLER_TEST(confirmingTestPassesSession) {
    control.setSession("e");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"e"}, testSettingsInterpreter.identity().session);
}

TEST_SETUP_CONTROLLER_TEST(confirmingTestPassesRmeSetting) {
    control.setRmeSetting("e");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"e"}, testSettingsInterpreter.identity().rmeSetting);
}

TEST_SETUP_CONTROLLER_TEST(confirmingTestPassesTransducer) {
    control.setTransducer("a");
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, testSettingsInterpreter.identity().transducer);
}

TEST_SETUP_CONTROLLER_TEST(
    confirmingTestSetupPassesDefaultRelativeOutputDirectory) {
    run(confirmingTestSetup);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"Documents/AvSpeechInNoise Data"},
        testSettingsInterpreter.identity().relativeOutputUrl.path);
}

TEST_SETUP_CONTROLLER_TEST(playCalibrationPassesLevel) {
    assertPassesLevel(playingCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playLeftSpeakerCalibrationPassesLevel) {
    assertPassesLevel(playingLeftSpeakerCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playRightSpeakerCalibrationPassesLevel) {
    assertPassesLevel(playingRightSpeakerCalibration);
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
    assertPassesAudioFileUrl(playingCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playLeftSpeakerCalibrationPassesFilePath) {
    assertPassesAudioFileUrl(playingLeftSpeakerCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playRightSpeakerCalibrationPassesFilePath) {
    assertPassesAudioFileUrl(playingRightSpeakerCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playCalibrationPassesAudioDevice) {
    assertPassesAudioDevice(playingCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playingLeftSpeakerCalibrationPassesAudioDevice) {
    assertPassesAudioDevice(playingLeftSpeakerCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playingRightSpeakerCalibrationPassesAudioDevice) {
    assertPassesAudioDevice(playingRightSpeakerCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playCalibrationPassesFullScaleLevel) {
    assertPassesFullScaleLevel(playingCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playLeftSpeakerCalibrationPassesFullScaleLevel) {
    assertPassesFullScaleLevel(playingLeftSpeakerCalibration);
}

TEST_SETUP_CONTROLLER_TEST(playRightSpeakerCalibrationPassesFullScaleLevel) {
    assertPassesFullScaleLevel(playingRightSpeakerCalibration);
}

TEST_SETUP_PRESENTER_TEST(presenterShowsViewWhenStarted) {
    presenter.start();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.shown());
}

TEST_SETUP_PRESENTER_TEST(presenterHidesViewWhenStopped) {
    presenter.stop();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(view.hidden());
}

auto contains(
    const std::vector<std::string> &items, const std::string &item) -> bool {
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

#define AV_SPEECH_IN_NOISE_EXPECT_ERROR_MESSAGE(sessionView, a)                \
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(a, errorMessage(sessionView))

TEST_SETUP_FAILURE_TEST(initializeTestShowsErrorMessageWhenModelFailsRequest) {
    testSettingsInterpreter.initializeAnyTestOnApply();
    runningATest.errorMessage = "a";
    runningATest.failOnRequest = true;
    confirmTestSetup(control);
    AV_SPEECH_IN_NOISE_EXPECT_ERROR_MESSAGE(sessionView, "a");
}

TEST_SETUP_FAILURE_TEST(
    initializeTestDoesNotHideSetupViewWhenModelFailsRequest) {
    testSettingsInterpreter.initializeAnyTestOnApply();
    confirmTestSetup(control);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(view.hidden());
}

TEST_SETUP_FAILURE_TEST(
    playingCalibrationShowsErrorMessageWhenModelFailsRequest) {
    runningATest.errorMessage = "a";
    runningATest.failOnRequest = true;
    control.playCalibration();
    AV_SPEECH_IN_NOISE_EXPECT_ERROR_MESSAGE(sessionView, "a");
}

TEST_SETUP_FAILURE_TEST(
    playingLeftSpeakerCalibrationShowsErrorMessageWhenModelFailsRequest) {
    runningATest.errorMessage = "a";
    runningATest.failOnRequest = true;
    control.playLeftSpeakerCalibration();
    AV_SPEECH_IN_NOISE_EXPECT_ERROR_MESSAGE(sessionView, "a");
}

TEST_SETUP_FAILURE_TEST(
    playingRightSpeakerCalibrationShowsErrorMessageWhenModelFailsRequest) {
    runningATest.errorMessage = "a";
    runningATest.failOnRequest = true;
    control.playRightSpeakerCalibration();
    AV_SPEECH_IN_NOISE_EXPECT_ERROR_MESSAGE(sessionView, "a");
}

TEST_SETUP_FAILURE_TEST(
    confirmTestSetupShowsErrorMessageWhenTextFileReaderFails) {
    control.setTestSettingsFile("a");
    textFileReader.failOnRead();
    control.confirmTestSetup();
    AV_SPEECH_IN_NOISE_EXPECT_ERROR_MESSAGE(sessionView, "Unable to read a");
}
}
}
