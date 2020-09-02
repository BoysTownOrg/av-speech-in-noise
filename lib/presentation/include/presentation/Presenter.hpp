#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_

#include <av-speech-in-noise/Model.hpp>
#include <vector>
#include <string>

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
    class ConsonantInput {
      public:
        class EventListener {
          public:
            virtual ~EventListener() = default;
            virtual void notifyThatReadyButtonHasBeenClicked() = 0;
            virtual void notifyThatResponseButtonHasBeenClicked() = 0;
        };
        virtual ~ConsonantInput() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual auto consonant() -> std::string = 0;
    };

    class ConsonantOutput {
      public:
        virtual ~ConsonantOutput() = default;
        virtual void hideCursor() = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void showReadyButton() = 0;
        virtual void hideReadyButton() = 0;
        virtual void hideResponseButtons() = 0;
        virtual void showResponseButtons() = 0;
    };

    class CoordinateResponseMeasureInput {
      public:
        class EventListener {
          public:
            virtual ~EventListener() = default;
            virtual void notifyThatReadyButtonHasBeenClicked() = 0;
            virtual void notifyThatResponseButtonHasBeenClicked() = 0;
        };
        virtual ~CoordinateResponseMeasureInput() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual auto numberResponse() -> std::string = 0;
        virtual auto greenResponse() -> bool = 0;
        virtual auto blueResponse() -> bool = 0;
        virtual auto whiteResponse() -> bool = 0;
    };

    class CoordinateResponseMeasureOutput {
      public:
        virtual ~CoordinateResponseMeasureOutput() = default;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void showResponseButtons() = 0;
        virtual void hideResponseButtons() = 0;
        virtual void showNextTrialButton() = 0;
        virtual void hideNextTrialButton() = 0;
    };

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

    class Experimenter {
      public:
        class EventListener {
          public:
            virtual ~EventListener() = default;
            virtual void exitTest() = 0;
            virtual void playTrial() = 0;
            virtual void submitPassedTrial() = 0;
            virtual void submitCorrectKeywords() = 0;
            virtual void submitFailedTrial() = 0;
            virtual void submitFreeResponse() = 0;
            virtual void declineContinuingTesting() = 0;
            virtual void acceptContinuingTesting() = 0;
        };

        virtual ~Experimenter() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void showContinueTestingDialog() = 0;
        virtual void hideContinueTestingDialog() = 0;
        virtual void setContinueTestingDialogMessage(const std::string &) = 0;
        virtual void showEvaluationButtons() = 0;
        virtual void hideEvaluationButtons() = 0;
        virtual void showFreeResponseSubmission() = 0;
        virtual void clearFreeResponse() = 0;
        virtual void hideFreeResponseSubmission() = 0;
        virtual void showCorrectKeywordsSubmission() = 0;
        virtual void hideCorrectKeywordsSubmission() = 0;
        virtual auto freeResponse() -> std::string = 0;
        virtual auto correctKeywords() -> std::string = 0;
        virtual auto flagged() -> bool = 0;
        virtual void hideExitTestButton() = 0;
        virtual void showExitTestButton() = 0;
        virtual void showNextTrialButton() = 0;
        virtual void hideNextTrialButton() = 0;
        virtual void display(std::string) = 0;
        virtual void secondaryDisplay(std::string) = 0;
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

class Presenter;

class TaskResponder {
  public:
    class EventListener {
      public:
        virtual ~EventListener() = default;
        virtual void notifyThatTaskHasStarted() = 0;
        virtual void notifyThatUserIsDoneResponding() = 0;
    };
    virtual ~TaskResponder() = default;
    virtual void becomeChild(Presenter *) = 0;
    virtual void subscribe(EventListener *) = 0;
};

class TaskPresenter {
  public:
    virtual ~TaskPresenter() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void showResponseSubmission() = 0;
};

class Presenter : public Model::EventListener {
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

    class CoordinateResponseMeasure
        : public View::CoordinateResponseMeasureInput::EventListener {
      public:
        explicit CoordinateResponseMeasure(
            View::CoordinateResponseMeasureInput *,
            View::CoordinateResponseMeasureOutput * = {});
        void notifyThatReadyButtonHasBeenClicked() override;
        void notifyThatResponseButtonHasBeenClicked() override;
        void start();
        void stop();
        void becomeChild(Presenter *parent);
        void showResponseButtons();
        auto subjectResponse() -> coordinate_response_measure::Response;

      private:
        auto colorResponse() -> coordinate_response_measure::Color;

        View::CoordinateResponseMeasureInput *view;
        View::CoordinateResponseMeasureOutput *outputView;
        Presenter *parent{};
    };

    class Experimenter : public View::Experimenter::EventListener {
      public:
        explicit Experimenter(View::Experimenter *);
        void exitTest() override;
        void playTrial() override;
        void submitPassedTrial() override;
        void submitFreeResponse() override;
        void submitFailedTrial() override;
        void submitCorrectKeywords() override;
        void declineContinuingTesting() override;
        void acceptContinuingTesting() override;
        void hideCorrectKeywordsSubmission();
        void becomeChild(Presenter *parent);
        void show();
        void start();
        void stop();
        void trialPlayed();
        void trialComplete();
        void readyNextTrial();
        void showContinueTestingDialog();
        void hideEvaluationButtons();
        void hideSubmissions();
        void setContinueTestingDialogMessage(const std::string &);
        void display(std::string);
        void secondaryDisplay(std::string);
        void showPassFailSubmission();
        void showCorrectKeywordsSubmission();
        void showFreeResponseSubmission();
        void clearFreeResponse();
        auto correctKeywords() -> CorrectKeywords;
        auto freeResponse() -> FreeResponse;

      private:
        Presenter *parent{};
        View::Experimenter *view;
    };

    class TrialCompletionHandler {
      public:
        virtual ~TrialCompletionHandler() = default;
        virtual void showResponseSubmission() = 0;
    };

    class CoordinateResponseMeasureTestTrialCompletionHandler
        : public TrialCompletionHandler {
      public:
        explicit CoordinateResponseMeasureTestTrialCompletionHandler(
            CoordinateResponseMeasure &coordinateResponseMeasure)
            : coordinateResponseMeasure{coordinateResponseMeasure} {}

        void showResponseSubmission() override {
            coordinateResponseMeasure.showResponseButtons();
        }

      private:
        CoordinateResponseMeasure &coordinateResponseMeasure;
    };

    class ConsonantTrialCompletionHandler : public TrialCompletionHandler {
      public:
        explicit ConsonantTrialCompletionHandler(TaskPresenter *presenter)
            : presenter{presenter} {}

        void showResponseSubmission() override {
            presenter->showResponseSubmission();
        }

      private:
        TaskPresenter *presenter;
    };

    class PassFailTrialCompletionHandler : public TrialCompletionHandler {
      public:
        explicit PassFailTrialCompletionHandler(
            Experimenter &experimenterPresenter)
            : experimenterPresenter{experimenterPresenter} {}

        void showResponseSubmission() override {
            experimenterPresenter.showPassFailSubmission();
        }

      private:
        Experimenter &experimenterPresenter;
    };

    class CorrectKeywordsTrialCompletionHandler
        : public TrialCompletionHandler {
      public:
        explicit CorrectKeywordsTrialCompletionHandler(
            Experimenter &experimenterPresenter)
            : experimenterPresenter{experimenterPresenter} {}

        void showResponseSubmission() override {
            experimenterPresenter.showCorrectKeywordsSubmission();
        }

      private:
        Experimenter &experimenterPresenter;
    };

    class FreeResponseTrialCompletionHandler : public TrialCompletionHandler {
      public:
        explicit FreeResponseTrialCompletionHandler(
            Experimenter &experimenterPresenter)
            : experimenterPresenter{experimenterPresenter} {}

        void showResponseSubmission() override {
            experimenterPresenter.clearFreeResponse();
            experimenterPresenter.showFreeResponseSubmission();
        }

      private:
        Experimenter &experimenterPresenter;
    };

    Presenter(Model &, View &, TestSetup &, CoordinateResponseMeasure &,
        Experimenter &, TestSettingsInterpreter &, TextFileReader &,
        TaskResponder * = {}, TaskPresenter * = {});
    void trialComplete() override;
    void run();
    void confirmTestSetup();
    void playTrial();
    void playCalibration();
    void browseForTestSettingsFile();
    void submitCoordinateResponse();
    void submitConsonantResponse();
    auto testComplete() -> bool;
    void playNextTrialIfNeeded();
    void submitFreeResponse();
    void submitPassedTrial();
    void submitFailedTrial();
    void submitCorrectKeywords();
    void declineContinuingTesting();
    void acceptContinuingTesting();
    void exitTest();

    static constexpr RealLevel fullScaleLevel{119};
    static constexpr SNR ceilingSnr{20};
    static constexpr SNR floorSnr{-40};
    static constexpr auto trackBumpLimit{10};

  private:
    void readyNextTrialAfter(void (Presenter::*f)());
    void submitFailedTrial_();
    void submitPassedTrial_();
    void submitFreeResponse_();
    void submitCorrectKeywords_();
    void showErrorMessage(std::string);
    void playCalibration_();
    void readyNextTrialIfNeeded();
    void showTest(Method);
    void switchToTestView(Method);
    void confirmTestSetup_();
    void applyIfBrowseNotCancelled(
        std::string s, void (TestSetup::*f)(std::string));
    auto trialCompletionHandler(Method) -> TrialCompletionHandler *;

    FreeResponseTrialCompletionHandler freeResponseTrialCompletionHandler;
    PassFailTrialCompletionHandler passFailTrialCompletionHandler;
    CorrectKeywordsTrialCompletionHandler correctKeywordsTrialCompletionHandler;
    CoordinateResponseMeasureTestTrialCompletionHandler
        coordinateResponseMeasureTrialCompletionHandler;
    ConsonantTrialCompletionHandler consonantTrialCompletionHandler;
    Model &model;
    View &view;
    TestSetup &testSetup;
    CoordinateResponseMeasure &coordinateResponseMeasurePresenter;
    Experimenter &experimenterPresenter;
    TestSettingsInterpreter &testSettingsInterpreter;
    TextFileReader &textFileReader;
    TrialCompletionHandler *trialCompletionHandler_{};
    TaskPresenter *consonantPresenterRefactored;
};

class ConsonantPresenter : public TaskResponder::EventListener,
                           public TaskPresenter {
  public:
    explicit ConsonantPresenter(Model &model, View::ConsonantOutput &view)
        : model{model}, view{view} {}
    void start() override {
        view.show();
        view.showReadyButton();
    }
    void stop() override {
        view.hideResponseButtons();
        view.hide();
    }
    void notifyThatTaskHasStarted() override {
        view.hideReadyButton();
        view.hideCursor();
    }
    void notifyThatUserIsDoneResponding() override {
        view.hideResponseButtons();
        if (!model.testComplete())
            view.hideCursor();
    }
    void showResponseSubmission() override { view.showResponseButtons(); }

  private:
    Model &model;
    View::ConsonantOutput &view;
};

class ConsonantResponder : public TaskResponder,
                           public View::ConsonantInput::EventListener {
  public:
    explicit ConsonantResponder(Model &model, View::ConsonantInput &view)
        : model{model}, view{view} {
        view.subscribe(this);
    }
    void subscribe(TaskResponder::EventListener *e) override { listener = e; }
    void notifyThatReadyButtonHasBeenClicked() override {
        parent->playTrial();
        listener->notifyThatTaskHasStarted();
    }
    void notifyThatResponseButtonHasBeenClicked() override {
        model.submit(ConsonantResponse{view.consonant().front()});
        parent->playNextTrialIfNeeded();
        listener->notifyThatUserIsDoneResponding();
    }
    void becomeChild(Presenter *p) override { parent = p; }

  private:
    Model &model;
    View::ConsonantInput &view;
    TaskResponder::EventListener *listener{};
    Presenter *parent{};
};
}

#endif
