#ifndef recognition_test_AdaptiveTrack_hpp
#define recognition_test_AdaptiveTrack_hpp

#include <vector>

class Track {
public:
    virtual ~Track() = default;
    struct Parameters {
        struct RunSequence {
            int runCount;
            int stepSize;
            int down;
            int up;
        };
        std::vector<RunSequence> runSequences;
        int startingX;
    };
    virtual void reset(const Parameters &) = 0;
    virtual void pushDown() = 0;
    virtual void pushUp() = 0;
    virtual int x() = 0;
    virtual bool complete() = 0;
    virtual int reversals() = 0;
};

namespace recognition_test {
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
        int x() override;
        void pushUp() override;
        void pushDown() override;
        bool complete() override;
        int reversals() override;
        void reset(const Parameters &) override;
        
    private:
        void updateConsecutiveCount(Direction);
        void updateReversals(Step);
        void reversal();
        bool complete_() const;
    };
}

#endif
