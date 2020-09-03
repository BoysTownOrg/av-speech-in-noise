#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_

#include "Task.hpp"
#include "Experimenter.hpp"
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

class View {
  public:
    class TestSetup {
      public:
        class EventListener {
          public:
            virtual ~EventListener() = default;
            virtual void notifyThatConfirmButtonHasBeenClicked() = 0;
            virtual void notifyThatPlayCalibrationButtonHasBeenClicked() = 0;
            virtual void
            notifyThatBrowseForTestSettingsButtonHasBeenClicked() = 0;
        };

        virtual ~TestSetup() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual void populateTransducerMenu(std::vector<std::string>) = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual auto testSettingsFile() -> std::string = 0;
        virtual auto startingSnr() -> std::string = 0;
        virtual auto testerId() -> std::string = 0;
        virtual auto subjectId() -> std::string = 0;
        virtual auto session() -> std::string = 0;
        virtual auto rmeSetting() -> std::string = 0;
        virtual auto transducer() -> std::string = 0;
        virtual void setTestSettingsFile(std::string) = 0;
    };

    class CorrectKeywordsInput {
      public:
        class EventListener {
          public:
            virtual ~EventListener() = default;
            virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
        };
        virtual ~CorrectKeywordsInput() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual auto correctKeywords() -> std::string = 0;
    };

    class CorrectKeywordsOutput {
      public:
        virtual ~CorrectKeywordsOutput() = default;
        virtual void showCorrectKeywordsSubmission() = 0;
        virtual void hideCorrectKeywordsSubmission() = 0;
    };

    class PassFailInput {
      public:
        class EventListener {
          public:
            virtual ~EventListener() = default;
            virtual void notifyThatCorrectButtonHasBeenClicked() = 0;
            virtual void notifyThatIncorrectButtonHasBeenClicked() = 0;
        };
        virtual ~PassFailInput() = default;
        virtual void subscribe(EventListener *) = 0;
    };

    class PassFailOutput {
      public:
        virtual ~PassFailOutput() = default;
        virtual void showEvaluationButtons() = 0;
        virtual void hideEvaluationButtons() = 0;
    };

    virtual ~View() = default;
    virtual void eventLoop() = 0;
    virtual auto browseForDirectory() -> std::string = 0;
    virtual auto browseForOpeningFile() -> std::string = 0;
    virtual auto audioDevice() -> std::string = 0;
    virtual void populateAudioDeviceMenu(std::vector<std::string>) = 0;
    virtual auto browseCancelled() -> bool = 0;
    virtual void showErrorMessage(std::string) = 0;
    virtual void showCursor() = 0;
};

class BadInput : public std::runtime_error {
  public:
    explicit BadInput(const std::string &s) : std::runtime_error{s} {}
};

static auto readInteger(const std::string &x, const std::string &identifier)
    -> int {
    try {
        return std::stoi(x);
    } catch (const std::invalid_argument &) {
        std::stringstream stream;
        stream << '"' << x << '"';
        stream << " is not a valid ";
        stream << identifier;
        stream << '.';
        throw BadInput{stream.str()};
    }
}

class Presenter : public Model::EventListener, public ParentPresenter {
  public:
    class TestSetup : public View::TestSetup::EventListener {
      public:
        explicit TestSetup(View::TestSetup *);
        void notifyThatPlayCalibrationButtonHasBeenClicked() override;
        void notifyThatConfirmButtonHasBeenClicked() override;
        void notifyThatBrowseForTestSettingsButtonHasBeenClicked() override;
        void show();
        void hide();
        void becomeChild(Presenter *parent);
        void setTestSettingsFile(std::string);
        auto testSettingsFile() -> std::string;
        auto testIdentity() -> TestIdentity;
        auto startingSnr() -> std::string;

      private:
        View::TestSetup *view;
        Presenter *parent{};
    };

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

    Presenter(Model &, View &, TestSetup &, Experimenter &,
        TestSettingsInterpreter &, TextFileReader &, TaskResponder * = {},
        TaskPresenter * = {}, TaskResponder * = {}, TaskPresenter * = {},
        TaskResponder * = {}, TaskPresenter * = {}, TaskResponder * = {},
        TaskPresenter * = {}, TaskResponder * = {}, TaskPresenter * = {});
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

    static constexpr RealLevel fullScaleLevel{119};
    static constexpr SNR ceilingSnr{20};
    static constexpr SNR floorSnr{-40};
    static constexpr auto trackBumpLimit{10};

  private:
    void readyNextTrialAfter(void (Presenter::*f)());
    void showErrorMessage(std::string);
    void playCalibration_();
    void showTest(Method);
    void switchToTestView(Method);
    void confirmTestSetup_();
    void applyIfBrowseNotCancelled(
        std::string s, void (TestSetup::*f)(std::string));
    auto taskPresenter(Method) -> TaskPresenter *;

    Model &model;
    View &view;
    TestSetup &testSetup;
    Experimenter &experimenterPresenter;
    TestSettingsInterpreter &testSettingsInterpreter;
    TextFileReader &textFileReader;
    TaskPresenter *consonantPresenter;
    TaskPresenter *coordinateResponseMeasurePresenter;
    TaskPresenter *freeResponsePresenter;
    TaskPresenter *correctKeywordsPresenter;
    TaskPresenter *passFailPresenter;
    TaskPresenter *taskPresenter_;
};

class FreeResponseInputView {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
    };
    virtual ~FreeResponseInputView() = default;
    virtual void subscribe(EventListener *) = 0;
    virtual auto flagged() -> bool = 0;
    virtual auto freeResponse() -> std::string = 0;
};

class FreeResponseOutputView {
  public:
    virtual ~FreeResponseOutputView() = default;
    virtual void showFreeResponseSubmission() = 0;
    virtual void hideFreeResponseSubmission() = 0;
    virtual void clearFreeResponse() = 0;
};

class FreeResponseResponder : public TaskResponder,
                              public FreeResponseInputView::EventListener {
  public:
    explicit FreeResponseResponder(Model &model, FreeResponseInputView &view)
        : model{model}, view{view} {
        view.subscribe(this);
    }
    void subscribe(TaskResponder::EventListener *e) override { listener = e; }
    void notifyThatSubmitButtonHasBeenClicked() override {
        model.submit(FreeResponse{view.freeResponse(), view.flagged()});
        listener->notifyThatUserIsDoneResponding();
        parent->readyNextTrialIfNeeded();
    }
    void becomeChild(ParentPresenter *p) override { parent = p; }

  private:
    Model &model;
    FreeResponseInputView &view;
    TaskResponder::EventListener *listener{};
    ParentPresenter *parent{};
};

class FreeResponsePresenter : public TaskPresenter {
  public:
    explicit FreeResponsePresenter(
        ExperimenterView &experimenterView, FreeResponseOutputView &view)
        : experimenterView{experimenterView}, view{view} {}
    void start() override {
        experimenterView.show();
        experimenterView.showNextTrialButton();
    }
    void stop() override {
        experimenterView.hide();
        view.hideFreeResponseSubmission();
    }
    void notifyThatTaskHasStarted() override {
        experimenterView.hideNextTrialButton();
    }
    void notifyThatUserIsDoneResponding() override {
        view.hideFreeResponseSubmission();
    }
    void showResponseSubmission() override {
        view.clearFreeResponse();
        view.showFreeResponseSubmission();
    }

  private:
    ExperimenterView &experimenterView;
    FreeResponseOutputView &view;
};

class CorrectKeywordsResponder
    : public TaskResponder,
      public View::CorrectKeywordsInput::EventListener {
  public:
    explicit CorrectKeywordsResponder(
        Model &model, View &mainView, View::CorrectKeywordsInput &view)
        : model{model}, mainView{mainView}, view{view} {
        view.subscribe(this);
    }
    void subscribe(TaskResponder::EventListener *e) override { listener = e; }
    void notifyThatSubmitButtonHasBeenClicked() override {
        try {
            CorrectKeywords p{};
            p.count = readInteger(view.correctKeywords(), "number");
            model.submit(p);
            listener->notifyThatUserIsDoneResponding();
            parent->showContinueTestingDialogWithResultsWhenComplete();
        } catch (const std::runtime_error &e) {
            mainView.showErrorMessage(e.what());
        }
    }
    void becomeChild(ParentPresenter *p) override { parent = p; }

  private:
    Model &model;
    View &mainView;
    View::CorrectKeywordsInput &view;
    TaskResponder::EventListener *listener{};
    ParentPresenter *parent{};
};

class CorrectKeywordsPresenter : public TaskPresenter {
  public:
    explicit CorrectKeywordsPresenter(
        ExperimenterView &experimenterView, View::CorrectKeywordsOutput &view)
        : experimenterView{experimenterView}, view{view} {}
    void start() override {
        experimenterView.show();
        experimenterView.showNextTrialButton();
    }
    void stop() override {
        experimenterView.hide();
        view.hideCorrectKeywordsSubmission();
    }
    void notifyThatTaskHasStarted() override {
        experimenterView.hideNextTrialButton();
    }
    void notifyThatUserIsDoneResponding() override {
        view.hideCorrectKeywordsSubmission();
    }
    void showResponseSubmission() override {
        view.showCorrectKeywordsSubmission();
    }

  private:
    ExperimenterView &experimenterView;
    View::CorrectKeywordsOutput &view;
};

class PassFailResponder : public TaskResponder,
                          public View::PassFailInput::EventListener {
  public:
    explicit PassFailResponder(Model &model, View::PassFailInput &view)
        : model{model} {
        view.subscribe(this);
    }
    void subscribe(TaskResponder::EventListener *e) override { listener = e; }
    void notifyThatCorrectButtonHasBeenClicked() override {
        model.submitCorrectResponse();
        listener->notifyThatUserIsDoneResponding();
        parent->showContinueTestingDialogWithResultsWhenComplete();
    }
    void notifyThatIncorrectButtonHasBeenClicked() override {
        model.submitIncorrectResponse();
        listener->notifyThatUserIsDoneResponding();
        parent->showContinueTestingDialogWithResultsWhenComplete();
    }
    void becomeChild(ParentPresenter *p) override { parent = p; }

  private:
    Model &model;
    TaskResponder::EventListener *listener{};
    ParentPresenter *parent{};
};

class PassFailPresenter : public TaskPresenter {
  public:
    explicit PassFailPresenter(
        ExperimenterView &experimenterView, View::PassFailOutput &view)
        : experimenterView{experimenterView}, view{view} {}
    void start() override {
        experimenterView.show();
        experimenterView.showNextTrialButton();
    }
    void stop() override {
        experimenterView.hide();
        view.hideEvaluationButtons();
    }
    void notifyThatTaskHasStarted() override {
        experimenterView.hideNextTrialButton();
    }
    void notifyThatUserIsDoneResponding() override {
        view.hideEvaluationButtons();
    }
    void showResponseSubmission() override { view.showEvaluationButtons(); }

  private:
    ExperimenterView &experimenterView;
    View::PassFailOutput &view;
};
}

#endif
