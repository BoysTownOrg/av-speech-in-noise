#include "SessionControllerImpl.hpp"

namespace av_speech_in_noise {
SessionControllerImpl::SessionControllerImpl(Model &model, SessionView &view,
    TestSetupPresenter *testSetupPresenter,
    TestPresenter *experimenterPresenter)
    : view{view}, testSetupPresenter{testSetupPresenter},
      experimenterPresenter{experimenterPresenter} {
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
}
}
