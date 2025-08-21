#ifndef AV_SPEECH_IN_NOISE_TEST_TESTPRESENTERSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_TESTPRESENTERSTUB_HPP_

#include <av-speech-in-noise/ui/Test.hpp>

namespace av_speech_in_noise {
class TestPresenterStub : public TestPresenter {
  public:
    void apply(TaskPresenter &p) override { taskPresenter = &p; }
    void notifyThatTrialHasStarted() override {}
    void updateAdaptiveTestResults() override {}
    void updateTrialInformation() override {}
    void hideResponseSubmission() override {}
    void hideExitTestButton() override {}
    void notifyThatNextTrialIsReady() override {}
    void completeTask() override {}
    void start() override {}
    void stop() override {}
    TaskPresenter *taskPresenter{};
};
}

#endif
