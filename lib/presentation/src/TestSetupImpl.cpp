#include "TestSetupImpl.hpp"
#include <functional>
#include <utility>

namespace av_speech_in_noise {
TestSetupControllerImpl::TestSetupControllerImpl(
    SessionController &sessionController, Model &model, SessionView &mainView,
    TestSetupControl &control, TestSettingsInterpreter &testSettingsInterpreter,
    TextFileReader &textFileReader, TestSetupPresenter &presenter,
    std::map<Method, TaskPresenter &> taskPresenters)
    : taskPresenters{std::move(taskPresenters)},
      sessionController{sessionController}, model{model}, sessionView{mainView},
      control{control}, testSettingsInterpreter{testSettingsInterpreter},
      textFileReader{textFileReader}, presenter{presenter} {
    control.attach(this);
}

static void showErrorMessageOnRuntimeError(
    SessionView &view, const std::function<void()> &f) {
    try {
        f();
    } catch (const std::runtime_error &e) {
        view.showErrorMessage(e.what());
    }
}

void TestSetupControllerImpl::notifyThatConfirmButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(sessionView, [&] {
        const auto testSettings{
            textFileReader.read({control.testSettingsFile()})};
        TestIdentity p;
        p.subjectId = control.subjectId();
        p.testerId = control.testerId();
        p.session = control.session();
        p.rmeSetting = control.rmeSetting();
        p.transducer = control.transducer();
        testSettingsInterpreter.initialize(model, testSettings, p,
            SNR{readInteger(control.startingSnr(), "starting SNR")});
        if (!model.testComplete())
            sessionController.prepare(taskPresenters.at(
                testSettingsInterpreter.method(testSettings)));
    });
}

static auto calibration(TestSettingsInterpreter &testSettingsInterpreter,
    TextFileReader &textFileReader, TestSetupControl &control,
    SessionView &sessionView) -> Calibration {
    auto calibration{testSettingsInterpreter.calibration(
        textFileReader.read({control.testSettingsFile()}))};
    calibration.audioDevice = sessionView.audioDevice();
    return calibration;
}

void TestSetupControllerImpl::notifyThatPlayCalibrationButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(sessionView, [&] {
        model.playCalibration(calibration(
            testSettingsInterpreter, textFileReader, control, sessionView));
    });
}

void TestSetupControllerImpl::
    notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(sessionView, [&] {
        model.playLeftSpeakerCalibration(calibration(
            testSettingsInterpreter, textFileReader, control, sessionView));
    });
}

void TestSetupControllerImpl::
    notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked() {
    showErrorMessageOnRuntimeError(sessionView, [&] {
        model.playRightSpeakerCalibration(calibration(
            testSettingsInterpreter, textFileReader, control, sessionView));
    });
}

void TestSetupControllerImpl::
    notifyThatBrowseForTestSettingsButtonHasBeenClicked() {
    auto file{sessionView.browseForOpeningFile()};
    if (!sessionView.browseCancelled())
        presenter.showTestSettingsFile(file);
}

TestSetupPresenterImpl::TestSetupPresenterImpl(TestSetupView &view)
    : view{view} {
    view.populateTransducerMenu({name(Transducer::headphone),
        name(Transducer::oneSpeaker), name(Transducer::twoSpeakers)});
}

void TestSetupPresenterImpl::start() { view.show(); }

void TestSetupPresenterImpl::stop() { view.hide(); }

void TestSetupPresenterImpl::showTestSettingsFile(const std::string &s) {
    view.setTestSettingsFile(s);
}
}
