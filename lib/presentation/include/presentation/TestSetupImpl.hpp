#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUPIMPL_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TESTSETUPIMPL_HPP_

#include "TestSetup.hpp"
#include "View.hpp"
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

class TestSetupResponderImpl : public TestSetupInputView::EventListener,
                               public TestSetupResponder {
  public:
    explicit TestSetupResponderImpl(Model &model, View &mainView,
        TestSetupInputView &view,
        TestSettingsInterpreter &testSettingsInterpreter,
        TextFileReader &textFileReader)
        : model{model}, mainView{mainView}, view{view},
          testSettingsInterpreter{testSettingsInterpreter},
          textFileReader{textFileReader} {
        view.subscribe(this);
    }
    void notifyThatConfirmButtonHasBeenClicked() override {
        try {
            const auto testSettings{
                textFileReader.read({view.testSettingsFile()})};
            TestIdentity p;
            p.subjectId = view.subjectId();
            p.testerId = view.testerId();
            p.session = view.session();
            p.rmeSetting = view.rmeSetting();
            p.transducer = view.transducer();
            testSettingsInterpreter.initialize(model, testSettings, p,
                SNR{readInteger(view.startingSnr(), "starting SNR")});
            if (!model.testComplete())
                parent->prepare(testSettingsInterpreter.method(testSettings));
        } catch (const std::runtime_error &e) {
            mainView.showErrorMessage(e.what());
        }
    }
    void notifyThatPlayCalibrationButtonHasBeenClicked() override {
        auto p{testSettingsInterpreter.calibration(
            textFileReader.read({view.testSettingsFile()}))};
        p.audioDevice = mainView.audioDevice();
        model.playCalibration(p);
    }
    void notifyThatBrowseForTestSettingsButtonHasBeenClicked() override {
        auto file{mainView.browseForOpeningFile()};
        if (!mainView.browseCancelled())
            listener->notifyThatUserHasSelectedTestSettingsFile(file);
    }
    void becomeChild(SomethingIDK *p) override { parent = p; }
    void subscribe(TestSetupResponder::EventListener *e) override {
        listener = e;
    }

  private:
    Model &model;
    View &mainView;
    TestSetupInputView &view;
    TestSettingsInterpreter &testSettingsInterpreter;
    TextFileReader &textFileReader;
    SomethingIDK *parent{};
    TestSetupResponder::EventListener *listener{};
};

class TestSetupPresenterImpl : public TestSetupPresenter {
  public:
    explicit TestSetupPresenterImpl(TestSetupOutputView &view) : view{view} {
        view.populateTransducerMenu({name(Transducer::headphone),
            name(Transducer::oneSpeaker), name(Transducer::twoSpeakers)});
    }
    void start() override { view.show(); }
    void stop() override { view.hide(); }
    void notifyThatUserHasSelectedTestSettingsFile(
        const std::string &s) override {
        view.setTestSettingsFile(s);
    }

  private:
    TestSetupOutputView &view;
};
}

#endif
