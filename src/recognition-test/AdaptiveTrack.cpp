#include "AdaptiveTrack.hpp"

namespace av_coordinate_response_measure {
    AdaptiveTrack::AdaptiveTrack(const Settings &p) {
        sequenceIndex = 0;
        sameDirectionConsecutiveCount = 0;
        runCounter = 0;
        reversals_ = 0;
        previousDirection = Direction::undefined;
        previousStep = Step::undefined;
        stepSizes.clear();
        
        x_ = p.startingX;
        for (const auto &sequence : *p.rule) {
            if (sequence.runCount) {
                stepSizes.push_back(sequence.stepSize);
                runCounts.push_back(sequence.runCount);
                up.push_back(sequence.up);
                down.push_back(sequence.down);
            }
        }
        stepSizes.push_back(0);
    }
    
    void AdaptiveTrack::pushUp() {
        if (complete_())
            return;
        
        auto direction = Direction::up;
        updateConsecutiveCount(direction);
        if (sameDirectionConsecutiveCount == up[sequenceIndex])
            stepUp();
        previousDirection = direction;
    }

    void AdaptiveTrack::updateConsecutiveCount(Direction direction) {
        sameDirectionConsecutiveCount = previousDirection == direction
            ? sameDirectionConsecutiveCount + 1
            : 1;
    }
    
    void AdaptiveTrack::stepUp() {
        updateReversals(Step::fall);
        x_ += stepSize();
        sameDirectionConsecutiveCount = 0;
        previousStep = Step::rise;
    }

    void AdaptiveTrack::updateReversals(Step step) {
        if (previousStep == step)
            reversal();
    }

    int AdaptiveTrack::stepSize() {
        return stepSizes[sequenceIndex];
    }

    void AdaptiveTrack::reversal() {
        ++reversals_;
        if (++runCounter == runCounts[sequenceIndex]) {
            runCounter = 0;
            ++sequenceIndex;
        }
    }
    
    void AdaptiveTrack::pushDown() {
        if (complete_())
            return;
        
        auto direction = Direction::down;
        updateConsecutiveCount(direction);
        if (sameDirectionConsecutiveCount == down[sequenceIndex])
            stepDown();
        previousDirection = direction;
    }

    void AdaptiveTrack::stepDown() {
        updateReversals(Step::rise);
        x_ -= stepSize();
        sameDirectionConsecutiveCount = 0;
        previousStep = Step::fall;
    }

    int AdaptiveTrack::x() {
        return x_;
    }

    bool AdaptiveTrack::complete() {
        return complete_();
    }

    bool AdaptiveTrack::complete_() const {
        return sequenceIndex == runCounts.size();
    }

    int AdaptiveTrack::reversals() {
        return reversals_;
    }
}
