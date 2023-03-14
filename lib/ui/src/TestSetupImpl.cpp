#include "TestSetupImpl.hpp"
#include "av-speech-in-noise/core/TextFileReader.hpp"
#include <functional>
#include <sstream>
#include <utility>

namespace av_speech_in_noise {
TestSetupController::TestSetupController(TestSetupControl &control,
    SessionController &sessionController, SessionControl &sessionControl,
    TestSetupPresenter &presenter, RunningATestFacade &model,
    TestSettingsInterpreter &testSettingsInterpreter,
    TextFileReader &textFileReader)
    : control{control}, sessionController{sessionController},
      sessionControl{sessionControl}, presenter{presenter}, model{model},
      testSettingsInterpreter{testSettingsInterpreter}, textFileReader{
                                                            textFileReader} {
    control.attach(this);
}

static void showErrorMessageOnRuntimeError(
    TestSetupPresenter &presenter, const std::function<void()> &f) {
    try {
        f();
    } catch (const std::runtime_error &e) {
        presenter.updateErrorMessage(e.what());
    }
}

static auto readTestSettingsFile(
    TextFileReader &textFileReader, TestSetupControl &control) -> std::string {
    const auto localUrl{LocalUrl{control.testSettingsFile()}};
    try {
        return textFileReader.read(localUrl);
    } catch (const TextFileReader::FileDoesNotExist &) {
        std::stringstream stream;
        stream << "Unable to read " << localUrl.path;
        throw std::runtime_error{stream.str()};
    }
}

void TestSetupController::notifyThatConfirmButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(presenter, [&] {
        TestIdentity testIdentity;
        testIdentity.subjectId = control.subjectId();
        testIdentity.testerId = control.testerId();
        testIdentity.session = control.session();
        testIdentity.rmeSetting = control.rmeSetting();
        testIdentity.transducer = control.transducer();
        testIdentity.relativeOutputUrl.path = "Documents/AvSpeechInNoise Data";
        testSettingsInterpreter.initialize(model, sessionController,
            readTestSettingsFile(textFileReader, control), testIdentity,
            SNR{readInteger(control.startingSnr(), "starting SNR")});
    });
}

static auto calibration(TestSettingsInterpreter &testSettingsInterpreter,
    TextFileReader &textFileReader, TestSetupControl &control,
    SessionControl &sessionControl) -> Calibration {
    auto calibration{testSettingsInterpreter.calibration(
        readTestSettingsFile(textFileReader, control))};
    calibration.audioDevice = sessionControl.audioDevice();
    return calibration;
}

void TestSetupController::notifyThatPlayCalibrationButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(presenter, [&] {
        model.playCalibration(calibration(
            testSettingsInterpreter, textFileReader, control, sessionControl));
    });
}

void TestSetupController::
    notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(presenter, [&] {
        model.playLeftSpeakerCalibration(calibration(
            testSettingsInterpreter, textFileReader, control, sessionControl));
    });
}

void TestSetupController::
    notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(presenter, [&] {
        model.playRightSpeakerCalibration(calibration(
            testSettingsInterpreter, textFileReader, control, sessionControl));
    });
}

TestSetupPresenterImpl::TestSetupPresenterImpl(
    TestSetupView &view, SessionView &sessionView)
    : view{view}, sessionView{sessionView} {
    view.populateTransducerMenu({name(Transducer::headphone),
        name(Transducer::oneSpeaker), name(Transducer::twoSpeakers)});
}

void TestSetupPresenterImpl::start() { view.show(); }

void TestSetupPresenterImpl::stop() { view.hide(); }

void TestSetupPresenterImpl::updateErrorMessage(std::string_view s) {
    sessionView.showErrorMessage(s);
}
}
