#ifndef presentation_Presenter_h
#define presentation_Presenter_h

#include <av-speech-in-noise/Model.h>
#include <vector>
#include <string>

namespace av_speech_in_noise {
    class View {
    public:
        virtual ~View() = default;
        
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
            virtual std::string numberResponse() = 0;
            virtual bool greenResponse() = 0;
            virtual bool blueResponse() = 0;
            virtual bool whiteResponse() = 0;
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
            };
            
            virtual ~TestSetup() = default;
            virtual void subscribe(EventListener *) = 0;
            virtual void show() = 0;
            virtual void hide() = 0;
            virtual std::string maskerLevel_dB_SPL() = 0;
            virtual std::string calibrationLevel_dB_SPL() = 0;
            virtual std::string startingSnr_dB() = 0;
            virtual std::string maskerFilePath() = 0;
            virtual std::string calibrationFilePath() = 0;
            virtual std::string targetListDirectory() = 0;
            virtual std::string testerId() = 0;
            virtual std::string subjectId() = 0;
            virtual std::string condition() = 0;
            virtual std::string session() = 0;
            virtual std::string method() = 0;
            virtual void setMasker(std::string) = 0;
            virtual void setTargetListDirectory(std::string) = 0;
            virtual void setCalibrationFilePath(std::string) = 0;
            virtual void populateConditionMenu(std::vector<std::string> items) = 0;
            virtual void populateMethodMenu(std::vector<std::string> items) = 0;
        };
        
        class Experimenter {
        public:
            class EventListener {
            public:
                virtual ~EventListener() = default;
                virtual void playTrial() = 0;
                virtual void submitPassedTrial() = 0;
                virtual void submitFailedTrial() = 0;
                virtual void submitResponse() = 0;
            };
            
            virtual ~Experimenter() = default;
            virtual void subscribe(EventListener *) = 0;
            virtual void showNextTrialButton() = 0;
            virtual void hideNextTrialButton() = 0;
            virtual void show() = 0;
            virtual void hide() = 0;
            virtual void showEvaluationButtons() = 0;
            virtual void hideEvaluationButtons() = 0;
            virtual void showResponseSubmission() = 0;
            virtual void hideResponseSubmission() = 0;
            virtual std::string response() = 0;
        };
        
        virtual void eventLoop() = 0;
        virtual std::string browseForDirectory() = 0;
        virtual std::string browseForOpeningFile() = 0;
        virtual std::string audioDevice() = 0;
        virtual void populateAudioDeviceMenu(std::vector<std::string>) = 0;
        virtual bool browseCancelled() = 0;
        virtual void showErrorMessage(std::string) = 0;
    };
    
    
    enum class Method {
        adaptiveOpenSet,
        adaptiveClosedSet,
        fixedLevelOpenSet,
        fixedLevelClosedSet
    };
    
    constexpr const char *methodName(Method c) {
        switch (c) {
        case Method::adaptiveOpenSet:
            return "adaptive open-set";
        case Method::adaptiveClosedSet:
            return "adaptive closed-set";
        case Method::fixedLevelOpenSet:
            return "fixed-level open-set";
        case Method::fixedLevelClosedSet:
            return "fixed-level closed-set";
        }
    }

    class Presenter : public Model::EventListener {
    public:
        class TestSetup : public View::TestSetup::EventListener {
        public:
            explicit TestSetup(View::TestSetup *);
            void show();
            void hide();
            void becomeChild(Presenter *parent);
            void setMasker(std::string);
            void setStimulusList(std::string);
            void setCalibrationFilePath(std::string);
            AdaptiveTest adaptiveTest();
            FixedLevelTest fixedLevelTest();
            Calibration calibrationParameters();
            bool adaptiveClosedSet();
            bool adaptiveOpenSet();
            bool fixedLevelOpenSet();
            bool fixedLevelClosedSet();
            void playCalibration() override;
            void browseForTargetList() override;
            void browseForMasker() override;
            void confirmTestSetup() override;
            void browseForCalibration() override;
        private:
            TestInformation testInformation();
            CommonTest commonTest();
            Condition readCondition();
            bool method(Method m);
            int readMaskerLevel();
            int readCalibrationLevel();
            int readInteger(std::string x, std::string identifier);
            bool auditoryOnly();
            
            View::TestSetup *view;
            Presenter *parent;
        };
        
        class Subject : public View::Subject::EventListener {
        public:
            explicit Subject(View::Subject *);
            void show();
            void hide();
            void becomeChild(Presenter *parent);
            void showResponseButtons();
            coordinate_response_measure::SubjectResponse subjectResponse();
            void playTrial() override;
            void submitResponse() override;
        private:
            void hideResponseButtons();
            void showNextTrialButton();
            coordinate_response_measure::Color colorResponse();
            
            View::Subject *view;
            Presenter *parent;
        };
        
        class Experimenter : public View::Experimenter::EventListener {
            View::Experimenter *view;
        public:
            explicit Experimenter(View::Experimenter *);
            void becomeChild(Presenter *parent);
            void show();
            void hide();
            void showEvaluationButtons();
            void showResponseSubmission();
            FreeResponse openSetResponse();
            void playTrial() override;
            void prepareNextEvaluatedTrial();
            
            void submitPassedTrial() override;
            void submitResponse() override;
            void submitFailedTrial() override;
            
        private:
            void showNextTrialButton();
            Presenter *parent;
        };
        
        class TrialCompletionHandler {
        public:
            virtual ~TrialCompletionHandler() = default;
            virtual void showResponseView() = 0;
        };
        
        class AdaptiveClosedSetTestTrialCompletionHandler :
            public TrialCompletionHandler
        {
            Subject *subject;
        public:
            explicit AdaptiveClosedSetTestTrialCompletionHandler(
                Subject *subject
            ) :
                subject{subject} {}
            
            void showResponseView() override {
                subject->showResponseButtons();
            }
        };
        
        class AdaptiveOpenSetTestTrialCompletionHandler :
            public TrialCompletionHandler
        {
            Experimenter *experimenter;
        public:
            explicit AdaptiveOpenSetTestTrialCompletionHandler(
                Experimenter *experimenter
            ) :
                experimenter{experimenter} {}
            
            void showResponseView() override {
                experimenter->showEvaluationButtons();
            }
        };
        
        class FixedLevelOpenSetTestTrialCompletionHandler :
            public TrialCompletionHandler
        {
            Experimenter *experimenter;
        public:
            explicit FixedLevelOpenSetTestTrialCompletionHandler(
                Experimenter *experimenter
            ) :
                experimenter{experimenter} {}
            
            void showResponseView() override {
                experimenter->showResponseSubmission();
            }
        };
        
        class FixedLevelClosedSetTestTrialCompletionHandler :
            public TrialCompletionHandler
        {
            Subject *subject;
        public:
            explicit FixedLevelClosedSetTestTrialCompletionHandler(
                Subject *subject
            ) :
                subject{subject} {}
            
            void showResponseView() override {
                subject->showResponseButtons();
            }
        };
        
        Presenter(
            Model *,
            View *,
            TestSetup *,
            Subject *,
            Experimenter *
        );
        void trialComplete() override;
        void run();
        void browseForTargetList();
        void playTrial();
        void submitSubjectResponse();
        void submitExperimenterResponse();
        void browseForMasker();
        void browseForCalibration();
        void confirmTestSetup();
        void playCalibration();
        void submitPassedTrial();
        void submitFailedTrial();
        static int fullScaleLevel_dB_SPL;
        static int ceilingSnr_dB;
        static int floorSnr_dB;
        static TrackingRule targetLevelRule;
        
    private:
        void proceedToNextTrialAfter(
            void(Presenter::*f)()
        );
        void submitFailedTrial_();
        void submitPassedTrial_();
        void submitExperimenterResponse_();
        void submitSubjectResponse_();
        void hideTestView();
        void switchToSetupView();
        void showErrorMessage(std::string);
        class BadInput : public std::runtime_error {
        public:
            explicit BadInput(std::string s) : std::runtime_error{ std::move(s) } {}
        };
        void playCalibration_();
        void showTestSetup();
        bool testComplete();
        void proceedToNextTrial();
        void hideTestSetup();
        bool adaptiveClosedSet();
        bool adaptiveOpenSet();
        bool adaptiveTest();
        bool closedSet();
        bool fixedLevelClosedSet();
        void initializeTest();
        void showTestView();
        void switchToTestView();
        void confirmTestSetup_();
        void applyIfBrowseNotCancelled(
            std::string s,
            void(TestSetup::*f)(std::string)
        );
        TrialCompletionHandler *getTrialCompletionHandler();
        
        FixedLevelOpenSetTestTrialCompletionHandler fixedLevelOpenSetTrialCompletionHandler;
        FixedLevelClosedSetTestTrialCompletionHandler fixedLevelClosedSetTrialCompletionHandler;
        AdaptiveOpenSetTestTrialCompletionHandler adaptiveOpenSetTrialCompletionHandler;
        AdaptiveClosedSetTestTrialCompletionHandler adaptiveClosedSetTrialCompletionHandler;
        Model *model;
        View *view;
        TestSetup *testSetup;
        Subject *subject;
        Experimenter *experimenter;
        TrialCompletionHandler *trialCompletionHandler{};
    };
}

#endif
