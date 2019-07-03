#ifndef av_speech_in_noise_Model_h
#define av_speech_in_noise_Model_h

#include <stdexcept>
#include <string>
#include <vector>

namespace av_speech_in_noise {
    namespace coordinate_response_measure {
        enum class Color {
            green,
            red,
            blue,
            white,
            notAColor
        };
        
        struct SubjectResponse {
            int number;
            Color color;
        };
        
        struct Trial {
            int SNR_dB;
            int correctNumber;
            int subjectNumber;
            int reversals;
            Color correctColor;
            Color subjectColor;
            bool correct;
        };
    }
    
    enum class Condition {
        auditoryOnly,
        audioVisual
    };
    
    constexpr const char *conditionName(Condition c) {
        switch (c) {
        case Condition::auditoryOnly:
            return "auditory-only";
        case Condition::audioVisual:
            return "audio-visual";
        }
        return "unknown";
    }
    
    struct TrackingSequence {
        int runCount;
        int stepSize;
        int down;
        int up;
    };
    
    using TrackingRule = typename std::vector<TrackingSequence>;
    
    struct TestInformation {
        std::string subjectId;
        std::string testerId;
        std::string session;
    };
    
    struct CommonTest {
        std::string targetListDirectory;
        std::string maskerFilePath;
        int maskerLevel_dB_SPL;
        int fullScaleLevel_dB_SPL;
        Condition condition;
    };
    
    struct AdaptiveTest {
        CommonTest common;
        TestInformation information;
        const TrackingRule *targetLevelRule;
        int startingSnr_dB;
        int maskerLevel_dB_SPL;
        int fullScaleLevel_dB_SPL;
        int ceilingSnr_dB;
        Condition condition;
    };
    
    struct FixedLevelTest {
        CommonTest common;
        TestInformation information;
        int snr_dB;
        int maskerLevel_dB_SPL;
        int fullScaleLevel_dB_SPL;
        Condition condition;
    };
    
    struct AudioSettings {
        std::string audioDevice;
    };
    
    struct Calibration {
        AudioSettings audioSettings;
        std::string filePath;
        int level_dB_SPL;
        int fullScaleLevel_dB_SPL;
        Condition condition;
    };
    
    struct FreeResponse {
        std::string response;
    };
    
    struct FreeResponseTrial {
        std::string response;
        std::string target;
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
        class RequestFailure : public std::runtime_error {
        public:
            explicit RequestFailure(std::string s) :
                std::runtime_error{ std::move(s) } {}
        };
        virtual void initializeTest(const AdaptiveTest &) = 0;
        virtual void initializeTest(const FixedLevelTest &) = 0;
        virtual void playCalibration(const Calibration &) = 0;
        virtual void playTrial(const AudioSettings &) = 0;
        virtual void submitResponse(
            const coordinate_response_measure::SubjectResponse &
        ) = 0;
        virtual void submitResponse(const FreeResponse &) = 0;
        virtual void submitCorrectResponse() = 0;
        virtual void submitIncorrectResponse() = 0;
        virtual bool testComplete() = 0;
        virtual std::vector<std::string> audioDevices() = 0;
    };
}

#endif
