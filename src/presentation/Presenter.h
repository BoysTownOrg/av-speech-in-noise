#ifndef presentation_Presenter_h
#define presentation_Presenter_h

#include <string>

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
        virtual void initializeTest(TestParameters) = 0;
        virtual bool testComplete() = 0;
        virtual void playTrial() = 0;
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
        };

        class TesterView {
        public:
            class EventListener {
            public:
                virtual ~EventListener() = default;
                virtual void playTrial() = 0;
            };
            
            virtual ~TesterView() = default;
            virtual void subscribe(EventListener *) = 0;
            virtual void show() = 0;
            virtual void hide() = 0;
        };

        class TestSetupView {
        public:
            class EventListener {
            public:
                virtual ~EventListener() = default;
                virtual void confirmTestSetup() = 0;
            };
            
            virtual ~TestSetupView() = default;
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
        virtual TestSetupView *setupView() = 0;
        virtual TesterView *testerView() = 0;
        enum class DialogResponse {
            decline,
            accept,
            cancel
        };
        virtual DialogResponse showConfirmationDialog() = 0;
    };

    class Presenter : public View::EventListener {
    public:
        class TestSetup : public View::TestSetupView::EventListener {
        public:
            TestSetup(View::TestSetupView *);
            void setParent(Presenter *);
            void confirmTestSetup() override;
        private:
            Presenter *parent;
            View::TestSetupView *view;
        };
        
        class Tester : public View::TesterView::EventListener {
        public:
            Tester(View::TesterView *);
            void setParent(Presenter *);
            void playTrial() override;
        private:
            Presenter *parent;
            View::TesterView *view;
        };
        
        Presenter(Model *, View *);
        void run();
        void initializeTest(Model::TestParameters);
        void playTrial();
        void newTest() override;
        void openTest() override;
        void closeTest() override;
    private:
        TestSetup testSetup;
        Tester tester;
        Model *model;
        View *view;
    };
}

#endif
