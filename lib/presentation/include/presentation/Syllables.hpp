#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SYLLABLES_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_SYLLABLES_HPP_

#include "Task.hpp"
#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class SyllablesView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SyllablesView);
    virtual void hide() = 0;
};

class SyllablesControl {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SyllablesControl);
};

class SyllablesPresenter {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(SyllablesPresenter);
};

class SyllablesController : public SyllablesControl::Observer {
  public:
    explicit SyllablesController(SyllablesControl &) {}
};

class SyllablesPresenterImpl : public SyllablesPresenter, public TaskPresenter {
  public:
    explicit SyllablesPresenterImpl(SyllablesView &);
    void start() override;
    void stop() override;
    void showResponseSubmission() override;
    void hideResponseSubmission() override;

  private:
    SyllablesView &view;
};
}

#endif
