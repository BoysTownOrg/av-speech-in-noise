#include "Presenter.hpp"

namespace av_speech_in_noise {
Presenter::Presenter(Model &model, View &view,
    TestSetupController *testSetupController,
    TestSetupPresenter *testSetupPresenter,
    ExperimenterController *experimenterController,
    ExperimenterPresenter *experimenterPresenter)
    : view{view}, testSetupPresenter{testSetupPresenter},
      experimenterPresenter{experimenterPresenter} {
    if (testSetupController != nullptr) {
        testSetupController->subscribe(this);
        testSetupController->subscribe(testSetupPresenter);
    }
    if (experimenterController != nullptr) {
        experimenterController->subscribe(this);
        experimenterController->subscribe(experimenterPresenter);
    }
    view.populateAudioDeviceMenu(model.audioDevices());
}

void Presenter::run() { view.eventLoop(); }

void Presenter::prepare(Method m) {
    testSetupPresenter->stop();
    experimenterPresenter->initialize(m);
    experimenterPresenter->start();
}

void Presenter::notifyThatTestIsComplete() {
    experimenterPresenter->stop();
    testSetupPresenter->start();
}
}
