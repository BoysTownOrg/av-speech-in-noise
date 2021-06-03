#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SESSIONCONTROLLER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SESSIONCONTROLLER_HPP_

#include "TestSetup.hpp"
#include "Test.hpp"
#include "Session.hpp"
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class SubjectView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SubjectView);
    virtual void moveToScreen(int index) = 0;
};

class SessionControllerImpl : public SessionController {
  public:
    SessionControllerImpl(Model &, SessionView &, SubjectView &,
        TestSetupPresenter &, TestPresenter &);
    void notifyThatTestIsComplete() override;
    void prepare(TaskPresenter &) override;
    void attach(Observer *) override;

    static constexpr RealLevel fullScaleLevel{119};
    static constexpr SNR ceilingSnr{20};
    static constexpr SNR floorSnr{-40};
    static constexpr auto trackBumpLimit{10};

  private:
    SessionView &view;
    SubjectView &subjectView;
    Presenter &testSetupPresenter;
    TestPresenter &testPresenter;
    Observer *observer{};
};
}

#endif
