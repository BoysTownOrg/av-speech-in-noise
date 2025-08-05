#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SYLLABLES_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SYLLABLES_HPP_

#include "Task.hpp"
#include "Test.hpp"
#include "View.hpp"
#include "av-speech-in-noise/core/Configuration.hpp"
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <map>
#include <string>

namespace av_speech_in_noise::submitting_syllable {
class View : public av_speech_in_noise::View {
  public:
    virtual void clearFlag() = 0;
};

class Control {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatResponseButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Control);
    virtual void attach(Observer *) = 0;
    virtual auto syllable() -> std::string = 0;
    virtual auto flagged() -> bool = 0;
};

class Presenter {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Presenter);
};

class Controller : public Control::Observer {
  public:
    Controller(Control &, TestController &, Interactor &,
        std::map<std::string, Syllable, std::less<>> map);
    void notifyThatResponseButtonHasBeenClicked() override;

  private:
    std::map<std::string, Syllable, std::less<>> map;
    Control &control;
    TestController &testController;
    Interactor &model;
};

class PresenterImpl : public Presenter,
                      public TaskPresenter,
                      public Configurable {
  public:
    PresenterImpl(ConfigurationRegistry &, View &, TestView &, TestPresenter &);
    void start() override;
    void stop() override;
    void showResponseSubmission() override;
    void hideResponseSubmission() override;
    void configure(const std::string &, const std::string &) override;

  private:
    View &view;
    TestView &testView;
    TestPresenter &testPresenter;
};
}

#endif
