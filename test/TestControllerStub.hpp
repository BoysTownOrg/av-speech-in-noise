#ifndef AV_SPEECH_IN_NOISE_TESTS_TESTCONTROLLERSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TESTS_TESTCONTROLLERSTUB_HPP_

#include <presentation/Test.hpp>

namespace av_speech_in_noise {
class TestControllerStub : public TestController {
  public:
    void attach(Observer *) override {}
    void attach(SessionController *) override {}
    void notifyThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion()
        override {
        notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion_ =
            true;
    }
    [[nodiscard]] auto
    notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion()
        const -> bool {
        return notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion_;
    }
    void notifyThatUserIsDoneResponding() override {
        notifiedThatUserIsDoneResponding_ = true;
    }
    [[nodiscard]] auto notifiedThatUserIsDoneResponding() const -> bool {
        return notifiedThatUserIsDoneResponding_;
    }
    void notifyThatUserIsReadyForNextTrial() override {
        notifiedThatUserIsReadyForNextTrial_ = true;
    }
    [[nodiscard]] auto notifiedThatUserIsReadyForNextTrial() const -> bool {
        return notifiedThatUserIsReadyForNextTrial_;
    }
    [[nodiscard]] auto
    notifiedThatUserIsDoneRespondingForATestThatCongratulatesAtTheEnd() const
        -> bool {
        return notifiedThatUserIsDoneRespondingForATestThatCongratulatesAtTheEnd_;
    }
    void
    notifyThatUserIsDoneRespondingForATestThatCongratulatesAtTheEnd() override {
        notifiedThatUserIsDoneRespondingForATestThatCongratulatesAtTheEnd_ =
            true;
    }

  private:
    bool notifiedThatUserIsReadyForNextTrial_{};
    bool
        notifiedThatUserIsDoneRespondingForATestThatMayContinueAfterCompletion_{};
    bool notifiedThatUserIsDoneResponding_{};
    bool notifiedThatUserIsDoneRespondingForATestThatCongratulatesAtTheEnd_{};
};
}

#endif
