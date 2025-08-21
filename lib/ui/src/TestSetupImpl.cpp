#include "TestSetupImpl.hpp"
#include "Input.hpp"

#include <cmath>
#include <exception>
#include <functional>
#include <gsl/gsl_util>
#include <sstream>
#include <stdexcept>
#include <string>

namespace av_speech_in_noise {
TestSetupController::TestSetupController(TestSetupControl &control,
    SessionControl &sessionControl, TestSetupPresenter &presenter,
    RunningATest &runningATest,
    TestSettingsInterpreter &testSettingsInterpreter,
    TextFileReader &textFileReader, SessionController &sessionController)
    : control{control}, sessionControl{sessionControl}, presenter{presenter},
      runningATest{runningATest},
      testSettingsInterpreter{testSettingsInterpreter},
      textFileReader{textFileReader}, sessionController{sessionController} {
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

static auto roundedInteger(const std::string &x, const std::string &identifier)
    -> int {
    try {
        return gsl::narrow<int>(std::lround(std::stod(x)));
    } catch (const std::exception &) {
        std::stringstream stream;
        stream << '"' << x << '"';
        stream << " is not a valid ";
        stream << identifier;
        stream << '.';
        throw BadInput{stream.str()};
    }
}

void TestSetupController::notifyThatConfirmButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(presenter, [&] {
        testSettingsInterpreter.set(
            "relative output path", "Documents/AvSpeechInNoise Data");
        testSettingsInterpreter.set("puzzle", "");
        testSettingsInterpreter.set("subject ID", control.subjectId());
        testSettingsInterpreter.set("tester ID", control.testerId());
        testSettingsInterpreter.set("session", control.session());
        testSettingsInterpreter.set("RME setting", control.rmeSetting());
        testSettingsInterpreter.set("transducer", control.transducer());
        testSettingsInterpreter.set("starting SNR (dB)",
            std::to_string(
                roundedInteger(control.startingSnr(), "starting SNR")));

        testSettingsInterpreter.initializeTest(
            readTestSettingsFile(textFileReader, control));

        runningATest.initialize();
        if (!runningATest.testComplete())
            sessionController.prepare();
    });
}

static auto calibration(TestSettingsInterpreter &testSettingsInterpreter,
    TextFileReader &textFileReader, TestSetupControl &control,
    SessionControl &sessionControl) -> Calibration {
    testSettingsInterpreter.apply(readTestSettingsFile(textFileReader, control),
        {"masker", "masker level (dB SPL)"});
    Calibration calibration;
    calibration.audioDevice = sessionControl.audioDevice();
    return calibration;
}

void TestSetupController::notifyThatPlayCalibrationButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(presenter, [&] {
        runningATest.playCalibration(calibration(
            testSettingsInterpreter, textFileReader, control, sessionControl));
    });
}

void TestSetupController::
    notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(presenter, [&] {
        runningATest.playLeftSpeakerCalibration(calibration(
            testSettingsInterpreter, textFileReader, control, sessionControl));
    });
}

void TestSetupController::
    notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(presenter, [&] {
        runningATest.playRightSpeakerCalibration(calibration(
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
