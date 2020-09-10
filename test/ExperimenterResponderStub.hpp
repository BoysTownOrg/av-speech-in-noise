#ifndef AV_SPEECH_IN_NOISE_TESTS_EXPERIMENTERRESPONDERSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TESTS_EXPERIMENTERRESPONDERSTUB_HPP_

#include <presentation/Experimenter.hpp>

namespace av_speech_in_noise {
class ExperimenterResponderStub : public ExperimenterResponder {
  public:
    void subscribe(EventListener *) override {}
    void subscribe(IPresenter *) override {}
    void showContinueTestingDialogWithResultsWhenComplete() override {
        continueTestingDialogShownWithResultsWhenComplete_ = true;
    }
    [[nodiscard]] auto continueTestingDialogShownWithResultsWhenComplete() const
        -> bool {
        return continueTestingDialogShownWithResultsWhenComplete_;
    }
    void readyNextTrialIfNeeded() override {}
    void notifyThatUserIsReadyForNextTrial() override {
        notifiedThatUserIsReadyForNextTrial_ = true;
    }
    [[nodiscard]] auto notifiedThatUserIsReadyForNextTrial() const -> bool {
        return notifiedThatUserIsReadyForNextTrial_;
    }

  private:
    bool notifiedThatUserIsReadyForNextTrial_{};
    bool continueTestingDialogShownWithResultsWhenComplete_{};
};
}

#endif
