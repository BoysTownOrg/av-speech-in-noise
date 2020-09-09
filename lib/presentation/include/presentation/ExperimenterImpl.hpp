#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTERIMPL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTERIMPL_HPP_

#include "Experimenter.hpp"
#include "View.hpp"
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class ExperimenterResponderImpl : public ExperimenterInputView::EventListener,
                                  public ExperimenterResponder {
  public:
    explicit ExperimenterResponderImpl(
        Model &, View &, ExperimenterInputView &);
    void subscribe(ExperimenterResponder::EventListener *e) override;
    void exitTest() override;
    void playTrial() override;
    void declineContinuingTesting() override;
    void acceptContinuingTesting() override;
    void showContinueTestingDialogWithResultsWhenComplete() override;
    void readyNextTrialIfNeeded() override;
    void playNextTrialIfNeeded() override;
    void becomeChild(Presenter *p) override;

  private:
    Model &model;
    View &mainView;
    ExperimenterResponder::EventListener *listener{};
    Presenter *parent{};
};

class ExperimenterPresenterImpl : public ExperimenterPresenter {
  public:
    explicit ExperimenterPresenterImpl(ExperimenterOutputView &);
    void start() override;
    void stop() override;
    void notifyThatTrialHasStarted() override;
    void notifyThatTrialHasCompleted() override;
    void notifyThatNextTrialIsReady() override;
    void display(const std::string &s) override;
    void secondaryDisplay(const std::string &s) override;
    void showContinueTestingDialog() override;
    void setContinueTestingDialogMessage(const std::string &s) override;

  private:
    ExperimenterOutputView &view;
};
}

#endif
