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

TEST_F(AudioRecordingTests,
    playTrialForTestWithAudioRecordingInitializesRecorder) {
    outputFile.setParentPath("/Users/user/data");
    audioRecording.notifyThatNewTestIsReady("smile");
    audioRecording.notifyThatTrialWillBegin(3);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        "/Users/user/data/3-smile.wav", audioRecorder.fileUrl().path);
}
}