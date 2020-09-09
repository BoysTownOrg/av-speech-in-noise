#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_

#include "TestSetup.hpp"
#include "Task.hpp"
#include "Experimenter.hpp"
#include "View.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class Presenter : public Model::EventListener,
                  public SomethingIDK,
                  public IPresenter {
  public:
    Presenter(Model &, View &, TaskResponder *consonantResponder,
        TaskPresenter *consonantPresenter,
        TaskResponder *coordinateResponseMeasureResponder,
        TaskPresenter *coordinateResponseMeasurePresenter,
        TaskResponder *freeResponseResponder,
        TaskPresenter *freeResponsePresenter,
        TaskResponder *correctKeywordsResponder,
        TaskPresenter *correctKeywordsPresenter,
        TaskResponder *passFailResponder, TaskPresenter *passFailPresenter,
        TestSetupResponder *testSetupResponder,
        TestSetupPresenter *testSetupPresenter,
        ExperimenterResponder *experimenterResponder,
        ExperimenterPresenter *experimenterPresenter);
    void trialComplete() override;
    void readyNextTrial() override;
    void switchToTestSetupView() override;
    void prepare(Method m) override;
    void run();

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
    TaskPresenter *consonantPresenter;
    TaskPresenter *coordinateResponseMeasurePresenter;
    TaskPresenter *freeResponsePresenter;
    TaskPresenter *correctKeywordsPresenter;
    TaskPresenter *passFailPresenter;
    TaskPresenter *taskPresenter_;
    PresenterSimple *testSetupPresenter;
    ExperimenterPresenter *experimenterPresenter;
};
}

#endif
