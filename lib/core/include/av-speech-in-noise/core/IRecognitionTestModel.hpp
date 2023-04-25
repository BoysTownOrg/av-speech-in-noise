#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IRECOGNITIONTESTMODELHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_IRECOGNITIONTESTMODELHPP_

#include "Player.hpp"
#include "IModel.hpp"
#include "TestMethod.hpp"

#include <string>
#include <string_view>

namespace av_speech_in_noise {
class RunningATest {
  public:
    class Observer {
      public:
        AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Observer);
        virtual void notifyThatNewTestIsReady(std::string_view session) = 0;
        virtual void notifyThatTrialWillBegin(int trialNumber) = 0;
        virtual void notifyThatTargetWillPlayAt(
            const PlayerTimeWithDelay &) = 0;
        virtual void notifyThatStimulusHasEnded() = 0;
        virtual void notifyThatSubjectHasResponded() = 0;
    };
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(RunningATest);
    virtual void attach(RunningATestFacade::Observer *) = 0;
    virtual void initialize(
        TestMethod *, const Test &, Observer * = nullptr) = 0;
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
