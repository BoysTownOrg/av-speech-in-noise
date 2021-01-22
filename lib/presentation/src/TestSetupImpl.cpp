#include "TestSetupImpl.hpp"
#include <functional>
#include <utility>

namespace av_speech_in_noise {
TestSetupControllerImpl::TestSetupControllerImpl(TestSetupControl &control,
    SessionController &sessionController, SessionControl &sessionControl,
    TestSetupPresenter &presenter, Model &model,
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
        presenter.showErrorMessage(e.what());
    }
}

void TestSetupControllerImpl::notifyThatConfirmButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(presenter, [&] {
        const auto testSettings{
            textFileReader.read({control.testSettingsFile()})};
        TestIdentity p;
        p.subjectId = control.subjectId();
        p.testerId = control.testerId();
        p.session = control.session();
        p.rmeSetting = control.rmeSetting();
        p.transducer = control.transducer();
        testSettingsInterpreter.initialize(model, sessionController,
            testSettings, p,
            SNR{readInteger(control.startingSnr(), "starting SNR")});
    });
}

static auto calibration(TestSettingsInterpreter &testSettingsInterpreter,
    TextFileReader &textFileReader, TestSetupControl &control,
    SessionControl &sessionControl) -> Calibration {
    auto calibration{testSettingsInterpreter.calibration(
        textFileReader.read({control.testSettingsFile()}))};
    calibration.audioDevice = sessionControl.audioDevice();
    return calibration;
}

void TestSetupControllerImpl::notifyThatPlayCalibrationButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(presenter, [&] {
        model.playCalibration(calibration(
            testSettingsInterpreter, textFileReader, control, sessionControl));
    });
}

void TestSetupControllerImpl::
    notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(presenter, [&] {
        model.playLeftSpeakerCalibration(calibration(
            testSettingsInterpreter, textFileReader, control, sessionControl));
    });
}

void TestSetupControllerImpl::
    notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(presenter, [&] {
        model.playRightSpeakerCalibration(calibration(
            testSettingsInterpreter, textFileReader, control, sessionControl));
    });
}

void TestSetupControllerImpl::
    notifyThatBrowseForTestSettingsButtonHasBeenClicked() {
    auto file{sessionControl.browseForOpeningFile()};
    if (!sessionControl.browseCancelled())
        presenter.showTestSettingsFile(file);
}

TestSetupPresenterImpl::TestSetupPresenterImpl(
    TestSetupView &view, SessionView &sessionView)
    : view{view}, sessionView{sessionView} {
    view.populateTransducerMenu({name(Transducer::headphone),
        name(Transducer::oneSpeaker), name(Transducer::twoSpeakers)});
}

void TestSetupPresenterImpl::start() { view.show(); }

void TestSetupPresenterImpl::stop() { view.hide(); }

void TestSetupPresenterImpl::showTestSettingsFile(const std::string &s) {
    view.setTestSettingsFile(s);
}

void TestSetupPresenterImpl::showErrorMessage(std::string s) {
    sessionView.showErrorMessage(std::move(s));
}
}
