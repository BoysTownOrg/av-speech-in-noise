#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_

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
            virtual void submitFailedTrial() = 0;
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
        virtual void hideExitTestButton() = 0;
        virtual void showExitTestButton() = 0;
        virtual void showNextTrialButton() = 0;
        virtual void hideNextTrialButton() = 0;
        virtual void display(std::string) = 0;
        virtual void secondaryDisplay(std::string) = 0;
    };

    class FreeResponseInput {
      public:
        class EventListener {
          public:
            virtual ~EventListener() = default;
            virtual void notifyThatSubmitButtonHasBeenClicked() = 0;
        };
        virtual ~FreeResponseInput() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual auto flagged() -> bool = 0;
        virtual auto freeResponse() -> std::string = 0;
    };

    class FreeResponseOutput {
      public:
        virtual ~FreeResponseOutput() = default;
        virtual void showFreeResponseSubmission() = 0;
        virtual void hideFreeResponseSubmission() = 0;
        virtual void clearFreeResponse() = 0;
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

class TaskPresenter : virtual public TaskResponder::EventListener {
  public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void showResponseSubmission() = 0;
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

    class Experimenter : public View::Experimenter::EventListener {
      public:
        explicit Experimenter(View::Experimenter *,
            View::CorrectKeywordsInput * = {},
            View::CorrectKeywordsOutput * = {});
        void exitTest() override;
        void playTrial() override;
        void submitPassedTrial() override;
        void submitFailedTrial() override;
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
        auto correctKeywords() -> CorrectKeywords;

      private:
        Presenter *parent{};
        View::Experimenter *view;
        View::CorrectKeywordsInput *correctKeywordsInput;
        View::CorrectKeywordsOutput *correctKeywordsOutput;
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
            TaskPresenter *presenter)
            : presenter{presenter} {}

        void showResponseSubmission() override {
            presenter->showResponseSubmission();
        }

      private:
        TaskPresenter *presenter;
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
        explicit CorrectKeywordsTrialCompletionHandler(TaskPresenter *presenter)
            : presenter{presenter} {}

        void showResponseSubmission() override {
            presenter->showResponseSubmission();
        }

      private:
        TaskPresenter *presenter;
    };

    class FreeResponseTrialCompletionHandler : public TrialCompletionHandler {
      public:
        explicit FreeResponseTrialCompletionHandler(TaskPresenter *presenter)
            : presenter{presenter} {}

        void showResponseSubmission() override {
            presenter->showResponseSubmission();
        }

      private:
        TaskPresenter *presenter;
    };

    Presenter(Model &, View &, TestSetup &, Experimenter &,
        TestSettingsInterpreter &, TextFileReader &, TaskResponder * = {},
        TaskPresenter * = {}, TaskResponder * = {}, TaskPresenter * = {},
        TaskResponder * = {}, TaskPresenter * = {}, TaskResponder * = {},
        TaskPresenter * = {});
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
    void readyNextTrialIfNeeded();
    void showContinueTestingDialogWithResultsWhenComplete();

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
    Experimenter &experimenterPresenter;
    TestSettingsInterpreter &testSettingsInterpreter;
    TextFileReader &textFileReader;
    TrialCompletionHandler *trialCompletionHandler_{};
    TaskPresenter *consonantPresenter;
    TaskPresenter *coordinateResponseMeasurePresenter;
    TaskPresenter *freeResponsePresenter;
    TaskPresenter *correctKeywordsPresenter;
};

class ConsonantPresenter : public TaskPresenter {
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

static auto colorResponse(View::CoordinateResponseMeasureInput *inputView)
    -> coordinate_response_measure::Color {
    if (inputView->greenResponse())
        return coordinate_response_measure::Color::green;
    if (inputView->blueResponse())
        return coordinate_response_measure::Color::blue;
    if (inputView->whiteResponse())
        return coordinate_response_measure::Color::white;

    return coordinate_response_measure::Color::red;
}

static auto subjectResponse(View::CoordinateResponseMeasureInput *inputView)
    -> coordinate_response_measure::Response {
    coordinate_response_measure::Response p{};
    p.color = colorResponse(inputView);
    p.number = std::stoi(inputView->numberResponse());
    return p;
}

class CoordinateResponseMeasureResponder
    : public TaskResponder,
      public View::CoordinateResponseMeasureInput::EventListener {
  public:
    explicit CoordinateResponseMeasureResponder(
        Model &model, View::CoordinateResponseMeasureInput &view)
        : model{model}, view{view} {
        view.subscribe(this);
    }
    void subscribe(TaskResponder::EventListener *e) override { listener = e; }
    void notifyThatReadyButtonHasBeenClicked() override {
        parent->playTrial();
        listener->notifyThatTaskHasStarted();
    }
    void notifyThatResponseButtonHasBeenClicked() override {
        model.submit(subjectResponse(&view));
        parent->playNextTrialIfNeeded();
        listener->notifyThatUserIsDoneResponding();
    }
    void becomeChild(Presenter *p) override { parent = p; }

  private:
    Model &model;
    View::CoordinateResponseMeasureInput &view;
    TaskResponder::EventListener *listener{};
    Presenter *parent{};
};

class CoordinateResponseMeasurePresenter : public TaskPresenter {
  public:
    explicit CoordinateResponseMeasurePresenter(
        View::CoordinateResponseMeasureOutput &view)
        : view{view} {}
    void start() override {
        view.show();
        view.showNextTrialButton();
    }
    void stop() override {
        view.hideResponseButtons();
        view.hide();
    }
    void notifyThatTaskHasStarted() override { view.hideNextTrialButton(); }
    void notifyThatUserIsDoneResponding() override {
        view.hideResponseButtons();
    }
    void showResponseSubmission() override { view.showResponseButtons(); }

  private:
    View::CoordinateResponseMeasureOutput &view;
};

class FreeResponseResponder : public TaskResponder,
                              public View::FreeResponseInput::EventListener {
  public:
    explicit FreeResponseResponder(Model &model, View::FreeResponseInput &view)
        : model{model}, view{view} {
        view.subscribe(this);
    }
    void subscribe(TaskResponder::EventListener *e) override { listener = e; }
    void notifyThatSubmitButtonHasBeenClicked() override {
        model.submit(FreeResponse{view.freeResponse(), view.flagged()});
        listener->notifyThatUserIsDoneResponding();
        parent->readyNextTrialIfNeeded();
    }
    void becomeChild(Presenter *p) override { parent = p; }

  private:
    Model &model;
    View::FreeResponseInput &view;
    TaskResponder::EventListener *listener{};
    Presenter *parent{};
};

class FreeResponsePresenter : public TaskPresenter {
  public:
    explicit FreeResponsePresenter(
        View::Experimenter &experimenterView, View::FreeResponseOutput &view)
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
    View::Experimenter &experimenterView;
    View::FreeResponseOutput &view;
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
    void becomeChild(Presenter *p) override { parent = p; }

  private:
    Model &model;
    View &mainView;
    View::CorrectKeywordsInput &view;
    TaskResponder::EventListener *listener{};
    Presenter *parent{};
};

class CorrectKeywordsPresenter : public TaskPresenter {
  public:
    explicit CorrectKeywordsPresenter(
        View::Experimenter &experimenterView, View::CorrectKeywordsOutput &view)
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
    View::Experimenter &experimenterView;
    View::CorrectKeywordsOutput &view;
};
}

#endif
