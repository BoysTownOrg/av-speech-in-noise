#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IRECOGNITIONTESTMODELHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IRECOGNITIONTESTMODELHPP_

#include "Player.hpp"
#include "IModel.hpp"
#include "TestMethod.hpp"

#include <stdexcept>
#include <string>
#include <string_view>

namespace av_speech_in_noise {
class RunningATest {
  public:
    class RequestFailure : public std::runtime_error {
      public:
        explicit RequestFailure(const std::string &s) : std::runtime_error{s} {}
    };

    class TestObserver {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TestObserver);
        virtual void notifyThatNewTestIsReady(std::string_view session) {}
        virtual void notifyThatTrialWillBegin(int trialNumber) {}
        virtual void notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &) {}
        virtual void notifyThatStimulusHasEnded() {}
        virtual void notifyThatSubjectHasResponded() {}
    };

    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(RunningATest);
    virtual void attach(RunningATestFacade::Observer *) = 0;
    virtual void initialize(
        TestMethod *, const Test &, TestObserver * = nullptr) = 0;
    virtual void playTrial(const AudioSettings &) = 0;
    virtual void playCalibration(const Calibration &) = 0;
    virtual void playLeftSpeakerCalibration(const Calibration &) = 0;
    virtual void playRightSpeakerCalibration(const Calibration &) = 0;
    virtual void submit(const coordinate_response_measure::Response &) = 0;
    virtual auto testComplete() -> bool = 0;
    virtual auto audioDevices() -> AudioDevices = 0;
    virtual auto trialNumber() -> int = 0;
    virtual auto targetFileName() -> std::string = 0;
    virtual void prepareNextTrialIfNeeded() = 0;
    virtual auto playTrialTime() -> std::string = 0;
};
}

#endif
