#include "SessionControllerImpl.hpp"

namespace av_speech_in_noise {
SessionControllerImpl::SessionControllerImpl(Model &model, SessionView &view,
    TestSetupPresenter *testSetupPresenter,
    TestPresenter *experimenterPresenter)
    : testSetupPresenter{testSetupPresenter}, experimenterPresenter{
                                                  experimenterPresenter} {
    view.populateAudioDeviceMenu(model.audioDevices());
}

void SessionControllerImpl::prepare(Method m) {
    testSetupPresenter->stop();
    experimenterPresenter->initialize(m);
    experimenterPresenter->start();
}

void SessionControllerImpl::notifyThatTestIsComplete() {
    experimenterPresenter->stop();
    testSetupPresenter->start();
    if (observer != nullptr)
        observer->notifyThatTestIsComplete();
}

void SessionControllerImpl::attach(Observer *e) { observer = e; }
}
