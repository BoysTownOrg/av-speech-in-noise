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
        };
        
        class Experimenter {
        public:
            class EventListener {
            public:
                virtual ~EventListener() = default;
                virtual void playTrial() = 0;
                virtual void submitPassedTrial() = 0;
            };
            
            virtual ~Experimenter() = default;
            virtual void subscribe(EventListener *) = 0;
            virtual void showNextTrialButton() = 0;
            virtual void hideNextTrialButton() = 0;
            virtual void show() = 0;
            virtual void hide() = 0;
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
        fixedLevelOpenSet
    };
    
    constexpr const char *methodName(Method c) {
        switch (c) {
        case Method::adaptiveOpenSet:
            return "adaptive open-set";
        case Method::adaptiveClosedSet:
            return "adaptive closed-set";
        case Method::fixedLevelOpenSet:
            return "fixed-level open-set";
        }
    }

    class Presenter : public Model::EventListener {
    public:
        class TestSetup : public View::TestSetup::EventListener {
        public:
            explicit TestSetup(View::TestSetup *);
            void playCalibration() override;
            void browseForTargetList() override;
            void browseForMasker() override;
            void confirmTestSetup() override;
            void browseForCalibration() override;
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
        private:
            TestInformation testInformation();
            Condition readCondition();
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
            void playTrial() override;
            void submitResponse() override;
            void becomeChild(Presenter *parent);
            void hideResponseButtons();
            void hideNextTrialButton();
            void showResponseButtons();
            void show();
            void hide();
            coordinate_response_measure::SubjectResponse subjectResponse();
        private:
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
            void showNextTrialButton();
            void show();
            void hide();
            void playTrial() override;
            void submitPassedTrial() override;
        private:
            Presenter *parent;
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
        void submitResponse();
        void browseForMasker();
        void browseForCalibration();
        void confirmTestSetup();
        void playCalibration();
        void submitPassedTrial();
        static int fullScaleLevel_dB_SPL;
        static TrackingRule targetLevelRule;
        
    private:
        void switchToSetupView();
        void showErrorMessage(std::string);
        RUNTIME_ERROR(BadInput)
        void playCalibration_();
        void showTestSetup();
        bool testComplete();
        void proceedToNextTrial();
        void hideTestSetup();
        void hideResponseButtons();
        void showNextTrialButton();
        void showResponseButtons();
        bool adaptiveTest();
        void initializeTest();
        void switchToTestView();
        void confirmTestSetup_();
        void applyIfBrowseNotCancelled(
            std::string s,
            void(TestSetup::*f)(std::string)
        );
        
        Model *model;
        View *view;
        TestSetup *testSetup;
        Subject *subject;
        Experimenter *experimenter;
    };
}

#endif
