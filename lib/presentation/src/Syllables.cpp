#include "Syllables.hpp"

namespace av_speech_in_noise {
SyllablesPresenterImpl::SyllablesPresenterImpl(
    SyllablesView &view, TestView &testView)
    : view{view}, testView{testView} {}

void SyllablesPresenterImpl::start() { testView.showNextTrialButton(); }

void SyllablesPresenterImpl::stop() { view.hide(); }

void SyllablesPresenterImpl::showResponseSubmission() { view.show(); }

void SyllablesPresenterImpl::hideResponseSubmission() { view.hide(); }
}
