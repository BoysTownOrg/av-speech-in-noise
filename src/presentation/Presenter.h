#ifndef presentation_Presenter_h
#define presentation_Presenter_h

#include "Model.h"
#include <vector>
#include <string>

namespace presentation {
    std::string conditionName(Model::Test::Condition);

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
        };
        
        class Subject {
        public:
            virtual ~Subject() = default;
            virtual int numberResponse() = 0;
            virtual bool greenResponse() = 0;
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
            virtual std::string maskerLevel_dB_SPL() = 0;
            virtual std::string signalLevel_dB_SPL() = 0;
            virtual std::string startingSnr_dB() = 0;
            virtual std::string maskerFilePath() = 0;
            virtual std::string stimulusListDirectory() = 0;
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
        virtual void showErrorMessage(std::string) = 0;
        enum class DialogResponse {
            decline,
            accept,
            cancel
        };
        virtual DialogResponse showConfirmationDialog() = 0;
    };

    class Presenter : public View::EventListener {
    public:
        class TestSetup {
        public:
            TestSetup(Model *model, View::TestSetup *);
            void listen();
            void initializeTest();
            void tuneOut();
        private:
            int readInteger(std::string x, std::string identifier);
            bool auditoryOnly();
            Model::Test testParameters();
            
            Model *model;
            View::TestSetup *view;
        };
        
        class Tester {
        public:
            Tester(Model *, View::Tester *);
            void listen();
            void playTrial();
            void tuneOut();
        private:
            Model *model;
            View::Tester *view;
        };
        
        Presenter(Model *, View *);
        void run();
        void playTrial() override;
        void newTest() override;
        void openTest() override;
        void closeTest() override;
        void confirmTestSetup() override;
        void submitResponse() override;
        
    private:
        RUNTIME_ERROR(BadInput)
        void initializeTest_();
        
        TestSetup testSetup;
        Tester tester;
        Model *model;
        View *view;
    };
}

#endif
