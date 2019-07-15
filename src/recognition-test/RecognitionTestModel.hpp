#ifndef av_speech_in_noise_RecognitionTestModel_hpp
#define av_speech_in_noise_RecognitionTestModel_hpp

#include <av-speech-in-noise/Model.h>
#include <vector>
#include <memory>
#include <limits>

namespace av_speech_in_noise {
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
    
    class Randomizer {
    public:
        virtual ~Randomizer() = default;
        virtual double randomFloatBetween(double, double) = 0;
        virtual int randomIntBetween(int, int) = 0;
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
    
    class IAdaptiveMethod : public virtual TestMethod {
    public:
        virtual void initialize(const AdaptiveTest &) = 0;
    };
    
    class IFixedLevelMethod : public virtual TestMethod {
    public:
        virtual void initialize(const FixedLevelTest &) = 0;
    };
    
    class AdaptiveMethod : public IAdaptiveMethod {
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
        void initialize(const AdaptiveTest &) override;
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
    
    class IRecognitionTestModel_Internal {
    public:
        virtual ~IRecognitionTestModel_Internal() = default;
        virtual void initialize(
            TestMethod *,
            const CommonTest &,
            const TestInformation &
        ) = 0;
        virtual void playTrial(const AudioSettings &) = 0;
        virtual void submitResponse(
            const coordinate_response_measure::SubjectResponse &
        ) = 0;
        virtual bool testComplete() = 0;
        virtual std::vector<std::string> audioDevices() = 0;
        virtual void subscribe(Model::EventListener *) = 0;
        virtual void playCalibration(const Calibration &) = 0;
        virtual void submitCorrectResponse() = 0;
        virtual void submitIncorrectResponse() = 0;
        virtual void submitResponse(const FreeResponse &) = 0;
        virtual void throwIfTrialInProgress() = 0;
    };

    class RecognitionTestModel : public Model {
        IAdaptiveMethod *adaptiveMethod;
        IFixedLevelMethod *fixedLevelMethod;
        IRecognitionTestModel_Internal *model;
    public:
        RecognitionTestModel(
            IAdaptiveMethod *,
            IFixedLevelMethod *,
            IRecognitionTestModel_Internal *
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
