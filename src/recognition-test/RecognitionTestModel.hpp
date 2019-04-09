#ifndef recognition_test_Model_hpp
#define recognition_test_Model_hpp

#include <av-coordinated-response-measure/Model.h>
#include <vector>

namespace recognition_test {
    class InvalidAudioDevice {};
    
    class TargetPlayer {
    public:
        virtual ~TargetPlayer() = default;
        
        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void playbackComplete() = 0;
        };
        
        virtual void subscribe(EventListener *) = 0;
        virtual void setAudioDevice(std::string) = 0;
        virtual void play() = 0;
        virtual bool playing() = 0;
        virtual void loadFile(std::string filePath) = 0;
        virtual void hideVideo() = 0;
        virtual void showVideo() = 0;
        virtual double rms() = 0;
        virtual void setLevel_dB(double) = 0;
    };

    class MaskerPlayer {
    public:
        virtual ~MaskerPlayer() = default;
        
        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void fadeInComplete() = 0;
            virtual void fadeOutComplete() = 0;
        };
        
        virtual void subscribe(EventListener *) = 0;
        virtual std::vector<std::string> outputAudioDeviceDescriptions() = 0;
        virtual void setAudioDevice(std::string) = 0;
        virtual void fadeIn() = 0;
        virtual void fadeOut() = 0;
        virtual void loadFile(std::string filePath) = 0;
        virtual bool playing() = 0;
        virtual double rms() = 0;
        virtual void setLevel_dB(double) = 0;
    };

    class TargetList {
    public:
        virtual ~TargetList() = default;
        virtual void loadFromDirectory(std::string directory) = 0;
        virtual bool empty() = 0;
        virtual std::string next() = 0;
    };
    
    class OutputFile {
    public:
        virtual ~OutputFile() = default;
        virtual void openNewFile(
            const av_coordinated_response_measure::Model::Test &
        ) = 0;
        class OpenFailure {};
        virtual void writeTrial(
            const av_coordinated_response_measure::Trial &
        ) = 0;
        virtual void writeTest(
            const av_coordinated_response_measure::Model::Test &
        ) = 0;
        virtual void writeTrialHeading() = 0;
        virtual void close() = 0;
    };

    class RecognitionTestModel :
        public av_coordinated_response_measure::Model,
        public TargetPlayer::EventListener,
        public MaskerPlayer::EventListener
    {
        Test test{};
        MaskerPlayer *maskerPlayer;
        TargetList *targetList;
        TargetPlayer *targetPlayer;
        OutputFile *outputFile;
        Model::EventListener *listener_{};
    public:
        RecognitionTestModel(
            TargetList *,
            TargetPlayer *,
            MaskerPlayer *,
            OutputFile *
        );
        
        void initializeTest(const Test &) override;
        void playTrial(const AudioSettings &) override;
        void submitResponse(const SubjectResponse &) override;
        bool testComplete() override;
        std::vector<std::string> audioDevices() override;
        void subscribe(Model::EventListener *) override;
        void playCalibration(const Calibration &) override;
        void fadeInComplete() override;
        void fadeOutComplete() override;
        void playbackComplete() override;
    private:
        void prepareMasker(const Test &);
        void prepareOutputFile(const Test &);
        void tryOpeningOutputFile(const Test &);
        void loadStimulusList(const Test &);
        void loadMaskerFile(const Test &);
        bool noMoreTrials();
        bool trialInProgress();
        void loadNextTarget();
        void preparePlayers(const AudioSettings &);
        void startTrial();
        bool auditoryOnly(const Test &);
        void prepareVideo(const Test &);
        int desiredSignalLevel_dB();
        double signalLevel_dB();
        double maskerLevel_dB();
        void setTargetPlayerDevice(const std::string &);
        void setAudioDevices(const std::string &);
        void trySettingAudioDevices(const AudioSettings &);
        int findDeviceIndex(const AudioSettings &);
        void throwInvalidAudioDeviceOnErrorSettingDevice(
            void(RecognitionTestModel::*f)(const std::string &),
            const std::string &
        );
    };
}

#endif
