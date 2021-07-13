#include "SessionController.hpp"

namespace av_speech_in_noise {
SessionControllerImpl::SessionControllerImpl(
    TestSetupPresenter &testSetupPresenter, TestPresenter &testPresenter,
    SubjectPresenter &subjectPresenter)
    : testSetupPresenter{testSetupPresenter}, testPresenter{testPresenter},
      subjectPresenter{subjectPresenter} {}

static auto operator==(const Screen &a, const Screen &b) -> bool {
    return a.name == b.name;
}

void SessionControllerImpl::prepare(TaskPresenter &p) {
    testSetupPresenter.stop();
    testPresenter.initialize(p);
    testPresenter.start();
    subjectPresenter.start();
}

void SessionControllerImpl::notifyThatTestIsComplete() {
    subjectPresenter.stop();
    testPresenter.stop();
    testSetupPresenter.start();
    if (observer != nullptr)
        observer->notifyThatTestIsComplete();
}

void SessionControllerImpl::attach(Observer *e) { observer = e; }
}
