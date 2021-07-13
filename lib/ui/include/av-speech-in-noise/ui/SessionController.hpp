#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SESSIONCONTROLLER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SESSIONCONTROLLER_HPP_

#include "TestSetup.hpp"
#include "Test.hpp"
#include "Session.hpp"
#include "Subject.hpp"

#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class SessionControllerImpl : public SessionController {
  public:
    SessionControllerImpl(
        TestSetupPresenter &, TestPresenter &, SubjectPresenter &);
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
    SubjectPresenter &subjectPresenter;
    Observer *observer{};
};

class SessionPresenterImpl : public SessionPresenter {
  public:
    SessionPresenterImpl(SessionView &view, Model &model) {
        view.populateAudioDeviceMenu(model.audioDevices());
        view.populateSubjectScreenMenu(view.screens());
    }

    void start() override {}

    void stop() override {}
};
}

#endif
