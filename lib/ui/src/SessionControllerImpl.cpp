#include "SessionController.hpp"

namespace av_speech_in_noise {
SessionControllerImpl::SessionControllerImpl(
    TestSetupPresenter &testSetupPresenter, TestPresenter &testPresenter)
    : testSetupPresenter{testSetupPresenter}, testPresenter{testPresenter} {}

static auto operator==(const Screen &a, const Screen &b) -> bool {
    return a.name == b.name;
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
