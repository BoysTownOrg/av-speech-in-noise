#ifndef AV_SPEECH_IN_NOISE_TEST_RUNNINGATESTSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_RUNNINGATESTSTUB_HPP_

#include <av-speech-in-noise/core/IRecognitionTestModel.hpp>

namespace av_speech_in_noise {
class RunningATestStub : public RunningATest {
  public:
    void attach(RunningATestFacade::Observer *) override {}
    void initialize(TestMethod *tm, const Test &t, Observer *p) override {
        test = t;
        testMethod = tm;
        observer = p;
    }
    void playTrial(const AudioSettings &) override {}
    void playCalibration(const Calibration &) override {}
    void playLeftSpeakerCalibration(const Calibration &) override {}
    void playRightSpeakerCalibration(const Calibration &) override {}
    void submit(const coordinate_response_measure::Response &) override {}
    auto testComplete() -> bool override { return false; }
    auto audioDevices() -> AudioDevices override { return {}; }
    auto trialNumber() -> int override { return 0; }
    auto targetFileName() -> std::string override { return targetFileName_; }
    void prepareNextTrialIfNeeded() override {}
    auto playTrialTime() -> std::string override { return {}; }

    Test test;
    const TestMethod *testMethod{};
    const Observer *observer{};
    std::string targetFileName_;
};
}

#endif
