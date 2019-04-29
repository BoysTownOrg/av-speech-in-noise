#ifndef av_coordinate_response_measure_AdaptiveTrack_hpp
#define av_coordinate_response_measure_AdaptiveTrack_hpp

#include <recognition-test/RecognitionTestModel.hpp>

namespace av_coordinate_response_measure {
    class AdaptiveTrack : public Track {
        enum class Direction {
            undefined,
            up,
            down
        };
        enum class Step {
            undefined,
            rise,
            fall
        };
        std::vector<int> runCounts{};
        std::vector<int> stepSizes{};
        std::vector<int> up{};
        std::vector<int> down{};
        std::size_t sequenceIndex{};
        int x_{};
        int sameDirectionConsecutiveCount{};
        int runCounter{};
        int reversals_{};
        Direction previousDirection{Direction::undefined};
        Step previousStep{Step::undefined};
    public:
        explicit AdaptiveTrack(const Settings &);
        int x() override;
        void pushUp() override;
        void pushDown() override;
        bool complete() override;
        int reversals() override;
        
    private:
        void stepDown();
        void stepUp();
        int stepSize();
        void updateConsecutiveCount(Direction);
        void updateReversals(Step);
        void reversal();
        bool complete_() const;
    };
    
    class AdaptiveTrackFactory : public TrackFactory {
        std::shared_ptr<Track> make(const Track::Settings &s) override {
            return std::make_shared<AdaptiveTrack>(s);
        }
    };
}

#endif
