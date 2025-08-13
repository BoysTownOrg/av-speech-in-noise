#ifndef AV_SPEECH_IN_NOISE_TEST_RUNNINGATESTSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_RUNNINGATESTSTUB_HPP_

#include <av-speech-in-noise/core/IRunningATest.hpp>

namespace av_speech_in_noise {
class RunningATestStub : public RunningATest {
  public:
    void attach(RunningATest::RequestObserver *a) override {
        facadeObserver = a;
    }
    void initialize(TestMethod *tm) override {
        testMethod = tm;
        if (failOnRequest)
            throw RequestFailure{errorMessage};
    }
    void playTrial(const AudioSettings &t) override { trialAudioSettings = t; }
    void playCalibration(const Calibration &c) override {
        calibration_ = c;
        if (failOnRequest)
            throw RequestFailure{errorMessage};
    }
    void playLeftSpeakerCalibration(const Calibration &c) override {
        leftSpeakerCalibration_ = c;
        if (failOnRequest)
            throw RequestFailure{errorMessage};
    }
    void playRightSpeakerCalibration(const Calibration &c) override {
        rightSpeakerCalibration_ = c;
        if (failOnRequest)
            throw RequestFailure{errorMessage};
    }
    void submit(const coordinate_response_measure::Response &r) override {
        coordinateResponse = r;
    }
    auto testComplete() -> bool override { return testComplete_; }
    auto audioDevices() -> AudioDevices override { return audioDevices_; }
    auto trialNumber() -> int override { return trialNumber_; }
    auto targetFileName() -> std::string override { return targetFileName_; }
    void prepareNextTrialIfNeeded() override {
        nextTrialPreparedIfNeeded_ = true;
    }
    auto playTrialTime() -> std::string override { return {}; }
    void add(TestObserver &) override {}
    void remove(TestObserver &) override {}

    Calibration calibration_;
    Calibration leftSpeakerCalibration_;
    Calibration rightSpeakerCalibration_;
    AudioSettings trialAudioSettings;
    AudioDevices audioDevices_;
    coordinate_response_measure::Response coordinateResponse;
    RunningATest::RequestObserver *facadeObserver;
    const TestMethod *testMethod{};
    std::vector<std::reference_wrapper<TestObserver>> observer{};
    std::string targetFileName_;
    std::string errorMessage;
    int trialNumber_{};
    bool testComplete_{};
    bool failOnRequest{};
    bool nextTrialPreparedIfNeeded_{};
};
}

#endif
