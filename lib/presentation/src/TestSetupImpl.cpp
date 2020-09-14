#include "TestSetupImpl.hpp"

namespace av_speech_in_noise {
TestSetupControllerImpl::TestSetupControllerImpl(Model &model,
    SessionView &mainView, TestSetupControl &view,
    TestSettingsInterpreter &testSettingsInterpreter,
    TextFileReader &textFileReader)
    : model{model}, mainView{mainView}, view{view},
      testSettingsInterpreter{testSettingsInterpreter}, textFileReader{
                                                            textFileReader} {
    view.attach(this);
}

void TestSetupControllerImpl::notifyThatConfirmButtonHasBeenClicked() {
    try {
        const auto testSettings{textFileReader.read({view.testSettingsFile()})};
        TestIdentity p;
        p.subjectId = view.subjectId();
        p.testerId = view.testerId();
        p.session = view.session();
        p.rmeSetting = view.rmeSetting();
        p.transducer = view.transducer();
        testSettingsInterpreter.initialize(model, testSettings, p,
            SNR{readInteger(view.startingSnr(), "starting SNR")});
        if (!model.testComplete())
            parent->prepare(testSettingsInterpreter.method(testSettings));
    } catch (const std::runtime_error &e) {
        mainView.showErrorMessage(e.what());
    }
}
void TestSetupControllerImpl::notifyThatPlayCalibrationButtonHasBeenClicked() {
    auto p{testSettingsInterpreter.calibration(
        textFileReader.read({view.testSettingsFile()}))};
    p.audioDevice = mainView.audioDevice();
    model.playCalibration(p);
}
void TestSetupControllerImpl::
    notifyThatBrowseForTestSettingsButtonHasBeenClicked() {
    auto file{mainView.browseForOpeningFile()};
    if (!mainView.browseCancelled())
        listener->notifyThatUserHasSelectedTestSettingsFile(file);
}
void TestSetupControllerImpl::attach(SessionController *p) { parent = p; }
void TestSetupControllerImpl::attach(TestSetupController::Observer *e) {
    listener = e;
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
