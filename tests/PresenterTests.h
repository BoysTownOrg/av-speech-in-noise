#ifndef AV_SPEECH_IN_NOISE_TESTS_PRESENTERTESTS_H_
#define AV_SPEECH_IN_NOISE_TESTS_PRESENTERTESTS_H_

#include "assert-utility.h"
#include "av-speech-in-noise/Model.hpp"
#include <presentation/Presenter.hpp>
#include <gtest/gtest.h>
#include <algorithm>
#include <utility>

namespace av_speech_in_noise::tests::presentation {
template <typename T> class Collection {
  public:
    explicit Collection(std::vector<T> items = {}) : items{std::move(items)} {}

    [[nodiscard]] auto contains(const T &item) const -> bool {
        return std::find(items.begin(), items.end(), item) != items.end();
    }

  private:
    std::vector<T> items{};
};

class ModelStub : public Model {
  public:
    auto trialNumber() -> int override { return trialNumber_; }

    void setTrialNumber(int n) { trialNumber_ = n; }

    [[nodiscard]] auto
    fixedLevelTestWithSilentIntervalTargetsInitialized() const {
        return fixedLevelTestWithSilentIntervalTargetsInitialized_;
    }

    [[nodiscard]] auto fixedLevelTestWithAllTargetsInitialized() const {
        return fixedLevelTestWithAllTargetsInitialized_;
    }

    [[nodiscard]] auto initializedWithSingleSpeaker() const {
        return initializedWithSingleSpeaker_;
    }

    [[nodiscard]] auto initializedWithDelayedMasker() const {
        return initializedWithDelayedMasker_;
    }

    [[nodiscard]] auto defaultFixedLevelTestInitialized() const {
        return defaultFixedLevelTestInitialized_;
    }

    [[nodiscard]] auto defaultAdaptiveTestInitialized() const {
        return defaultAdaptiveTestInitialized_;
    }

    [[nodiscard]] auto trialPlayed() const { return trialPlayed_; }

    [[nodiscard]] auto adaptiveTest() const -> auto & { return adaptiveTest_; }

    [[nodiscard]] auto fixedLevelTest() const -> auto & {
        return fixedLevelTest_;
    }

    [[nodiscard]] auto calibration() const -> auto & { return calibration_; }

    [[nodiscard]] auto incorrectResponseSubmitted() const {
        return incorrectResponseSubmitted_;
    }

    [[nodiscard]] auto correctResponseSubmitted() const {
        return correctResponseSubmitted_;
    }

    [[nodiscard]] auto freeResponse() const { return freeResponse_; }

    [[nodiscard]] auto responseParameters() const -> auto & {
        return responseParameters_;
    }

    [[nodiscard]] auto trialParameters() const -> auto & {
        return trialParameters_;
    }

    void initialize(const FixedLevelTest &p) override {
        fixedLevelTest_ = p;
        defaultFixedLevelTestInitialized_ = true;
    }

    void initialize(const AdaptiveTest &p) override {
        adaptiveTest_ = p;
        defaultAdaptiveTestInitialized_ = true;
    }

    void initializeWithSingleSpeaker(const AdaptiveTest &p) override {
        adaptiveTest_ = p;
        initializedWithSingleSpeaker_ = true;
    }

    void initializeWithDelayedMasker(const AdaptiveTest &p) override {
        adaptiveTest_ = p;
        initializedWithDelayedMasker_ = true;
    }

    void initializeWithSilentIntervalTargets(const FixedLevelTest &p) override {
        fixedLevelTest_ = p;
        fixedLevelTestWithSilentIntervalTargetsInitialized_ = true;
    }

    void initializeWithAllTargets(const FixedLevelTest &p) override {
        fixedLevelTest_ = p;
        fixedLevelTestWithAllTargetsInitialized_ = true;
    }

    void completeTrial() { listener_->trialComplete(); }

    void setAudioDevices(std::vector<std::string> v) {
        audioDevices_ = std::move(v);
    }

    void setTestComplete() { testComplete_ = true; }

    auto testComplete() -> bool override { return testComplete_; }

    void playTrial(const AudioSettings &p) override {
        trialParameters_ = p;
        trialPlayed_ = true;
    }

    auto audioDevices() -> std::vector<std::string> override {
        return audioDevices_;
    }

    void submitResponse(
        const coordinate_response_measure::Response &p) override {
        responseParameters_ = p;
    }

    void subscribe(EventListener *listener) override { listener_ = listener; }

    void playCalibration(const Calibration &p) override { calibration_ = p; }

    void submitCorrectResponse() override { correctResponseSubmitted_ = true; }

    void submitIncorrectResponse() override {
        incorrectResponseSubmitted_ = true;
    }

    void submitResponse(const open_set::FreeResponse &s) override {
        freeResponse_ = s;
    }

  private:
    AdaptiveTest adaptiveTest_{};
    FixedLevelTest fixedLevelTest_{};
    Calibration calibration_{};
    AudioSettings trialParameters_{};
    coordinate_response_measure::Response responseParameters_{};
    std::vector<std::string> audioDevices_{};
    open_set::FreeResponse freeResponse_{};
    EventListener *listener_{};
    int trialNumber_{};
    bool testComplete_{};
    bool trialPlayed_{};
    bool defaultFixedLevelTestInitialized_{};
    bool defaultAdaptiveTestInitialized_{};
    bool fixedLevelTestWithSilentIntervalTargetsInitialized_{};
    bool fixedLevelTestWithAllTargetsInitialized_{};
    bool initializedWithSingleSpeaker_{};
    bool initializedWithDelayedMasker_{};
    bool correctResponseSubmitted_{};
    bool incorrectResponseSubmitted_{};
};

class ViewStub : public View {
  public:
    void setAudioDevice(std::string s) { audioDevice_ = std::move(s); }

    void showErrorMessage(std::string s) override {
        errorMessage_ = std::move(s);
    }

    auto errorMessage() { return errorMessage_; }

    void eventLoop() override { eventLoopCalled_ = true; }

    auto browseForDirectory() -> std::string override {
        return browseForDirectoryResult_;
    }

    auto browseCancelled() -> bool override { return browseCancelled_; }

    auto browseForOpeningFile() -> std::string override {
        return browseForOpeningFileResult_;
    }

    auto audioDevice() -> std::string override { return audioDevice_; }

    [[nodiscard]] auto eventLoopCalled() const { return eventLoopCalled_; }

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

    [[nodiscard]] auto audioDevices() const { return audioDevices_; }

    class TestSetupViewStub : public TestSetup {
      public:
        void useSingleSpeaker() { useSingleSpeaker_ = true; }

        void useDelayedMasker() { useDelayedMasker_ = true; }

        auto trackSettingsFile() -> std::string override {
            return trackSettingsFile_;
        }

        auto calibrationLevel_dB_SPL() -> std::string override {
            return calibrationLevel_;
        }

        auto maskerLevel_dB_SPL() -> std::string override {
            return maskerLevel_;
        }

        void confirmTestSetup() { listener_->confirmTestSetup(); }

        void playCalibration() { listener_->playCalibration(); }

        auto session() -> std::string override { return session_; }

        auto startingSnr_dB() -> std::string override { return startingSnr_; }

        void populateConditionMenu(std::vector<std::string> items) override {
            conditions_ = Collection{std::move(items)};
        }

        [[nodiscard]] auto conditions() const -> auto & { return conditions_; }

        [[nodiscard]] auto methods() const -> auto & { return methods_; }

        [[nodiscard]] auto shown() const { return shown_; }

        void show() override { shown_ = true; }

        void hide() override { hidden_ = true; }

        [[nodiscard]] auto hidden() const { return hidden_; }

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

        auto maskerFilePath() -> std::string override { return masker_; }

        auto targetListDirectory() -> std::string override {
            return stimulusList_;
        }

        auto testerId() -> std::string override { return testerId_; }

        auto subjectId() -> std::string override { return subjectId_; }

        auto condition() -> std::string override { return condition_; }

        auto calibrationFilePath() -> std::string override {
            return calibrationFilePath_;
        }

        void subscribe(EventListener *listener) override {
            listener_ = listener;
        }

        auto method() -> std::string override { return method_; }

        void populateMethodMenu(std::vector<std::string> items) override {
            methods_ = Collection{std::move(items)};
        }

        void browseForMasker() { listener_->browseForMasker(); }

        void browseForTargetList() { listener_->browseForTargetList(); }

        void browseForTrackSettingsFile() {
            listener_->browseForTrackSettingsFile();
        }

        void browseForCalibration() { listener_->browseForCalibration(); }

      private:
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
        bool useSingleSpeaker_{};
        bool useDelayedMasker_{};
    };

    class SubjectViewStub : public Subject {
      public:
        void show() override { shown_ = true; }

        [[nodiscard]] auto shown() const { return shown_; }

        [[nodiscard]] auto hidden() const { return hidden_; }

        auto whiteResponse() -> bool override { return grayResponse_; }

        void setGrayResponse() { grayResponse_ = true; }

        auto blueResponse() -> bool override { return blueResponse_; }

        void setBlueResponse() { blueResponse_ = true; }

        void setRedResponse() { redResponse_ = true; }

        void hideNextTrialButton() override { nextTrialButtonHidden_ = true; }

        [[nodiscard]] auto nextTrialButtonHidden() const {
            return nextTrialButtonHidden_;
        }

        void hideResponseButtons() override { responseButtonsHidden_ = true; }

        [[nodiscard]] auto responseButtonsHidden() const {
            return responseButtonsHidden_;
        }

        void showNextTrialButton() override { nextTrialButtonShown_ = true; }

        [[nodiscard]] auto nextTrialButtonShown() const {
            return nextTrialButtonShown_;
        }

        [[nodiscard]] auto responseButtonsShown() const {
            return responseButtonsShown_;
        }

        void setGreenResponse() { greenResponse_ = true; }

        void setNumberResponse(std::string s) {
            numberResponse_ = std::move(s);
        }

        auto numberResponse() -> std::string override {
            return numberResponse_;
        }

        auto greenResponse() -> bool override { return greenResponse_; }

        void showResponseButtons() override { responseButtonsShown_ = true; }

        void subscribe(EventListener *e) override { listener_ = e; }

        void hide() override { hidden_ = true; }

        void submitResponse() { listener_->submitResponse(); }

        void playTrial() { listener_->playTrial(); }

      private:
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
    };

    class TestingViewStub : public Testing {
      public:
        void submitFailedTrial() { listener_->submitFailedTrial(); }

        [[nodiscard]] auto responseSubmissionHidden() const {
            return responseSubmissionHidden_;
        }

        [[nodiscard]] auto evaluationButtonsHidden() const {
            return evaluationButtonsHidden_;
        }

        [[nodiscard]] auto nextTrialButtonShown() const {
            return nextTrialButtonShown_;
        }

        [[nodiscard]] auto evaluationButtonsShown() const {
            return evaluationButtonsShown_;
        }

        [[nodiscard]] auto responseSubmissionShown() const {
            return responseSubmissionShown_;
        }

        void showNextTrialButton() override { nextTrialButtonShown_ = true; }

        [[nodiscard]] auto shown() const { return shown_; }

        [[nodiscard]] auto hidden() const { return hidden_; }

        void show() override { shown_ = true; }

        void subscribe(EventListener *e) override { listener_ = e; }

        void hideNextTrialButton() override { nextTrialButtonHidden_ = true; }

        void hide() override { hidden_ = true; }

        void showEvaluationButtons() override {
            evaluationButtonsShown_ = true;
        }

        auto response() -> std::string override { return response_; }

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

        [[nodiscard]] auto nextTrialButtonHidden() const {
            return nextTrialButtonHidden_;
        }

        void submitPassedTrial() { listener_->submitPassedTrial(); }

        void setResponse(std::string s) { response_ = std::move(s); }

        void flagResponse() { flagged_ = true; }

        auto flagged() -> bool override { return flagged_; }

        void submitResponse() { listener_->submitResponse(); }

      private:
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
        bool flagged_{};
    };

    class ExperimenterViewStub : public Experimenter {
      public:
        void display(std::string s) override { displayed_ = std::move(s); }

        [[nodiscard]] auto displayed() const { return displayed_; }

        void showExitTestButton() override { exitTestButtonShown_ = true; }

        void hideExitTestButton() override { exitTestButtonHidden_ = true; }

        [[nodiscard]] auto exitTestButtonShown() const {
            return exitTestButtonShown_;
        }

        [[nodiscard]] auto exitTestButtonHidden() const {
            return exitTestButtonHidden_;
        }

        [[nodiscard]] auto shown() const { return shown_; }

        [[nodiscard]] auto hidden() const { return hidden_; }

        void show() override { shown_ = true; }

        void subscribe(EventListener *e) override { listener_ = e; }

        void hide() override { hidden_ = true; }

        void exitTest() { listener_->exitTest(); }

      private:
        std::string response_;
        std::string displayed_;
        EventListener *listener_{};
        bool shown_{};
        bool hidden_{};
        bool exitTestButtonHidden_{};
        bool exitTestButtonShown_{};
    };

  private:
    std::vector<std::string> audioDevices_;
    std::string errorMessage_;
    std::string browseForDirectoryResult_;
    std::string browseForOpeningFileResult_;
    std::string audioDevice_;
    bool eventLoopCalled_{};
    bool browseCancelled_{};
};

class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run() = 0;
};

static void run(UseCase &useCase) { useCase.run(); }

class ConditionUseCase : public virtual UseCase {
  public:
    virtual auto condition(ModelStub &) -> Condition = 0;
};

class LevelUseCase : public virtual UseCase {
  public:
    virtual auto fullScaleLevel(ModelStub &) -> int = 0;
};

class PlayingCalibration : public ConditionUseCase, public LevelUseCase {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit PlayingCalibration(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    auto condition(ModelStub &m) -> Condition override {
        return m.calibration().condition;
    }

    void run() override { view->playCalibration(); }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return m.calibration().fullScaleLevel_dB_SPL;
    }
};

class ConfirmingTestSetup : public virtual ConditionUseCase,
                            public virtual LevelUseCase {
  public:
    virtual auto snr_dB(ModelStub &) -> int = 0;
    virtual auto maskerLevel(ModelStub &) -> int = 0;
    virtual auto targetListDirectory(ModelStub &) -> std::string = 0;
    virtual auto subjectId(ModelStub &) -> std::string = 0;
    virtual auto testerId(ModelStub &) -> std::string = 0;
    virtual auto session(ModelStub &) -> std::string = 0;
    virtual auto method(ModelStub &) -> std::string = 0;
    virtual auto maskerFilePath(ModelStub &) -> std::string = 0;
};

static auto adaptiveTest(ModelStub &m) -> const AdaptiveTest & {
    return m.adaptiveTest();
}

static auto fixedLevelTest(ModelStub &m) -> const FixedLevelTest & {
    return m.fixedLevelTest();
}

static void confirmTestSetup(ViewStub::TestSetupViewStub *view) {
    view->confirmTestSetup();
}

class ConfirmingAdaptiveTest : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingAdaptiveTest(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override { confirmTestSetup(view); }

    static auto testIdentity(ModelStub &m) -> const TestIdentity & {
        return adaptiveTest(m).identity;
    }

    auto snr_dB(ModelStub &m) -> int override {
        return adaptiveTest(m).startingSnr_dB;
    }

    auto maskerLevel(ModelStub &m) -> int override {
        return adaptiveTest(m).maskerLevel_dB_SPL;
    }

    auto targetListDirectory(ModelStub &m) -> std::string override {
        return adaptiveTest(m).targetListDirectory;
    }

    auto subjectId(ModelStub &m) -> std::string override {
        return testIdentity(m).subjectId;
    }

    auto testerId(ModelStub &m) -> std::string override {
        return testIdentity(m).testerId;
    }

    auto session(ModelStub &m) -> std::string override {
        return testIdentity(m).session;
    }

    auto method(ModelStub &m) -> std::string override {
        return testIdentity(m).method;
    }

    auto maskerFilePath(ModelStub &m) -> std::string override {
        return adaptiveTest(m).maskerFilePath;
    }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return adaptiveTest(m).fullScaleLevel_dB_SPL;
    }

    auto condition(ModelStub &m) -> Condition override {
        return adaptiveTest(m).condition;
    }
};

namespace adaptive_test {
static auto testIdentity(ModelStub &m) -> const TestIdentity & {
    return adaptiveTest(m).identity;
}

static auto snr_dB(ModelStub &m) -> int {
    return adaptiveTest(m).startingSnr_dB;
}

static auto maskerLevel(ModelStub &m) -> int {
    return adaptiveTest(m).maskerLevel_dB_SPL;
}

static auto targetListDirectory(ModelStub &m) -> std::string {
    return adaptiveTest(m).targetListDirectory;
}

static auto subjectId(ModelStub &m) -> std::string {
    return testIdentity(m).subjectId;
}

static auto testerId(ModelStub &m) -> std::string {
    return testIdentity(m).testerId;
}

static auto session(ModelStub &m) -> std::string {
    return testIdentity(m).session;
}

static auto method(ModelStub &m) -> std::string {
    return testIdentity(m).method;
}

static auto maskerFilePath(ModelStub &m) -> std::string {
    return adaptiveTest(m).maskerFilePath;
}

static auto fullScaleLevel(ModelStub &m) -> int {
    return adaptiveTest(m).fullScaleLevel_dB_SPL;
}

static auto condition(ModelStub &m) -> Condition {
    return adaptiveTest(m).condition;
}
};

static void setMethod(ViewStub::TestSetupViewStub *view, Method m) {
    view->setMethod(methodName(m));
}

class ConfirmingDefaultAdaptiveClosedSetTest : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingDefaultAdaptiveClosedSetTest(
        ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override {
        setMethod(view, Method::defaultAdaptiveClosedSet);
        confirmTestSetup(view);
    }

    auto snr_dB(ModelStub &m) -> int override {
        return adaptive_test::snr_dB(m);
    }

    auto maskerLevel(ModelStub &m) -> int override {
        return adaptive_test::maskerLevel(m);
    }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return adaptive_test::fullScaleLevel(m);
    }

    auto targetListDirectory(ModelStub &m) -> std::string override {
        return adaptive_test::targetListDirectory(m);
    }

    auto subjectId(ModelStub &m) -> std::string override {
        return adaptive_test::subjectId(m);
    }

    auto testerId(ModelStub &m) -> std::string override {
        return adaptive_test::testerId(m);
    }

    auto session(ModelStub &m) -> std::string override {
        return adaptive_test::session(m);
    }

    auto method(ModelStub &m) -> std::string override {
        return adaptive_test::method(m);
    }

    auto maskerFilePath(ModelStub &m) -> std::string override {
        return adaptive_test::maskerFilePath(m);
    }

    auto condition(ModelStub &m) -> Condition override {
        return adaptive_test::condition(m);
    }
};

class ConfirmingAdaptiveClosedSetTestWithSingleSpeaker
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingAdaptiveClosedSetTestWithSingleSpeaker(
        ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override {
        setMethod(view, Method::adaptiveClosedSetWithSingleSpeaker);
        confirmTestSetup(view);
    }

    auto snr_dB(ModelStub &m) -> int override {
        return adaptive_test::snr_dB(m);
    }

    auto maskerLevel(ModelStub &m) -> int override {
        return adaptive_test::maskerLevel(m);
    }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return adaptive_test::fullScaleLevel(m);
    }

    auto targetListDirectory(ModelStub &m) -> std::string override {
        return adaptive_test::targetListDirectory(m);
    }

    auto subjectId(ModelStub &m) -> std::string override {
        return adaptive_test::subjectId(m);
    }

    auto testerId(ModelStub &m) -> std::string override {
        return adaptive_test::testerId(m);
    }

    auto session(ModelStub &m) -> std::string override {
        return adaptive_test::session(m);
    }

    auto method(ModelStub &m) -> std::string override {
        return adaptive_test::method(m);
    }

    auto maskerFilePath(ModelStub &m) -> std::string override {
        return adaptive_test::maskerFilePath(m);
    }

    auto condition(ModelStub &m) -> Condition override {
        return adaptive_test::condition(m);
    }
};

class ConfirmingAdaptiveClosedSetTestWithDelayedMasker
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingAdaptiveClosedSetTestWithDelayedMasker(
        ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override {
        setMethod(view, Method::adaptiveClosedSetWithDelayedMasker);
        confirmTestSetup(view);
    }

    auto snr_dB(ModelStub &m) -> int override {
        return adaptive_test::snr_dB(m);
    }

    auto maskerLevel(ModelStub &m) -> int override {
        return adaptive_test::maskerLevel(m);
    }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return adaptive_test::fullScaleLevel(m);
    }

    auto targetListDirectory(ModelStub &m) -> std::string override {
        return adaptive_test::targetListDirectory(m);
    }

    auto subjectId(ModelStub &m) -> std::string override {
        return adaptive_test::subjectId(m);
    }

    auto testerId(ModelStub &m) -> std::string override {
        return adaptive_test::testerId(m);
    }

    auto session(ModelStub &m) -> std::string override {
        return adaptive_test::session(m);
    }

    auto method(ModelStub &m) -> std::string override {
        return adaptive_test::method(m);
    }

    auto maskerFilePath(ModelStub &m) -> std::string override {
        return adaptive_test::maskerFilePath(m);
    }

    auto condition(ModelStub &m) -> Condition override {
        return adaptive_test::condition(m);
    }
};

class ConfirmingAdaptiveOpenSetTest : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingAdaptiveOpenSetTest(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override {
        setMethod(view, Method::adaptiveOpenSet);
        confirmTestSetup(view);
    }

    auto snr_dB(ModelStub &m) -> int override {
        return adaptive_test::snr_dB(m);
    }

    auto maskerLevel(ModelStub &m) -> int override {
        return adaptive_test::maskerLevel(m);
    }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return adaptive_test::fullScaleLevel(m);
    }

    auto targetListDirectory(ModelStub &m) -> std::string override {
        return adaptive_test::targetListDirectory(m);
    }

    auto subjectId(ModelStub &m) -> std::string override {
        return adaptive_test::subjectId(m);
    }

    auto testerId(ModelStub &m) -> std::string override {
        return adaptive_test::testerId(m);
    }

    auto session(ModelStub &m) -> std::string override {
        return adaptive_test::session(m);
    }

    auto method(ModelStub &m) -> std::string override {
        return adaptive_test::method(m);
    }

    auto maskerFilePath(ModelStub &m) -> std::string override {
        return adaptive_test::maskerFilePath(m);
    }

    auto condition(ModelStub &m) -> Condition override {
        return adaptive_test::condition(m);
    }
};

namespace fixed_level_test {
static auto identity(ModelStub &m) -> const TestIdentity & {
    return fixedLevelTest(m).identity;
}

static auto snr_dB(ModelStub &m) -> int { return fixedLevelTest(m).snr_dB; }

static auto maskerLevel(ModelStub &m) -> int {
    return fixedLevelTest(m).maskerLevel_dB_SPL;
}

static auto fullScaleLevel(ModelStub &m) -> int {
    return fixedLevelTest(m).fullScaleLevel_dB_SPL;
}

static auto targetListDirectory(ModelStub &m) -> std::string {
    return fixedLevelTest(m).targetListDirectory;
}

static auto subjectId(ModelStub &m) -> std::string {
    return identity(m).subjectId;
}

static auto testerId(ModelStub &m) -> std::string {
    return identity(m).testerId;
}

static auto session(ModelStub &m) -> std::string { return identity(m).session; }

static auto method(ModelStub &m) -> std::string { return identity(m).method; }

static auto maskerFilePath(ModelStub &m) -> std::string {
    return fixedLevelTest(m).maskerFilePath;
}

static auto condition(ModelStub &m) -> Condition {
    return fixedLevelTest(m).condition;
}
};

class ConfirmingDefaultFixedLevelOpenSetTest : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingDefaultFixedLevelOpenSetTest(
        ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override {
        setMethod(view, Method::defaultFixedLevelOpenSet);
        confirmTestSetup(view);
    }

    auto snr_dB(ModelStub &m) -> int override {
        return fixed_level_test::snr_dB(m);
    }

    auto maskerLevel(ModelStub &m) -> int override {
        return fixed_level_test::maskerLevel(m);
    }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return fixed_level_test::fullScaleLevel(m);
    }

    auto targetListDirectory(ModelStub &m) -> std::string override {
        return fixed_level_test::targetListDirectory(m);
    }

    auto subjectId(ModelStub &m) -> std::string override {
        return fixed_level_test::subjectId(m);
    }

    auto testerId(ModelStub &m) -> std::string override {
        return fixed_level_test::testerId(m);
    }

    auto session(ModelStub &m) -> std::string override {
        return fixed_level_test::session(m);
    }

    auto method(ModelStub &m) -> std::string override {
        return fixed_level_test::method(m);
    }

    auto maskerFilePath(ModelStub &m) -> std::string override {
        return fixed_level_test::maskerFilePath(m);
    }

    auto condition(ModelStub &m) -> Condition override {
        return fixed_level_test::condition(m);
    }
};

class ConfirmingDefaultFixedLevelClosedSetTest : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingDefaultFixedLevelClosedSetTest(
        ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override {
        setMethod(view, Method::defaultFixedLevelClosedSet);
        confirmTestSetup(view);
    }

    auto snr_dB(ModelStub &m) -> int override {
        return fixed_level_test::snr_dB(m);
    }

    auto maskerLevel(ModelStub &m) -> int override {
        return fixed_level_test::maskerLevel(m);
    }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return fixed_level_test::fullScaleLevel(m);
    }

    auto targetListDirectory(ModelStub &m) -> std::string override {
        return fixed_level_test::targetListDirectory(m);
    }

    auto subjectId(ModelStub &m) -> std::string override {
        return fixed_level_test::subjectId(m);
    }

    auto testerId(ModelStub &m) -> std::string override {
        return fixed_level_test::testerId(m);
    }

    auto session(ModelStub &m) -> std::string override {
        return fixed_level_test::session(m);
    }

    auto method(ModelStub &m) -> std::string override {
        return fixed_level_test::method(m);
    }

    auto maskerFilePath(ModelStub &m) -> std::string override {
        return fixed_level_test::maskerFilePath(m);
    }

    auto condition(ModelStub &m) -> Condition override {
        return fixed_level_test::condition(m);
    }
};

class ConfirmingFixedLevelClosedSetTestWithSilentIntervalTargets
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingFixedLevelClosedSetTestWithSilentIntervalTargets(
        ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override {
        setMethod(view, Method::fixedLevelClosedSetWithSilentIntervalTargets);
        confirmTestSetup(view);
    }

    auto snr_dB(ModelStub &m) -> int override {
        return fixed_level_test::snr_dB(m);
    }

    auto maskerLevel(ModelStub &m) -> int override {
        return fixed_level_test::maskerLevel(m);
    }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return fixed_level_test::fullScaleLevel(m);
    }

    auto targetListDirectory(ModelStub &m) -> std::string override {
        return fixed_level_test::targetListDirectory(m);
    }

    auto subjectId(ModelStub &m) -> std::string override {
        return fixed_level_test::subjectId(m);
    }

    auto testerId(ModelStub &m) -> std::string override {
        return fixed_level_test::testerId(m);
    }

    auto session(ModelStub &m) -> std::string override {
        return fixed_level_test::session(m);
    }

    auto method(ModelStub &m) -> std::string override {
        return fixed_level_test::method(m);
    }

    auto maskerFilePath(ModelStub &m) -> std::string override {
        return fixed_level_test::maskerFilePath(m);
    }

    auto condition(ModelStub &m) -> Condition override {
        return fixed_level_test::condition(m);
    }
};

class ConfirmingFixedLevelOpenSetTestWithSilentIntervalTargets
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingFixedLevelOpenSetTestWithSilentIntervalTargets(
        ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override {
        setMethod(view, Method::fixedLevelOpenSetWithSilentIntervalTargets);
        confirmTestSetup(view);
    }

    auto snr_dB(ModelStub &m) -> int override {
        return fixed_level_test::snr_dB(m);
    }

    auto maskerLevel(ModelStub &m) -> int override {
        return fixed_level_test::maskerLevel(m);
    }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return fixed_level_test::fullScaleLevel(m);
    }

    auto targetListDirectory(ModelStub &m) -> std::string override {
        return fixed_level_test::targetListDirectory(m);
    }

    auto subjectId(ModelStub &m) -> std::string override {
        return fixed_level_test::subjectId(m);
    }

    auto testerId(ModelStub &m) -> std::string override {
        return fixed_level_test::testerId(m);
    }

    auto session(ModelStub &m) -> std::string override {
        return fixed_level_test::session(m);
    }

    auto method(ModelStub &m) -> std::string override {
        return fixed_level_test::method(m);
    }

    auto maskerFilePath(ModelStub &m) -> std::string override {
        return fixed_level_test::maskerFilePath(m);
    }

    auto condition(ModelStub &m) -> Condition override {
        return fixed_level_test::condition(m);
    }
};

class ConfirmingFixedLevelOpenSetTestWithAllTargets
    : public ConfirmingTestSetup {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit ConfirmingFixedLevelOpenSetTestWithAllTargets(
        ViewStub::TestSetupViewStub *view)
        : view{view} {}

    void run() override {
        setMethod(view, Method::fixedLevelOpenSetWithAllTargets);
        confirmTestSetup(view);
    }

    auto snr_dB(ModelStub &m) -> int override {
        return fixed_level_test::snr_dB(m);
    }

    auto maskerLevel(ModelStub &m) -> int override {
        return fixed_level_test::maskerLevel(m);
    }

    auto fullScaleLevel(ModelStub &m) -> int override {
        return fixed_level_test::fullScaleLevel(m);
    }

    auto targetListDirectory(ModelStub &m) -> std::string override {
        return fixed_level_test::targetListDirectory(m);
    }

    auto subjectId(ModelStub &m) -> std::string override {
        return fixed_level_test::subjectId(m);
    }

    auto testerId(ModelStub &m) -> std::string override {
        return fixed_level_test::testerId(m);
    }

    auto session(ModelStub &m) -> std::string override {
        return fixed_level_test::session(m);
    }

    auto method(ModelStub &m) -> std::string override {
        return fixed_level_test::method(m);
    }

    auto maskerFilePath(ModelStub &m) -> std::string override {
        return fixed_level_test::maskerFilePath(m);
    }

    auto condition(ModelStub &m) -> Condition override {
        return fixed_level_test::condition(m);
    }
};

class TrialSubmission : public virtual UseCase {
  public:
    virtual auto nextTrialButtonShown() -> bool = 0;
    virtual auto responseViewShown() -> bool = 0;
    virtual auto responseViewHidden() -> bool = 0;
};

class RespondingFromSubject : public TrialSubmission {
    ViewStub::SubjectViewStub *view;

  public:
    explicit RespondingFromSubject(ViewStub::SubjectViewStub *view)
        : view{view} {}

    void run() override { view->submitResponse(); }

    auto nextTrialButtonShown() -> bool override {
        return view->nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view->responseButtonsShown();
    }

    auto responseViewHidden() -> bool override {
        return view->responseButtonsHidden();
    }
};

class RespondingFromExperimenter : public TrialSubmission {
    ViewStub::TestingViewStub *view;

  public:
    explicit RespondingFromExperimenter(ViewStub::TestingViewStub *view)
        : view{view} {}

    void run() override { view->submitResponse(); }

    auto nextTrialButtonShown() -> bool override {
        return view->nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view->responseSubmissionShown();
    }

    auto responseViewHidden() -> bool override {
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

    auto nextTrialButtonShown() -> bool override {
        return view->nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view->evaluationButtonsShown();
    }

    auto responseViewHidden() -> bool override {
        return view->evaluationButtonsHidden();
    }
};

class SubmittingFailedTrial : public TrialSubmission {
    ViewStub::TestingViewStub *view;

  public:
    explicit SubmittingFailedTrial(ViewStub::TestingViewStub *view)
        : view{view} {}

    void run() override { view->submitFailedTrial(); }

    auto nextTrialButtonShown() -> bool override {
        return view->nextTrialButtonShown();
    }

    auto responseViewShown() -> bool override {
        return view->evaluationButtonsShown();
    }

    auto responseViewHidden() -> bool override {
        return view->evaluationButtonsHidden();
    }
};

class PlayingTrial : public virtual UseCase {
  public:
    virtual auto nextTrialButtonHidden() -> bool = 0;
    virtual auto nextTrialButtonShown() -> bool = 0;
};

class PlayingTrialFromSubject : public PlayingTrial {
    ViewStub::SubjectViewStub *view;

  public:
    explicit PlayingTrialFromSubject(ViewStub::SubjectViewStub *view)
        : view{view} {}

    void run() override { view->playTrial(); }

    auto nextTrialButtonHidden() -> bool override {
        return view->nextTrialButtonHidden();
    }

    auto nextTrialButtonShown() -> bool override {
        return view->nextTrialButtonShown();
    }
};

class PlayingTrialFromExperimenter : public PlayingTrial {
    ViewStub::TestingViewStub *view;

  public:
    explicit PlayingTrialFromExperimenter(ViewStub::TestingViewStub *view)
        : view{view} {}

    void run() override { view->playTrial(); }

    auto nextTrialButtonHidden() -> bool override {
        return view->nextTrialButtonHidden();
    }

    auto nextTrialButtonShown() -> bool override {
        return view->nextTrialButtonShown();
    }
};

class BrowsingUseCase : public virtual UseCase {
  public:
    virtual void setResult(ViewStub &, std::string) = 0;
};

class BrowsingEnteredPathUseCase : public virtual BrowsingUseCase {
  public:
    virtual auto entry() -> std::string = 0;
    virtual void setEntry(std::string) = 0;
};

class BrowsingForMasker : public BrowsingEnteredPathUseCase {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit BrowsingForMasker(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    auto entry() -> std::string override { return view->maskerFilePath(); }

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

    auto entry() -> std::string override { return view->trackSettingsFile(); }

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

    auto entry() -> std::string override { return view->targetListDirectory(); }

    void setEntry(std::string s) override {
        view->setTargetListDirectory(std::move(s));
    }
};

class BrowsingForCalibration : public BrowsingEnteredPathUseCase {
    ViewStub::TestSetupViewStub *view;

  public:
    explicit BrowsingForCalibration(ViewStub::TestSetupViewStub *view)
        : view{view} {}

    auto entry() -> std::string override { return view->calibrationFilePath(); }

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

    auto construct() -> Presenter {
        return {model, view, testSetup, subject, experimenter, testing};
    }
};

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
    Presenter presenter{model, view, testSetup, subject, experimenter, testing};
    BrowsingForTrackSettingsFile browsingForTrackSettingsFile{&setupView};
    BrowsingForTargetList browsingForTargetList{&setupView};
    BrowsingForMasker browsingForMasker{&setupView};
    BrowsingForCalibration browsingForCalibration{&setupView};
    ConfirmingDefaultAdaptiveClosedSetTest confirmingAdaptiveClosedSetTest{
        &setupView};
    ConfirmingAdaptiveClosedSetTestWithSingleSpeaker
        confirmingAdaptiveClosedSetTestWithSingleSpeaker{&setupView};
    ConfirmingAdaptiveClosedSetTestWithDelayedMasker
        confirmingAdaptiveClosedSetTestWithDelayedMasker{&setupView};
    ConfirmingAdaptiveOpenSetTest confirmingAdaptiveOpenSetTest{&setupView};
    ConfirmingDefaultFixedLevelOpenSetTest confirmingFixedLevelOpenSetTest{
        &setupView};
    ConfirmingDefaultFixedLevelClosedSetTest confirmingFixedLevelClosedSetTest{
        &setupView};
    ConfirmingFixedLevelClosedSetTestWithSilentIntervalTargets
        confirmingFixedLevelClosedSetSilentIntervalsTest{&setupView};
    ConfirmingFixedLevelOpenSetTestWithSilentIntervalTargets
        confirmingFixedLevelOpenSetSilentIntervalsTest{&setupView};
    ConfirmingFixedLevelOpenSetTestWithAllTargets
        confirmingFixedLevelOpenSetTestWithAllTargets{&setupView};
    PlayingCalibration playingCalibration{&setupView};
    PlayingTrialFromSubject playingTrialFromSubject{&subjectView};
    PlayingTrialFromExperimenter playingTrialFromExperimenter{&testingView};
    RespondingFromSubject respondingFromSubject{&subjectView};
    RespondingFromExperimenter respondingFromExperimenter{&testingView};
    SubmittingPassedTrial submittingPassedTrial{&testingView};
    SubmittingFailedTrial submittingFailedTrial{&testingView};
    ExitingTest exitingTest{&experimenterView};

    static auto auditoryOnlyConditionName() -> std::string {
        return conditionName(Condition::auditoryOnly);
    }

    static auto audioVisualConditionName() -> std::string {
        return conditionName(Condition::audioVisual);
    }

    void respondFromSubject() { subjectView.submitResponse(); }

    void respondFromExperimenter() { testingView.submitResponse(); }

    void exitTest() { experimenterView.exitTest(); }

    void playCalibration() { setupView.playCalibration(); }

    void assertSetupViewShown() { assertTrue(setupViewShown()); }

    auto setupViewShown() -> bool { return setupView.shown(); }

    void assertSetupViewNotShown() { assertFalse(setupViewShown()); }

    void assertSetupViewHidden() { assertTrue(setupViewHidden()); }

    auto setupViewHidden() -> bool { return setupView.hidden(); }

    void assertSetupViewNotHidden() { assertFalse(setupViewHidden()); }

    void assertExperimenterViewShown() { assertTrue(experimenterViewShown()); }

    void assertTestingViewShown() { assertTrue(testingViewShown()); }

    auto experimenterViewShown() -> bool { return experimenterView.shown(); }

    auto testingViewShown() -> bool { return testingView.shown(); }

    void assertExperimenterViewHidden() {
        assertTrue(experimenterViewHidden());
    }

    void assertTestingViewHidden() { assertTrue(testingViewHidden()); }

    auto experimenterViewHidden() -> bool { return experimenterView.hidden(); }

    auto testingViewHidden() -> bool { return testingView.hidden(); }

    void assertExperimenterViewNotHidden() {
        assertFalse(experimenterViewHidden());
    }

    void assertTestingViewNotHidden() { assertFalse(testingViewHidden()); }

    void assertSubjectViewShown() { assertTrue(subjectViewShown()); }

    auto subjectViewShown() -> bool { return subjectView.shown(); }

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

    static void assertEntryEquals(
        BrowsingEnteredPathUseCase &useCase, const std::string &s) {
        assertEqual(s, entry(useCase));
    }

    static auto entry(BrowsingEnteredPathUseCase &useCase) -> std::string {
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

    void assertSetupViewConditionsContains(const std::string &s) {
        assertTrue(setupView.conditions().contains(s));
    }

    void assertSetupViewMethodsContains(const std::string &s) {
        assertTrue(setupView.methods().contains(s));
    }

    void assertSetupViewMethodsContains(Method m) {
        assertTrue(setupView.methods().contains(methodName(m)));
    }

    void setCondition(std::string s) { setupView.setCondition(std::move(s)); }

    auto errorMessage() -> std::string { return view.errorMessage(); }

    void assertModelPassedCondition(coordinate_response_measure::Color c) {
        assertEqual(c, model.responseParameters().color);
    }

    auto adaptiveTest() -> const AdaptiveTest & { return model.adaptiveTest(); }

    auto calibration() -> const Calibration & { return model.calibration(); }

    void assertInvalidCalibrationLevelShowsErrorMessage(UseCase &useCase) {
        setCalibrationLevel("a");
        run(useCase);
        assertErrorMessageEquals("'a' is not a valid calibration level.");
    }

    void assertErrorMessageEquals(const std::string &s) {
        assertEqual(s, errorMessage());
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

    auto modelCondition(ConditionUseCase &useCase) -> Condition {
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

    void assertPlaysTrial(UseCase &useCase) {
        run(useCase);
        assertTrue(trialPlayed());
    }

    auto trialPlayed() -> bool { return model.trialPlayed(); }

    static void assertHidesPlayTrialButton(PlayingTrial &useCase) {
        run(useCase);
        assertTrue(useCase.nextTrialButtonHidden());
    }

    void assertHidesExitTestButton(PlayingTrial &useCase) {
        run(useCase);
        assertTrue(exitTestButtonHidden());
    }

    auto exitTestButtonHidden() -> bool {
        return experimenterView.exitTestButtonHidden();
    }

    auto exitTestButtonShown() -> bool {
        return experimenterView.exitTestButtonShown();
    }

    static void assertConfirmTestSetupShowsNextTrialButton(
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

    static void assertShowsNextTrialButton(TrialSubmission &useCase) {
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
        assertFalse(model.defaultFixedLevelTestInitialized());
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

    void assertPassesMethod(ConfirmingTestSetup &useCase) {
        run(useCase);
        assertEqual(setupView.method(), useCase.method(model));
    }

    void assertPassesFullScaleLevel(LevelUseCase &useCase) {
        run(useCase);
        assertEqual(
            Presenter::fullScaleLevel_dB_SPL, useCase.fullScaleLevel(model));
    }

    void assertPassesCeilingSNR(UseCase &useCase) {
        run(useCase);
        assertEqual(Presenter::ceilingSnr_dB,
            tests::presentation::adaptiveTest(model).ceilingSnr_dB);
    }

    void assertPassesFloorSNR(UseCase &useCase) {
        run(useCase);
        assertEqual(Presenter::floorSnr_dB,
            tests::presentation::adaptiveTest(model).floorSnr_dB);
    }

    void assertPassesTrackBumpLimit(UseCase &useCase) {
        run(useCase);
        assertEqual(Presenter::trackBumpLimit,
            tests::presentation::adaptiveTest(model).trackBumpLimit);
    }

    void assertPassesTrackSettingsFile(UseCase &useCase) {
        setupView.setTrackSettingsFile("e");
        run(useCase);
        assertEqual(
            "e", tests::presentation::adaptiveTest(model).trackSettingsFile);
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

    static void assertResponseViewHidden(TrialSubmission &useCase) {
        run(useCase);
        assertTrue(useCase.responseViewHidden());
    }

    void assertShowsSubjectView(UseCase &useCase) {
        run(useCase);
        assertSubjectViewShown();
    }

    void assertDoesNotInitializeAdaptiveTest(UseCase &useCase) {
        run(useCase);
        assertFalse(model.defaultAdaptiveTestInitialized());
    }
};

class RequestFailingModel : public Model {
    std::string errorMessage{};

  public:
    auto trialNumber() -> int override { return 0; }

    void setErrorMessage(std::string s) { errorMessage = std::move(s); }

    void initialize(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initialize(const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithSilentIntervalTargets(const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithAllTargets(const FixedLevelTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithSingleSpeaker(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void initializeWithDelayedMasker(const AdaptiveTest &) override {
        throw RequestFailure{errorMessage};
    }

    void playTrial(const AudioSettings &) override {
        throw RequestFailure{errorMessage};
    }

    void submitResponse(
        const coordinate_response_measure::Response &) override {
        throw RequestFailure{errorMessage};
    }

    void submitResponse(const open_set::FreeResponse &) override {
        throw RequestFailure{errorMessage};
    }

    void playCalibration(const Calibration &) override {
        throw RequestFailure{errorMessage};
    }

    auto testComplete() -> bool override { return {}; }
    auto audioDevices() -> std::vector<std::string> override { return {}; }
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
            *model, view, testSetup, subject, experimenter, testing};
        setupView.confirmTestSetup();
    }

    void assertConfirmTestSetupShowsErrorMessage(const std::string &s) {
        confirmTestSetup();
        assertEqual(s, view.errorMessage());
    }

    void assertConfirmTestSetupDoesNotHideSetupView() {
        confirmTestSetup();
        assertFalse(setupView.hidden());
    }
};
}

#endif
