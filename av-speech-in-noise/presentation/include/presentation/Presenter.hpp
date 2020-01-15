#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_PRESENTER_HPP_

#include <av-speech-in-noise/Model.hpp>
#include <vector>
#include <string>

namespace av_speech_in_noise {
class View {
  public:
    class Testing {
      public:
        class EventListener {
          public:
            virtual ~EventListener() = default;
            virtual void playTrial() = 0;
            virtual void submitPassedTrial() = 0;
            virtual void submitFailedTrial() = 0;
            virtual void submitResponse() = 0;
        };

        virtual ~Testing() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual void showNextTrialButton() = 0;
        virtual void hideNextTrialButton() = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void showEvaluationButtons() = 0;
        virtual void hideEvaluationButtons() = 0;
        virtual void showResponseSubmission() = 0;
        virtual void hideResponseSubmission() = 0;
        virtual auto response() -> std::string = 0;
        virtual auto flagged() -> bool = 0;
    };

    class Subject {
      public:
        class EventListener {
          public:
            virtual ~EventListener() = default;
            virtual void playTrial() = 0;
            virtual void submitResponse() = 0;
        };

        virtual ~Subject() = default;
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
            virtual void browseForTargetList() = 0;
            virtual void browseForMasker() = 0;
            virtual void browseForCalibration() = 0;
            virtual void browseForTrackSettingsFile() = 0;
        };

        virtual ~TestSetup() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual auto maskerLevel_dB_SPL() -> std::string = 0;
        virtual auto calibrationLevel_dB_SPL() -> std::string = 0;
        virtual auto startingSnr_dB() -> std::string = 0;
        virtual auto maskerFilePath() -> std::string = 0;
        virtual auto calibrationFilePath() -> std::string = 0;
        virtual auto targetListDirectory() -> std::string = 0;
        virtual auto trackSettingsFile() -> std::string = 0;
        virtual auto testerId() -> std::string = 0;
        virtual auto subjectId() -> std::string = 0;
        virtual auto condition() -> std::string = 0;
        virtual auto session() -> std::string = 0;
        virtual auto method() -> std::string = 0;
        virtual void setMasker(std::string) = 0;
        virtual void setTargetListDirectory(std::string) = 0;
        virtual void setCalibrationFilePath(std::string) = 0;
        virtual void setTrackSettingsFile(std::string) = 0;
        virtual void populateConditionMenu(std::vector<std::string>) = 0;
        virtual void populateMethodMenu(std::vector<std::string>) = 0;
    };

    class Experimenter {
      public:
        class EventListener {
          public:
            virtual ~EventListener() = default;
            virtual void exitTest() = 0;
        };

        virtual ~Experimenter() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void hideExitTestButton() = 0;
        virtual void showExitTestButton() = 0;
        virtual void display(std::string) = 0;
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

enum class Method {
    adaptiveOpenSet,
    defaultAdaptiveClosedSet,
    adaptiveClosedSetSingleSpeaker,
    adaptiveClosedSetDelayedMasker,
    defaultFixedLevelOpenSet,
    fixedLevelOpenSetSilentIntervals,
    fixedLevelOpenSetAllStimuli,
    defaultFixedLevelClosedSet,
    fixedLevelClosedSetSilentIntervals
};

constexpr auto methodName(Method c) -> const char * {
    switch (c) {
    case Method::adaptiveOpenSet:
        return "adaptive open-set";
    case Method::defaultAdaptiveClosedSet:
        return "adaptive closed-set";
    case Method::adaptiveClosedSetSingleSpeaker:
        return "adaptive closed-set single speaker";
    case Method::adaptiveClosedSetDelayedMasker:
        return "adaptive closed-set delayed masker";
    case Method::defaultFixedLevelOpenSet:
        return "fixed-level open-set with replacement";
    case Method::defaultFixedLevelClosedSet:
        return "fixed-level closed-set with replacement";
    case Method::fixedLevelOpenSetSilentIntervals:
        return "fixed-level open-set silent intervals";
    case Method::fixedLevelClosedSetSilentIntervals:
        return "fixed-level closed-set silent intervals";
    case Method::fixedLevelOpenSetAllStimuli:
        return "fixed-level open-set all stimuli";
    }
}

class Presenter : public Model::EventListener {
  public:
    class Testing : public View::Testing::EventListener {
      public:
        explicit Testing(View::Testing *);
        void becomeChild(Presenter *parent);
        void show();
        void hide();
        void showEvaluationButtons();
        void showResponseSubmission();
        void showNextTrialButton();
        auto openSetResponse() -> open_set::FreeResponse;
        void playTrial() override;
        void submitPassedTrial() override;
        void submitResponse() override;
        void submitFailedTrial() override;

      private:
        void prepareNextEvaluatedTrial();

        Presenter *parent{};
        View::Testing *view;
    };

    class TestSetup : public View::TestSetup::EventListener {
      public:
        explicit TestSetup(View::TestSetup *);
        void show();
        void hide();
        void becomeChild(Presenter *parent);
        void setMasker(std::string);
        void setStimulusList(std::string);
        void setCalibrationFilePath(std::string);
        void setTrackSettingsFile(std::string);
        auto adaptiveTest() -> AdaptiveTest;
        auto fixedLevelTest() -> FixedLevelTest;
        auto calibrationParameters() -> Calibration;
        auto closedSet() -> bool;
        auto defaultAdaptive() -> bool;
        auto adaptiveClosedSet() -> bool;
        auto adaptiveOpenSet() -> bool;
        auto fixedLevelOpenSet() -> bool;
        auto fixedLevelClosedSet() -> bool;
        auto fixedLevelClosedSetSilentIntervals() -> bool;
        auto fixedLevelSilentIntervals() -> bool;
        auto fixedLevelAllStimuli() -> bool;
        auto singleSpeaker() -> bool;
        auto adaptiveClosedSetDelayedMasker() -> bool;
        auto adaptiveClosedSetSingleSpeaker() -> bool;
        auto delayedMasker() -> bool;
        void playCalibration() override;
        void browseForTargetList() override;
        void browseForMasker() override;
        void confirmTestSetup() override;
        void browseForCalibration() override;
        void browseForTrackSettingsFile() override;

      private:
        auto defaultAdaptiveClosedSet() -> bool;
        auto testIdentity() -> TestIdentity;
        void initialize(Test &);
        auto readCondition() -> Condition;
        auto method(Method m) -> bool;
        auto readMaskerLevel() -> int;
        auto readCalibrationLevel() -> int;
        auto auditoryOnly() -> bool;

        View::TestSetup *view;
        Presenter *parent{};
    };

    class Subject : public View::Subject::EventListener {
      public:
        explicit Subject(View::Subject *);
        void show();
        void hide();
        void becomeChild(Presenter *parent);
        void showResponseButtons();
        auto subjectResponse() -> coordinate_response_measure::Response;
        void playTrial() override;
        void submitResponse() override;

      private:
        void hideResponseButtons();
        void showNextTrialButton();
        auto colorResponse() -> coordinate_response_measure::Color;

        View::Subject *view;
        Presenter *parent{};
    };

    class Experimenter : public View::Experimenter::EventListener {
      public:
        explicit Experimenter(View::Experimenter *);
        void becomeChild(Presenter *parent);
        void show();
        void hide();
        void hideExitTestButton();
        void showExitTestButton();
        void display(std::string);
        void exitTest() override;

      private:
        Presenter *parent{};
        View::Experimenter *view;
    };

    class TrialCompletionHandler {
      public:
        virtual ~TrialCompletionHandler() = default;
        virtual void showResponseView() = 0;
    };

    class AdaptiveClosedSetTestTrialCompletionHandler
        : public TrialCompletionHandler {
      public:
        explicit AdaptiveClosedSetTestTrialCompletionHandler(Subject *subject)
            : subject{subject} {}

        void showResponseView() override { subject->showResponseButtons(); }

      private:
        Subject *subject;
    };

    class AdaptiveOpenSetTestTrialCompletionHandler
        : public TrialCompletionHandler {
      public:
        explicit AdaptiveOpenSetTestTrialCompletionHandler(Testing *testing)
            : testing{testing} {}

        void showResponseView() override { testing->showEvaluationButtons(); }

      private:
        Testing *testing;
    };

    class FixedLevelOpenSetTestTrialCompletionHandler
        : public TrialCompletionHandler {
      public:
        explicit FixedLevelOpenSetTestTrialCompletionHandler(Testing *testing)
            : testing{testing} {}

        void showResponseView() override { testing->showResponseSubmission(); }

      private:
        Testing *testing;
    };

    class FixedLevelClosedSetTestTrialCompletionHandler
        : public TrialCompletionHandler {
      public:
        explicit FixedLevelClosedSetTestTrialCompletionHandler(Subject *subject)
            : subject{subject} {}

        void showResponseView() override { subject->showResponseButtons(); }

      private:
        Subject *subject;
    };

    Presenter(
        Model *, View *, TestSetup *, Subject *, Experimenter *, Testing *);
    void trialComplete() override;
    void run();
    void browseForTargetList();
    void playTrial();
    void submitSubjectResponse();
    void submitExperimenterResponse();
    void browseForMasker();
    void browseForCalibration();
    void browseForTrackSettingsFile();
    void confirmTestSetup();
    void playCalibration();
    void submitPassedTrial();
    void submitFailedTrial();
    void exitTest();
    static int fullScaleLevel_dB_SPL;
    static int ceilingSnr_dB;
    static int floorSnr_dB;
    static int trackBumpLimit;

  private:
    auto fixedLevelSilentIntervals() -> bool;
    auto fixedLevelAllStimuli() -> bool;
    auto defaultAdaptive() -> bool;
    auto singleSpeaker() -> bool;
    auto delayedMasker() -> bool;
    auto adaptiveClosedSetDelayedMasker() -> bool;
    auto adaptiveClosedSetSingleSpeaker() -> bool;
    void proceedToNextTrialAfter(void (Presenter::*f)());
    void submitFailedTrial_();
    void submitPassedTrial_();
    void submitExperimenterResponse_();
    void submitSubjectResponse_();
    void hideTestView();
    void switchToSetupView();
    void showErrorMessage(std::string);
    void playCalibration_();
    void showTestSetup();
    auto testComplete() -> bool;
    void proceedToNextTrial();
    void hideTestSetup();
    auto adaptiveClosedSet() -> bool;
    auto adaptiveOpenSet() -> bool;
    auto closedSet() -> bool;
    auto fixedLevelClosedSet() -> bool;
    auto fixedLevelClosedSetSilentIntervals() -> bool;
    void initializeTest();
    void showTestView();
    void switchToTestView();
    void confirmTestSetup_();
    void applyIfBrowseNotCancelled(
        std::string s, void (TestSetup::*f)(std::string));
    auto trialCompletionHandler() -> TrialCompletionHandler *;

    FixedLevelOpenSetTestTrialCompletionHandler
        fixedLevelOpenSetTrialCompletionHandler;
    FixedLevelClosedSetTestTrialCompletionHandler
        fixedLevelClosedSetTrialCompletionHandler;
    AdaptiveOpenSetTestTrialCompletionHandler
        adaptiveOpenSetTrialCompletionHandler;
    AdaptiveClosedSetTestTrialCompletionHandler
        adaptiveClosedSetTrialCompletionHandler;
    Model *model;
    View *view;
    TestSetup *testSetup;
    Subject *subject;
    Experimenter *experimenter;
    Testing *testing;
    TrialCompletionHandler *trialCompletionHandler_{};
};
}

#endif
