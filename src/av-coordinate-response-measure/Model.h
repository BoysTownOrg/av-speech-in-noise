#ifndef av_coordinated_response_measure_Model_h
#define av_coordinated_response_measure_Model_h

#include <stdexcept>
#include <string>

#define RUNTIME_ERROR(class_name) \
    class class_name : public std::runtime_error {\
    public:\
        explicit class_name(std::string s) : std::runtime_error{ s } {}\
};

namespace av_coordinate_response_measure {
    enum class Color {
        green,
        red,
        blue,
        white
    };
    
    enum class Condition {
        auditoryOnly,
        audioVisual
    };
    
    struct Trial {
        int SNR_dB;
        int correctNumber;
        int subjectNumber;
        int reversals;
        Color correctColor;
        Color subjectColor;
    };
    
    constexpr const char *conditionName(Condition c) {
        switch (c) {
        case Condition::auditoryOnly:
            return "auditory-only";
        case Condition::audioVisual:
            return "audio-visual";
        }
    }
    
    struct TrackingSequence {
        int runCount;
        int stepSize;
        int down;
        int up;
    };
    
    using TrackingRule = typename std::vector<TrackingSequence>;
    
    struct Test {
        std::string targetListDirectory;
        std::string subjectId;
        std::string testerId;
        std::string maskerFilePath;
        std::string session;
        int startingSnr_dB;
        int maskerLevel_dB_SPL;
        int fullScaleLevel_dB_SPL;
        Condition condition;
        const TrackingRule *targetLevelRule;
    };
    
    struct AudioSettings {
        std::string audioDevice;
    };
    
    struct Calibration {
        std::string filePath;
        std::string audioDevice;
        int level_dB_SPL;
        int fullScaleLevel_dB_SPL;
    };
    
    struct SubjectResponse {
        int number;
        Color color;
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
        virtual void initializeTest(const Test &) = 0;
        virtual void playCalibration(const Calibration &) = 0;
        virtual void playTrial(const AudioSettings &) = 0;
        virtual void submitResponse(const SubjectResponse &) = 0;
        virtual bool testComplete() = 0;
        virtual std::vector<std::string> audioDevices() = 0;
    };
}

#endif
