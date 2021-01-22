#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_METHOD_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_METHOD_HPP_

#include "Task.hpp"
#include <av-speech-in-noise/Interface.hpp>
#include <vector>
#include <string>

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

class SessionView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SessionView);
    virtual void eventLoop() = 0;
    virtual void populateAudioDeviceMenu(std::vector<std::string>) = 0;
    virtual void showErrorMessage(std::string) = 0;
};

class SessionControl {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SessionControl);
    virtual auto browseForDirectory() -> std::string = 0;
    virtual auto browseForOpeningFile() -> std::string = 0;
    virtual auto browseCancelled() -> bool = 0;
    virtual auto audioDevice() -> std::string = 0;
};
}

#endif
