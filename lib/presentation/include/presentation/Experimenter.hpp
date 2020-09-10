#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EXPERIMENTER_HPP_

#include "PresenterSimple.hpp"
#include "Method.hpp"
#include <string>

namespace av_speech_in_noise {
class ExperimenterInputView {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void exitTest() = 0;
        virtual void playTrial() = 0;
        virtual void declineContinuingTesting() = 0;
        virtual void acceptContinuingTesting() = 0;
    };
    virtual void subscribe(EventListener *) = 0;
    virtual ~ExperimenterInputView() = default;
};

class ExperimenterOutputView {
  public:
    virtual ~ExperimenterOutputView() = default;
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

class ExperimenterView : public virtual ExperimenterInputView,
                         public virtual ExperimenterOutputView {};

class IPresenter {
  public:
    virtual ~IPresenter() = default;
    virtual void notifyThatTestIsComplete() = 0;
};

class ExperimenterResponder {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatTrialHasStarted() = 0;
        virtual void setContinueTestingDialogMessage(const std::string &) = 0;
        virtual void showContinueTestingDialog() = 0;
        virtual void display(const std::string &) = 0;
        virtual void secondaryDisplay(const std::string &) = 0;
        virtual void notifyThatNextTrialIsReady() = 0;
    };
    virtual ~ExperimenterResponder() = default;
    virtual void subscribe(EventListener *) = 0;
    virtual void subscribe(IPresenter *) = 0;
    virtual void showContinueTestingDialogWithResultsWhenComplete() = 0;
    virtual void notifyThatUserIsDoneResponding() = 0;
    virtual void notifyThatUserIsReadyForNextTrial() = 0;
};

class ExperimenterPresenter
    : public virtual ExperimenterResponder::EventListener,
      public virtual PresenterSimple {
  public:
    virtual void initialize(Method) = 0;
};
}

#endif
