#ifndef presentation_Presenter_h
#define presentation_Presenter_h

#include <av-coordinate-response-measure/Model.h>
#include <vector>
#include <string>

namespace av_coordinate_response_measure {
    class View {
    public:
        virtual ~View() = default;

        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void newTest() = 0;
            virtual void openTest() = 0;
            virtual void closeTest() = 0;
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
            virtual std::string numberResponse() = 0;
            virtual bool greenResponse() = 0;
            virtual bool blueResponse() = 0;
            virtual bool whiteResponse() = 0;
            virtual void showResponseButtons() = 0;
            virtual void hideResponseButtons() = 0;
            virtual void showNextTrialButton() = 0;
            virtual void hideNextTrialButton() = 0;
        };

        class Tester {
        public:
            virtual ~Tester() = default;
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
            virtual void setMasker(std::string) = 0;
            virtual void setTargetListDirectory(std::string) = 0;
            virtual void setCalibrationFilePath(std::string) = 0;
            virtual void populateConditionMenu(std::vector<std::string> items) = 0;
        };
        
        virtual void subscribe(EventListener *) = 0;
        virtual void eventLoop() = 0;
        virtual std::string browseForDirectory() = 0;
        virtual std::string browseForOpeningFile() = 0;
        virtual std::string audioDevice() = 0;
        virtual void populateAudioDeviceMenu(std::vector<std::string>) = 0;
        virtual bool browseCancelled() = 0;
        virtual void showErrorMessage(std::string) = 0;
        enum class DialogResponse {
            decline,
            accept,
            cancel
        };
        virtual DialogResponse showConfirmationDialog() = 0;
    };

    class Presenter :
        public View::EventListener,
        public Model::EventListener
    {
    public:
        class TestSetup : public View::TestSetup::EventListener {
        public:
            TestSetup(View::TestSetup *);
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
            Test testParameters();
            Calibration calibrationParameters();
        private:
            int readMaskerLevel();
            int readCalibrationLevel();
            int readInteger(std::string x, std::string identifier);
            bool auditoryOnly();
            
            View::TestSetup *view;
            Presenter *parent;
        };
        
        class Tester {
        public:
            Tester(View::Tester *);
            void show();
            void hide();
        private:
            View::Tester *view;
        };
        
        class Subject : public View::Subject::EventListener {
        public:
            Subject(View::Subject *);
            void playTrial() override;
            void submitResponse() override;
            void becomeChild(Presenter *parent);
            void hideResponseButtons();
            void showNextTrialButton();
            void hideNextTrialButton();
            void showResponseButtons();
            SubjectResponse subjectResponse();
        private:
            Color colorResponse();
            
            View::Subject *view;
            Presenter *parent;
        };
        
        Presenter(
            Model *,
            View *,
            TestSetup *,
            Tester *,
            Subject *
        );
        void newTest() override;
        void openTest() override;
        void closeTest() override;
        void trialComplete() override;
        void run();
        void browseForTargetList();
        void playTrial();
        void submitResponse();
        void browseForMasker();
        void browseForCalibration();
        void confirmTestSetup();
        void playCalibration();
        static int fullScaleLevel_dB_SPL;
        static TrackingRule targetLevelRule;
        
    private:
        void showErrorMessage(std::string);
        RUNTIME_ERROR(BadInput)
        void playCalibration_();
        void hideTesterView();
        void showTestSetup();
        void proceedToNextTrial();
        bool userCancels();
        void hideTestSetup();
        void showTesterView();
        void hideResponseButtons();
        void showNextTrialButton();
        void hideNextTrialButton();
        void showResponseButtons();
        void initializeTest_();
        
        Model *model;
        View *view;
        TestSetup *testSetup;
        Tester *tester;
        Subject *subject;
    };
}

#endif
