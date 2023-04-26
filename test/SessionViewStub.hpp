#ifndef AV_SPEECH_IN_NOISE_TEST_SESSIONVIEWSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_SESSIONVIEWSTUB_HPP_

#include <av-speech-in-noise/ui/Session.hpp>
#include <string>
#include <utility>
#include <vector>

namespace av_speech_in_noise {
class SessionViewStub : public SessionView {
  public:
    void showErrorMessage(std::string_view s) override { errorMessage_ = s; }

    auto errorMessage() -> std::string { return errorMessage_; }

    void populateAudioDeviceMenu(std::vector<std::string> v) override {
        audioDevices_ = std::move(v);
    }

    auto audioDevices() -> std::vector<std::string> { return audioDevices_; }

    auto subjectScreens() -> std::vector<Screen> { return subjectScreens_; }

    void setScreens(const std::vector<Screen> &v) { screens_ = v; }

    void setSubject(Screen s) { subjectScreen_ = std::move(s); }

    auto subjectScreen() -> Screen override { return subjectScreen_; }

    auto screens() -> std::vector<Screen> override { return screens_; }

    void populateSubjectScreenMenu(const std::vector<Screen> &v) override {
        subjectScreens_ = v;
    }

  private:
    std::vector<std::string> audioDevices_;
    std::vector<Screen> subjectScreens_;
    std::vector<Screen> screens_;
    std::string errorMessage_;
    Screen subjectScreen_;
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
