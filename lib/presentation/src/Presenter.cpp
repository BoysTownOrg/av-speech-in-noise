#include "Presenter.hpp"

namespace av_speech_in_noise {
Presenter::Presenter(Model &model, View &view,
    TestSetupResponder *testSetupResponder,
    TestSetupPresenter *testSetupPresenter,
    ExperimenterResponder *experimenterResponder,
    ExperimenterPresenter *experimenterPresenter)
    : view{view}, testSetupPresenter{testSetupPresenter},
      experimenterPresenter{experimenterPresenter} {
    if (testSetupResponder != nullptr) {
        testSetupResponder->becomeChild(this);
        testSetupResponder->subscribe(testSetupPresenter);
    }
    if (experimenterResponder != nullptr) {
        experimenterResponder->becomeChild(this);
        experimenterResponder->subscribe(experimenterPresenter);
    }
    view.populateAudioDeviceMenu(model.audioDevices());
}

void Presenter::run() { view.eventLoop(); }

void Presenter::prepare(Method m) {
    testSetupPresenter->stop();
    experimenterPresenter->initialize(m);
    experimenterPresenter->start();
}

void Presenter::switchToTestSetupView() {
    experimenterPresenter->stop();
    testSetupPresenter->start();
}
}
