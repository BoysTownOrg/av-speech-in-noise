#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_VIEW_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_VIEW_HPP_

#include <vector>
#include <string>

namespace av_speech_in_noise {
class SessionView {
  public:
    virtual ~SessionView() = default;
    virtual void eventLoop() = 0;
    virtual auto browseForDirectory() -> std::string = 0;
    virtual auto browseForOpeningFile() -> std::string = 0;
    virtual auto audioDevice() -> std::string = 0;
    virtual void populateAudioDeviceMenu(std::vector<std::string>) = 0;
    virtual auto browseCancelled() -> bool = 0;
    virtual void showErrorMessage(std::string) = 0;
};
}

#endif
