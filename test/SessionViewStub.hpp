#ifndef AV_SPEECH_IN_NOISE_TEST_SESSIONVIEWSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_SESSIONVIEWSTUB_HPP_

#include <presentation/Session.hpp>
#include <utility>

namespace av_speech_in_noise {
class SessionViewStub : public SessionView {
  public:
    void showErrorMessage(std::string_view s) override { errorMessage_ = s; }

    auto errorMessage() -> std::string { return errorMessage_; }

    void eventLoop() override { eventLoopCalled_ = true; }

    void populateAudioDeviceMenu(std::vector<std::string> v) override {
        audioDevices_ = std::move(v);
    }

    auto audioDevices() -> std::vector<std::string> { return audioDevices_; }

  private:
    std::vector<std::string> audioDevices_;
    std::string errorMessage_;
    bool eventLoopCalled_{};
};

class SessionControlStub : public SessionControl {
  public:
    void setAudioDevice(std::string s) { audioDevice_ = std::move(s); }

    auto audioDevice() -> std::string override { return audioDevice_; }

  private:
    std::string audioDevice_;
};
}

#endif
