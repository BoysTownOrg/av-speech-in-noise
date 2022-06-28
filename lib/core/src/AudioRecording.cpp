#include "AudioRecording.hpp"

#include <sstream>

namespace av_speech_in_noise {
AudioRecording::AudioRecording(
    AudioRecorder &audioRecorder, OutputFile &outputFile)
    : audioRecorder{audioRecorder}, outputFile{outputFile} {}

void AudioRecording::notifyThatTrialWillBegin(int trialNumber) {
    std::stringstream filename;
    filename << trialNumber << '-' << session << ".wav";
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
