#ifndef AV_SPEECH_IN_NOISE_TEST_SESSIONVIEWSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_SESSIONVIEWSTUB_HPP_

#include <presentation/SessionView.hpp>

namespace av_speech_in_noise {
class SessionViewStub : public SessionView {
  public:
    void showErrorMessage(std::string s) override {
        errorMessage_ = std::move(s);
    }

    auto errorMessage() -> std::string { return errorMessage_; }

    auto audioDevice() -> std::string override { return {}; }
    void eventLoop() override {}
    auto browseForDirectory() -> std::string override { return {}; }
    auto browseCancelled() -> bool override { return {}; }
    auto browseForOpeningFile() -> std::string override { return {}; }
    void populateAudioDeviceMenu(std::vector<std::string>) override {}

  private:
    std::string errorMessage_;
};
}

#endif
