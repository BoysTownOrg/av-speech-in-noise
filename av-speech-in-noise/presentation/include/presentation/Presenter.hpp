#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_

#include <av-speech-in-noise/Model.hpp>
#include <vector>
#include <string>

namespace av_speech_in_noise {
enum class Method {
    adaptivePassFail,
    adaptiveCorrectKeywords,
    defaultAdaptiveCoordinateResponseMeasure,
    adaptiveCoordinateResponseMeasureWithSingleSpeaker,
    adaptiveCoordinateResponseMeasureWithDelayedMasker,
    fixedLevelFreeResponseWithTargetReplacement,
    fixedLevelFreeResponseWithSilentIntervalTargets,
    fixedLevelFreeResponseWithAllTargets,
    fixedLevelCoordinateResponseMeasureWithTargetReplacement,
    fixedLevelCoordinateResponseMeasureWithSilentIntervalTargets,
    unknown
};

class TestSettingsInterpreter {
  public:
    virtual ~TestSettingsInterpreter() = default;
    virtual void initialize(Model &, const std::string &, const TestIdentity &) = 0;
    virtual auto method(const std::string &) -> Method = 0;
};

class TextFileReader {
  public:
    virtual ~TextFileReader() = default;
    virtual auto read(const std::string &) -> std::string = 0;
};

class View {
  public:
    class CoordinateResponseMeasure {
      public:
        class EventListener {
          public:
            virtual ~EventListener() = default;
            virtual void playTrial() = 0;
            virtual void submitResponse() = 0;
        };

        virtual ~CoordinateResponseMeasure() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual auto numberResponse() -> std::string = 0;
        virtual auto greenResponse() -> bool = 0;
        virtual auto blueResponse() -> bool = 0;
        virtual auto whiteResponse() -> bool = 0;
        virtual void showResponseButtons() = 0;
        virtual void hideResponseButtons() = 0;
        virtual void showNextTrialButton() = 0;
        virtual void hideNextTrialButton() = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
    };

    class TestSetup {
      public:
        class EventListener {
          public:
            virtual ~EventListener() = default;
            virtual void confirmTestSetup() = 0;
            virtual void playCalibration() = 0;
            virtual void browseForCalibration() = 0;
            virtual void browseForTestSettingsFile() = 0;
        };

        virtual ~TestSetup() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual auto calibrationLevel_dB_SPL() -> std::string = 0;
        virtual auto calibrationFilePath() -> std::string = 0;
        virtual auto testSettingsFile() -> std::string = 0;
        virtual auto testerId() -> std::string = 0;
        virtual auto subjectId() -> std::string = 0;
        virtual auto session() -> std::string = 0;
        virtual auto condition() -> std::string = 0;
        virtual void setCalibrationFilePath(std::string) = 0;
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
        };

        virtual ~Experimenter() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void showEvaluationButtons() = 0;
        virtual void hideEvaluationButtons() = 0;
        virtual void showFreeResponseSubmission() = 0;
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
};

class Presenter : public Model::EventListener {
  public:
    class TestSetup : public View::TestSetup::EventListener {
      public:
        explicit TestSetup(View::TestSetup *);
        void playCalibration() override;
        void confirmTestSetup() override;
        void browseForCalibration() override;
        void browseForTestSettingsFile() override;
        void show();
        void hide();
        void becomeChild(Presenter *parent);
        void setCalibrationFilePath(std::string);
        void setTestSettingsFile(std::string);
        auto calibrationParameters() -> Calibration;
        auto testSettingsFile() -> std::string;
        auto testIdentity() -> TestIdentity;

      private:
        auto readCondition() -> Condition;
        auto readCalibrationLevel() -> int;
        auto auditoryOnly() -> bool;

        View::TestSetup *view;
        Presenter *parent{};
    };

    class CoordinateResponseMeasure
        : public View::CoordinateResponseMeasure::EventListener {
      public:
        explicit CoordinateResponseMeasure(View::CoordinateResponseMeasure *);
        void playTrial() override;
        void submitResponse() override;
        void start();
        void stop();
        void becomeChild(Presenter *parent);
        void showResponseButtons();
        auto subjectResponse() -> coordinate_response_measure::Response;

      private:
        auto colorResponse() -> coordinate_response_measure::Color;

        View::CoordinateResponseMeasure *view;
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
        void becomeChild(Presenter *parent);
        void show();
        void start();
        void stop();
        void trialPlayed();
        void trialComplete();
        void readyNextTrial();
        void display(std::string);
        void secondaryDisplay(std::string);
        void showPassFailSubmission();
        void showCorrectKeywordsSubmission();
        void showFreeResponseSubmission();
        auto correctKeywords() -> open_set::CorrectKeywords;
        auto freeResponse() -> open_set::FreeResponse;

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
            experimenterPresenter.showFreeResponseSubmission();
        }

      private:
        Experimenter &experimenterPresenter;
    };

    Presenter(Model &, View &, TestSetup &, CoordinateResponseMeasure &,
        Experimenter &, TestSettingsInterpreter &, TextFileReader &);
    void trialComplete() override;
    void run();
    void confirmTestSetup();
    void playTrial();
    void playCalibration();
    void browseForCalibration();
    void browseForTestSettingsFile();
    void submitSubjectResponse();
    void submitFreeResponse();
    void submitPassedTrial();
    void submitFailedTrial();
    void submitCorrectKeywords();
    void exitTest();

    static constexpr auto fullScaleLevel_dB_SPL{119};
    static constexpr auto ceilingSnr_dB{20};
    static constexpr auto floorSnr_dB{-40};
    static constexpr auto trackBumpLimit{10};

  private:
    void proceedToNextTrialAfter(void (Presenter::*f)());
    void submitFailedTrial_();
    void submitPassedTrial_();
    void submitFreeResponse_();
    void submitCorrectKeywords_();
    void hideTest();
    void switchToTestSetupView();
    void showErrorMessage(std::string);
    void playCalibration_();
    void showTestSetup();
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
    Model &model;
    View &view;
    TestSetup &testSetup;
    CoordinateResponseMeasure &coordinateResponseMeasurePresenter;
    Experimenter &experimenterPresenter;
    TestSettingsInterpreter &testSettingsInterpreter;
    TextFileReader &textFileReader;
    TrialCompletionHandler *trialCompletionHandler_{};
};
}

#endif
