#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTERIMPL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTERIMPL_HPP_

#include "Experimenter.hpp"
#include "View.hpp"
#include "Task.hpp"
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class ExperimenterResponderImpl : public ExperimenterInputView::EventListener,
                                  public ExperimenterResponder {
  public:
    explicit ExperimenterResponderImpl(Model &, View &, ExperimenterInputView &,
        TaskResponder *consonantResponder, TaskPresenter *consonantPresenter,
        TaskResponder *coordinateResponseMeasureResponder,
        TaskPresenter *coordinateResponseMeasurePresenter,
        TaskResponder *freeResponseResponder,
        TaskPresenter *freeResponsePresenter,
        TaskResponder *correctKeywordsResponder,
        TaskPresenter *correctKeywordsPresenter,
        TaskResponder *passFailResponder, TaskPresenter *passFailPresenter);
    void subscribe(ExperimenterResponder::EventListener *e) override;
    void subscribe(IPresenter *p) override;
    void exitTest() override;
    void playTrial() override;
    void declineContinuingTesting() override;
    void acceptContinuingTesting() override;
    void showContinueTestingDialogWithResultsWhenComplete() override;
    void readyNextTrialIfNeeded() override;
    void playNextTrialIfNeeded() override;
    void nextTrial() override;

  private:
    Model &model;
    View &mainView;
    ExperimenterResponder::EventListener *listener{};
    IPresenter *parent{};
};

class ExperimenterPresenterImpl : public Model::EventListener,
                                  public ExperimenterPresenter {
  public:
    explicit ExperimenterPresenterImpl(Model &, ExperimenterOutputView &,
        TaskPresenter *consonantPresenter,
        TaskPresenter *coordinateResponseMeasurePresenter,
        TaskPresenter *freeResponsePresenter,
        TaskPresenter *correctKeywordsPresenter,
        TaskPresenter *passFailPresenter);
    void trialComplete() override;
    void start() override;
    void stop() override;
    void notifyThatTrialHasStarted() override;
    void notifyThatNextTrialIsReady() override;
    void display(const std::string &s) override;
    void secondaryDisplay(const std::string &s) override;
    void showContinueTestingDialog() override;
    void setContinueTestingDialogMessage(const std::string &s) override;
    void initialize(Method) override;

  private:
    auto taskPresenter(Method m) -> TaskPresenter *;

    Model &model;
    ExperimenterOutputView &view;
    TaskPresenter *consonantPresenter;
    TaskPresenter *coordinateResponseMeasurePresenter;
    TaskPresenter *freeResponsePresenter;
    TaskPresenter *correctKeywordsPresenter;
    TaskPresenter *passFailPresenter;
    TaskPresenter *taskPresenter_;
};
}

#endif
