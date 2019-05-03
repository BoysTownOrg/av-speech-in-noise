#include "assert-utility.h"
#include <presentation/Presenter.h>
#include <gtest/gtest.h>

namespace {
    using namespace av_speech_in_noise;
    
    template<typename T>
    class Collection {
        std::vector<T> items{};
    public:
        Collection(std::vector<T> items = {}) : items{std::move(items)} {}
        
        bool contains(T item) const {
            return std::find(items.begin(), items.end(), item) != items.end();
        }
    };
    
    class ModelStub : public Model {
        AdaptiveTest adaptiveTest_;
        FixedLevelTest fixedLevelTest_;
        Calibration calibrationParameters_;
        AudioSettings trialParameters_;
        coordinate_response_measure::SubjectResponse responseParameters_;
        std::vector<std::string> audioDevices_;
        EventListener *listener_{};
        bool testComplete_{};
        bool trialPlayed_{};
        bool fixedLevelTestInitialized_{};
        bool adaptiveTestInitialized_{};
    public:
        void completeTrial() {
            listener_->trialComplete();
        }
        
        void setAudioDevices(std::vector<std::string> v) {
            audioDevices_ = std::move(v);
        }
        
        auto &responseParameters() const {
            return responseParameters_;
        }
        
        auto &trialParameters() const {
            return trialParameters_;
        }
        
        void setTestComplete() {
            testComplete_ = true;
        }
        
        bool testComplete() override {
            return testComplete_;
        }
        
        void playTrial(const AudioSettings &p) override {
            trialParameters_ = p;
            trialPlayed_ = true;
        }
        
        void initializeTest(const AdaptiveTest &p) override {
            adaptiveTestInitialized_ = true;
            adaptiveTest_ = p;
        }
        
        std::vector<std::string> audioDevices() override {
            return audioDevices_;
        }
        
        void submitResponse(
            const coordinate_response_measure::SubjectResponse &p
        ) override {
            responseParameters_ = p;
        }
        
        void subscribe(EventListener *listener) override {
            listener_ = listener;
        }
        
        void playCalibration(const Calibration &p) override {
            calibrationParameters_ = p;
        }
        
        void submitCorrectResponse() override {

        }
        
        void submitIncorrectResponse() override {
            
        }
        
        void initializeTest(const FixedLevelTest &p) override {
            fixedLevelTestInitialized_ = true;
            fixedLevelTest_ = p;
        }
        
        auto trialPlayed() const {
            return trialPlayed_;
        }
        
        auto &adaptiveTest() const {
            return adaptiveTest_;
        }
        
        auto &fixedLevelTest() const {
            return fixedLevelTest_;
        }
        
        auto &calibrationParameters() const {
            return calibrationParameters_;
        }
        
        auto fixedLevelTestInitialized() const {
            return fixedLevelTestInitialized_;
        }
        
        auto adaptiveTestInitialized() const {
            return adaptiveTestInitialized_;
        }
    };

    class ViewStub : public View {
        std::vector<std::string> audioDevices_{};
        std::string errorMessage_{};
        std::string browseForDirectoryResult_{};
        std::string browseForOpeningFileResult_{};
        std::string audioDevice_{};
        bool eventLoopCalled_{};
        bool confirmationDialogShown_{};
        bool browseCancelled_{};
    public:
        void setAudioDevice(std::string s) {
            audioDevice_ = std::move(s);
        }
        
        void showErrorMessage(std::string s) override {
            errorMessage_ = std::move(s);
        }
        
        auto errorMessage() {
            return errorMessage_;
        }
        
        void eventLoop() override {
            eventLoopCalled_ = true;
        }
        
        std::string browseForDirectory() override {
            return browseForDirectoryResult_;
        }
        
        bool browseCancelled() override {
            return browseCancelled_;
        }
        
        std::string browseForOpeningFile() override {
            return browseForOpeningFileResult_;
        }
        
        std::string audioDevice() override {
            return audioDevice_;
        }
        
        auto eventLoopCalled() const {
            return eventLoopCalled_;
        }
        
        void setBrowseForDirectoryResult(std::string s) {
            browseForDirectoryResult_ = std::move(s);
        }
        
        void setBrowseForOpeningFileResult(std::string s) {
            browseForOpeningFileResult_ = std::move(s);
        }
        
        void setBrowseCancelled() {
            browseCancelled_ = true;
        }
        
        void populateAudioDeviceMenu(std::vector<std::string> v) override {
            audioDevices_ = std::move(v);
        }
        
        auto audioDevices() const {
            return audioDevices_;
        }

        class TestSetupViewStub : public TestSetup {
            Collection<std::string> conditions_{};
            std::string signalLevel_{"0"};
            std::string calibrationLevel_{"0"};
            std::string startingSnr_{"0"};
            std::string maskerLevel_{"0"};
            std::string masker_{};
            std::string condition_{};
            std::string stimulusList_{};
            std::string subjectId_{};
            std::string testerId_{};
            std::string session_{};
            std::string calibrationFilePath_{};
            std::string method_{};
            EventListener *listener_{};
            bool shown_{};
            bool hidden_{};
        public:
            std::string calibrationLevel_dB_SPL() override {
                return calibrationLevel_;
            }
            
            std::string maskerLevel_dB_SPL() override {
                return maskerLevel_;
            }
            
            void confirmTestSetup() {
                listener_->confirmTestSetup();
            }
            
            void playCalibration() {
                listener_->playCalibration();
            }
            
            std::string session() override {
                return session_;
            }
            
            std::string startingSnr_dB() override {
                return startingSnr_;
            }
            
            void populateConditionMenu(std::vector<std::string> items) override {
                conditions_ = std::move(items);
            }
            
            auto &conditions() const {
                return conditions_;
            }
            
            auto shown() const {
                return shown_;
            }
            
            void show() override {
                shown_ = true;
            }
            
            void hide() override {
                hidden_ = true;
            }
            
            auto hidden() const {
                return hidden_;
            }
            
            void setStartingSnr(std::string s) {
                startingSnr_ = std::move(s);
            }
            
            void setMethod(std::string s) {
                method_ = std::move(s);
            }
            
            void setCalibrationLevel(std::string s) {
                calibrationLevel_ = std::move(s);
            }
            
            void setMaskerLevel(std::string s) {
                maskerLevel_ = std::move(s);
            }
            
            void setCalibrationFilePath(std::string s) override {
                calibrationFilePath_ = std::move(s);
            }
            
            void setCondition(std::string s) {
                condition_ = std::move(s);
            }

            void setMasker(std::string s) override {
                masker_ = std::move(s);
            }
            
            void setSession(std::string s) {
                session_ = std::move(s);
            }
            
            void setTargetListDirectory(std::string s) override {
                stimulusList_ = std::move(s);
            }
            
            void setSubjectId(std::string s) {
                subjectId_ = std::move(s);
            }
            
            void setTesterId(std::string s) {
                testerId_ = std::move(s);
            }

            std::string maskerFilePath() override {
                return masker_;
            }

            std::string targetListDirectory() override {
                return stimulusList_;
            }

            std::string testerId() override {
                return testerId_;
            }

            std::string subjectId() override {
                return subjectId_;
            }

            std::string condition() override {
                return condition_;
            }
            
            std::string calibrationFilePath() override {
                return calibrationFilePath_;
            }
            
            void subscribe(EventListener *listener) override {
                listener_ = listener;
            }
            
            std::string method() override {
                return method_;
            }
            
            void browseForMasker() {
                listener_->browseForMasker();
            }
            
            void browseForTargetList() {
                listener_->browseForTargetList();
            }
            
            void browseForCalibration() {
                listener_->browseForCalibration();
            }
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
        public:
            void show() override {
                shown_ = true;
            }
            
            auto shown() const {
                return shown_;
            }
            
            bool whiteResponse() override {
                return grayResponse_;
            }
            
            void setGrayResponse() {
                grayResponse_ = true;
            }
            
            bool blueResponse() override {
                return blueResponse_;
            }
            
            void setBlueResponse() {
                blueResponse_ = true;
            }
            
            void setRedResponse() {
                redResponse_ = true;
            }
            
            void hideNextTrialButton() override {
                nextTrialButtonHidden_ = true;
            }
            
            auto nextTrialButtonHidden() const {
                return nextTrialButtonHidden_;
            }
            
            void hideResponseButtons() override {
                responseButtonsHidden_ = true;
            }
            
            auto responseButtonsHidden() const {
                return responseButtonsHidden_;
            }
            
            void showNextTrialButton() override {
                nextTrialButtonShown_ = true;
            }
            
            auto nextTrialButtonShown() const {
                return nextTrialButtonShown_;
            }
            
            auto responseButtonsShown() const {
                return responseButtonsShown_;
            }
            
            void setGreenResponse() {
                greenResponse_ = true;
            }
            
            void setNumberResponse(std::string s) {
                numberResponse_ = s;
            }
            
            std::string numberResponse() override {
                return numberResponse_;
            }
            
            bool greenResponse() override {
                return greenResponse_;
            }
            
            void showResponseButtons() override {
                responseButtonsShown_ = true;
            }
            
            void subscribe(EventListener *e) override {
                listener_ = e;
            }
        
            void submitResponse() {
                listener_->submitResponse();
            }
            
            void playTrial() {
                listener_->playTrial();
            }
        };
        
        class ExperimenterViewStub : public Experimenter {
            EventListener *listener_{};
            bool nextTrialButtonShown_{};
            bool shown_{};
            bool nextTrialButtonHidden_{};
            bool hidden_{};
        public:
            auto nextTrialButtonShown() const {
                return nextTrialButtonShown_;
            }
            
            void showNextTrialButton() override {
                nextTrialButtonShown_ = true;
            }
            
            auto shown() const {
                return shown_;
            }
            
            auto hidden() const {
                return hidden_;
            }
            
            void show() override {
                shown_ = true;
            }
            
            void subscribe(EventListener *e) override {
                listener_ = e;
            }
            
            void hideNextTrialButton() override {
                nextTrialButtonHidden_ = true;
            }
            
            void hide() override {
                hidden_ = true;
            }
            
            void playTrial() {
                listener_->playTrial();
            }
            
            auto nextTrialButtonHidden() const {
                return nextTrialButtonHidden_;
            }
            
            void submitPassedTrial() {
                listener_->submitPassedTrial();
            }
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
    
    class ConfirmingTestSetup : public virtual ConditionUseCase {
    public:
        virtual int snr_dB(ModelStub &) = 0;
        virtual int maskerLevel(ModelStub &) = 0;
    };
    
    class ConfirmingAdaptiveTest : public ConfirmingTestSetup {
        ViewStub::TestSetupViewStub *view;
    public:
        explicit ConfirmingAdaptiveTest(ViewStub::TestSetupViewStub *view) :
            view{view} {}
        
        void run() override {
            view->confirmTestSetup();
        }
        
        int snr_dB(ModelStub &m) override {
            return m.adaptiveTest().startingSnr_dB;
        }
        
        int maskerLevel(ModelStub &m) override {
            return m.adaptiveTest().maskerLevel_dB_SPL;
        }
        
        Condition condition(ModelStub &m) override {
            return m.adaptiveTest().condition;
        }
    };
    
    class ConfirmingAdaptiveClosedSetTest : public ConfirmingTestSetup {
        ConfirmingAdaptiveTest confirmingAdaptiveTest;
        ViewStub::TestSetupViewStub *view;
    public:
        explicit ConfirmingAdaptiveClosedSetTest(ViewStub::TestSetupViewStub *view) :
            confirmingAdaptiveTest{view},
            view{view} {}
        
        void run() override {
            view->setMethod(methodName(Method::adaptiveClosedSet));
            confirmingAdaptiveTest.run();
        }
        
        int snr_dB(ModelStub &m) override {
            return confirmingAdaptiveTest.snr_dB(m);
        }
        
        int maskerLevel(ModelStub &m) override {
            return confirmingAdaptiveTest.maskerLevel(m);
        }
        
        Condition condition(ModelStub &m) override {
            return confirmingAdaptiveTest.condition(m);
        }
    };
    
    class ConfirmingAdaptiveOpenSetTest : public ConfirmingTestSetup {
        ConfirmingAdaptiveTest confirmingAdaptiveTest;
        ViewStub::TestSetupViewStub *view;
    public:
        explicit ConfirmingAdaptiveOpenSetTest(ViewStub::TestSetupViewStub *view) :
            confirmingAdaptiveTest{view},
            view{view} {}
        
        void run() override {
            view->setMethod(methodName(Method::adaptiveOpenSet));
            confirmingAdaptiveTest.run();
        }
        
        int snr_dB(ModelStub &m) override {
            return confirmingAdaptiveTest.snr_dB(m);
        }
        
        int maskerLevel(ModelStub &m) override {
            return confirmingAdaptiveTest.maskerLevel(m);
        }
        
        Condition condition(ModelStub &m) override {
            return confirmingAdaptiveTest.condition(m);
        }
    };
    
    class ConfirmingFixedLevelOpenSetTest : public ConfirmingTestSetup {
        ViewStub::TestSetupViewStub *view;
    public:
        explicit ConfirmingFixedLevelOpenSetTest(ViewStub::TestSetupViewStub *view) :
            view{view} {}
        
        void run() override {
            view->setMethod(methodName(Method::adaptiveOpenSet));
            view->confirmTestSetup();
        }
        
        int snr_dB(ModelStub &m) override {
            return m.fixedLevelTest().snr_dB;
        }
        
        int maskerLevel(ModelStub &m) override {
            return m.fixedLevelTest().maskerLevel_dB_SPL;
        }
        
        Condition condition(ModelStub &m) override {
            return m.fixedLevelTest().condition;
        }
    };
    
    class TrialSubmission : public virtual UseCase {};
    
    class RespondingFromSubject : public TrialSubmission {
        ViewStub::SubjectViewStub *view;
    public:
        explicit RespondingFromSubject(ViewStub::SubjectViewStub *view) :
            view{view} {}
        
        void run() override {
            view->submitResponse();
        }
    };
    
    class SubmittingPassedTrial : public TrialSubmission {
        ViewStub::ExperimenterViewStub *view;
    public:
        explicit SubmittingPassedTrial(ViewStub::ExperimenterViewStub *view) :
            view{view} {}
        
        void run() override {
            view->submitPassedTrial();
        }
    };
    
    class PlayingTrial : public virtual UseCase {
    public:
        virtual void setMethod(ViewStub::TestSetupViewStub &) = 0;
        virtual bool nextTrialButtonHidden() = 0;
        virtual bool nextTrialButtonShown() = 0;
    };
    
    class PlayingTrialFromSubject : public PlayingTrial {
        ViewStub::SubjectViewStub *view;
    public:
        explicit PlayingTrialFromSubject(ViewStub::SubjectViewStub *view) :
            view{view} {}
        
        void run() override {
            view->playTrial();
        }
        
        bool nextTrialButtonHidden() override {
            return view->nextTrialButtonHidden();
        }
        
        void setMethod(ViewStub::TestSetupViewStub &v) override {
            v.setMethod(methodName(Method::adaptiveClosedSet));
        }
        
        bool nextTrialButtonShown() override {
            return view->nextTrialButtonShown();
        }
    };
    
    class PlayingTrialFromExperimenter : public PlayingTrial {
        ViewStub::ExperimenterViewStub *view;
    public:
        explicit PlayingTrialFromExperimenter(ViewStub::ExperimenterViewStub *view) :
            view{view} {}
        
        void run() override {
            view->playTrial();
        }
        
        bool nextTrialButtonHidden() override {
            return view->nextTrialButtonHidden();
        }
        
        void setMethod(ViewStub::TestSetupViewStub &v) override {
            v.setMethod(methodName(Method::adaptiveOpenSet));
        }
        
        bool nextTrialButtonShown() override {
            return view->nextTrialButtonShown();
        }
    };
    
    class TestSetupUseCase {
    public:
        virtual ~TestSetupUseCase() = default;
        virtual void run(ViewStub::TestSetupViewStub &) = 0;
    };

    class BrowsingUseCase : public TestSetupUseCase {
    public:
        virtual void setResult(ViewStub &, std::string) = 0;
    };

    class BrowsingEnteredPathUseCase : public BrowsingUseCase {
    public:
        virtual std::string entry(ViewStub::TestSetupViewStub &) = 0;
        virtual void setEntry(ViewStub::TestSetupViewStub &, std::string) = 0;
    };
    
    class BrowsingForMasker : public BrowsingEnteredPathUseCase {
    public:
        std::string entry(ViewStub::TestSetupViewStub &view) override {
            return view.maskerFilePath();
        }
        
        void setEntry(ViewStub::TestSetupViewStub &view, std::string s) override {
            view.setMasker(s);
        }
        
        void setResult(ViewStub &view, std::string s) override {
            return view.setBrowseForOpeningFileResult(s);
        }
        
        void run(
            ViewStub::TestSetupViewStub &listener
        ) override {
            listener.browseForMasker();
        }
    };

    class BrowsingForTargetList : public BrowsingEnteredPathUseCase {
    public:
        void run(
            ViewStub::TestSetupViewStub &listener
        ) override {
            listener.browseForTargetList();
        }

        void setResult(ViewStub &view, std::string s) override {
            view.setBrowseForDirectoryResult(s);
        }
        
        std::string entry(ViewStub::TestSetupViewStub &view) override {
            return view.targetListDirectory();
        }
        
        void setEntry(ViewStub::TestSetupViewStub &view, std::string s) override {
            view.setTargetListDirectory(s);
        }
    };
    
    class BrowsingForCalibration : public BrowsingEnteredPathUseCase {
    public:
        std::string entry(ViewStub::TestSetupViewStub &view) override {
            return view.calibrationFilePath();
        }
        
        void setEntry(ViewStub::TestSetupViewStub &view, std::string s) override {
            view.setCalibrationFilePath(std::move(s));
        }
        
        void setResult(ViewStub &view, std::string s) override {
            return view.setBrowseForOpeningFileResult(s);
        }
        
        void run(
            ViewStub::TestSetupViewStub &listener
        ) override {
            listener.browseForCalibration();
        }
    };
    
    class PlayingCalibration : public ConditionUseCase {
        ViewStub::TestSetupViewStub *view;
    public:
        explicit PlayingCalibration(ViewStub::TestSetupViewStub *view) :
            view{view} {}
        
        Condition condition(ModelStub &m) override {
            return m.calibrationParameters().condition;
        }
        
        void run() override {
            view->playCalibration();
        }
    };

    class PresenterConstructionTests : public ::testing::Test {
    protected:
        ModelStub model{};
        ViewStub::TestSetupViewStub setupView{};
        ViewStub::SubjectViewStub subjectView{};
        ViewStub::ExperimenterViewStub experimenterView{};
        ViewStub view{};
        Presenter::TestSetup testSetup{&setupView};
        Presenter::Subject subject{&subjectView};
        Presenter::Experimenter experimenter{&experimenterView};
        
        Presenter construct() {
            return {&model, &view, &testSetup, &subject, &experimenter};
        }
    };
    
    TEST_F(PresenterConstructionTests, populatesAudioDeviceMenu) {
        model.setAudioDevices({"a", "b", "c"});
        auto presenter = construct();
        assertEqual({"a", "b", "c"}, view.audioDevices());
    }

    class PresenterTests : public ::testing::Test {
    protected:
        ModelStub model;
        ViewStub view;
        ViewStub::TestSetupViewStub setupView;
        ViewStub::SubjectViewStub subjectView;
        ViewStub::ExperimenterViewStub experimenterView;
        Presenter::TestSetup testSetup{&setupView};
        Presenter::Experimenter experimenter{&experimenterView};
        Presenter::Subject subject{&subjectView};
        Presenter presenter{
            &model,
            &view,
            &testSetup,
            &subject,
            &experimenter
        };
        BrowsingForTargetList browsingForTargetList;
        BrowsingForMasker browsingForMasker;
        BrowsingForCalibration browsingForCalibration;
        ConfirmingAdaptiveClosedSetTest confirmingAdaptiveClosedSetTest{&setupView};
        ConfirmingAdaptiveOpenSetTest confirmingAdaptiveOpenSetTest{&setupView};
        ConfirmingFixedLevelOpenSetTest confirmingFixedLevelOpenSetTest{&setupView};
        PlayingCalibration playingCalibration{&setupView};
        PlayingTrialFromSubject playingTrialFromSubject{&subjectView};
        PlayingTrialFromExperimenter playingTrialFromExperimenter{&experimenterView};
        RespondingFromSubject respondingFromSubject{&subjectView};
        SubmittingPassedTrial submittingPassedTrial{&experimenterView};
        
        std::string auditoryOnlyConditionName() {
            return conditionName(Condition::auditoryOnly);
        }
        
        std::string audioVisualConditionName() {
            return conditionName(Condition::audioVisual);
        }
        
        void respondFromSubject() {
            subjectView.submitResponse();
        }
        
        void submitPassedTrial() {
            experimenterView.submitPassedTrial();
        }
        
        void confirmTestSetup() {
            setupView.confirmTestSetup();
        }
        
        void playCalibration() {
            setupView.playCalibration();
        }
        
        void confirmTestSetupWithInvalidInput() {
            setupView.setStartingSnr("?");
            confirmTestSetup();
        }
        
        void assertSetupViewShown() {
            EXPECT_TRUE(setupViewShown());
        }
        
        bool setupViewShown() {
            return setupView.shown();
        }
        
        void assertSetupViewNotShown() {
            EXPECT_FALSE(setupViewShown());
        }
        
        void assertSetupViewHidden() {
            EXPECT_TRUE(setupViewHidden());
        }
        
        bool setupViewHidden() {
            return setupView.hidden();
        }
        
        void assertSetupViewNotHidden() {
            EXPECT_FALSE(setupViewHidden());
        }
        
        void assertExperimenterViewShown() {
            EXPECT_TRUE(experimenterView.shown());
        }
        
        void assertExperimenterViewHidden() {
            EXPECT_TRUE(experimenterViewHidden());
        }
        
        bool experimenterViewHidden() {
            return experimenterView.hidden();
        }
        
        void assertExperimenterViewNotHidden() {
            EXPECT_FALSE(experimenterViewHidden());
        }
        
        void assertSubjectViewShown() {
            EXPECT_TRUE(subjectViewShown());
        }
        
        bool subjectViewShown() {
            return subjectView.shown();
        }
        
        void assertSubjectViewNotShown() {
            EXPECT_FALSE(subjectViewShown());
        }

        void assertBrowseResultPassedToEntry(
            BrowsingEnteredPathUseCase &useCase
        ) {
            setBrowsingResult(useCase, "a");
            run(useCase);
            assertEntryEquals(useCase, "a");
        }
        
        void setBrowsingResult(
            BrowsingEnteredPathUseCase &useCase,
            std::string s
        ) {
            useCase.setResult(view, std::move(s));
        }

        void run(TestSetupUseCase &useCase) {
            useCase.run(setupView);
        }
        
        void assertEntryEquals(
            BrowsingEnteredPathUseCase &useCase,
            std::string s
        ) {
            assertEqual(std::move(s), entry(useCase));
        }
        
        std::string entry(BrowsingEnteredPathUseCase &useCase) {
            return useCase.entry(setupView);
        }

        void assertCancellingBrowseDoesNotChangePath(
            BrowsingEnteredPathUseCase &useCase
        ) {
            useCase.setEntry(setupView, "a");
            setBrowsingResult(useCase, "b");
            view.setBrowseCancelled();
            run(useCase);
            assertEntryEquals(useCase, "a");
        }
        
        void completeTrial() {
            model.completeTrial();
        }
        
        void assertResponseButtonsShown() {
            EXPECT_TRUE(subjectView.responseButtonsShown());
        }
        
        void assertNextTrialButtonShownForSubject() {
            EXPECT_TRUE(nextTrialButtonShownForSubject());
        }
        
        bool nextTrialButtonShownForSubject() {
            return subjectView.nextTrialButtonShown();
        }
        
        void assertNextTrialButtonNotShown() {
            EXPECT_FALSE(nextTrialButtonShownForSubject());
        }
        
        void assertResponseButtonsHidden() {
            EXPECT_TRUE(subjectView.responseButtonsHidden());
        }
        
        void assertSetupViewConditionsContains(std::string s) {
            EXPECT_TRUE(setupView.conditions().contains(std::move(s)));
        }
        
        void setCondition(std::string s) {
            setupView.setCondition(std::move(s));
        }
        
        std::string errorMessage() {
            return view.errorMessage();
        }
        
        void assertModelPassedCondition(coordinate_response_measure::Color c) {
            EXPECT_EQ(c, model.responseParameters().color);
        }
        
        const AdaptiveTest &adaptiveTest() {
            return model.adaptiveTest();
        }
        
        const Calibration &modelCalibrationParameters() {
            return model.calibrationParameters();
        }
        
        void assertInvalidCalibrationLevelShowsErrorMessage(
            UseCase &useCase
        ) {
            setCalibrationLevel("a");
            run(useCase);
            assertErrorMessageEquals("'a' is not a valid calibration level.");
        }
        
        void assertErrorMessageEquals(std::string s) {
            assertEqual(std::move(s), errorMessage());
        }
        
        void assertInvalidMaskerLevelShowsErrorMessage(
            UseCase &useCase
        ) {
            setMaskerLevel("a");
            run(useCase);
            assertErrorMessageEquals("'a' is not a valid masker level.");
        }
        
        void setAudioDevice(std::string s) {
            view.setAudioDevice(std::move(s));
        }
        
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
            EXPECT_EQ(c, modelCondition(useCase));
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
        
        void setTestComplete() {
            model.setTestComplete();
        }
        
        void setAdaptiveOpenSet() {
            setMethod(Method::adaptiveOpenSet);
        }
        
        void setMethod(Method m) {
            setupView.setMethod(methodName(m));
        }
        
        void setAdaptiveClosedSet() {
            setMethod(Method::adaptiveClosedSet);
        }
        
        void assertAudioDevicePassedToTrial(PlayingTrial &useCase) {
            setAudioDevice("a");
            run(useCase);
            assertEqual("a", model.trialParameters().audioDevice);
        }
        
        void run(UseCase &useCase) {
            useCase.run();
        }
        
        void assertPlaysTrial(PlayingTrial &useCase) {
            run(useCase);
            EXPECT_TRUE(model.trialPlayed());
        }
        
        void assertHidesPlayTrialButton(PlayingTrial &useCase) {
            run(useCase);
            EXPECT_TRUE(useCase.nextTrialButtonHidden());
        }
        
        void assertConfirmTestSetupShowsNextTrialButton(PlayingTrial &useCase) {
            useCase.setMethod(setupView);
            confirmTestSetup();
            EXPECT_TRUE(useCase.nextTrialButtonShown());
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
            run(useCase);
            assertExperimenterViewHidden();
        }
        
        void assertStartingSnrPassedToModel(ConfirmingTestSetup &useCase) {
            setStartingSnr("1");
            run(useCase);
            EXPECT_EQ(1, useCase.snr_dB(model));
        }
        
        void assertMaskerLevelPassedToModel(ConfirmingTestSetup &useCase) {
            setMaskerLevel("2");
            run(useCase);
            EXPECT_EQ(2, useCase.maskerLevel(model));
        }
    };

    TEST_F(PresenterTests, populatesConditionMenu) {
        assertSetupViewConditionsContains(auditoryOnlyConditionName());
        assertSetupViewConditionsContains(audioVisualConditionName());
    }

    TEST_F(PresenterTests, callsEventLoopWhenRun) {
        presenter.run();
        EXPECT_TRUE(view.eventLoopCalled());
    }

    TEST_F(PresenterTests, confirmTestSetupHidesTestSetupView) {
        confirmTestSetup();
        assertSetupViewHidden();
    }

    TEST_F(PresenterTests, confirmTestSetupShowsExperimenterView) {
        confirmTestSetup();
        assertExperimenterViewShown();
    }

    TEST_F(
        PresenterTests,
        confirmTestSetupShowsSubjectViewWhenAdaptiveClosedSet
    ) {
        setAdaptiveClosedSet();
        confirmTestSetup();
        assertSubjectViewShown();
    }

    TEST_F(
        PresenterTests,
        confirmTestSetupDoesNotShowSubjectViewWhenAdaptiveOpenSet
    ) {
        setAdaptiveOpenSet();
        confirmTestSetup();
        assertSubjectViewNotShown();
    }

    TEST_F(PresenterTests, confirmAdaptiveTestSetupDoesNotInitializeFixedLevelTest) {
        run(confirmingAdaptiveClosedSetTest);
        EXPECT_FALSE(model.fixedLevelTestInitialized());
    }

    TEST_F(PresenterTests, confirmFixedLevelTestSetupDoesNotInitializeAdaptiveTest) {
        run(confirmingFixedLevelOpenSetTest);
        EXPECT_FALSE(model.adaptiveTestInitialized());
    }

    TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesStartingSnr) {
        assertStartingSnrPassedToModel(confirmingAdaptiveClosedSetTest);
    }

    TEST_F(PresenterTests, confirmFixedLevelOpenSetTestSetupPassesStartingSnr) {
        assertStartingSnrPassedToModel(confirmingFixedLevelOpenSetTest);
    }

    TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesMaskerLevel) {
        assertMaskerLevelPassedToModel(confirmingAdaptiveClosedSetTest);
    }

    TEST_F(PresenterTests, confirmFixedLevelOpenSetTestSetupPassesMaskerLevel) {
        assertMaskerLevelPassedToModel(confirmingFixedLevelOpenSetTest);
    }

    TEST_F(PresenterTests, playCalibrationPassesLevel) {
        setCalibrationLevel("1");
        playCalibration();
        EXPECT_EQ(1, modelCalibrationParameters().level_dB_SPL);
    }

    TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesTargetList) {
        setupView.setTargetListDirectory("a");
        run(confirmingAdaptiveClosedSetTest);
        assertEqual("a", adaptiveTest().targetListDirectory);
    }

    TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesSubjectId) {
        setupView.setSubjectId("b");
        run(confirmingAdaptiveClosedSetTest);
        assertEqual("b", adaptiveTest().subjectId);
    }

    TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesTesterId) {
        setupView.setTesterId("c");
        run(confirmingAdaptiveClosedSetTest);
        assertEqual("c", adaptiveTest().testerId);
    }

    TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesMasker) {
        setupView.setMasker("d");
        run(confirmingAdaptiveClosedSetTest);
        assertEqual("d", adaptiveTest().maskerFilePath);
    }

    TEST_F(PresenterTests, playCalibrationPassesFilePath) {
        setupView.setCalibrationFilePath("a");
        playCalibration();
        assertEqual("a", modelCalibrationParameters().filePath);
    }

    TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesSession) {
        setupView.setSession("e");
        run(confirmingAdaptiveClosedSetTest);
        assertEqual("e", adaptiveTest().session);
    }

    TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesFullScaleLevel) {
        run(confirmingAdaptiveClosedSetTest);
        EXPECT_EQ(
            Presenter::fullScaleLevel_dB_SPL,
            adaptiveTest().fullScaleLevel_dB_SPL
        );
    }

    TEST_F(PresenterTests, playCalibrationPassesFullScaleLevel) {
        playCalibration();
        EXPECT_EQ(
            Presenter::fullScaleLevel_dB_SPL,
            modelCalibrationParameters().fullScaleLevel_dB_SPL
        );
    }

    TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesTargetLevelRule) {
        run(confirmingAdaptiveClosedSetTest);
        EXPECT_EQ(
            &Presenter::targetLevelRule,
            adaptiveTest().targetLevelRule
        );
    }

    TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesAudioVisualCondition) {
        assertAudioVisualConditionPassedToModel(confirmingAdaptiveClosedSetTest);
    }

    TEST_F(PresenterTests, confirmingAdaptiveClosedSetTestPassesAuditoryOnlyCondition) {
        assertAuditoryOnlyConditionPassedToModel(confirmingAdaptiveClosedSetTest);
    }

    TEST_F(PresenterTests, playCalibrationPassesAudioVisualCondition) {
        assertAudioVisualConditionPassedToModel(playingCalibration);
    }

    TEST_F(PresenterTests, playCalibrationPassesAuditoryOnlyCondition) {
        assertAuditoryOnlyConditionPassedToModel(playingCalibration);
    }

    TEST_F(
        PresenterTests,
        confirmTestSetupShowsNextTrialButtonForSubjectWhenAdaptiveClosedSet
    ) {
        assertConfirmTestSetupShowsNextTrialButton(playingTrialFromSubject);
    }

    TEST_F(
        PresenterTests,
        confirmTestSetupShowsNextTrialButtonForExperimenterWhenAdaptiveOpenSet
    ) {
        assertConfirmTestSetupShowsNextTrialButton(playingTrialFromExperimenter);
    }

    TEST_F(
        PresenterTests,
        confirmingAdaptiveClosedSetTestWithInvalidMaskerLevelShowsErrorMessage
    ) {
        assertInvalidMaskerLevelShowsErrorMessage(confirmingAdaptiveClosedSetTest);
    }

    TEST_F(PresenterTests, playCalibrationWithInvalidLevelShowsErrorMessage) {
        assertInvalidCalibrationLevelShowsErrorMessage(playingCalibration);
    }

    TEST_F(PresenterTests, playingTrialFromSubjectPlaysTrial) {
        assertPlaysTrial(playingTrialFromSubject);
    }

    TEST_F(PresenterTests, playingTrialFromExperimenterPlaysTrial) {
        assertPlaysTrial(playingTrialFromExperimenter);
    }

    TEST_F(PresenterTests, playingTrialHidesNextTrialButton) {
        assertHidesPlayTrialButton(playingTrialFromSubject);
    }

    TEST_F(PresenterTests, playingTrialHidesNextTrialButtonForExperimenter) {
        assertHidesPlayTrialButton(playingTrialFromExperimenter);
    }

    TEST_F(PresenterTests, playingTrialFromSubjectPassesAudioDevice) {
        assertAudioDevicePassedToTrial(playingTrialFromSubject);
    }

    TEST_F(PresenterTests, playingTrialFromExperimenterPassesAudioDevice) {
        assertAudioDevicePassedToTrial(playingTrialFromExperimenter);
    }

    TEST_F(PresenterTests, playCalibrationPassesAudioDevice) {
        setAudioDevice("b");
        playCalibration();
        assertEqual("b", modelCalibrationParameters().audioDevice);
    }

    TEST_F(PresenterTests, subjectResponsePassesNumberResponse) {
        subjectView.setNumberResponse("1");
        respondFromSubject();
        EXPECT_EQ(1, model.responseParameters().number);
    }

    TEST_F(PresenterTests, subjectResponsePassesGreenColor) {
        subjectView.setGreenResponse();
        respondFromSubject();
        assertModelPassedCondition(coordinate_response_measure::Color::green);
    }

    TEST_F(PresenterTests, subjectResponsePassesRedColor) {
        subjectView.setRedResponse();
        respondFromSubject();
        assertModelPassedCondition(coordinate_response_measure::Color::red);
    }

    TEST_F(PresenterTests, subjectResponsePassesBlueColor) {
        subjectView.setBlueResponse();
        respondFromSubject();
        assertModelPassedCondition(coordinate_response_measure::Color::blue);
    }

    TEST_F(PresenterTests, subjectResponsePassesWhiteColor) {
        subjectView.setGrayResponse();
        respondFromSubject();
        assertModelPassedCondition(coordinate_response_measure::Color::white);
    }

    TEST_F(PresenterTests, respondFromSubjectShowsSetupViewWhenTestComplete) {
        assertCompleteTestShowsSetupView(respondingFromSubject);
    }

    TEST_F(PresenterTests, submitPassedTrialShowsSetupViewWhenTestComplete) {
        assertCompleteTestShowsSetupView(submittingPassedTrial);
    }

    TEST_F(PresenterTests, respondFromSubjectDoesNotShowSetupViewWhenTestIncomplete) {
        assertIncompleteTestDoesNotShowSetupView(respondingFromSubject);
    }

    TEST_F(PresenterTests, submitPassedTrialDoesNotShowSetupViewWhenTestIncomplete) {
        assertIncompleteTestDoesNotShowSetupView(submittingPassedTrial);
    }

    TEST_F(PresenterTests, respondFromSubjectHidesExperimenterViewWhenTestComplete) {
        assertCompleteTestHidesExperimenterView(respondingFromSubject);
    }

    TEST_F(PresenterTests, submitPassedTrialHidesExperimenterViewWhenTestComplete) {
        assertCompleteTestHidesExperimenterView(submittingPassedTrial);
    }

    TEST_F(PresenterTests, respondFromSubjectDoesNotHideExperimenterViewWhenTestIncomplete) {
        respondFromSubject();
        assertExperimenterViewNotHidden();
    }

    TEST_F(PresenterTests, submitPassedTrialDoesNotHideExperimenterViewWhenTestIncomplete) {
        submitPassedTrial();
        assertExperimenterViewNotHidden();
    }

    TEST_F(PresenterTests, subjectResponseShowsNextTrialButton) {
        respondFromSubject();
        assertNextTrialButtonShownForSubject();
    }

    TEST_F(PresenterTests, subjectResponseDoesNotShowNextTrialButtonWhenTestComplete) {
        setTestComplete();
        respondFromSubject();
        assertNextTrialButtonNotShown();
    }

    TEST_F(PresenterTests, subjectResponseHidesResponseButtons) {
        respondFromSubject();
        assertResponseButtonsHidden();
    }

    TEST_F(PresenterTests, browseForTargetListUpdatesTargetList) {
        assertBrowseResultPassedToEntry(browsingForTargetList);
    }

    TEST_F(PresenterTests, browseForMaskerUpdatesMasker) {
        assertBrowseResultPassedToEntry(browsingForMasker);
    }

    TEST_F(PresenterTests, browseForCalibrationUpdatesCalibrationFilePaths) {
        assertBrowseResultPassedToEntry(browsingForCalibration);
    }

    TEST_F(PresenterTests, browseForTargetListCancelDoesNotChangeTargetList) {
        assertCancellingBrowseDoesNotChangePath(browsingForTargetList);
    }

    TEST_F(PresenterTests, browseForMaskerCancelDoesNotChangeMasker) {
        assertCancellingBrowseDoesNotChangePath(browsingForMasker);
    }

    TEST_F(PresenterTests, browseForCalibrationCancelDoesNotChangeCalibrationFilePath) {
        assertCancellingBrowseDoesNotChangePath(browsingForCalibration);
    }

    TEST_F(PresenterTests, trialCompleteShowsResponseButtons) {
        completeTrial();
        assertResponseButtonsShown();
    }

    TEST_F(PresenterTests, confirmTestSetupWithInvalidSnrShowsErrorMessage) {
        setStartingSnr("a");
        confirmTestSetup();
        assertErrorMessageEquals("'a' is not a valid SNR.");
    }

    TEST_F(PresenterTests, confirmTestSetupWithInvalidInputDoesNotHideSetupView) {
        confirmTestSetupWithInvalidInput();
        assertSetupViewNotHidden();
    }

    class RequestFailingModel : public Model {
        std::string errorMessage{};
    public:
        void setErrorMessage(std::string s) {
            errorMessage = std::move(s);
        }
        
        void initializeTest(const AdaptiveTest &) override {
            throw RequestFailure{errorMessage};
        }
        
        void initializeTest(const FixedLevelTest &) override {
            throw RequestFailure{errorMessage};
        }
        
        void playTrial(const AudioSettings &) override {
            throw RequestFailure{errorMessage};
        }
        
        void submitResponse(const coordinate_response_measure::SubjectResponse &) override {
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
        Presenter::TestSetup testSetup{&setupView};
        Presenter::Subject subject{&subjectView};
        Presenter::Experimenter experimenter{&experimenterView};
        
        void useFailingModel(std::string s = {}) {
            failure.setErrorMessage(std::move(s));
            model = &failure;
        }
        
        void confirmTestSetup() {
            Presenter presenter{
                model,
                &view,
                &testSetup,
                &subject,
                &experimenter
            };
            setupView.confirmTestSetup();
        }
        
        void assertConfirmTestSetupShowsErrorMessage(std::string s) {
            confirmTestSetup();
            assertEqual(std::move(s), view.errorMessage());
        }
        
        void assertConfirmTestSetupDoesNotHideSetupView() {
            confirmTestSetup();
            ASSERT_FALSE(setupView.hidden());
        }
    };

    TEST_F(
        PresenterFailureTests,
        initializeTestShowsErrorMessageWhenModelFailsRequest
    ) {
        useFailingModel("a");
        assertConfirmTestSetupShowsErrorMessage("a");
    }

    TEST_F(
        PresenterFailureTests,
        initializeTestDoesNotHideSetupViewWhenModelFailsRequest
    ) {
        useFailingModel();
        assertConfirmTestSetupDoesNotHideSetupView();
    }
}
