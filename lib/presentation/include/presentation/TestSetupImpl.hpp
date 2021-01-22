#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUPIMPL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUPIMPL_HPP_

#include "TestSetup.hpp"
#include "SessionController.hpp"
#include "Input.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <string>

namespace av_speech_in_noise {
class TestSettingsInterpreter {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(
        TestSettingsInterpreter);
    virtual void initialize(Model &, SessionController &, const std::string &,
        const TestIdentity &, SNR) = 0;
    virtual auto calibration(const std::string &) -> Calibration = 0;
};

class TextFileReader {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TextFileReader);
    virtual auto read(const LocalUrl &) -> std::string = 0;
};

enum class Transducer { headphone, oneSpeaker, twoSpeakers, unknown };

constexpr auto name(Transducer c) -> const char * {
    switch (c) {
    case Transducer::headphone:
        return "headphone";
    case Transducer::oneSpeaker:
        return "1 speaker";
    case Transducer::twoSpeakers:
        return "2 speakers";
    case Transducer::unknown:
        return "unknown";
    }
}

class TestSetupControllerImpl : public TestSetupControl::Observer {
  public:
    TestSetupControllerImpl(TestSetupControl &, SessionController &,
        SessionControl &, TestSetupPresenter &, Model &,
        TestSettingsInterpreter &, TextFileReader &);
    void notifyThatConfirmButtonHasBeenClicked() override;
    void notifyThatPlayCalibrationButtonHasBeenClicked() override;
    void notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked() override;
    void notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked() override;
    void notifyThatBrowseForTestSettingsButtonHasBeenClicked() override;

  private:
    TestSetupControl &control;
    SessionController &sessionController;
    SessionControl &sessionControl;
    TestSetupPresenter &presenter;
    Model &model;
    TestSettingsInterpreter &testSettingsInterpreter;
    TextFileReader &textFileReader;
};

class TestSetupPresenterImpl : public TestSetupPresenter {
  public:
    explicit TestSetupPresenterImpl(TestSetupView &, SessionView &);
    void start() override;
    void stop() override;
    void showTestSettingsFile(const std::string &s) override;
    void showErrorMessage(std::string) override;

  private:
    TestSetupView &view;
    SessionView &sessionView;
};
}

#endif
