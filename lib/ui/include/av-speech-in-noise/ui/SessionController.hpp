#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SESSIONCONTROLLER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SESSIONCONTROLLER_HPP_

#include "TestSetup.hpp"
#include "Test.hpp"
#include "Session.hpp"
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class SessionControllerImpl : public SessionController {
  public:
    SessionControllerImpl(
        Model &, SessionView &, TestSetupPresenter &, TestPresenter &);
    void notifyThatTestIsComplete() override;
    void prepare(TaskPresenter &) override;
    void attach(Observer *) override;

    static constexpr RealLevel fullScaleLevel{119};
    static constexpr SNR ceilingSnr{20};
    static constexpr SNR floorSnr{-40};
    static constexpr auto trackBumpLimit{10};

  private:
    Presenter &testSetupPresenter;
    TestPresenter &testPresenter;
    Observer *observer{};
};
}

#endif
