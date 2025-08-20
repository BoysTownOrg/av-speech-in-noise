#include "SessionController.hpp"

namespace av_speech_in_noise {
SessionControllerImpl::SessionControllerImpl(
    TestSetupPresenter &testSetupPresenter, TestPresenter &testPresenter,
    SubjectPresenter &subjectPresenter)
    : testSetupPresenter{testSetupPresenter}, testPresenter{testPresenter},
      subjectPresenter{subjectPresenter} {}

void SessionControllerImpl::prepare() {
    testSetupPresenter.stop();
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
