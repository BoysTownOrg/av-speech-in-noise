#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FREERESPONSE_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FREERESPONSE_HPP_

#include "Task.hpp"
#include "Test.hpp"
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class FreeResponseControl {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(FreeResponseControl);
    virtual void attach(Observer *) = 0;
    virtual auto flagged() -> bool = 0;
    virtual auto freeResponse() -> std::string = 0;
};

class FreeResponseView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(FreeResponseView);
    virtual void showFreeResponseSubmission() = 0;
    virtual void hideFreeResponseSubmission() = 0;
    virtual void clearFreeResponse() = 0;
    virtual void clearFlag() = 0;
};

class FreeResponseController : public TaskController,
                               public FreeResponseControl::Observer {
  public:
    FreeResponseController(TestController &, Model &, FreeResponseControl &);
    void notifyThatSubmitButtonHasBeenClicked() override;

  private:
    TestController &testController;
    Model &model;
    FreeResponseControl &control;
};

class FreeResponsePresenter : public TaskPresenter {
  public:
    FreeResponsePresenter(TestView &, FreeResponseView &);
    void start() override;
    void stop() override;
    void hideResponseSubmission() override;
    void showResponseSubmission() override;

  private:
    TestView &testView;
    FreeResponseView &view;
};
}

#endif
