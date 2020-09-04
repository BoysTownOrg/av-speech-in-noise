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
    class Experimenter : public ExperimenterInputView::EventListener {
      public:
        explicit Experimenter(ExperimenterView *, ExperimenterInputView * = {},
            ExperimenterOutputView * = {});
        void exitTest() override;
        void playTrial() override;
        void declineContinuingTesting() override;
        void acceptContinuingTesting() override;
        void becomeChild(Presenter *parent);
        void show();
        void start();
        void stop();
        void trialPlayed();
        void trialComplete();
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
        TestSetupResponder *, TestSetupPresenter *);
    void trialComplete() override;
    void playTrial() override;
    void playNextTrialIfNeeded() override;
    void readyNextTrialIfNeeded() override;
    void showContinueTestingDialogWithResultsWhenComplete() override;
    void run();
    auto testComplete() -> bool;
    void declineContinuingTesting();
    void acceptContinuingTesting();
    void exitTest();
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
};

class ExperimenterResponder : public ExperimenterInputView::EventListener {
  public:
    explicit ExperimenterResponder(Model &model) : model{model} {}
    void exitTest() override {}
    void playTrial() override {}
    void declineContinuingTesting() override {}
    void acceptContinuingTesting() override {}
    // void becomeChild(ParentPresenter *p) override;

  private:
    Model &model;
};

class ExperimenterPresenter {
  public:
    explicit ExperimenterPresenter(ExperimenterOutputView &view) : view{view} {}
    void start() { view.show(); }
    void stop() { view.hide(); }

  private:
    ExperimenterOutputView &view;
};
}

#endif
