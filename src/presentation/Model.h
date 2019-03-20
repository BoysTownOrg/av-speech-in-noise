#ifndef Model_h
#define Model_h

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
        
        struct Test {
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
            int fullScaleLevel_dB_SPL;
            Condition condition;
        };
        virtual void initializeTest(const Test &) = 0;
        
        struct Trial {
            std::string audioDevice;
        };
        virtual void playTrial(const Trial &) = 0;
        
        struct SubjectResponse {
            enum class Color {
                green
            };
            
            int number;
            Color color;
        };
        virtual void submitResponse(const SubjectResponse &) = 0;
        
        virtual bool testComplete() = 0;
        virtual std::vector<std::string> audioDevices() = 0;
    };
}

#endif
