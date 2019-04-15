#ifndef av_coordinate_response_measure_RecognitionTestModel_hpp
#define av_coordinate_response_measure_RecognitionTestModel_hpp

#include <av-coordinate-response-measure/Model.h>
#include <vector>

namespace av_coordinate_response_measure {
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
        virtual void subscribeToPlaybackCompletion() = 0;
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
    
    class Track {
    public:
        virtual ~Track() = default;
        struct Settings {
            const Rule *rule;
            int startingX;
        };
        virtual void reset(const Settings &) = 0;
        virtual void pushDown() = 0;
        virtual void pushUp() = 0;
        virtual int x() = 0;
        virtual bool complete() = 0;
        virtual int reversals() = 0;
    };

    class TargetList {
    public:
        virtual ~TargetList() = default;
        virtual void loadFromDirectory(std::string directory) = 0;
        virtual bool empty() = 0;
        virtual std::string next() = 0;
        virtual std::string current() = 0;
    };
    
    class ResponseEvaluator {
    public:
        virtual ~ResponseEvaluator() = default;
        virtual bool correct(
            std::string filePath,
            const SubjectResponse &
        ) = 0;
    };
    
    class OutputFile {
    public:
        virtual ~OutputFile() = default;
        virtual void openNewFile(
            const Test &
        ) = 0;
        class OpenFailure {};
        virtual void writeTrial(
            const Trial &
        ) = 0;
        virtual void writeTest(
            const Test &
        ) = 0;
        virtual void writeTrialHeading() = 0;
        virtual void close() = 0;
    };

    class RecognitionTestModel :
        public Model,
        public TargetPlayer::EventListener,
        public MaskerPlayer::EventListener
    {
        Test test{};
        MaskerPlayer *maskerPlayer;
        TargetList *targetList;
        TargetPlayer *targetPlayer;
        Track *snrTrack;
        ResponseEvaluator *evaluator;
        OutputFile *outputFile;
        Model::EventListener *listener_{};
    public:
        RecognitionTestModel(
            TargetList *,
            TargetPlayer *,
            MaskerPlayer *,
            Track *,
            ResponseEvaluator *,
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
        void prepareSnrTrack();
        void playTarget();
        void playCalibration_(const Calibration &);
        void prepareMasker();
        void prepareOutputFile();
        void tryOpeningOutputFile();
        void prepareTargets();
        void loadMaskerFile();
        bool noMoreTrials();
        bool trialInProgress();
        void loadNextTarget();
        void preparePlayers(const AudioSettings &);
        void startTrial();
        bool auditoryOnly();
        void prepareVideo();
        int desiredMaskerLevel_dB();
        double unalteredTargetLevel_dB();
        double targetLevel_dB();
        double maskerLevel_dB();
        void setTargetPlayerDevice(const std::string &);
        void setAudioDevices(const std::string &);
        void trySettingAudioDevices(const AudioSettings &);
        int findDeviceIndex(const AudioSettings &);
        void throwInvalidAudioDeviceOnErrorSettingDevice(
            void(RecognitionTestModel::*f)(const std::string &),
            const std::string &
        );
        void loadTargetFile(std::string);
        void setTargetLevel_dB(double);
    };
}

#endif
