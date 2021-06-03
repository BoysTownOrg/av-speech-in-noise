#include "SessionController.hpp"
#include <algorithm>
#include <iterator>

namespace av_speech_in_noise {
SessionControllerImpl::SessionControllerImpl(Model &model, SessionView &view,
    SubjectView &subjectView, TestSetupPresenter &testSetupPresenter,
    TestPresenter &testPresenter)
    : view{view}, subjectView{subjectView},
      testSetupPresenter{testSetupPresenter}, testPresenter{testPresenter} {
    view.populateAudioDeviceMenu(model.audioDevices());
    view.populateSubjectScreenMenu(view.screens());
}

static auto operator==(const Screen &a, const Screen &b) -> bool {
    return a.name == b.name;
}

void SessionControllerImpl::prepare(TaskPresenter &p) {
    testSetupPresenter.stop();
    testPresenter.initialize(p);
    const auto screens{view.screens()};
    if (const auto it{
            std::find(screens.begin(), screens.end(), view.subjectScreen())};
        it != screens.end())
        subjectView.moveToScreen(std::distance(screens.begin(), it));
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
