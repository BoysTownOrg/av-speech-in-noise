#include "AudioRecording.hpp"

#include <sstream>

namespace av_speech_in_noise {
AudioRecording::AudioRecording(
    AudioRecorder &audioRecorder, OutputFile &outputFile)
    : audioRecorder{audioRecorder}, outputFile{outputFile} {}

void AudioRecording::notifyThatTrialWillBegin(int trialNumber) {
    std::stringstream stream;
    stream << trialNumber << '-' << session << ".wav";
    audioRecorder.initialize(LocalUrl{outputFile.parentPath() / stream.str()});
}

void AudioRecording::notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &) {}

void AudioRecording::notifyThatStimulusHasEnded() { audioRecorder.start(); }

void AudioRecording::notifyThatSubjectHasResponded() { audioRecorder.stop(); }
}
