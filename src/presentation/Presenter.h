#ifndef presentation_Presenter_h
#define presentation_Presenter_h

#include <string>
#include <vector>
#include <stdexcept>
#include <string>

#define RUNTIME_ERROR(class_name) \
    class class_name : public std::runtime_error {\
    public:\
        explicit class_name(std::string s) : std::runtime_error{ s } {}\
};

namespace presentation {
    class Model {
    public:
        virtual ~Model() = default;
        RUNTIME_ERROR(RequestFailure)
        
        struct TestParameters {
            enum class Condition {
                auditoryOnly,
                audioVisual
            };
            std::string stimulusListDirectory;
            std::string subjectId;
            std::string testerId;
            std::string maskerFilePath;
            int maskerLevel_dB_SPL;
            int signalLevel_dB_SPL;
            Condition condition;
        };
        virtual void initializeTest(const TestParameters &) = 0;
        
        struct TrialParameters {
            std::string audioDevice;
        };
        virtual void playTrial(const TrialParameters &) = 0;
        
        struct ResponseParameters {
            enum class Color {
                green
            };
            
            int number;
            Color color;
        };
        virtual void submitResponse(const ResponseParameters &) = 0;
        
        virtual bool testComplete() = 0;
        virtual std::vector<std::string> audioDevices() = 0;
    };
    
    std::string conditionName(Model::TestParameters::Condition);

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
        
        class SubjectView {
        public:
            virtual ~SubjectView() = default;
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
            virtual std::string maskerFilePath() = 0;
            virtual std::string stimulusListDirectory() = 0;
            virtual std::string testerId() = 0;
            virtual std::string subjectId() = 0;
            virtual std::string condition() = 0;
            virtual void populateConditionMenu(std::vector<std::string> items) = 0;
        };
        
        virtual void subscribe(EventListener *) = 0;
        virtual void showConfirmTestSetupButton() = 0;
        virtual void hideConfirmTestSetupButton() = 0;
        virtual void eventLoop() = 0;
        virtual TestSetup *testSetup() = 0;
        virtual Tester *tester() = 0;
        virtual SubjectView *subject() = 0;
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
            Model::TestParameters testParameters();
            
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
