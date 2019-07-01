#include "AdaptiveTrack.hpp"

namespace adaptive_track {
    AdaptiveTrack::AdaptiveTrack(const Settings &p) :
        x_{p.startingX},
        ceiling_{p.ceiling}
    {
        for (const auto &sequence : *p.rule)
            if (sequence.runCount) {
                stepSizes.push_back(sequence.stepSize);
                runCounts.push_back(sequence.runCount);
                up.push_back(sequence.up);
                down.push_back(sequence.down);
            }
        stepSizes.push_back(0);
    }
    
    void AdaptiveTrack::pushUp() {
        if (complete_())
            return;
        
        auto direction = Direction::up;
        updateConsecutiveCount(direction);
        if (sameDirectionConsecutiveCount == up.at(sequenceIndex))
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
        x_ = std::min(x_, ceiling_);
        sameDirectionConsecutiveCount = 0;
        previousStep = Step::rise;
    }

    void AdaptiveTrack::updateReversals(Step step) {
        if (previousStep == step)
            reversal();
    }

    int AdaptiveTrack::stepSize() {
        return stepSizes.at(sequenceIndex);
    }

    void AdaptiveTrack::reversal() {
        ++reversals_;
        if (++runCounter == runCounts.at(sequenceIndex)) {
            runCounter = 0;
            ++sequenceIndex;
        }
    }
    
    void AdaptiveTrack::pushDown() {
        if (complete_())
            return;
        
        auto direction = Direction::down;
        updateConsecutiveCount(direction);
        if (sameDirectionConsecutiveCount == down.at(sequenceIndex))
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
