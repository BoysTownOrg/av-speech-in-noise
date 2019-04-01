#ifndef presentation_Presenter_h
#define presentation_Presenter_h

#include <av-coordinated-response-measure/Model.h>
#include <vector>
#include <string>

namespace presentation {
    std::string conditionName(av_coordinated_response_measure::Condition);

    class View {
    public:
        virtual ~View() = default;

        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void newTest() = 0;
            virtual void openTest() = 0;
            virtual void closeTest() = 0;
            virtual void confirmTestSetup() = 0;
            virtual void playTrial() = 0;
            virtual void submitResponse() = 0;
            virtual void browseForStimulusList() = 0;
            virtual void browseForMasker() = 0;
        };
        
        class Subject {
        public:
            virtual ~Subject() = default;
            virtual std::string numberResponse() = 0;
            virtual bool greenResponse() = 0;
            virtual bool blueResponse() = 0;
            virtual bool grayResponse() = 0;
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
            virtual std::string audioDevice() = 0;
            virtual void populateAudioDeviceMenu(std::vector<std::string>) = 0;
        };

        class TestSetup {
        public:
            virtual ~TestSetup() = default;
            virtual void show() = 0;
            virtual void hide() = 0;
            virtual std::string signalLevel_dB_SPL() = 0;
            virtual std::string startingSnr_dB() = 0;
            virtual std::string maskerFilePath() = 0;
            virtual void setMasker(std::string) = 0;
            virtual std::string stimulusListDirectory() = 0;
            virtual void setStimulusList(std::string) = 0;
            virtual std::string testerId() = 0;
            virtual std::string subjectId() = 0;
            virtual std::string condition() = 0;
            virtual void populateConditionMenu(std::vector<std::string> items) = 0;
        };
        
        virtual void subscribe(EventListener *) = 0;
        virtual void eventLoop() = 0;
        virtual TestSetup *testSetup() = 0;
        virtual Tester *tester() = 0;
        virtual Subject *subject() = 0;
        virtual std::string browseForDirectory() = 0;
        virtual std::string browseForOpeningFile() = 0;
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
        public av_coordinated_response_measure::Model::EventListener
    {
    public:
        class TestSetup {
        public:
            TestSetup(av_coordinated_response_measure::Model *model, View::TestSetup *);
            void listen();
            void initializeTest();
            void tuneOut();
        private:
            int readInteger(std::string x, std::string identifier);
            bool auditoryOnly();
            av_coordinated_response_measure::Model::Test testParameters();
            
            av_coordinated_response_measure::Model *model;
            View::TestSetup *view;
        };
        
        class Tester {
        public:
            Tester(av_coordinated_response_measure::Model *, View::Tester *);
            void listen();
            void playTrial();
            void tuneOut();
        private:
            av_coordinated_response_measure::Model *model;
            View::Tester *view;
        };
        
        Presenter(av_coordinated_response_measure::Model *, View *);
        void run();
        void playTrial() override;
        void newTest() override;
        void openTest() override;
        void closeTest() override;
        void confirmTestSetup() override;
        void submitResponse() override;
        void browseForStimulusList() override;
        void browseForMasker() override;
        void trialComplete() override;
        
    private:
        RUNTIME_ERROR(BadInput)
        void initializeTest_();
        void switchToSetupIfTestComplete();
        
        TestSetup testSetup;
        Tester tester;
        av_coordinated_response_measure::Model *model;
        View *view;
    };
}

#endif
