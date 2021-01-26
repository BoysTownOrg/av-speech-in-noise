#include "Syllables.hpp"
#include <utility>

namespace av_speech_in_noise {
SyllablesController::SyllablesController(SyllablesControl &control,
    TestController &testController, Model &model,
    std::map<std::string, Syllable, std::less<>> map)
    : map{std::move(map)}, control{control},
      testController{testController}, model{model} {
    control.attach(this);
}

static auto syllable(const std::map<std::string, Syllable, std::less<>> &map,
    std::string_view s) -> Syllable {
    return map.count(s) == 0 ? Syllable::unknown : map.at(std::string{s});
}

void SyllablesController::notifyThatResponseButtonHasBeenClicked() {
    model.submit(SyllableResponse{syllable(map, control.syllable())});
    testController.notifyThatUserIsDoneResponding();
}

SyllablesPresenterImpl::SyllablesPresenterImpl(
    SyllablesView &view, TestView &testView)
    : view{view}, testView{testView} {}

void SyllablesPresenterImpl::start() { testView.showNextTrialButton(); }

void SyllablesPresenterImpl::stop() { view.hide(); }

void SyllablesPresenterImpl::showResponseSubmission() { view.show(); }

void SyllablesPresenterImpl::hideResponseSubmission() { view.hide(); }
}
