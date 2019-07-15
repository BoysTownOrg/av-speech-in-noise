#ifndef av_speech_in_noise_AdaptiveMethod_hpp
#define av_speech_in_noise_AdaptiveMethod_hpp

#include "RecognitionTestModel.hpp"
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
    
    class TargetListReader {
    public:
        virtual ~TargetListReader() = default;
        using lists_type = typename std::vector<std::shared_ptr<TargetList>>;
        virtual lists_type read(std::string directory) = 0;
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
}

#endif
