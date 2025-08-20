#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EMOTION_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_EMOTION_HPP_

#include "View.hpp"
#include "Task.hpp"
#include "Test.hpp"

#include <av-speech-in-noise/core/Configuration.hpp>
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>
#include <vector>

namespace av_speech_in_noise::submitting_emotion {
class UI : public virtual View {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatPlayButtonHasBeenClicked() = 0;
        virtual void notifyThatResponseButtonHasBeenClicked() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(UI);
    virtual void attach(Observer *) = 0;
    virtual auto emotion() -> Emotion = 0;
    virtual auto playButton() -> View & = 0;
    virtual auto cursor() -> View & = 0;
    virtual auto responseButtons() -> View & = 0;
    virtual void populateResponseButtons(
        const std::vector<std::vector<Emotion>> &) = 0;
};

class SystemTime {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SystemTime);
    virtual auto nowSeconds() -> double = 0;
};

class Presenter : public UI::Observer,
                  public TaskPresenter,
                  public Configurable {
  public:
    Presenter(ConfigurationRegistry &, UI &, TestController &, Interactor &,
        SystemTime &, TestPresenter &);
    void notifyThatPlayButtonHasBeenClicked() override;
    void notifyThatResponseButtonHasBeenClicked() override;
    void start() override;
    void stop() override;
    void showResponseSubmission() override;
    void hideResponseSubmission() override;
    void notifyThatTrialHasStarted() override;
    void configure(const std::string &key, const std::string &value) override;

  private:
    UI &ui;
    TestController &testController;
    Interactor &interactor;
    SystemTime &systemTime;
    TestPresenter &testPresenter;
    double lastResponseShownSeconds{};
};
}

#endif
