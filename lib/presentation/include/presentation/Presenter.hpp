#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_

#include "TestSetup.hpp"
#include "Task.hpp"
#include "Experimenter.hpp"
#include "View.hpp"
#include "Input.hpp"
#include "PresenterSimple.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <vector>
#include <string>

namespace av_speech_in_noise {

class Presenter : public Model::EventListener,
                  public ParentPresenter,
                  public SomethingIDK {
  public:
    class Experimenter {
      public:
        explicit Experimenter(ExperimenterView *, ExperimenterInputView * = {},
            ExperimenterOutputView * = {});
        void becomeChild(Presenter *parent);
        void readyNextTrial();
        void showContinueTestingDialog();
        void hideSubmissions();
        void setContinueTestingDialogMessage(const std::string &);
        void display(std::string);
        void secondaryDisplay(std::string);

      private:
        Presenter *parent{};
        ExperimenterView *view;
        ExperimenterInputView *inputView;
        ExperimenterOutputView *outputView;
    };

    Presenter(Model &, View &, Experimenter &, TaskResponder *, TaskPresenter *,
        TaskResponder *, TaskPresenter *, TaskResponder *, TaskPresenter *,
        TaskResponder *, TaskPresenter *, TaskResponder *, TaskPresenter *,
        TestSetupResponder *, TestSetupPresenter *,
        ExperimenterResponder * = {}, ExperimenterPresenter * = {});
    void trialComplete() override;
    void playTrial() override;
    void playNextTrialIfNeeded() override;
    void readyNextTrialIfNeeded() override;
    void showContinueTestingDialogWithResultsWhenComplete() override;
    void run();
    void switchToTestSetupView();
    void prepare(Method m) override {
        switchToTestView(m);
        taskPresenter_ = taskPresenter(m);
    }

    static constexpr RealLevel fullScaleLevel{119};
    static constexpr SNR ceilingSnr{20};
    static constexpr SNR floorSnr{-40};
    static constexpr auto trackBumpLimit{10};

  private:
    void readyNextTrialAfter(void (Presenter::*f)());
    void showErrorMessage(std::string);
    void showTest(Method);
    void switchToTestView(Method);
    auto taskPresenter(Method) -> TaskPresenter *;

    Model &model;
    View &view;
    Experimenter &experimenterPresenter;
    TaskPresenter *consonantPresenter;
    TaskPresenter *coordinateResponseMeasurePresenter;
    TaskPresenter *freeResponsePresenter;
    TaskPresenter *correctKeywordsPresenter;
    TaskPresenter *passFailPresenter;
    TaskPresenter *taskPresenter_;
    PresenterSimple *testSetupPresenter;
    ExperimenterPresenter *experimenterPresenterRefactored;
};

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

  private:
    ExperimenterOutputView &view;
};
}

#endif
