#include "AudioRecording.hpp"
#include "FileSystemPath.hpp"

#include <sstream>

namespace av_speech_in_noise {
AudioRecording::AudioRecording(
    AudioRecorder &audioRecorder, OutputFile &outputFile, TimeStamp &timeStamp)
    : audioRecorder{audioRecorder}, outputFile{outputFile}, timeStamp{
                                                                timeStamp} {}

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

void AudioRecording::notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &) {}

void AudioRecording::notifyThatStimulusHasEnded() { audioRecorder.start(); }

void AudioRecording::notifyThatSubjectHasResponded() { audioRecorder.stop(); }

void AudioRecording::notifyThatNewTestIsReady(std::string_view session) {
    this->session = session;
}
}
