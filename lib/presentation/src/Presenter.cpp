#include "Presenter.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/name.hpp>
#include <string>
#include <sstream>
#include <functional>

namespace av_speech_in_noise {
Presenter::Presenter(Model &model, View &view,
    TestSetupResponder *testSetupResponder,
    TestSetupPresenter *testSetupPresenter,
    ExperimenterResponder *experimenterResponder,
    ExperimenterPresenter *experimenterPresenter)
    : model{model}, view{view}, testSetupPresenter{testSetupPresenter},
      experimenterPresenter{experimenterPresenter} {
    model.subscribe(this);
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

void Presenter::switchToTestView(Method m) {}

void Presenter::trialComplete() {
    experimenterPresenter->notifyThatTrialHasCompleted();
    view.showCursor();
}

void Presenter::readyNextTrial() {}

void Presenter::switchToTestSetupView() {
    testSetupPresenter->start();
    experimenterPresenter->stop();
}
}
