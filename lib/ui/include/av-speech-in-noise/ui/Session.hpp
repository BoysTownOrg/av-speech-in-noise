#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SESSION_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SESSION_HPP_

#include "Task.hpp"
#include "Presenter.hpp"

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/core/IModel.hpp>

#include <vector>
#include <string>
#include <string_view>

namespace av_speech_in_noise {
class SessionController {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatTestIsComplete() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SessionController);
    virtual void notifyThatTestIsComplete() = 0;
    virtual void prepare(TaskPresenter &) = 0;
    virtual void attach(Observer *) {}
};

class SessionPresenter : public Presenter {};

class SessionView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SessionView);
    virtual void populateAudioDeviceMenu(std::vector<std::string>) = 0;
    virtual void populateSubjectScreenMenu(const std::vector<Screen> &) = 0;
    virtual auto screens() -> std::vector<Screen> = 0;
    virtual void showErrorMessage(std::string_view) = 0;
    virtual auto subjectScreen() -> Screen = 0;
};

class SessionControl {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SessionControl);
    virtual auto audioDevice() -> std::string = 0;
};
}

#endif
