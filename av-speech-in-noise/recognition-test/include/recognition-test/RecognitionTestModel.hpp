#ifndef av_speech_in_noise_RecognitionTestModel_hpp
#define av_speech_in_noise_RecognitionTestModel_hpp

#include <av-speech-in-noise/Model.h>
#include <vector>
#include <memory>

namespace av_speech_in_noise {
    class InvalidAudioDevice {};
    class InvalidAudioFile {};
    
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
        virtual double durationSeconds() = 0;
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
        virtual double durationSeconds() = 0;
        virtual void seekSeconds(double) = 0;
        virtual double fadeTimeSeconds() = 0;
    };
    
    class Track {
    public:
        virtual ~Track() = default;
        struct Settings {
            const TrackingRule *rule;
            int startingX;
        };
        virtual void pushDown() = 0;
        virtual void pushUp() = 0;
        virtual int x() = 0;
        virtual bool complete() = 0;
        virtual int reversals() = 0;
    };
    
    class TrackFactory {
    public:
        virtual ~TrackFactory() = default;
        virtual std::shared_ptr<Track> make(const Track::Settings &) = 0;
    };

    class TargetList {
    public:
        virtual ~TargetList() = default;
        virtual void loadFromDirectory(std::string directory) = 0;
        virtual std::string next() = 0;
        virtual std::string current() = 0;
    };
    
    class FiniteTargetList : public virtual TargetList {
    public:
        virtual bool empty() = 0;
    };
    
    class TargetListReader {
    public:
        virtual ~TargetListReader() = default;
        using lists_type = typename std::vector<std::shared_ptr<TargetList>>;
        virtual lists_type read(std::string directory) = 0;
    };
    
    class ResponseEvaluator {
    public:
        virtual ~ResponseEvaluator() = default;
        virtual bool correct(
            const std::string &filePath,
            const coordinate_response_measure::SubjectResponse &
        ) = 0;
        virtual coordinate_response_measure::Color correctColor(const std::string &filePath) = 0;
        virtual int correctNumber(const std::string &filePath) = 0;
        virtual std::string fileName(const std::string &filePath) = 0;
    };
    
    class OutputFile {
    public:
        virtual ~OutputFile() = default;
        virtual void openNewFile(const TestInformation &) = 0;
        class OpenFailure {};
        virtual void writeTrial(const coordinate_response_measure::Trial &) = 0;
        virtual void writeTrial(const FreeResponseTrial &) = 0;
        virtual void writeTest(const AdaptiveTest &) = 0;
        virtual void writeTest(const FixedLevelTest &) = 0;
        virtual void writeCoordinateResponseTrialHeading() = 0;
        virtual void writeFreeResponseTrialHeading() = 0;
        virtual void close() = 0;
    };
    
    class Randomizer {
    public:
        virtual ~Randomizer() = default;
        virtual double randomFloatBetween(double, double) = 0;
        virtual int randomIntBetween(int, int) = 0;
    };

    class RecognitionTestModel :
        public Model,
        public TargetPlayer::EventListener,
        public MaskerPlayer::EventListener
    {
        struct TargetListWithTrack {
            TargetList *list;
            std::shared_ptr<Track> track;
        };
        
        TargetListReader::lists_type lists{};
        std::vector<TargetListWithTrack> targetListsWithTracks{};
        int maskerLevel_dB_SPL{};
        int fullScaleLevel_dB_SPL{};
        int snr_dB{};
        TargetListReader *targetListSetReader;
        FiniteTargetList *finiteTargetList;
        MaskerPlayer *maskerPlayer;
        TargetPlayer *targetPlayer;
        TrackFactory *snrTrackFactory;
        ResponseEvaluator *evaluator;
        OutputFile *outputFile;
        Randomizer *randomizer;
        Model::EventListener *listener_{};
        Track *currentSnrTrack{};
        TargetList *currentTargetList{};
        bool fixedLevelTest{};
        bool justWroteFreeResponseTrial{};
        bool justWroteCoordinateResponseTrial{};
    public:
        RecognitionTestModel(
            TargetListReader *,
            FiniteTargetList *,
            TargetPlayer *,
            MaskerPlayer *,
            TrackFactory *,
            ResponseEvaluator *,
            OutputFile *,
            Randomizer *
        );
        void initializeTest(const AdaptiveTest &) override;
        void initializeTest(const FixedLevelTest &) override;
        void playTrial(const AudioSettings &) override;
        void submitResponse(const coordinate_response_measure::SubjectResponse &) override;
        bool testComplete() override;
        std::vector<std::string> audioDevices() override;
        void subscribe(Model::EventListener *) override;
        void playCalibration(const Calibration &) override;
        void submitCorrectResponse() override;
        void submitIncorrectResponse() override;
        void submitResponse(const FreeResponse &) override;
        void fadeInComplete() override;
        void fadeOutComplete() override;
        void playbackComplete() override;
    private:
        void prepareNextAdaptiveTrialAfterRemovingCompleteTracks();
        void preparePlayersForNextTrial();
        void prepareNextAdaptiveTrial();
        void readTargetLists(const AdaptiveTest &);
        void throwIfTrialInProgress();
        void writeTrial(const coordinate_response_measure::SubjectResponse &);
        std::string currentTarget();
        bool correct(const coordinate_response_measure::SubjectResponse &);
        void updateSnr(const coordinate_response_measure::SubjectResponse &);
        void removeCompleteTracks();
        void selectNextList();
        void prepareSnrTracks(const AdaptiveTest &);
        void setTargetPlayerDevice(const Calibration &);
        double calibrationLevel_dB(const Calibration &);
        void trySettingTargetLevel(const Calibration &);
        void playCalibration_(const Calibration &);
        void prepareMasker(const std::string &);
        void tryOpeningOutputFile(const TestInformation &);
        void prepareTargets(const AdaptiveTest &);
        void loadMaskerFile(const std::string &);
        void playTarget();
        bool noMoreTrials();
        bool trialInProgress();
        void prepareTargetPlayer();
        void seekRandomMaskerPosition();
        void preparePlayersToPlay(const AudioSettings &);
        void startTrial();
        bool auditoryOnly(const Condition &);
        void prepareVideo(const Condition &);
        int desiredMaskerLevel_dB();
        double unalteredTargetLevel_dB();
        int adaptiveSnr_dB();
        double targetLevel_dB();
        double maskerLevel_dB();
        void setTargetPlayerDevice_(const std::string &);
        void setAudioDevices_(const std::string &);
        void setAudioDevices(const AudioSettings &);
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
