#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTERIMPL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTERIMPL_HPP_

#include "Experimenter.hpp"
#include "View.hpp"
#include "Presenter.hpp"
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class ExperimenterResponderImpl : public ExperimenterInputView::EventListener,
                                  public ExperimenterResponder {
  public:
    explicit ExperimenterResponderImpl(
        Model &model, View &mainView, ExperimenterInputView &view)
        : model{model}, mainView{mainView} {
        view.subscribe(this);
    }
    void subscribe(ExperimenterResponder::EventListener *e) override {
        listener = e;
    }
    void exitTest() override { parent->switchToTestSetupView(); }
    void playTrial() override {
        AudioSettings p;
        p.audioDevice = mainView.audioDevice();
        model.playTrial(p);
        listener->notifyThatTrialHasStarted();
    }
    void declineContinuingTesting() override {
        parent->switchToTestSetupView();
    }
    void acceptContinuingTesting() override {
        model.restartAdaptiveTestWhilePreservingTargets();
        parent->readyNextTrialIfNeeded();
    }
    void becomeChild(Presenter *p) override { parent = p; }

  private:
    Model &model;
    View &mainView;
    ExperimenterResponder::EventListener *listener{};
    Presenter *parent{};
};

class ExperimenterPresenterImpl : public ExperimenterPresenter {
  public:
    explicit ExperimenterPresenterImpl(ExperimenterOutputView &view)
        : view{view} {}

    void start() override { view.show(); }

    void stop() override {
        view.hideContinueTestingDialog();
        view.hide();
    }

    void notifyThatTrialHasStarted() override {
        view.hideExitTestButton();
        view.hideNextTrialButton();
    }

    void notifyThatTrialHasCompleted() override { view.showExitTestButton(); }

    void notifyThatNextTrialIsReady() override {
        view.hideContinueTestingDialog();
        view.showNextTrialButton();
    }

    void display(const std::string &s) override { view.display(s); }

    void secondaryDisplay(const std::string &s) override {
        view.secondaryDisplay(s);
    }

    void showContinueTestingDialog() override {
        view.showContinueTestingDialog();
    }

    void hideContinueTestingDialog() override {
        view.hideContinueTestingDialog();
    }

    void setContinueTestingDialogMessage(const std::string &s) override {
        view.setContinueTestingDialogMessage(s);
    }

  private:
    ExperimenterOutputView &view;
};
}

#endif
