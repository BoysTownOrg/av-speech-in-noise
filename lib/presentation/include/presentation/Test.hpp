#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TEST_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TEST_HPP_

#include "View.hpp"
#include "Presenter.hpp"
#include "Session.hpp"
#include "Task.hpp"
#include <av-speech-in-noise/Interface.hpp>
#include <string>

namespace av_speech_in_noise {
class TestControl {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void exitTest() = 0;
        virtual void playTrial() = 0;
        virtual void declineContinuingTesting() = 0;
        virtual void acceptContinuingTesting() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TestControl);
    virtual void attach(Observer *) = 0;
};

class TestView : public virtual View {
  public:
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

class TestController {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatTrialHasStarted() = 0;
        virtual void setContinueTestingDialogMessage(const std::string &) = 0;
        virtual void showContinueTestingDialog() = 0;
        virtual void display(const std::string &) = 0;
        virtual void secondaryDisplay(const std::string &) = 0;
        virtual void hideResponseSubmission() {}
        virtual void notifyThatNextTrialIsReady() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TestController);
    virtual void attach(Observer *) {}
    virtual void attach(SessionController *) {}
    virtual void
    notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion() = 0;
    virtual void notifyThatUserIsDoneResponding() = 0;
    virtual void notifyThatUserIsReadyForNextTrial() = 0;
    virtual void notifyThatUserIsDoneRespondingAndIsReadyForNextTrial() {}
};

class TestPresenter : public virtual TestController::Observer,
                      public virtual Presenter {
  public:
    virtual void initialize(TaskPresenter &) = 0;
};
}

#endif
