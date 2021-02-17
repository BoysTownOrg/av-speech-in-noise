#ifndef AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_IRECOGNITIONTEST_HPP_
#define AV_SPEECH_IN_NOISE_RECOGNITION_TEST_INCLUDE_RECOGNITION_TEST_IRECOGNITIONTEST_HPP_

#include "TestMethod.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class RecognitionTestModel {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(RecognitionTestModel);
    virtual void attach(Model::Observer *) = 0;
    virtual void initialize(TestMethod *, const Test &) = 0;
    virtual void initializeWithSingleSpeaker(TestMethod *, const Test &) = 0;
    virtual void initializeWithDelayedMasker(TestMethod *, const Test &) = 0;
    virtual void initializeWithEyeTracking(TestMethod *, const Test &) = 0;
    virtual void playTrial(const AudioSettings &) = 0;
    virtual void playCalibration(const Calibration &) = 0;
    virtual void playLeftSpeakerCalibration(const Calibration &) = 0;
    virtual void playRightSpeakerCalibration(const Calibration &) = 0;
    virtual void submit(const coordinate_response_measure::Response &) = 0;
    virtual void submit(const FreeResponse &) = 0;
    virtual void submit(const ThreeKeywordsResponse &) = 0;
    virtual void submit(const CorrectKeywords &) = 0;
    virtual void submit(const ConsonantResponse &) = 0;
    virtual void submit(const SyllableResponse &) {}
    virtual void submitCorrectResponse() = 0;
    virtual void submitIncorrectResponse() = 0;
    virtual auto testComplete() -> bool = 0;
    virtual auto audioDevices() -> AudioDevices = 0;
    virtual auto trialNumber() -> int = 0;
    virtual auto targetFileName() -> std::string = 0;
    virtual void prepareNextTrialIfNeeded() = 0;
};
}

#endif
