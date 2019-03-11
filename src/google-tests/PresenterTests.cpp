#include "assert-utility.h"
#include <presentation/Presenter.h>
#include <gtest/gtest.h>

namespace {
    class ModelStub : public presentation::Model {
        TestParameters testParameters_{};
        bool testComplete_{};
        bool trialPlayed_{};
    public:
        void setTestIncomplete() {
            testComplete_ = false;
        }
        
        bool testComplete() override {
            return testComplete_;
        }
        
        void playTrial() override {
            trialPlayed_ = true;
        }
        
        void initializeTest(Model::TestParameters p) override {
            testParameters_ = std::move(p);
        }
        
        auto trialPlayed() const {
            return trialPlayed_;
        }
        
        auto &testParameters() const {
            return testParameters_;
        }
    };

    class ViewStub : public presentation::View {
        std::string errorMessage_{};
        DialogResponse dialogResponse_{};
        EventListener *listener_{};
        TestSetup *setupView_;
        Tester *testerView_;
        bool eventLoopCalled_{};
        bool confirmationDialogShown_{};
    public:
        ViewStub(
            TestSetup *setupView,
            Tester *testerView
        ) :
            setupView_{setupView},
            testerView_{testerView} {}
        
        void showErrorMessage(std::string s) override {
            errorMessage_ = std::move(s);
        }
        
        auto errorMessage() {
            return errorMessage_;
        }
        
        auto listener() const {
            return listener_;
        }
        
        void subscribe(EventListener *listener) override {
            listener_ = listener;
        }
        
        void eventLoop() override {
            eventLoopCalled_ = true;
        }
        
        TestSetup *testSetup() override {
            return setupView_;
        }
        
        Tester *tester() override {
            return testerView_;
        }
        
        DialogResponse showConfirmationDialog() override {
            confirmationDialogShown_ = true;
            return dialogResponse_;
        }
        
        auto eventLoopCalled() const {
            return eventLoopCalled_;
        }
        
        void newTest() {
            listener_->newTest();
        }
        
        void openTest() {
            listener_->openTest();
        }
        
        void close() {
            listener_->closeTest();
        }
        
        auto confirmationDialogShown() const {
            return confirmationDialogShown_;
        }
        
        void setDialogResponse(DialogResponse response) {
            dialogResponse_ = response;
        }

        class TestSetupViewStub : public TestSetup {
            std::string signalLevel_{"0"};
            std::string maskerLevel_{"0"};
            std::string condition_{};
            std::string masker_{};
            std::string stimulusList_{};
            std::string subjectId_{};
            std::string testerId_{};
            EventListener *listener_{};
            bool shown_{};
            bool hidden_{};
        public:
            auto shown() const {
                return shown_;
            }
            
            void show() override {
                shown_ = true;
            }
            
            void subscribe(EventListener *listener) override {
                listener_ = listener;
            }
            
            void hide() override {
                hidden_ = true;
            }
            
            std::string maskerLevel_dB_SPL() override {
                return maskerLevel_;
            }
            
            std::string signalLevel_dB_SPL() override {
                return signalLevel_;
            }
            
            void confirm() {
                listener_->confirmTestSetup();
            }
            
            bool hidden() const {
                return hidden_;
            }
            
            void setSignalLevel(std::string s) {
                signalLevel_ = std::move(s);
            }
            
            void setMaskerLevel(std::string s) {
                maskerLevel_ = std::move(s);
            }
            
            void setCondition(std::string s) {
                condition_ = std::move(s);
            }

            void setMasker(std::string s) {
                masker_ = std::move(s);
            }
            
            void setStimulusList(std::string s) {
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

            std::string stimulusListDirectory() override {
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
        };
        
        class TesterViewStub : public Tester {
            EventListener *listener_{};
            bool shown_{};
            bool hidden_{};
        public:
            auto shown() const {
                return shown_;
            }
            
            void show() override {
                shown_ = true;
            }
            
            void subscribe(EventListener *listener) override {
                listener_ = listener;
            }
            
            void hide() override {
                hidden_ = true;
            }
            
            void playTrial() {
                listener_->playTrial();
            }
            
            auto hidden() const {
                return hidden_;
            }
        };
    };

    class PresenterTests : public ::testing::Test {
    protected:
        ModelStub model{};
        ViewStub::TestSetupViewStub setupView{};
        ViewStub::TesterViewStub testerView{};
        ViewStub view{&setupView, &testerView};
        presentation::Presenter presenter{&model, &view};
        
        void confirmTestSetupWithInvalidInput() {
            setupView.setMaskerLevel("?");
            setupView.confirm();
        }
    };

    TEST_F(PresenterTests, subscribesToViewEvents) {
        EXPECT_EQ(&presenter, view.listener());
    }

    TEST_F(PresenterTests, callsEventLoopWhenRun) {
        presenter.run();
        EXPECT_TRUE(view.eventLoopCalled());
    }

    TEST_F(PresenterTests, newTestShowsTestSetupView) {
        view.newTest();
        EXPECT_TRUE(setupView.shown());
    }

    TEST_F(PresenterTests, confirmTestSetupHidesTestSetupView) {
        setupView.confirm();
        EXPECT_TRUE(setupView.hidden());
    }

    TEST_F(PresenterTests, confirmTestSetupPassesParametersToModel) {
        setupView.setMaskerLevel("1");
        setupView.setSignalLevel("2");
        setupView.setStimulusList("a");
        setupView.setSubjectId("b");
        setupView.setTesterId("c");
        setupView.setMasker("d");
        setupView.confirm();
        EXPECT_EQ(1, model.testParameters().maskerLevel_dB_SPL);
        EXPECT_EQ(2, model.testParameters().signalLevel_dB_SPL);
        assertEqual("a", model.testParameters().stimulusListDirectory);
        assertEqual("b", model.testParameters().subjectId);
        assertEqual("c", model.testParameters().testerId);
        assertEqual("d", model.testParameters().maskerFilePath);
    }

    TEST_F(PresenterTests, confirmTestSetupPassesAudioVisualCondition) {
        setupView.setCondition("Audio-visual");
        setupView.confirm();
        EXPECT_EQ(
            presentation::Model::TestParameters::Condition::audioVisual,
            model.testParameters().condition
        );
    }

    TEST_F(PresenterTests, confirmTestSetupPassesAuditoryOnlyCondition) {
        setupView.setCondition("Auditory-only");
        setupView.confirm();
        EXPECT_EQ(
            presentation::Model::TestParameters::Condition::auditoryOnly,
            model.testParameters().condition
        );
    }

    TEST_F(PresenterTests, confirmTestSetupWithInvalidLevelShowsErrorMessage) {
        setupView.setMaskerLevel("a");
        setupView.confirm();
        assertEqual("'a' is not a valid masker level.", view.errorMessage());
    }

    TEST_F(PresenterTests, confirmTestSetupWithInvalidInputDoesNotHideSetupView) {
        confirmTestSetupWithInvalidInput();
        EXPECT_FALSE(setupView.hidden());
    }

    TEST_F(PresenterTests, confirmTestSetupWithInvalidInputDoesNotShowTesterView) {
        confirmTestSetupWithInvalidInput();
        EXPECT_FALSE(testerView.shown());
    }

    TEST_F(PresenterTests, confirmTestSetupShowsTesterView) {
        setupView.confirm();
        EXPECT_TRUE(testerView.shown());
    }

    TEST_F(PresenterTests, openTestShowsTesterView) {
        view.openTest();
        EXPECT_TRUE(testerView.shown());
    }

    TEST_F(PresenterTests, playingTrialDoesNotHideViewWhileTestInProgress) {
        model.setTestIncomplete();
        testerView.playTrial();
        EXPECT_FALSE(testerView.hidden());
    }

    TEST_F(PresenterTests, playingTrialPlaysTrial) {
        testerView.playTrial();
        EXPECT_TRUE(model.trialPlayed());
    }

    TEST_F(PresenterTests, closingTestPromptsTesterToSave) {
        view.close();
        EXPECT_TRUE(view.confirmationDialogShown());
    }

    TEST_F(PresenterTests, closingTestHidesTesterViewIfUserDeclinesSaving) {
        view.setDialogResponse(presentation::View::DialogResponse::decline);
        view.close();
        EXPECT_TRUE(testerView.hidden());
    }

    TEST_F(PresenterTests, closingTestHidesTesterViewIfUserAcceptsSaving) {
        view.setDialogResponse(presentation::View::DialogResponse::accept);
        view.close();
        EXPECT_TRUE(testerView.hidden());
    }

    TEST_F(PresenterTests, closingTestDoesNotHideTesterViewIfUserCancels) {
        view.setDialogResponse(presentation::View::DialogResponse::cancel);
        view.close();
        EXPECT_FALSE(testerView.hidden());
    }

    class RequestFailingModel : public presentation::Model {
        std::string errorMessage{};
    public:
        void setErrorMessage(std::string s) {
            errorMessage = std::move(s);
        }
        
        void initializeTest(presentation::Model::TestParameters) override {
            throw RequestFailure{errorMessage};
        }
        
        void playTrial() override {
            throw RequestFailure{errorMessage};
        }
        
        bool testComplete() override { return {}; }
    };

    class PresenterFailureTests : public ::testing::Test {
    protected:
        RequestFailingModel failure{};
        ModelStub defaultModel{};
        presentation::Model *model{&defaultModel};
        ViewStub::TestSetupViewStub setupView{};
        ViewStub::TesterViewStub testerView{};
        ViewStub view{&setupView, &testerView};
        
        void useFailingModel(std::string s = {}) {
            failure.setErrorMessage(std::move(s));
            model = &failure;
        }
        
        void confirmTestSetup() {
            presentation::Presenter presenter{model, &view};
            setupView.confirm();
        }
        
        void assertConfirmTestSetupShowsErrorMessage(std::string s) {
            confirmTestSetup();
            assertEqual(std::move(s), view.errorMessage());
        }
        
        void assertConfirmTestSetupDoesNotHideSetupView() {
            confirmTestSetup();
            ASSERT_FALSE(setupView.hidden());
        }
        
        void assertConfirmTestSetupDoesNotShowTesterView() {
            confirmTestSetup();
            ASSERT_FALSE(testerView.shown());
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

    TEST_F(
        PresenterFailureTests,
        initializeTestDoesNotShowTesterViewWhenModelFailsRequest
    ) {
        useFailingModel();
        assertConfirmTestSetupDoesNotShowTesterView();
    }
}
