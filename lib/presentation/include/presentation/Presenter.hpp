#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_

#include "Task.hpp"
#include "Experimenter.hpp"
#include "View.hpp"
#include "Input.hpp"
#include "PresenterSimple.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <vector>
#include <string>
#include <sstream>

namespace av_speech_in_noise {
enum class Method {
    adaptivePassFail,
    adaptivePassFailWithEyeTracking,
    adaptiveCorrectKeywords,
    adaptiveCorrectKeywordsWithEyeTracking,
    adaptiveCoordinateResponseMeasure,
    adaptiveCoordinateResponseMeasureWithSingleSpeaker,
    adaptiveCoordinateResponseMeasureWithDelayedMasker,
    adaptiveCoordinateResponseMeasureWithEyeTracking,
    fixedLevelFreeResponseWithTargetReplacement,
    fixedLevelFreeResponseWithSilentIntervalTargets,
    fixedLevelFreeResponseWithAllTargets,
    fixedLevelFreeResponseWithAllTargetsAndEyeTracking,
    fixedLevelCoordinateResponseMeasureWithTargetReplacement,
    fixedLevelCoordinateResponseMeasureWithTargetReplacementAndEyeTracking,
    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets,
    fixedLevelConsonants,
    unknown
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

class Presenter;

class TestSetupResponder {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatUserHasSelectedTestSettingsFile(
            const std::string &) = 0;
    };
    virtual ~TestSetupResponder() = default;
    virtual void becomeChild(Presenter *) = 0;
    virtual void subscribe(EventListener *) = 0;
};

class TestSetupPresenter : public virtual TestSetupResponder::EventListener,
                           public virtual PresenterSimple {};

class Presenter : public Model::EventListener, public ParentPresenter {
  public:
    class Experimenter : public ExperimenterView::EventListener {
      public:
        explicit Experimenter(ExperimenterView *);
        void exitTest() override;
        void playTrial() override;
        void declineContinuingTesting() override;
        void acceptContinuingTesting() override;
        void becomeChild(Presenter *parent);
        void show();
        void start();
        void stop();
        void trialPlayed();
        void trialComplete();
        void readyNextTrial();
        void showContinueTestingDialog();
        void hideSubmissions();
        void setContinueTestingDialogMessage(const std::string &);
        void display(std::string);
        void secondaryDisplay(std::string);

      private:
        Presenter *parent{};
        ExperimenterView *view;
    };

    Presenter(Model &, View &, Experimenter &, TestSettingsInterpreter &,
        TextFileReader &, TaskResponder * = {}, TaskPresenter * = {},
        TaskResponder * = {}, TaskPresenter * = {}, TaskResponder * = {},
        TaskPresenter * = {}, TaskResponder * = {}, TaskPresenter * = {},
        TaskResponder * = {}, TaskPresenter * = {}, TestSetupResponder * = {},
        TestSetupPresenter * = {});
    void trialComplete() override;
    void playTrial() override;
    void playNextTrialIfNeeded() override;
    void readyNextTrialIfNeeded() override;
    void showContinueTestingDialogWithResultsWhenComplete() override;
    void run();
    void confirmTestSetup();
    void playCalibration();
    void browseForTestSettingsFile();
    auto testComplete() -> bool;
    void declineContinuingTesting();
    void acceptContinuingTesting();
    void exitTest();
    void prepare(Method m) {
        switchToTestView(m);
        taskPresenter_ = taskPresenter(m);
    }

    static constexpr RealLevel fullScaleLevel{119};
    static constexpr SNR ceilingSnr{20};
    static constexpr SNR floorSnr{-40};
    static constexpr auto trackBumpLimit{10};

  private:
    void readyNextTrialAfter(void (Presenter::*f)());
    void showErrorMessage(std::string);
    void showTest(Method);
    void switchToTestView(Method);
    auto taskPresenter(Method) -> TaskPresenter *;

    Model &model;
    View &view;
    Experimenter &experimenterPresenter;
    TestSettingsInterpreter &testSettingsInterpreter;
    TextFileReader &textFileReader;
    TaskPresenter *consonantPresenter;
    TaskPresenter *coordinateResponseMeasurePresenter;
    TaskPresenter *freeResponsePresenter;
    TaskPresenter *correctKeywordsPresenter;
    TaskPresenter *passFailPresenter;
    TaskPresenter *taskPresenter_;
    PresenterSimple *testSetupPresenter;
};

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
    void becomeChild(Presenter *p) override { parent = p; }
    void subscribe(TestSetupResponder::EventListener *e) override {
        listener = e;
    }

  private:
    Model &model;
    View &mainView;
    TestSetupInputView &view;
    TestSettingsInterpreter &testSettingsInterpreter;
    TextFileReader &textFileReader;
    Presenter *parent{};
    TestSetupResponder::EventListener *listener;
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
