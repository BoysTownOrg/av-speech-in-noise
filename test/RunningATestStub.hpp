#ifndef AV_SPEECH_IN_NOISE_TEST_RUNNINGATESTSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_RUNNINGATESTSTUB_HPP_

#include "av-speech-in-noise/Model.hpp"
#include <av-speech-in-noise/core/IRecognitionTestModel.hpp>

namespace av_speech_in_noise {
class RunningATestStub : public RunningATest {
  public:
    void attach(RunningATestFacade::Observer *a) override {
        facadeObserver = a;
    }
    void initialize(TestMethod *tm, const Test &t, Observer *p) override {
        test = t;
        testMethod = tm;
        observer = p;
    }
    void playTrial(const AudioSettings &) override {}
    void playCalibration(const Calibration &c) override {
        calibration_ = c;
        if (failOnRequest)
            throw RunningATestFacade::RequestFailure{errorMessage};
    }
    void playLeftSpeakerCalibration(const Calibration &c) override {
        leftSpeakerCalibration_ = c;
        if (failOnRequest)
            throw RunningATestFacade::RequestFailure{errorMessage};
    }
    void playRightSpeakerCalibration(const Calibration &c) override {
        rightSpeakerCalibration_ = c;
        if (failOnRequest)
            throw RunningATestFacade::RequestFailure{errorMessage};
    }
    void submit(const coordinate_response_measure::Response &) override {}
    auto testComplete() -> bool override { return testComplete_; }
    auto audioDevices() -> AudioDevices override { return audioDevices_; }
    auto trialNumber() -> int override { return trialNumber_; }
    auto targetFileName() -> std::string override { return targetFileName_; }
    void prepareNextTrialIfNeeded() override {
        nextTrialPreparedIfNeeded_ = true;
    }
    auto playTrialTime() -> std::string override { return {}; }

    Calibration calibration_;
    Calibration leftSpeakerCalibration_;
    Calibration rightSpeakerCalibration_;
    AudioDevices audioDevices_;
    Test test;
    RunningATestFacade::Observer *facadeObserver;
    const TestMethod *testMethod{};
    const Observer *observer{};
    std::string targetFileName_;
    std::string errorMessage;
    int trialNumber_{};
    bool testComplete_{};
    bool failOnRequest{};
    bool nextTrialPreparedIfNeeded_{};
};
}

#endif
