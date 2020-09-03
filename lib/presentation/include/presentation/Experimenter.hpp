#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTER_HPP_

#include <string>

namespace av_speech_in_noise {
class ExperimenterView {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void exitTest() = 0;
        virtual void playTrial() = 0;
        virtual void declineContinuingTesting() = 0;
        virtual void acceptContinuingTesting() = 0;
    };

    virtual ~ExperimenterView() = default;
    virtual void subscribe(EventListener *) = 0;
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void showContinueTestingDialog() = 0;
    virtual void hideContinueTestingDialog() = 0;
    virtual void setContinueTestingDialogMessage(const std::string &) = 0;
    virtual void hideExitTestButton() = 0;
    virtual void showExitTestButton() = 0;
    virtual void showNextTrialButton() = 0;
    virtual void hideNextTrialButton() = 0;
    virtual void display(std::string) = 0;
    virtual void secondaryDisplay(std::string) = 0;
};
}

#endif
