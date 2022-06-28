#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_AUDIORECORDINGHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_AUDIORECORDINGHPP_

#include "IRecognitionTestModel.hpp"
#include "IOutputFile.hpp"

#include <av-speech-in-noise/Interface.hpp>

#include <string>

namespace av_speech_in_noise {
class AudioRecorder {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(AudioRecorder);
    virtual void initialize(const LocalUrl &) = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
};

class AudioRecording : public RunningATest::Observer {
  public:
    AudioRecording(AudioRecorder &, OutputFile &);
    void notifyThatNewTestIsReady(std::string_view session) override;
    void notifyThatTrialWillBegin(int trialNumber) override;
    void notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &) override;
    void notifyThatStimulusHasEnded() override;
    void notifyThatSubjectHasResponded() override;

  private:
    std::string session;
    AudioRecorder &audioRecorder;
    OutputFile &outputFile;
};
}

#endif
