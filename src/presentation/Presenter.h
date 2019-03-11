#ifndef presentation_Presenter_h
#define presentation_Presenter_h

#include <string>

namespace recognition_test {
    class SubjectView;
}

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
            virtual void confirmTestSetup() = 0;
            virtual void openTest() = 0;
            virtual void playTrial() = 0;
            virtual void closeTest() = 0;
        };

        class TesterView {
        public:
            virtual ~TesterView() = default;
            virtual void subscribe(EventListener *) = 0;
            virtual void show() = 0;
            virtual void hide() = 0;
        };

        class TestSetupView {
        public:
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
        virtual recognition_test::SubjectView *subjectView() = 0;
        enum class DialogResponse {
            decline,
            accept,
            cancel
        };
        virtual DialogResponse showConfirmationDialog() = 0;
    };

    class Presenter : public View::EventListener {
        Model *model;
        View *view;
    public:
        Presenter(Model *, View *view);
        void run();
        void newTest() override;
        void confirmTestSetup() override;
        void openTest() override;
        void playTrial() override;
        void closeTest() override;
    };
}

#endif
