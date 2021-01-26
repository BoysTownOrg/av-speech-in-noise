#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SYLLABLES_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SYLLABLES_HPP_

#include "Task.hpp"
#include "Test.hpp"
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <map>
#include <string>

namespace av_speech_in_noise {
class SyllablesView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SyllablesView);
    virtual void hide() = 0;
    virtual void show() = 0;
};

class SyllablesControl {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatResponseButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SyllablesControl);
    virtual void attach(Observer *) = 0;
    virtual auto syllable() -> std::string = 0;
};

class SyllablesPresenter {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SyllablesPresenter);
};

class SyllablesController : public SyllablesControl::Observer {
  public:
    SyllablesController(SyllablesControl &, TestController &, Model &,
        std::map<std::string, Syllable, std::less<>> map);
    void notifyThatResponseButtonHasBeenClicked() override;

  private:
    std::map<std::string, Syllable, std::less<>> map;
    SyllablesControl &control;
    TestController &testController;
    Model &model;
};

class SyllablesPresenterImpl : public SyllablesPresenter, public TaskPresenter {
  public:
    SyllablesPresenterImpl(SyllablesView &, TestView &);
    void start() override;
    void stop() override;
    void showResponseSubmission() override;
    void hideResponseSubmission() override;

  private:
    SyllablesView &view;
    TestView &testView;
};
}

#endif
