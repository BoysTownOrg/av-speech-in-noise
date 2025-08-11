#include "AudioRecorderStub.hpp"
#include "ConfigurationRegistryStub.hpp"
#include "OutputFileStub.hpp"
#include "RunningATestStub.hpp"
#include "TimeStampStub.hpp"
#include "assert-utility.hpp"

#include <av-speech-in-noise/core/AudioRecording.hpp>

#include <gtest/gtest.h>

namespace av_speech_in_noise {
class AudioRecordingTests : public ::testing::Test {
  protected:
    AudioRecorderStub audioRecorder;
    OutputFileStub outputFile;
    TimeStampStub timeStamp;
    ConfigurationRegistryStub registry;
    RunningATestStub runningATest;
    AudioRecording audioRecording{
        registry, audioRecorder, outputFile, timeStamp, runningATest};
};

#define AUDIO_RECORDING_TEST(a) TEST_F(AudioRecordingTests, a)

AUDIO_RECORDING_TEST(initializesRecorderWhenTrialWillBegin) {
    outputFile.setParentPath("/Users/user/data");
    audioRecording.notifyThatNewTestIsReady("smile");
    timeStamp.setYear(1);
    timeStamp.setMonth(2);
    timeStamp.setDayOfMonth(7);
    timeStamp.setHour(4);
    timeStamp.setMinute(5);
    timeStamp.setSecond(6);
    audioRecording.notifyThatTrialWillBegin(3);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/data/3-smile-1-2-7-4-5-6.wav",
        audioRecorder.fileUrl().path);
}

AUDIO_RECORDING_TEST(generateFileNameCapturesTimePriorToQueries) {
    audioRecording.notifyThatTrialWillBegin(1);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(beginsWith(timeStamp.log(), "capture"));
}

AUDIO_RECORDING_TEST(startsRecordingWhenTargetAboutToStart) {
    audioRecording.notifyThatTargetWillPlayAt({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(audioRecorder.started());
}

AUDIO_RECORDING_TEST(stopsRecordingWhenSubjectResponds) {
    audioRecording.notifyThatSubjectHasResponded();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(audioRecorder.stopped());
}
}
