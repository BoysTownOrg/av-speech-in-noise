#include "AudioRecorderStub.hpp"
#include "OutputFileStub.hpp"
#include "assert-utility.hpp"

#include <av-speech-in-noise/core/AudioRecording.hpp>

#include <gtest/gtest.h>

namespace av_speech_in_noise {
class AudioRecordingTests : public ::testing::Test {
  protected:
    AudioRecorderStub audioRecorder;
    OutputFileStub outputFile;
    AudioRecording audioRecording{audioRecorder, outputFile};
};

#define AUDIO_RECORDING_TEST(a) TEST_F(AudioRecordingTests, a)

AUDIO_RECORDING_TEST(initializesRecorderWhenTrialWillBegin) {
    outputFile.setParentPath("/Users/user/data");
    audioRecording.notifyThatNewTestIsReady("smile");
    audioRecording.notifyThatTrialWillBegin(3);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "/Users/user/data/3-smile.wav", audioRecorder.fileUrl().path);
}

AUDIO_RECORDING_TEST(startsRecordingWhenStimulusEnds) {
    audioRecording.notifyThatStimulusHasEnded();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(audioRecorder.started());
}

AUDIO_RECORDING_TEST(stopsRecordingWhenSubjectResponds) {
    audioRecording.notifyThatSubjectHasResponded();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(audioRecorder.stopped());
}
}