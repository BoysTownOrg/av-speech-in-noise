#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_VIEW_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_VIEW_HPP_

#include <vector>
#include <string>

namespace av_speech_in_noise {

class TestSetupInputView {};

class TestSetupOutputView {};

class TestSetupView : public virtual TestSetupOutputView,
                      public virtual TestSetupInputView {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatConfirmButtonHasBeenClicked() = 0;
        virtual void notifyThatPlayCalibrationButtonHasBeenClicked() = 0;
        virtual void notifyThatBrowseForTestSettingsButtonHasBeenClicked() = 0;
    };

    virtual ~TestSetupView() = default;
    virtual void subscribe(EventListener *) = 0;
    virtual void populateTransducerMenu(std::vector<std::string>) = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual auto testSettingsFile() -> std::string = 0;
    virtual auto startingSnr() -> std::string = 0;
    virtual auto testerId() -> std::string = 0;
    virtual auto subjectId() -> std::string = 0;
    virtual auto session() -> std::string = 0;
    virtual auto rmeSetting() -> std::string = 0;
    virtual auto transducer() -> std::string = 0;
    virtual void setTestSettingsFile(std::string) = 0;
};

class View {
  public:
    virtual ~View() = default;
    virtual void eventLoop() = 0;
    virtual auto browseForDirectory() -> std::string = 0;
    virtual auto browseForOpeningFile() -> std::string = 0;
    virtual auto audioDevice() -> std::string = 0;
    virtual void populateAudioDeviceMenu(std::vector<std::string>) = 0;
    virtual auto browseCancelled() -> bool = 0;
    virtual void showErrorMessage(std::string) = 0;
    virtual void showCursor() = 0;
};
}

#endif
