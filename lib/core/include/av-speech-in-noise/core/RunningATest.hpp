#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_RECOGNITIONTESTMODELHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_RECOGNITIONTESTMODELHPP_

#include "Configuration.hpp"
#include "Randomizer.hpp"
#include "IMaskerPlayer.hpp"
#include "IOutputFile.hpp"
#include "IRunningATest.hpp"
#include "IResponseEvaluator.hpp"
#include "ITargetPlayer.hpp"

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/Model.hpp>

#include <string>

namespace av_speech_in_noise {
class Clock {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(Clock);
    virtual auto time() -> std::string = 0;
};

auto trialDuration(TargetPlayer &target, MaskerPlayer &masker) -> Duration;

class RunningATestImpl : public TargetPlayer::Observer,
                         public MaskerPlayer::Observer,
                         public Configurable,
                         public RunningATest,
                         public Writable {
  public:
    RunningATestImpl(TargetPlayer &, MaskerPlayer &, ResponseEvaluator &,
        OutputFile &, Randomizer &, Clock &, ConfigurationRegistry &);
    void attach(RunningATest::RequestObserver *) override;
    void initialize(TestMethod *, const Test &,
        std::vector<std::reference_wrapper<TestObserver>>) override;
    void playTrial(const AudioSettings &) override;
    void playCalibration(const Calibration &) override;
    void playLeftSpeakerCalibration(const Calibration &) override;
    void playRightSpeakerCalibration(const Calibration &) override;
    void submit(const coordinate_response_measure::Response &) override;
    auto testComplete() -> bool override;
    auto audioDevices() -> AudioDevices override;
    auto trialNumber() -> int override;
    auto targetFileName() -> std::string override;
    void fadeInComplete(const AudioSampleTimeWithOffset &) override;
    void fadeOutComplete() override;
    void prepareNextTrialIfNeeded() override;
    void notifyThatPreRollHasCompleted() override;
    auto playTrialTime() -> std::string override;
    void configure(const std::string &key, const std::string &value) override;
    void write(std::ostream &stream) override;
    static constexpr Delay maskerChannelDelay{0.004};
    static constexpr Duration targetOffsetFringeDuration{
        targetOnsetFringeDuration};

  private:
    Test test;
    RationalNumber videoScale{2, 3};
    MaskerPlayer &maskerPlayer;
    TargetPlayer &targetPlayer;
    ResponseEvaluator &evaluator;
    OutputFile &outputFile;
    Randomizer &randomizer;
    Clock &clock;
    std::string playTrialTime_;
    std::vector<std::reference_wrapper<TestObserver>> testObservers;
    RunningATest::RequestObserver *requestObserver{};
    TestMethod *testMethod{};
    int trialNumber_{};
    Condition condition{};
    bool trialInProgress_{};
    bool keepVideoShown{};
};
}

#endif
