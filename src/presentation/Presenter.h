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
        RUNTIME_ERROR(RequestFailure)
        virtual bool testComplete() = 0;
        
        struct TrialParameters {
            std::string audioDevice;
        };
        virtual void playTrial(const TrialParameters &) = 0;
        
        virtual std::vector<std::string> audioDevices() = 0;
    };

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
        };

        class Tester {
        public:
            class EventListener {
            public:
                virtual ~EventListener() = default;
                virtual void playTrial() = 0;
            };
            
            virtual ~Tester() = default;
            virtual void subscribe(EventListener *) = 0;
            virtual void show() = 0;
            virtual void hide() = 0;
            virtual std::string audioDevice() = 0;
            virtual void populateAudioDeviceMenu(std::vector<std::string>) = 0;
        };

        class TestSetup {
        public:
            class EventListener {
            public:
                virtual ~EventListener() = default;
                virtual void confirm() = 0;
            };
            
            virtual ~TestSetup() = default;
            virtual void subscribe(EventListener *) = 0;
            virtual void show() = 0;
            virtual void hide() = 0;
            virtual std::string maskerLevel_dB_SPL() = 0;
            virtual std::string signalLevel_dB_SPL() = 0;
            virtual std::string maskerFilePath() = 0;
            virtual std::string stimulusListDirectory() = 0;
            virtual std::string testerId() = 0;
            virtual std::string subjectId() = 0;
            virtual std::string condition() = 0;
        };
        
        virtual void subscribe(EventListener *) = 0;
        virtual void eventLoop() = 0;
        virtual TestSetup *testSetup() = 0;
        virtual Tester *tester() = 0;
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
        class TestSetup : public View::TestSetup::EventListener {
        public:
            explicit TestSetup(View::TestSetup *);
            void setParent(Presenter *);
            void run();
            void submitRequest(Model *);
            void close();
            void confirm() override;
        private:
            int readInteger(std::string x, std::string identifier);
            Model::TestParameters testParameters();
            
            Presenter *parent;
            View::TestSetup *view;
        };
        
        class Tester : public View::Tester::EventListener {
        public:
            explicit Tester(View::Tester *);
            void setParent(Presenter *);
            void initialize(Model *);
            void run();
            void submitRequest(Model *);
            void close();
            void playTrial() override;
        private:
            Presenter *parent;
            View::Tester *view;
        };
        
        Presenter(Model *, View *);
        void run();
        void initializeTest();
        void playTrial();
        void newTest() override;
        void openTest() override;
        void closeTest() override;
        void confirmTestSetup() override;
        
        RUNTIME_ERROR(BadInput)
    private:
        void initializeTest_();
        
        TestSetup testSetup;
        Tester tester;
        Model *model;
        View *view;
    };
}

#endif
