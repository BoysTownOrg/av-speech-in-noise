#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FIXEDPASSFAIL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FIXEDPASSFAIL_HPP_

#include "Task.hpp"
#include "Test.hpp"
#include "PassFail.hpp"

#include <av-speech-in-noise/core/Configuration.hpp>
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise::submitting_fixed_pass_fail {
class Presenter : public TaskPresenter,
                  public submitting_pass_fail::UI::Observer,
                  public Configurable {
  public:
    Presenter(ConfigurationRegistry &, TestController &, TestView &,
        Interactor &, submitting_pass_fail::UI &, TestPresenter &);
    void notifyThatCorrectButtonHasBeenClicked() override;
    void notifyThatIncorrectButtonHasBeenClicked() override;
    void notifyThatShowAnswerButtonHasBeenClicked() override {}
    void start() override;
    void stop() override;
    void hideResponseSubmission() override;
    void showResponseSubmission() override;
    void configure(const std::string &key, const std::string &value) override;

  private:
    TestController &testController;
    Interactor &interactor;
    TestView &testView;
    submitting_pass_fail::UI &ui;
    TestPresenter &testPresenter;
};
}

#endif
