#include "Syllables.hpp"
#include <utility>

namespace av_speech_in_noise::submitting_syllable {
Controller::Controller(Control &control, TestController &testController,
    Interactor &model, std::map<std::string, Syllable, std::less<>> map)
    : map{std::move(map)}, control{control},
      testController{testController}, model{model} {
    control.attach(this);
}

static auto syllable(const std::map<std::string, Syllable, std::less<>> &map,
    std::string_view s) -> Syllable {
    return map.count(s) == 0 ? Syllable::unknown : map.at(std::string{s});
}

void Controller::notifyThatResponseButtonHasBeenClicked() {
    SyllableResponse syllableResponse;
    syllableResponse.flagged = control.flagged();
    syllableResponse.syllable = syllable(map, control.syllable());
    model.submit(syllableResponse);
    testController.notifyThatUserIsDoneResponding();
}

PresenterImpl::PresenterImpl(View &view, TestView &testView)
    : view{view}, testView{testView} {}

void PresenterImpl::start() { testView.showNextTrialButton(); }

void PresenterImpl::stop() { view.hide(); }

void PresenterImpl::showResponseSubmission() {
    view.clearFlag();
    view.show();
}

void PresenterImpl::hideResponseSubmission() { view.hide(); }
}
