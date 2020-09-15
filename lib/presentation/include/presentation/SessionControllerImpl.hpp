#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SESSIONCONTROLLERIMPL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SESSIONCONTROLLERIMPL_HPP_

#include "TestSetup.hpp"
#include "Test.hpp"
#include "SessionView.hpp"
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class SessionControllerImpl : public SessionController {
  public:
    SessionControllerImpl(Model &, SessionView &,
        TestSetupPresenter *testSetupPresenter,
        TestPresenter *experimenterPresenter);
    void notifyThatTestIsComplete() override;
    void prepare(Method m) override;
    void run();

    static constexpr RealLevel fullScaleLevel{119};
    static constexpr SNR ceilingSnr{20};
    static constexpr SNR floorSnr{-40};
    static constexpr auto trackBumpLimit{10};

  private:
    SessionView &view;
    Presenter *testSetupPresenter;
    TestPresenter *experimenterPresenter;
};
}

#endif
