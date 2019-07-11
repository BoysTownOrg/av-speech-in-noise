#ifndef av_speech_in_noise_RecognitionTestModel_hpp
#define av_speech_in_noise_RecognitionTestModel_hpp

#include <av-speech-in-noise/Model.h>
#include <gsl/gsl>
#include <vector>
#include <memory>
#include <limits>

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
            int ceiling = std::numeric_limits<int>::max();
            int floor = std::numeric_limits<int>::min();
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
        virtual coordinate_response_measure::Color correctColor(
            const std::string &filePath
        ) = 0;
        virtual int correctNumber(const std::string &filePath) = 0;
        virtual std::string fileName(const std::string &filePath) = 0;
    };
    
    class OutputFile {
    public:
        virtual ~OutputFile() = default;
        virtual void openNewFile(const TestInformation &) = 0;
        class OpenFailure {};
        virtual void writeTrial(
            const coordinate_response_measure::AdaptiveTrial &
        ) = 0;
        virtual void writeTrial(
            const coordinate_response_measure::FixedLevelTrial &
        ) = 0;
        virtual void writeTrial(const FreeResponseTrial &) = 0;
        virtual void writeTest(const AdaptiveTest &) = 0;
        virtual void writeTest(const FixedLevelTest &) = 0;
        virtual void close() = 0;
        virtual void save() = 0;
    };
    
    class Randomizer {
    public:
        virtual ~Randomizer() = default;
        virtual double randomFloatBetween(double, double) = 0;
        virtual int randomIntBetween(int, int) = 0;
    };
    
    class TestMethod {
    public:
        virtual ~TestMethod() = default;
        virtual bool complete() = 0;
        virtual std::string next() = 0;
        virtual std::string current() = 0;
        virtual int snr_dB() = 0;
        virtual void submitCorrectResponse() = 0;
        virtual void submitIncorrectResponse() = 0;
        virtual void submitResponse(const FreeResponse &) = 0;
        virtual void writeTestingParameters(OutputFile *) = 0;
        virtual void writeLastCoordinateResponse(OutputFile *) = 0;
        virtual void submitResponse(
            const coordinate_response_measure::SubjectResponse &
        ) = 0;
    };
    
    class AdaptiveMethod : public TestMethod {
        struct TargetListWithTrack {
            TargetList *list;
            std::shared_ptr<Track> track;
        };
        TargetListReader::lists_type lists{};
        std::vector<TargetListWithTrack> targetListsWithTracks{};
        Track::Settings trackSettings{};
        coordinate_response_measure::AdaptiveTrial lastTrial{};
        const AdaptiveTest *test{};
        TargetListReader *targetListSetReader;
        TrackFactory *snrTrackFactory;
        ResponseEvaluator *evaluator;
        Randomizer *randomizer;
        Track *currentSnrTrack{};
        TargetList *currentTargetList{};
    public:
        AdaptiveMethod(
            TargetListReader *,
            TrackFactory *,
            ResponseEvaluator *,
            Randomizer *
        );
        void initialize(const AdaptiveTest &);
        int snr_dB() override;
        void submitIncorrectResponse() override;
        void submitCorrectResponse() override;
        bool complete() override;
        std::string next() override;
        std::string current() override;
        void writeLastCoordinateResponse(OutputFile *) override;
        void writeTestingParameters(OutputFile *) override;
        void submitResponse(
            const coordinate_response_measure::SubjectResponse &
        ) override;
        void submitResponse(const FreeResponse &) override;
        
    private:
        void selectNextListAfter(void(AdaptiveMethod::*)());
        void prepareSnrTracks();
        void makeSnrTracks();
        void makeTrackWithList(TargetList *list);
        void selectNextList();
        void removeCompleteTracks();
        bool complete(const TargetListWithTrack &);
        bool correct(
            const std::string &,
            const coordinate_response_measure::SubjectResponse &
        );
        void incorrect();
        void correct();
    };

    class FixedLevelMethod : public TestMethod {
        coordinate_response_measure::FixedLevelTrial lastTrial{};
        const FixedLevelTest *test{};
        TargetList *targetList;
        ResponseEvaluator *evaluator;
        int snr_dB_{};
        int trials_{};
        bool complete_{};
    public:
        FixedLevelMethod(TargetList *, ResponseEvaluator *);
        void initialize(const FixedLevelTest &);
        int snr_dB() override;
        std::string next() override;
        bool complete() override;
        std::string current() override;
        void submitIncorrectResponse() override;
        void submitCorrectResponse() override;
        void writeLastCoordinateResponse(OutputFile *) override;
        void writeTestingParameters(OutputFile *) override;
        void submitResponse(
            const coordinate_response_measure::SubjectResponse &
        ) override;
        void submitResponse(const FreeResponse &) override;
    private:
        void updateCompletion();
    };
    
    class RecognitionTestModel_Internal :
        public TargetPlayer::EventListener,
        public MaskerPlayer::EventListener
    {
        MaskerPlayer *maskerPlayer;
        TargetPlayer *targetPlayer;
        ResponseEvaluator *evaluator;
        OutputFile *outputFile;
        Randomizer *randomizer;
        Model::EventListener *listener_{};
        TestMethod *testMethod{};
        int maskerLevel_dB_SPL{};
        int fullScaleLevel_dB_SPL{};
        Condition condition;
    public:
        RecognitionTestModel_Internal(
            TargetPlayer *,
            MaskerPlayer *,
            ResponseEvaluator *,
            OutputFile *,
            Randomizer *
        );
        void initialize(TestMethod *, const CommonTest &, const TestInformation &);
        void playTrial(const AudioSettings &);
        void submitResponse(const coordinate_response_measure::SubjectResponse &);
        bool testComplete();
        std::vector<std::string> audioDevices();
        void subscribe(Model::EventListener *);
        void playCalibration(const Calibration &);
        void submitCorrectResponse();
        void submitIncorrectResponse();
        void submitResponse(const FreeResponse &);
        void fadeInComplete() override;
        void fadeOutComplete() override;
        void playbackComplete() override;
        void throwIfTrialInProgress();
    private:
        void submitCorrectResponse_();
        void submitIncorrectResponse_();
        void writeTrial(const FreeResponse &p);
        void prepareCommonTest(const CommonTest &, const TestInformation &);
        void storeLevels(const CommonTest &common);
        void preparePlayersForNextTrial();
        std::string currentTarget();
        bool correct(const coordinate_response_measure::SubjectResponse &);
        void submitResponse_(const coordinate_response_measure::SubjectResponse &);
        void setTargetPlayerDevice(const Calibration &);
        double calibrationLevel_dB(const Calibration &);
        void trySettingTargetLevel(const Calibration &);
        void playCalibration_(const Calibration &);
        void prepareMasker(const std::string &);
        void tryOpeningOutputFile_(const TestInformation &);
        void tryOpeningOutputFile(const TestInformation &);
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
        double targetLevel_dB();
        double maskerLevel_dB();
        void setTargetPlayerDevice_(const std::string &);
        void setAudioDevices_(const std::string &);
        void setAudioDevices(const AudioSettings &);
        int findDeviceIndex(const AudioSettings &);
        void throwInvalidAudioDeviceOnErrorSettingDevice(
            void(RecognitionTestModel_Internal::*f)(const std::string &),
            const std::string &
        );
        void throwInvalidAudioFileOnErrorLoading(
            void (RecognitionTestModel_Internal::*f)(const std::string &),
            const std::string &file
        );
        void loadTargetFile(std::string);
        void setTargetLevel_dB(double);
    };

    class RecognitionTestModel : public Model {
        AdaptiveMethod *adaptiveMethod;
        FixedLevelMethod *fixedLevelMethod;
        RecognitionTestModel_Internal *model;
    public:
        RecognitionTestModel(
            AdaptiveMethod *,
            FixedLevelMethod *,
            RecognitionTestModel_Internal *
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
    };
}

#endif
