#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FIXEDPASSFAIL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_FIXEDPASSFAIL_HPP_

#include "Task.hpp"
#include "Test.hpp"
#include "PassFail.hpp"

#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise::submitting_fixed_pass_fail {
class Presenter : public TaskPresenter,
                  public submitting_pass_fail::UI::Observer {
  public:
    Presenter(
        TestController &, TestView &, Interactor &, submitting_pass_fail::UI &);
    void notifyThatCorrectButtonHasBeenClicked() override;
    void notifyThatIncorrectButtonHasBeenClicked() override;
    void start() override;
    void stop() override;
    void hideResponseSubmission() override;
    void showResponseSubmission() override;

  private:
    TestController &testController;
    Interactor &interactor;
    TestView &testView;
    submitting_pass_fail::UI &ui;
};
}

#endif
