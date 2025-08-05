#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTIMPL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTIMPL_HPP_

#include "Test.hpp"
#include "Session.hpp"
#include "Task.hpp"

#include <av-speech-in-noise/core/Configuration.hpp>
#include <av-speech-in-noise/core/IAdaptiveMethod.hpp>
#include <av-speech-in-noise/core/IRunningATest.hpp>
#include <av-speech-in-noise/core/IModel.hpp>
#include <av-speech-in-noise/Model.hpp>

namespace av_speech_in_noise {
class TestControllerImpl : public TestControl::Observer, public TestController {
  public:
    TestControllerImpl(SessionController &, RunningATest &, AdaptiveMethod &,
        SessionControl &, TestControl &, TestPresenter &);
    void exitTest() override;
    void playTrial() override;
    void declineContinuingTesting() override;
    void acceptContinuingTesting() override;
    void notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion()
        override;
    void notifyThatUserIsDoneResponding() override;
    void notifyThatUserIsReadyForNextTrial() override;
    void notifyThatUserIsDoneRespondingAndIsReadyForNextTrial() override;
    void notifyThatUserHasRespondedButTrialIsNotQuiteDone() override;

  private:
    SessionController &sessionController;
    RunningATest &runningATest;
    AdaptiveMethod &adaptiveMethod;
    SessionControl &sessionControl;
    TestPresenter &presenter;
};

class TestPresenterImpl : public RunningATest::RequestObserver,
                          public TestPresenter,
                          public Configurable {
  public:
    TestPresenterImpl(RunningATest &, AdaptiveMethod &, TestView &);
    void subscribe(ConfigurationRegistry &);
    void initialize(TaskPresenter &) override;
    void start() override;
    void stop() override;
    void notifyThatPlayTrialHasCompleted() override;
    void notifyThatTrialHasStarted() override;
    void notifyThatNextTrialIsReady() override;
    void hideResponseSubmission() override;
    void hideExitTestButton() override;
    void updateTrialInformation() override;
    void updateAdaptiveTestResults() override;
    void completeTask() override;
    void configure(const std::string &key, const std::string &value) override;

  private:
    RunningATest &runningATest;
    AdaptiveMethod &adaptiveMethod;
    TestView &view;
    TaskPresenter *taskPresenter;
    bool showTargetFilename{true};
};
}

#endif
