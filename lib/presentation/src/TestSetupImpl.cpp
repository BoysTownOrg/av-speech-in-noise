#include "TestSetupImpl.hpp"

namespace av_speech_in_noise {
TestSetupControllerImpl::TestSetupControllerImpl(Model &model,
    SessionView &mainView, TestSetupControl &control,
    TestSettingsInterpreter &testSettingsInterpreter,
    TextFileReader &textFileReader)
    : model{model}, sessionView{mainView}, control{control},
      testSettingsInterpreter{testSettingsInterpreter}, textFileReader{
                                                            textFileReader} {
    control.attach(this);
}

void TestSetupControllerImpl::notifyThatConfirmButtonHasBeenClicked() {
    try {
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
            controller->prepare(testSettingsInterpreter.method(testSettings));
    } catch (const std::runtime_error &e) {
        sessionView.showErrorMessage(e.what());
    }
}

void TestSetupControllerImpl::notifyThatPlayCalibrationButtonHasBeenClicked() {
    auto p{testSettingsInterpreter.calibration(
        textFileReader.read({control.testSettingsFile()}))};
    p.audioDevice = sessionView.audioDevice();
    model.playCalibration(p);
}

void TestSetupControllerImpl::
    notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked() {
    auto p{testSettingsInterpreter.calibration(
        textFileReader.read({control.testSettingsFile()}))};
    p.audioDevice = sessionView.audioDevice();
    model.playLeftSpeakerCalibration(p);
}

void TestSetupControllerImpl::
    notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked() {
    auto p{testSettingsInterpreter.calibration(
        textFileReader.read({control.testSettingsFile()}))};
    p.audioDevice = sessionView.audioDevice();
    model.playRightSpeakerCalibration(p);
}

void TestSetupControllerImpl::
    notifyThatBrowseForTestSettingsButtonHasBeenClicked() {
    auto file{sessionView.browseForOpeningFile()};
    if (!sessionView.browseCancelled())
        observer->notifyThatUserHasSelectedTestSettingsFile(file);
}

void TestSetupControllerImpl::attach(SessionController *p) { controller = p; }

void TestSetupControllerImpl::attach(TestSetupController::Observer *e) {
    observer = e;
}

TestSetupPresenterImpl::TestSetupPresenterImpl(TestSetupView &view)
    : view{view} {
    view.populateTransducerMenu({name(Transducer::headphone),
        name(Transducer::oneSpeaker), name(Transducer::twoSpeakers)});
}

void TestSetupPresenterImpl::start() { view.show(); }

void TestSetupPresenterImpl::stop() { view.hide(); }

void TestSetupPresenterImpl::notifyThatUserHasSelectedTestSettingsFile(
    const std::string &s) {
    view.setTestSettingsFile(s);
}
}
