#ifndef AV_SPEECH_IN_NOISE_TESTS_TASKRESPONDERLISTENERSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TESTS_TASKRESPONDERLISTENERSTUB_HPP_

#include <presentation/Task.hpp>

namespace av_speech_in_noise {
class TaskResponderListenerStub : public TaskResponder::EventListener {
  public:
    void notifyThatTaskHasStarted() override {
        notifiedThatTaskHasStarted_ = true;
    }
    void notifyThatUserIsDoneResponding() override {
        notifiedThatUserIsDoneResponding_ = true;
    }
    [[nodiscard]] auto notifiedThatUserIsDoneResponding() const -> bool {
        return notifiedThatUserIsDoneResponding_;
    }
    [[nodiscard]] auto notifiedThatTaskHasStarted() const -> bool {
        return notifiedThatTaskHasStarted_;
    }

  private:
    bool notifiedThatUserIsDoneResponding_{};
    bool notifiedThatTaskHasStarted_{};
};
}

#endif
