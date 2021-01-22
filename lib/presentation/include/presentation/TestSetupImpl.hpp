#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUPIMPL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUPIMPL_HPP_

#include "TestSetup.hpp"
#include "SessionView.hpp"
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
    TestSetupControllerImpl(SessionController &, Model &, SessionView &,
        TestSetupControl &, TestSettingsInterpreter &, TextFileReader &,
        TestSetupPresenter &);
    void notifyThatConfirmButtonHasBeenClicked() override;
    void notifyThatPlayCalibrationButtonHasBeenClicked() override;
    void notifyThatPlayLeftSpeakerCalibrationButtonHasBeenClicked() override;
    void notifyThatPlayRightSpeakerCalibrationButtonHasBeenClicked() override;
    void notifyThatBrowseForTestSettingsButtonHasBeenClicked() override;

  private:
    SessionController &sessionController;
    Model &model;
    SessionView &sessionView;
    TestSetupControl &control;
    TestSettingsInterpreter &testSettingsInterpreter;
    TextFileReader &textFileReader;
    TestSetupPresenter &presenter;
};

class TestSetupPresenterImpl : public TestSetupPresenter {
  public:
    explicit TestSetupPresenterImpl(TestSetupView &view);
    void start() override;
    void stop() override;
    void showTestSettingsFile(const std::string &s) override;

  private:
    TestSetupView &view;
};
}

#endif
