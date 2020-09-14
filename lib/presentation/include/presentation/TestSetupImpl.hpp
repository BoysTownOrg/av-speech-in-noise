#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUPIMPL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUPIMPL_HPP_

#include "TestSetup.hpp"
#include "SessionView.hpp"
#include "Input.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class TestSettingsInterpreter {
  public:
    virtual ~TestSettingsInterpreter() = default;
    virtual void initialize(
        Model &, const std::string &, const TestIdentity &, SNR) = 0;
    virtual auto method(const std::string &) -> Method = 0;
    virtual auto calibration(const std::string &) -> Calibration = 0;
};

class TextFileReader {
  public:
    virtual ~TextFileReader() = default;
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

class TestSetupControllerImpl : public TestSetupControl::Observer,
                                public TestSetupController {
  public:
    explicit TestSetupControllerImpl(Model &model, SessionView &mainView,
        TestSetupControl &view,
        TestSettingsInterpreter &testSettingsInterpreter,
        TextFileReader &textFileReader);
    void notifyThatConfirmButtonHasBeenClicked() override;
    void notifyThatPlayCalibrationButtonHasBeenClicked() override;
    void notifyThatBrowseForTestSettingsButtonHasBeenClicked() override;
    void attach(SessionController *p) override;
    void attach(TestSetupController::Observer *e) override;

  private:
    Model &model;
    SessionView &mainView;
    TestSetupControl &view;
    TestSettingsInterpreter &testSettingsInterpreter;
    TextFileReader &textFileReader;
    SessionController *parent{};
    TestSetupController::Observer *listener{};
};

class TestSetupPresenterImpl : public TestSetupPresenter {
  public:
    explicit TestSetupPresenterImpl(TestSetupView &view);
    void start() override;
    void stop() override;
    void notifyThatUserHasSelectedTestSettingsFile(
        const std::string &s) override;

  private:
    TestSetupView &view;
};
}

#endif
