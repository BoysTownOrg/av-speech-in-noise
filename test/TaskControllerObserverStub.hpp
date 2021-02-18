#ifndef AV_SPEECH_IN_NOISE_TESTS_TASKCONTROLLEROBSERVERSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TESTS_TASKCONTROLLEROBSERVERSTUB_HPP_

#include <presentation/Task.hpp>

namespace av_speech_in_noise {
class TaskControllerObserverStub : public TaskController::Observer {
  public:
    void notifyThatTaskHasStarted() override {
        notifiedThatTaskHasStarted_ = true;
    }

    [[nodiscard]] auto notifiedThatTaskHasStarted() const -> bool {
        return notifiedThatTaskHasStarted_;
    }

    [[nodiscard]] auto notifiedThatTrialHasStarted() const -> bool {
        return notifiedThatTrialHasStarted_;
    }

    void notifyThatTrialHasStarted() override {
        notifiedThatTrialHasStarted_ = true;
    }

  private:
    bool notifiedThatTrialHasStarted_{};
    bool notifiedThatTaskHasStarted_{};
};
}

#endif
