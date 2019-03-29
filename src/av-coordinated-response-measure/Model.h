#ifndef av_coordinated_response_measure_Model_h
#define av_coordinated_response_measure_Model_h

#include <stdexcept>
#include <string>

#define RUNTIME_ERROR(class_name) \
    class class_name : public std::runtime_error {\
    public:\
        explicit class_name(std::string s) : std::runtime_error{ s } {}\
};

namespace av_coordinated_response_measure {
    enum class Color {
        green,
        red
    };
    
    enum class Condition {
        auditoryOnly,
        audioVisual
    };
    
    class Model {
    public:
        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void trialComplete() = 0;
        };
        
        virtual ~Model() = default;
        virtual void subscribe(EventListener *) = 0;
        RUNTIME_ERROR(RequestFailure)
        
        struct Test {
            std::string stimulusListDirectory;
            std::string subjectId;
            std::string testerId;
            std::string maskerFilePath;
            std::string session;
            int startingSnr_dB;
            int signalLevel_dB_SPL;
            int fullScaleLevel_dB_SPL;
            Condition condition;
        };
        virtual void initializeTest(const Test &) = 0;
        
        struct Trial {
            std::string audioDevice;
        };
        virtual void playTrial(const Trial &) = 0;
        
        struct SubjectResponse {
            int number;
            Color color;
        };
        virtual void submitResponse(const SubjectResponse &) = 0;
        
        virtual bool testComplete() = 0;
        virtual std::vector<std::string> audioDevices() = 0;
    };
    
    struct Trial {
        int SNR_dB;
        int correctNumber;
        int subjectNumber;
        Color correctColor;
        Color subjectColor;
        int reversals;
    };
}

#endif
