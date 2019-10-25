#ifndef AV_SPEECH_IN_NOISE_TESTS_PRESENTERTESTS_H_
#define AV_SPEECH_IN_NOISE_TESTS_PRESENTERTESTS_H_

#include "assert-utility.h"
#include <presentation/Presenter.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests::presentation {
template <typename T> class Collection {
    std::vector<T> items{};

  public:
    explicit Collection(std::vector<T> items = {}) : items{std::move(items)} {}

    bool contains(const T &item) const {
        return std::find(items.begin(), items.end(), item) != items.end();
    }
};

class ModelStub : public Model {
    AdaptiveTest adaptiveTest_{};
    FixedLevelTest fixedLevelTest_{};
    Calibration calibration_{};
    AudioSettings trialParameters_{};
    coordinate_response_measure::SubjectResponse responseParameters_{};
    std::vector<std::string> audioDevices_{};
    FreeResponse freeResponse_{};
    EventListener *listener_{};
    int trialNumber_{};
    bool testComplete_{};
    bool trialPlayed_{};
    bool fixedLevelTestInitialized_{};
    bool adaptiveTestInitialized_{};
    bool correctResponseSubmitted_{};
    bool incorrectResponseSubmitted_{};
    bool initializedWithFiniteTargets_{};

  public:
    int trialNumber() override { return trialNumber_; }

    void setTrialNumber(int n) { trialNumber_ = n; }

    auto initializedWithFiniteTargets() const {
        return initializedWithFiniteTargets_;
    }

    void initializeTestWithFiniteTargets(const FixedLevelTest &p) override {
        fixedLevelTest_ = p;
        initializedWithFiniteTargets_ = true;
    }

    auto incorrectResponseSubmitted() const {
        return incorrectResponseSubmitted_;
    }

    auto correctResponseSubmitted() const { return correctResponseSubmitted_; }

    auto freeResponse() const { return freeResponse_; }

    void completeTrial() { listener_->trialComplete(); }

    void setAudioDevices(std::vector<std::string> v) {
        audioDevices_ = std::move(v);
    }

    auto &responseParameters() const { return responseParameters_; }

    auto &trialParameters() const { return trialParameters_; }

    void setTestComplete() { testComplete_ = true; }

    bool testComplete() override { return testComplete_; }

    void playTrial(const AudioSettings &p) override {
        trialParameters_ = p;
        trialPlayed_ = true;
    }

    void initializeTest(const AdaptiveTest &p) override {
        adaptiveTestInitialized_ = true;
        adaptiveTest_ = p;
    }

    std::vector<std::string> audioDevices() override { return audioDevices_; }

    void submitResponse(
        const coordinate_response_measure::SubjectResponse &p) override {
        responseParameters_ = p;
    }

    void subscribe(EventListener *listener) override { listener_ = listener; }

    void playCalibration(const Calibration &p) override { calibration_ = p; }

    void submitCorrectResponse() override { correctResponseSubmitted_ = true; }

    void submitIncorrectResponse() override {
        incorrectResponseSubmitted_ = true;
    }

    void initializeTest(const FixedLevelTest &p) override {
        fixedLevelTestInitialized_ = true;
        fixedLevelTest_ = p;
    }

    void submitResponse(const FreeResponse &s) override { freeResponse_ = s; }

    auto trialPlayed() const { return trialPlayed_; }

    auto &adaptiveTest() const { return adaptiveTest_; }

    auto &fixedLevelTest() const { return fixedLevelTest_; }

    auto &calibration() const { return calibration_; }

    auto fixedLevelTestInitialized() const {
        return fixedLevelTestInitialized_;
    }

    auto adaptiveTestInitialized() const { return adaptiveTestInitialized_; }
};

class ViewStub : public View {
    std::vector<std::string> audioDevices_;
    std::string errorMessage_;
    std::string browseForDirectoryResult_;
    std::string browseForOpeningFileResult_;
    std::string audioDevice_;
    bool eventLoopCalled_{};
    bool browseCancelled_{};

  public:
    void setAudioDevice(std::string s) { audioDevice_ = std::move(s); }

    void showErrorMessage(std::string s) override {
        errorMessage_ = std::move(s);
    }

    auto errorMessage() { return errorMessage_; }

    void eventLoop() override { eventLoopCalled_ = true; }

    std::string browseForDirectory() override {
        return browseForDirectoryResult_;
    }

    bool browseCancelled() override { return browseCancelled_; }

    std::string browseForOpeningFile() override {
        return browseForOpeningFileResult_;
    }

    std::string audioDevice() override { return audioDevice_; }

    auto eventLoopCalled() const { return eventLoopCalled_; }

    void setBrowseForDirectoryResult(std::string s) {
        browseForDirectoryResult_ = std::move(s);
    }

    void setBrowseForOpeningFileResult(std::string s) {
        browseForOpeningFileResult_ = std::move(s);
    }

    void setBrowseCancelled() { browseCancelled_ = true; }

    void populateAudioDeviceMenu(std::vector<std::string> v) override {
        audioDevices_ = std::move(v);
    }

    auto audioDevices() const { return audioDevices_; }

    class TestSetupViewStub : public TestSetup {
        Collection<std::string> conditions_;
        Collection<std::string> methods_;
        std::string signalLevel_{"0"};
        std::string calibrationLevel_{"0"};
        std::string startingSnr_{"0"};
        std::string maskerLevel_{"0"};
        std::string masker_;
        std::string condition_;
        std::string stimulusList_;
        std::string subjectId_;
        std::string testerId_;
        std::string session_;
        std::string calibrationFilePath_;
        std::string method_;
        std::string trackSettingsFile_;
        EventListener *listener_{};
        bool shown_{};
        bool hidden_{};
        bool useFiniteTargets_{};

      public:
        bool usingTargetsWithoutReplacement() override {
            return useFiniteTargets_;
        }

        void useFiniteTargets() { useFiniteTargets_ = true; }

        std::string trackSettingsFile() override { return trackSettingsFile_; }

        std::string calibrationLevel_dB_SPL() override {
            return calibrationLevel_;
        }

        std::string maskerLevel_dB_SPL() override { return maskerLevel_; }

        void confirmTestSetup() { listener_->confirmTestSetup(); }

        void playCalibration() { listener_->playCalibration(); }

        std::string session() override { return session_; }

        std::string startingSnr_dB() override { return startingSnr_; }

        void populateConditionMenu(std::vector<std::string> items) override {
            conditions_ = Collection{std::move(items)};
        }

        auto &conditions() const { return conditions_; }

        auto &methods() const { return methods_; }

        auto shown() const { return shown_; }

        void show() override { shown_ = true; }

        void hide() override { hidden_ = true; }

        auto hidden() const { return hidden_; }

        void setStartingSnr(std::string s) { startingSnr_ = std::move(s); }

        void setMethod(std::string s) { method_ = std::move(s); }

        void setCalibrationLevel(std::string s) {
            calibrationLevel_ = std::move(s);
        }

        void setMaskerLevel(std::string s) { maskerLevel_ = std::move(s); }

        void setCalibrationFilePath(std::string s) override {
            calibrationFilePath_ = std::move(s);
        }

        void setCondition(std::string s) { condition_ = std::move(s); }

        void setMasker(std::string s) override { masker_ = std::move(s); }

        void setTrackSettingsFile(std::string s) override {
            trackSettingsFile_ = std::move(s);
        }

        void setSession(std::string s) { session_ = std::move(s); }

        void setTargetListDirectory(std::string s) override {
            stimulusList_ = std::move(s);
        }

        void setSubjectId(std::string s) { subjectId_ = std::move(s); }

        void setTesterId(std::string s) { testerId_ = std::move(s); }

        std::string maskerFilePath() override { return masker_; }

        std::string targetListDirectory() override { return stimulusList_; }

        std::string testerId() override { return testerId_; }

        std::string subjectId() override { return subjectId_; }

        std::string condition() override { return condition_; }

        std::string calibrationFilePath() override {
            return calibrationFilePath_;
        }

        void subscribe(EventListener *listener) override {
            listener_ = listener;
        }

        std::string method() override { return method_; }

        void populateMethodMenu(std::vector<std::string> items) override {
            methods_ = Collection{std::move(items)};
        }

        void browseForMasker() { listener_->browseForMasker(); }

        void browseForTargetList() { listener_->browseForTargetList(); }

        void browseForTrackSettingsFile() {
            listener_->browseForTrackSettingsFile();
        }

        void browseForCalibration() { listener_->browseForCalibration(); }
    };

    class SubjectViewStub : public Subject {
        std::string numberResponse_{"0"};
        EventListener *listener_{};
        bool greenResponse_{};
        bool redResponse_{};
        bool blueResponse_{};
        bool grayResponse_{};
        bool responseButtonsShown_{};
        bool nextTrialButtonShown_{};
        bool responseButtonsHidden_{};
        bool nextTrialButtonHidden_{};
        bool shown_{};
        bool hidden_{};

      public:
        void show() override { shown_ = true; }

        auto shown() const { return shown_; }

        auto hidden() const { return hidden_; }

        bool whiteResponse() override { return grayResponse_; }

        void setGrayResponse() { grayResponse_ = true; }

        bool blueResponse() override { return blueResponse_; }

        void setBlueResponse() { blueResponse_ = true; }

        void setRedResponse() { redResponse_ = true; }

        void hideNextTrialButton() override { nextTrialButtonHidden_ = true; }

        auto nextTrialButtonHidden() const { return nextTrialButtonHidden_; }

        void hideResponseButtons() override { responseButtonsHidden_ = true; }

        auto responseButtonsHidden() const { return responseButtonsHidden_; }

        void showNextTrialButton() override { nextTrialButtonShown_ = true; }

        auto nextTrialButtonShown() const { return nextTrialButtonShown_; }

        auto responseButtonsShown() const { return responseButtonsShown_; }

        void setGreenResponse() { greenResponse_ = true; }

        void setNumberResponse(std::string s) {
            numberResponse_ = std::move(s);
        }

        std::string numberResponse() override { return numberResponse_; }

        bool greenResponse() override { return greenResponse_; }

        void showResponseButtons() override { responseButtonsShown_ = true; }

        void subscribe(EventListener *e) override { listener_ = e; }

        void hide() override { hidden_ = true; }

        void submitResponse() { listener_->submitResponse(); }

        void playTrial() { listener_->playTrial(); }
    };

    class TestingViewStub : public Testing {
        std::string response_;
        EventListener *listener_{};
        bool nextTrialButtonShown_{};
        bool shown_{};
        bool nextTrialButtonHidden_{};
        bool hidden_{};
        bool evaluationButtonsShown_{};
        bool responseSubmissionShown_{};
        bool responseSubmissionHidden_{};
        bool evaluationButtonsHidden_{};

      public:
        void submitFailedTrial() { listener_->submitFailedTrial(); }

        auto responseSubmissionHidden() const {
            return responseSubmissionHidden_;
        }

        auto evaluationButtonsHidden() const {
            return evaluationButtonsHidden_;
        }

        auto nextTrialButtonShown() const { return nextTrialButtonShown_; }

        auto evaluationButtonsShown() const { return evaluationButtonsShown_; }

        auto responseSubmissionShown() const {
            return responseSubmissionShown_;
        }

        void showNextTrialButton() override { nextTrialButtonShown_ = true; }

        auto shown() const { return shown_; }

        auto hidden() const { return hidden_; }

        void show() override { shown_ = true; }

        void subscribe(EventListener *e) override { listener_ = e; }

        void hideNextTrialButton() override { nextTrialButtonHidden_ = true; }

        void hide() override { hidden_ = true; }

        void showEvaluationButtons() override {
            evaluationButtonsShown_ = true;
        }

        std::string response() override { return response_; }

        void showResponseSubmission() override {
            responseSubmissionShown_ = true;
        }

        void hideResponseSubmission() override {
            responseSubmissionHidden_ = true;
        }

        void hideEvaluationButtons() override {
            evaluationButtonsHidden_ = true;
        }

        void playTrial() { listener_->playTrial(); }

        auto nextTrialButtonHidden() const { return nextTrialButtonHidden_; }

        void submitPassedTrial() { listener_->submitPassedTrial(); }

        void setResponse(std::string s) { response_ = std::move(s); }

        void submitResponse() { listener_->submitResponse(); }
    };

    class ExperimenterViewStub : public Experimenter {
        std::string response_;
        std::string displayed_;
        EventListener *listener_{};
        bool shown_{};
        bool hidden_{};
        bool exitTestButtonHidden_{};
        bool exitTestButtonShown_{};

      public:
        void display(std::string s) override { displayed_ = std::move(s); }

        auto displayed() const { return displayed_; }

        void showExitTestButton() override { exitTestButtonShown_ = true; }

        void hideExitTestButton() override { exitTestButtonHidden_ = true; }

        auto exitTestButtonShown() const { return exitTestButtonShown_; }

        auto exitTestButtonHidden() const { return exitTestButtonHidden_; }

        auto shown() const { return shown_; }

        auto hidden() const { return hidden_; }

        void show() override { shown_ = true; }

        void subscribe(EventListener *e) override { listener_ = e; }

        void hide() override { hidden_ = true; }

        void exitTest() { listener_->exitTest(); }
    };
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run() = 0;
};

class ConditionUseCase : public virtual UseCase {
  public:
    virtual Condition condition(ModelStub &) = 0;
};

class LevelUseCase : public virtual UseCase {
  public:
    virtual int fullScaleLevel(ModelStub &) = 0;
};

class PlayingCalibration : public ConditionUseCase, public LevelUseCase {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit PlayingCalibration(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    Condition condition(ModelStub &m) override {
        return m.calibration().condition;
    }

    void run() override { view->playCalibration(); }

    int fullScaleLevel(ModelStub &m) override {
        return m.calibration().fullScaleLevel_dB_SPL;
    }
};

class ConfirmingTestSetup : public virtual ConditionUseCase,
                            public virtual LevelUseCase {
  public:
    virtual int snr_dB(ModelStub &) = 0;
    virtual int maskerLevel(ModelStub &) = 0;
    virtual std::string targetListDirectory(ModelStub &) = 0;
    virtual std::string subjectId(ModelStub &) = 0;
    virtual std::string testerId(ModelStub &) = 0;
    virtual std::string session(ModelStub &) = 0;
    virtual std::string maskerFilePath(ModelStub &) = 0;
};

class ConfirmingAdaptiveTest_ : public virtual ConfirmingTestSetup {
  public:
    virtual int ceilingSnr_dB(ModelStub &) = 0;
    virtual int floorSnr_dB(ModelStub &) = 0;
    virtual int trackBumpLimit(ModelStub &) = 0;
    virtual std::string trackSettingsFile(ModelStub &) = 0;
};

class ConfirmingAdaptiveTest : public ConfirmingAdaptiveTest_ {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingAdaptiveTest(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    auto adaptiveTest(ModelStub &m) { return m.adaptiveTest(); }

    auto common(ModelStub &m) { return adaptiveTest(m).common; }

    auto information(ModelStub &m) { return adaptiveTest(m).information; }

    void run() override { view->confirmTestSetup(); }

    int snr_dB(ModelStub &m) override { return adaptiveTest(m).startingSnr_dB; }

    int maskerLevel(ModelStub &m) override {
        return common(m).maskerLevel_dB_SPL;
    }

    std::string targetListDirectory(ModelStub &m) override {
        return common(m).targetListDirectory;
    }

    std::string subjectId(ModelStub &m) override {
        return information(m).subjectId;
    }

    std::string testerId(ModelStub &m) override {
        return information(m).testerId;
    }

    std::string session(ModelStub &m) override {
        return information(m).session;
    }

    std::string maskerFilePath(ModelStub &m) override {
        return common(m).maskerFilePath;
    }

    int fullScaleLevel(ModelStub &m) override {
        return common(m).fullScaleLevel_dB_SPL;
    }

    Condition condition(ModelStub &m) override { return common(m).condition; }

    int ceilingSnr_dB(ModelStub &m) override {
        return adaptiveTest(m).ceilingSnr_dB;
    }

    int floorSnr_dB(ModelStub &m) override {
        return adaptiveTest(m).floorSnr_dB;
    }

    std::string trackSettingsFile(ModelStub &m) override {
        return adaptiveTest(m).trackSettingsFile;
    }

    int trackBumpLimit(ModelStub &m) override {
        return adaptiveTest(m).trackBumpLimit;
    }
};

void setMethod(ViewStub::TestSetupViewStub *view, Method m) {
    view->setMethod(methodName(m));
}

class ConfirmingAdaptiveClosedSetTest : public ConfirmingAdaptiveTest_ {
    ConfirmingAdaptiveTest confirmingAdaptiveTest;
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingAdaptiveClosedSetTest(ViewStub::TestSetupViewStub *view)
        : confirmingAdaptiveTest{view}, view{view} {}

    void run() override {
        setMethod(view, Method::adaptiveClosedSet);
        confirmingAdaptiveTest.run();
    }

    int snr_dB(ModelStub &m) override {
        return confirmingAdaptiveTest.snr_dB(m);
    }

    int maskerLevel(ModelStub &m) override {
        return confirmingAdaptiveTest.maskerLevel(m);
    }

    int fullScaleLevel(ModelStub &m) override {
        return confirmingAdaptiveTest.fullScaleLevel(m);
    }

    std::string targetListDirectory(ModelStub &m) override {
        return confirmingAdaptiveTest.targetListDirectory(m);
    }

    std::string subjectId(ModelStub &m) override {
        return confirmingAdaptiveTest.subjectId(m);
    }

    std::string testerId(ModelStub &m) override {
        return confirmingAdaptiveTest.testerId(m);
    }

    std::string session(ModelStub &m) override {
        return confirmingAdaptiveTest.session(m);
    }

    std::string maskerFilePath(ModelStub &m) override {
        return confirmingAdaptiveTest.maskerFilePath(m);
    }

    Condition condition(ModelStub &m) override {
        return confirmingAdaptiveTest.condition(m);
    }

    int ceilingSnr_dB(ModelStub &m) override {
        return confirmingAdaptiveTest.ceilingSnr_dB(m);
    }

    int floorSnr_dB(ModelStub &m) override {
        return confirmingAdaptiveTest.floorSnr_dB(m);
    }

    std::string trackSettingsFile(ModelStub &m) override {
        return confirmingAdaptiveTest.trackSettingsFile(m);
    }

    int trackBumpLimit(ModelStub &m) override {
        return confirmingAdaptiveTest.trackBumpLimit(m);
    }
};

class ConfirmingAdaptiveOpenSetTest : public ConfirmingAdaptiveTest_ {
    ConfirmingAdaptiveTest confirmingAdaptiveTest;
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingAdaptiveOpenSetTest(ViewStub::TestSetupViewStub *view)
        : confirmingAdaptiveTest{view}, view{view} {}

    void run() override {
        setMethod(view, Method::adaptiveOpenSet);
        confirmingAdaptiveTest.run();
    }

    int snr_dB(ModelStub &m) override {
        return confirmingAdaptiveTest.snr_dB(m);
    }

    int maskerLevel(ModelStub &m) override {
        return confirmingAdaptiveTest.maskerLevel(m);
    }

    int fullScaleLevel(ModelStub &m) override {
        return confirmingAdaptiveTest.fullScaleLevel(m);
    }

    std::string targetListDirectory(ModelStub &m) override {
        return confirmingAdaptiveTest.targetListDirectory(m);
    }

    std::string subjectId(ModelStub &m) override {
        return confirmingAdaptiveTest.subjectId(m);
    }

    std::string testerId(ModelStub &m) override {
        return confirmingAdaptiveTest.testerId(m);
    }

    std::string session(ModelStub &m) override {
        return confirmingAdaptiveTest.session(m);
    }

    std::string maskerFilePath(ModelStub &m) override {
        return confirmingAdaptiveTest.maskerFilePath(m);
    }

    Condition condition(ModelStub &m) override {
        return confirmingAdaptiveTest.condition(m);
    }

    int ceilingSnr_dB(ModelStub &m) override {
        return confirmingAdaptiveTest.ceilingSnr_dB(m);
    }

    int floorSnr_dB(ModelStub &m) override {
        return confirmingAdaptiveTest.floorSnr_dB(m);
    }

    std::string trackSettingsFile(ModelStub &m) override {
        return confirmingAdaptiveTest.trackSettingsFile(m);
    }

    int trackBumpLimit(ModelStub &m) override {
        return confirmingAdaptiveTest.trackBumpLimit(m);
    }
};

class ConfirmingFixedLevelTest : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingFixedLevelTest(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override { view->confirmTestSetup(); }

    auto fixedLevelTest(ModelStub &m) { return m.fixedLevelTest(); }

    auto common(ModelStub &m) { return fixedLevelTest(m).common; }

    auto information(ModelStub &m) { return fixedLevelTest(m).information; }

    int snr_dB(ModelStub &m) override { return fixedLevelTest(m).snr_dB; }

    int maskerLevel(ModelStub &m) override {
        return common(m).maskerLevel_dB_SPL;
    }

    int fullScaleLevel(ModelStub &m) override {
        return common(m).fullScaleLevel_dB_SPL;
    }

    std::string targetListDirectory(ModelStub &m) override {
        return common(m).targetListDirectory;
    }

    std::string subjectId(ModelStub &m) override {
        return information(m).subjectId;
    }

    std::string testerId(ModelStub &m) override {
        return information(m).testerId;
    }

    std::string session(ModelStub &m) override {
        return information(m).session;
    }

    std::string maskerFilePath(ModelStub &m) override {
        return common(m).maskerFilePath;
    }

    Condition condition(ModelStub &m) override { return common(m).condition; }
};

class ConfirmingFixedLevelOpenSetTest : public ConfirmingTestSetup {
    ConfirmingFixedLevelTest confirmingFixedLevelTest;
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingFixedLevelOpenSetTest(ViewStub::TestSetupViewStub *view)
        : confirmingFixedLevelTest{view}, view{view} {}

    void run() override {
        setMethod(view, Method::fixedLevelOpenSet);
        confirmingFixedLevelTest.run();
    }

    int snr_dB(ModelStub &m) override {
        return confirmingFixedLevelTest.snr_dB(m);
    }

    int maskerLevel(ModelStub &m) override {
        return confirmingFixedLevelTest.maskerLevel(m);
    }

    int fullScaleLevel(ModelStub &m) override {
        return confirmingFixedLevelTest.fullScaleLevel(m);
    }

    std::string targetListDirectory(ModelStub &m) override {
        return confirmingFixedLevelTest.targetListDirectory(m);
    }

    std::string subjectId(ModelStub &m) override {
        return confirmingFixedLevelTest.subjectId(m);
    }

    std::string testerId(ModelStub &m) override {
        return confirmingFixedLevelTest.testerId(m);
    }

    std::string session(ModelStub &m) override {
        return confirmingFixedLevelTest.session(m);
    }

    std::string maskerFilePath(ModelStub &m) override {
        return confirmingFixedLevelTest.maskerFilePath(m);
    }

    Condition condition(ModelStub &m) override {
        return confirmingFixedLevelTest.condition(m);
    }
};

class ConfirmingFixedLevelClosedSetTest : public ConfirmingTestSetup {
    ConfirmingFixedLevelTest confirmingFixedLevelTest;
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingFixedLevelClosedSetTest(
        ViewStub::TestSetupViewStub *view)
        : confirmingFixedLevelTest{view}, view{view} {}

    void run() override {
        setMethod(view, Method::fixedLevelClosedSet);
        confirmingFixedLevelTest.run();
    }

    int snr_dB(ModelStub &m) override {
        return confirmingFixedLevelTest.snr_dB(m);
    }

    int maskerLevel(ModelStub &m) override {
        return confirmingFixedLevelTest.maskerLevel(m);
    }

    int fullScaleLevel(ModelStub &m) override {
        return confirmingFixedLevelTest.fullScaleLevel(m);
    }

    std::string targetListDirectory(ModelStub &m) override {
        return confirmingFixedLevelTest.targetListDirectory(m);
    }

    std::string subjectId(ModelStub &m) override {
        return confirmingFixedLevelTest.subjectId(m);
    }

    std::string testerId(ModelStub &m) override {
        return confirmingFixedLevelTest.testerId(m);
    }

    std::string session(ModelStub &m) override {
        return confirmingFixedLevelTest.session(m);
    }

    std::string maskerFilePath(ModelStub &m) override {
        return confirmingFixedLevelTest.maskerFilePath(m);
    }

    Condition condition(ModelStub &m) override {
        return confirmingFixedLevelTest.condition(m);
    }
};

class TrialSubmission : public virtual UseCase {
  public:
    virtual bool nextTrialButtonShown() = 0;
    virtual bool responseViewShown() = 0;
    virtual bool responseViewHidden() = 0;
};

class RespondingFromSubject : public TrialSubmission {
    ViewStub::SubjectViewStub *view;

  public:
    explicit RespondingFromSubject(ViewStub::SubjectViewStub *view)
        : view{view} {}

    void run() override { view->submitResponse(); }

    bool nextTrialButtonShown() override {
        return view->nextTrialButtonShown();
    }

    bool responseViewShown() override { return view->responseButtonsShown(); }

    bool responseViewHidden() override { return view->responseButtonsHidden(); }
};

class RespondingFromExperimenter : public TrialSubmission {
    ViewStub::TestingViewStub *view;

  public:
    explicit RespondingFromExperimenter(ViewStub::TestingViewStub *view)
        : view{view} {}

    void run() override { view->submitResponse(); }

    bool nextTrialButtonShown() override {
        return view->nextTrialButtonShown();
    }

    bool responseViewShown() override {
        return view->responseSubmissionShown();
    }

    bool responseViewHidden() override {
        return view->responseSubmissionHidden();
    }
};

class ExitingTest : public UseCase {
    ViewStub::ExperimenterViewStub *view;

  public:
    explicit ExitingTest(ViewStub::ExperimenterViewStub *view) : view{view} {}

    void run() override { view->exitTest(); }
};

class SubmittingPassedTrial : public TrialSubmission {
    ViewStub::TestingViewStub *view;

  public:
    explicit SubmittingPassedTrial(ViewStub::TestingViewStub *view)
        : view{view} {}

    void run() override { view->submitPassedTrial(); }

    bool nextTrialButtonShown() override {
        return view->nextTrialButtonShown();
    }

    bool responseViewShown() override { return view->evaluationButtonsShown(); }

    bool responseViewHidden() override {
        return view->evaluationButtonsHidden();
    }
};

class SubmittingFailedTrial : public TrialSubmission {
    ViewStub::TestingViewStub *view;

  public:
    explicit SubmittingFailedTrial(ViewStub::TestingViewStub *view)
        : view{view} {}

    void run() override { view->submitFailedTrial(); }

    bool nextTrialButtonShown() override {
        return view->nextTrialButtonShown();
    }

    bool responseViewShown() override { return view->evaluationButtonsShown(); }

    bool responseViewHidden() override {
        return view->evaluationButtonsHidden();
    }
};

class PlayingTrial : public virtual UseCase {
  public:
    virtual bool nextTrialButtonHidden() = 0;
    virtual bool nextTrialButtonShown() = 0;
};

class PlayingTrialFromSubject : public PlayingTrial {
    ViewStub::SubjectViewStub *view;

  public:
    explicit PlayingTrialFromSubject(ViewStub::SubjectViewStub *view)
        : view{view} {}

    void run() override { view->playTrial(); }

    bool nextTrialButtonHidden() override {
        return view->nextTrialButtonHidden();
    }

    bool nextTrialButtonShown() override {
        return view->nextTrialButtonShown();
    }
};

class PlayingTrialFromExperimenter : public PlayingTrial {
    ViewStub::TestingViewStub *view;

  public:
    explicit PlayingTrialFromExperimenter(ViewStub::TestingViewStub *view)
        : view{view} {}

    void run() override { view->playTrial(); }

    bool nextTrialButtonHidden() override {
        return view->nextTrialButtonHidden();
    }

    bool nextTrialButtonShown() override {
        return view->nextTrialButtonShown();
    }
};

class BrowsingUseCase : public virtual UseCase {
  public:
    virtual void setResult(ViewStub &, std::string) = 0;
};

class BrowsingEnteredPathUseCase : public virtual BrowsingUseCase {
  public:
    virtual std::string entry() = 0;
    virtual void setEntry(std::string) = 0;
};

class BrowsingForMasker : public BrowsingEnteredPathUseCase {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit BrowsingForMasker(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    std::string entry() override { return view->maskerFilePath(); }

    void setEntry(std::string s) override { view->setMasker(std::move(s)); }

    void setResult(ViewStub &view_, std::string s) override {
        return view_.setBrowseForOpeningFileResult(s);
    }

    void run() override { view->browseForMasker(); }
};

class BrowsingForTrackSettingsFile : public BrowsingEnteredPathUseCase {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit BrowsingForTrackSettingsFile(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override { view->browseForTrackSettingsFile(); }

    void setResult(ViewStub &view_, std::string s) override {
        view_.setBrowseForOpeningFileResult(s);
    }

    std::string entry() override { return view->trackSettingsFile(); }

    void setEntry(std::string s) override {
        view->setTrackSettingsFile(std::move(s));
    }
};

class BrowsingForTargetList : public BrowsingEnteredPathUseCase {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit BrowsingForTargetList(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override { view->browseForTargetList(); }

    void setResult(ViewStub &view_, std::string s) override {
        view_.setBrowseForDirectoryResult(s);
    }

    std::string entry() override { return view->targetListDirectory(); }

    void setEntry(std::string s) override {
        view->setTargetListDirectory(std::move(s));
    }
};

class BrowsingForCalibration : public BrowsingEnteredPathUseCase {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit BrowsingForCalibration(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    std::string entry() override { return view->calibrationFilePath(); }

    void setEntry(std::string s) override {
        view->setCalibrationFilePath(std::move(s));
    }

    void setResult(ViewStub &view_, std::string s) override {
        return view_.setBrowseForOpeningFileResult(s);
    }

    void run() override { view->browseForCalibration(); }
};

class PresenterConstructionTests : public ::testing::Test {
  protected:
    ModelStub model;
    ViewStub::TestSetupViewStub setupView;
    ViewStub::SubjectViewStub subjectView;
    ViewStub::ExperimenterViewStub experimenterView;
    ViewStub::TestingViewStub testingView;
    ViewStub view;
    Presenter::TestSetup testSetup{&setupView};
    Presenter::Subject subject{&subjectView};
    Presenter::Experimenter experimenter{&experimenterView};
    Presenter::Testing testing{&testingView};

    Presenter construct() {
        return {&model, &view, &testSetup, &subject, &experimenter, &testing};
    }
};

TEST_F(PresenterConstructionTests, populatesAudioDeviceMenu) {
    model.setAudioDevices({"a", "b", "c"});
    construct();
    assertEqual({"a", "b", "c"}, view.audioDevices());
}

class PresenterTests : public ::testing::Test {
  protected:
    ModelStub model;
    ViewStub view;
    ViewStub::TestSetupViewStub setupView;
    ViewStub::SubjectViewStub subjectView;
    ViewStub::ExperimenterViewStub experimenterView;
    ViewStub::TestingViewStub testingView;
    Presenter::TestSetup testSetup{&setupView};
    Presenter::Experimenter experimenter{&experimenterView};
    Presenter::Testing testing{&testingView};
    Presenter::Subject subject{&subjectView};
    Presenter presenter{
        &model, &view, &testSetup, &subject, &experimenter, &testing};
    BrowsingForTrackSettingsFile browsingForTrackSettingsFile{&setupView};
    BrowsingForTargetList browsingForTargetList{&setupView};
    BrowsingForMasker browsingForMasker{&setupView};
    BrowsingForCalibration browsingForCalibration{&setupView};
    ConfirmingAdaptiveClosedSetTest confirmingAdaptiveClosedSetTest{&setupView};
    ConfirmingAdaptiveOpenSetTest confirmingAdaptiveOpenSetTest{&setupView};
    ConfirmingFixedLevelOpenSetTest confirmingFixedLevelOpenSetTest{&setupView};
    ConfirmingFixedLevelClosedSetTest confirmingFixedLevelClosedSetTest{
        &setupView};
    PlayingCalibration playingCalibration{&setupView};
    PlayingTrialFromSubject playingTrialFromSubject{&subjectView};
    PlayingTrialFromExperimenter playingTrialFromExperimenter{&testingView};
    RespondingFromSubject respondingFromSubject{&subjectView};
    RespondingFromExperimenter respondingFromExperimenter{&testingView};
    SubmittingPassedTrial submittingPassedTrial{&testingView};
    SubmittingFailedTrial submittingFailedTrial{&testingView};
    ExitingTest exitingTest{&experimenterView};

    std::string auditoryOnlyConditionName() {
        return conditionName(Condition::auditoryOnly);
    }

    std::string audioVisualConditionName() {
        return conditionName(Condition::audioVisual);
    }

    void respondFromSubject() { subjectView.submitResponse(); }

    void respondFromExperimenter() { testingView.submitResponse(); }

    void exitTest() { experimenterView.exitTest(); }

    void playCalibration() { setupView.playCalibration(); }

    void assertSetupViewShown() { assertTrue(setupViewShown()); }

    bool setupViewShown() { return setupView.shown(); }

    void assertSetupViewNotShown() { assertFalse(setupViewShown()); }

    void assertSetupViewHidden() { assertTrue(setupViewHidden()); }

    bool setupViewHidden() { return setupView.hidden(); }

    void assertSetupViewNotHidden() { assertFalse(setupViewHidden()); }

    void assertExperimenterViewShown() { assertTrue(experimenterViewShown()); }

    void assertTestingViewShown() { assertTrue(testingViewShown()); }

    bool experimenterViewShown() { return experimenterView.shown(); }

    bool testingViewShown() { return testingView.shown(); }

    void assertExperimenterViewHidden() {
        assertTrue(experimenterViewHidden());
    }

    void assertTestingViewHidden() { assertTrue(testingViewHidden()); }

    bool experimenterViewHidden() { return experimenterView.hidden(); }

    bool testingViewHidden() { return testingView.hidden(); }

    void assertExperimenterViewNotHidden() {
        assertFalse(experimenterViewHidden());
    }

    void assertTestingViewNotHidden() { assertFalse(testingViewHidden()); }

    void assertSubjectViewShown() { assertTrue(subjectViewShown()); }

    bool subjectViewShown() { return subjectView.shown(); }

    void assertSubjectViewNotShown() { assertFalse(subjectViewShown()); }

    void assertSubjectViewHidden() { assertTrue(subjectView.hidden()); }

    void assertBrowseResultPassedToEntry(BrowsingEnteredPathUseCase &useCase) {
        setBrowsingResult(useCase, "a");
        run(useCase);
        assertEntryEquals(useCase, "a");
    }

    void setBrowsingResult(BrowsingEnteredPathUseCase &useCase, std::string s) {
        useCase.setResult(view, std::move(s));
    }

    void assertEntryEquals(BrowsingEnteredPathUseCase &useCase, std::string s) {
        assertEqual(std::move(s), entry(useCase));
    }

    std::string entry(BrowsingEnteredPathUseCase &useCase) {
        return useCase.entry();
    }

    void assertCancellingBrowseDoesNotChangePath(
        BrowsingEnteredPathUseCase &useCase) {
        useCase.setEntry("a");
        setBrowsingResult(useCase, "b");
        view.setBrowseCancelled();
        run(useCase);
        assertEntryEquals(useCase, "a");
    }

    void completeTrial() { model.completeTrial(); }

    void assertSetupViewConditionsContains(std::string s) {
        assertTrue(setupView.conditions().contains(std::move(s)));
    }

    void assertSetupViewMethodsContains(std::string s) {
        assertTrue(setupView.methods().contains(std::move(s)));
    }

    void assertSetupViewMethodsContains(Method m) {
        assertTrue(setupView.methods().contains(methodName(m)));
    }

    void setCondition(std::string s) { setupView.setCondition(std::move(s)); }

    std::string errorMessage() { return view.errorMessage(); }

    void assertModelPassedCondition(coordinate_response_measure::Color c) {
        assertEqual(c, model.responseParameters().color);
    }

    const AdaptiveTest &adaptiveTest() { return model.adaptiveTest(); }

    const Calibration &calibration() { return model.calibration(); }

    void assertInvalidCalibrationLevelShowsErrorMessage(UseCase &useCase) {
        setCalibrationLevel("a");
        run(useCase);
        assertErrorMessageEquals("'a' is not a valid calibration level.");
    }

    void assertErrorMessageEquals(std::string s) {
        assertEqual(std::move(s), errorMessage());
    }

    void assertInvalidMaskerLevelShowsErrorMessage(UseCase &useCase) {
        setMaskerLevel("a");
        run(useCase);
        assertErrorMessageEquals("'a' is not a valid masker level.");
    }

    void setAudioDevice(std::string s) { view.setAudioDevice(std::move(s)); }

    void setCalibrationLevel(std::string s) {
        setupView.setCalibrationLevel(std::move(s));
    }

    void setMaskerLevel(std::string s) {
        setupView.setMaskerLevel(std::move(s));
    }

    void assertAudioVisualConditionPassedToModel(ConditionUseCase &useCase) {
        setCondition(audioVisualConditionName());
        run(useCase);
        assertModelPassedCondition(useCase, Condition::audioVisual);
    }

    void assertModelPassedCondition(ConditionUseCase &useCase, Condition c) {
        assertEqual(c, modelCondition(useCase));
    }

    Condition modelCondition(ConditionUseCase &useCase) {
        return useCase.condition(model);
    }

    void assertAuditoryOnlyConditionPassedToModel(ConditionUseCase &useCase) {
        setCondition(auditoryOnlyConditionName());
        run(useCase);
        assertModelPassedCondition(useCase, Condition::auditoryOnly);
    }

    void setStartingSnr(std::string s) {
        setupView.setStartingSnr(std::move(s));
    }

    void setTestComplete() { model.setTestComplete(); }

    void assertAudioDevicePassedToTrial(PlayingTrial &useCase) {
        setAudioDevice("a");
        run(useCase);
        assertEqual("a", model.trialParameters().audioDevice);
    }

    void run(UseCase &useCase) { useCase.run(); }

    void assertPlaysTrial(UseCase &useCase) {
        run(useCase);
        assertTrue(trialPlayed());
    }

    bool trialPlayed() { return model.trialPlayed(); }

    void assertHidesPlayTrialButton(PlayingTrial &useCase) {
        run(useCase);
        assertTrue(useCase.nextTrialButtonHidden());
    }

    void assertHidesExitTestButton(PlayingTrial &useCase) {
        run(useCase);
        assertTrue(exitTestButtonHidden());
    }

    bool exitTestButtonHidden() {
        return experimenterView.exitTestButtonHidden();
    }

    bool exitTestButtonShown() {
        return experimenterView.exitTestButtonShown();
    }

    void assertConfirmTestSetupShowsNextTrialButton(
        ConfirmingTestSetup &confirmingTest, PlayingTrial &playingTrial) {
        run(confirmingTest);
        assertTrue(playingTrial.nextTrialButtonShown());
    }

    void assertCompleteTestShowsSetupView(TrialSubmission &useCase) {
        setTestComplete();
        run(useCase);
        assertSetupViewShown();
    }

    void assertIncompleteTestDoesNotShowSetupView(TrialSubmission &useCase) {
        run(useCase);
        assertSetupViewNotShown();
    }

    void assertCompleteTestHidesExperimenterView(TrialSubmission &useCase) {
        setTestComplete();
        assertHidesExperimenterView(useCase);
    }

    void assertCompleteTestHidesTestingView(TrialSubmission &useCase) {
        setTestComplete();
        assertHidesTestingView(useCase);
    }

    void assertHidesExperimenterView(UseCase &useCase) {
        run(useCase);
        assertExperimenterViewHidden();
    }

    void assertHidesTestingView(UseCase &useCase) {
        run(useCase);
        assertTestingViewHidden();
    }

    void assertCompleteTestDoesNotPlayTrial(UseCase &useCase) {
        setTestComplete();
        run(useCase);
        assertFalse(trialPlayed());
    }

    void assertDoesNotHideExperimenterView(TrialSubmission &useCase) {
        run(useCase);
        assertExperimenterViewNotHidden();
    }

    void assertDoesNotHideTestingView(TrialSubmission &useCase) {
        run(useCase);
        assertTestingViewNotHidden();
    }

    void assertShowsNextTrialButton(TrialSubmission &useCase) {
        run(useCase);
        assertTrue(useCase.nextTrialButtonShown());
    }

    void assertStartingSnrPassedToModel(ConfirmingTestSetup &useCase) {
        setStartingSnr("1");
        run(useCase);
        assertEqual(1, useCase.snr_dB(model));
    }

    void assertMaskerLevelPassedToModel(ConfirmingTestSetup &useCase) {
        setMaskerLevel("2");
        run(useCase);
        assertEqual(2, useCase.maskerLevel(model));
    }

    void assertHidesTestSetupView(UseCase &useCase) {
        run(useCase);
        assertSetupViewHidden();
    }

    void assertDoesNotHideTestSetupView(UseCase &useCase) {
        run(useCase);
        assertSetupViewNotHidden();
    }

    void assertShowsExperimenterView(UseCase &useCase) {
        run(useCase);
        assertExperimenterViewShown();
    }

    void assertShowsTestingView(UseCase &useCase) {
        run(useCase);
        assertTestingViewShown();
    }

    void assertDoesNotShowSubjectView(UseCase &useCase) {
        run(useCase);
        assertSubjectViewNotShown();
    }

    void assertDoesNotInitializeFixedLevelTest(UseCase &useCase) {
        run(useCase);
        assertFalse(model.fixedLevelTestInitialized());
    }

    void assertPassesTargetListDirectory(ConfirmingTestSetup &useCase) {
        setupView.setTargetListDirectory("a");
        run(useCase);
        assertEqual("a", useCase.targetListDirectory(model));
    }

    void assertPassesSubjectId(ConfirmingTestSetup &useCase) {
        setupView.setSubjectId("b");
        run(useCase);
        assertEqual("b", useCase.subjectId(model));
    }

    void assertPassesTesterId(ConfirmingTestSetup &useCase) {
        setupView.setTesterId("c");
        run(useCase);
        assertEqual("c", useCase.testerId(model));
    }

    void assertPassesMasker(ConfirmingTestSetup &useCase) {
        setupView.setMasker("d");
        run(useCase);
        assertEqual("d", useCase.maskerFilePath(model));
    }

    void assertPassesSession(ConfirmingTestSetup &useCase) {
        setupView.setSession("e");
        run(useCase);
        assertEqual("e", useCase.session(model));
    }

    void assertPassesFullScaleLevel(LevelUseCase &useCase) {
        run(useCase);
        assertEqual(
            Presenter::fullScaleLevel_dB_SPL, useCase.fullScaleLevel(model));
    }

    void assertPassesCeilingSNR(ConfirmingAdaptiveTest_ &useCase) {
        run(useCase);
        assertEqual(Presenter::ceilingSnr_dB, useCase.ceilingSnr_dB(model));
    }

    void assertPassesFloorSNR(ConfirmingAdaptiveTest_ &useCase) {
        run(useCase);
        assertEqual(Presenter::floorSnr_dB, useCase.floorSnr_dB(model));
    }

    void assertPassesTrackBumpLimit(ConfirmingAdaptiveTest_ &useCase) {
        run(useCase);
        assertEqual(Presenter::trackBumpLimit, useCase.trackBumpLimit(model));
    }

    void assertPassesTrackSettingsFile(ConfirmingAdaptiveTest_ &useCase) {
        setupView.setTrackSettingsFile("e");
        run(useCase);
        assertEqual("e", useCase.trackSettingsFile(model));
    }

    void assertInvalidSnrShowsErrorMessage(UseCase &useCase) {
        setStartingSnr("a");
        run(useCase);
        assertErrorMessageEquals("'a' is not a valid SNR.");
    }

    void assertSetupViewNotHiddenWhenSnrIsInvalid(UseCase &useCase) {
        setupView.setStartingSnr("?");
        run(useCase);
        assertSetupViewNotHidden();
    }

    void assertCompleteTrialShowsResponseView(
        ConfirmingTestSetup &useCase, TrialSubmission &trialSubmission) {
        run(useCase);
        completeTrial();
        assertTrue(trialSubmission.responseViewShown());
    }

    void assertShowsTrialNumber(UseCase &useCase) {
        setTrialNumber(1);
        run(useCase);
        assertDisplayedToExperimenter("Trial 1");
    }

    void setTrialNumber(int n) { model.setTrialNumber(n); }

    void assertDisplayedToExperimenter(const std::string &s) {
        assertEqual(s, experimenterView.displayed());
    }

    void assertResponseViewHidden(TrialSubmission &useCase) {
        run(useCase);
        assertTrue(useCase.responseViewHidden());
    }

    void assertShowsSubjectView(UseCase &useCase) {
        run(useCase);
        assertSubjectViewShown();
    }

    void assertDoesNotInitializeAdaptiveTest(UseCase &useCase) {
        run(useCase);
        assertFalse(model.adaptiveTestInitialized());
    }
};

class RequestFailingModel : public Model {
    std::string errorMessage{};

  public:
    int trialNumber() override { return 0; }

    void setErrorMessage(std::string s) { errorMessage = std::move(s); }

    void initializeTest(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeTest(const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeTestWithFiniteTargets(const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void playTrial(const AudioSettings &) override {
        throw RequestFailure{errorMessage};
    }

    void submitResponse(
        const coordinate_response_measure::SubjectResponse &) override {
        throw RequestFailure{errorMessage};
    }

    void submitResponse(const FreeResponse &) override {
        throw RequestFailure{errorMessage};
    }

    void playCalibration(const Calibration &) override {
        throw RequestFailure{errorMessage};
    }

    bool testComplete() override { return {}; }
    std::vector<std::string> audioDevices() override { return {}; }
    void subscribe(EventListener *) override {}
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
};

class PresenterFailureTests : public ::testing::Test {
  protected:
    RequestFailingModel failure;
    ModelStub defaultModel;
    Model *model{&defaultModel};
    ViewStub view;
    ViewStub::TestSetupViewStub setupView;
    ViewStub::SubjectViewStub subjectView;
    ViewStub::ExperimenterViewStub experimenterView;
    ViewStub::TestingViewStub testingView;
    Presenter::TestSetup testSetup{&setupView};
    Presenter::Subject subject{&subjectView};
    Presenter::Experimenter experimenter{&experimenterView};
    Presenter::Testing testing{&testingView};

    void useFailingModel(std::string s = {}) {
        failure.setErrorMessage(std::move(s));
        model = &failure;
    }

    void confirmTestSetup() {
        Presenter presenter{
            model, &view, &testSetup, &subject, &experimenter, &testing};
        setupView.confirmTestSetup();
    }

    void assertConfirmTestSetupShowsErrorMessage(std::string s) {
        confirmTestSetup();
        assertEqual(std::move(s), view.errorMessage());
    }

    void assertConfirmTestSetupDoesNotHideSetupView() {
        confirmTestSetup();
        assertFalse(setupView.hidden());
    }
};
}

#endif
