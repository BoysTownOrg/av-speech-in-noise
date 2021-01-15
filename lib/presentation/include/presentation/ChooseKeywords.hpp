#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CHOOSEKEYWORDS_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_CHOOSEKEYWORDS_HPP_

#include "View.hpp"
#include "Task.hpp"
#include <av-speech-in-noise/Interface.hpp>

namespace av_speech_in_noise {
class ChooseKeywordsView {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(ChooseKeywordsView);
};

class ChooseKeywordsPresenter : public TaskPresenter {
  public:
    ChooseKeywordsPresenter(TestView &, ChooseKeywordsView &);
    void start() override;
    void stop() override;
    void notifyThatTaskHasStarted() override;
    void notifyThatUserIsDoneResponding() override;
    void notifyThatTrialHasStarted() override;
    void showResponseSubmission() override;

  private:
    TestView &testView;
    ChooseKeywordsView &view;
};
}

#endif
