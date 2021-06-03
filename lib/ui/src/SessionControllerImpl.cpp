#include "SessionController.hpp"

namespace av_speech_in_noise {
SessionControllerImpl::SessionControllerImpl(Model &model, SessionView &view,
    TestSetupPresenter &testSetupPresenter, TestPresenter &testPresenter)
    : testSetupPresenter{testSetupPresenter}, testPresenter{testPresenter} {
    view.populateAudioDeviceMenu(model.audioDevices());
    view.populateSubjectScreenMenu(view.screens());
}

void SessionControllerImpl::prepare(TaskPresenter &p) {
    testSetupPresenter.stop();
    testPresenter.initialize(p);
    testPresenter.start();
}

void SessionControllerImpl::notifyThatTestIsComplete() {
    testPresenter.stop();
    testSetupPresenter.start();
    if (observer != nullptr)
        observer->notifyThatTestIsComplete();
}

void SessionControllerImpl::attach(Observer *e) { observer = e; }
}
