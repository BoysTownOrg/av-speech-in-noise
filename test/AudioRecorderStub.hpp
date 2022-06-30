#ifndef AV_SPEECH_IN_NOISE_TEST_AUDIORECORDERSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_AUDIORECORDERSTUB_HPP_

#include <av-speech-in-noise/core/AudioRecording.hpp>

namespace av_speech_in_noise {
class AudioRecorderStub : public AudioRecorder {
  public:
    [[nodiscard]] auto started() const -> bool { return started_; }

    void start() override { started_ = true; }

    auto fileUrl() -> LocalUrl { return fileUrl_; }

    void initialize(const LocalUrl &url) override {
        fileUrl_ = url;
        initialized_ = true;
    }

    [[nodiscard]] auto initialized() const -> bool { return initialized_; }

    [[nodiscard]] auto stopped() const -> bool { return stopped_; }

    void stop() override { stopped_ = true; }

  private:
    LocalUrl fileUrl_;
    bool started_{};
    bool initialized_{};
    bool stopped_{};
};
}

#endif
