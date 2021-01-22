#ifndef AV_SPEECH_IN_NOISE_TEST_SESSIONVIEWSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_SESSIONVIEWSTUB_HPP_

#include <presentation/Session.hpp>
#include <utility>

namespace av_speech_in_noise {
class SessionViewStub : public SessionView {
  public:
    void showErrorMessage(std::string s) override {
        errorMessage_ = std::move(s);
    }

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

    void setBrowseForOpeningFileResult(std::string s) {
        browseForOpeningFileResult_ = std::move(s);
    }

    void setBrowseCancelled() { browseCancelled_ = true; }

    auto audioDevice() -> std::string override { return audioDevice_; }

    auto browseForDirectory() -> std::string override {
        return browseForDirectoryResult_;
    }

    auto browseCancelled() -> bool override { return browseCancelled_; }

    auto browseForOpeningFile() -> std::string override {
        return browseForOpeningFileResult_;
    }

  private:
    std::string browseForDirectoryResult_;
    std::string browseForOpeningFileResult_;
    bool browseCancelled_{};
    std::string audioDevice_;
};
}

#endif
