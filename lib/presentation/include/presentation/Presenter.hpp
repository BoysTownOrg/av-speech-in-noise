#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_

#include "TestSetup.hpp"
#include "Experimenter.hpp"
#include "View.hpp"
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class Presenter : public SomethingIDK, public IPresenter {
  public:
    Presenter(Model &, View &, TestSetupResponder *testSetupResponder,
        TestSetupPresenter *testSetupPresenter,
        ExperimenterResponder *experimenterResponder,
        ExperimenterPresenter *experimenterPresenter);
    void switchToTestSetupView() override;
    void prepare(Method m) override;
    void run();

    static constexpr RealLevel fullScaleLevel{119};
    static constexpr SNR ceilingSnr{20};
    static constexpr SNR floorSnr{-40};
    static constexpr auto trackBumpLimit{10};

  private:
    View &view;
    PresenterSimple *testSetupPresenter;
    ExperimenterPresenter *experimenterPresenter;
};
}

#endif
