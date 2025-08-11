#include "AudioRecording.hpp"
#include "FileSystemPath.hpp"

#include <sstream>

namespace av_speech_in_noise {
AudioRecording::AudioRecording(ConfigurationRegistry &registry,
    AudioRecorder &audioRecorder, OutputFile &outputFile, TimeStamp &timeStamp,
    RunningATest &runningATest)
    : audioRecorder{audioRecorder}, outputFile{outputFile},
      timeStamp{timeStamp}, runningATest{runningATest} {
    registry.subscribe(*this, "method");
}

void AudioRecording::notifyThatTrialWillBegin(int trialNumber) {
    timeStamp.capture();
    std::stringstream filename;
    filename << trialNumber << '-' << session;
    filename << '-' << timeStamp.year();
    filename << '-' << timeStamp.month();
    filename << '-' << timeStamp.dayOfMonth();
    filename << '-' << timeStamp.hour();
    filename << '-' << timeStamp.minute();
    filename << '-' << timeStamp.second();
    filename << ".wav";
    audioRecorder.initialize(
        LocalUrl{outputFile.parentPath() / filename.str()});
}

void AudioRecording::notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &) {
    audioRecorder.start();
}

void AudioRecording::notifyThatStimulusHasEnded() {}

void AudioRecording::notifyThatSubjectHasResponded() { audioRecorder.stop(); }

void AudioRecording::notifyThatNewTestIsReady(std::string_view session) {
    this->session = session;
}

void AudioRecording::configure(
    const std::string &key, const std::string &value) {
    if (key == "method") {
        if (contains(value, "audio recording"))
            runningATest.add(*this);
        else
            runningATest.remove(*this);
    }
}
}
