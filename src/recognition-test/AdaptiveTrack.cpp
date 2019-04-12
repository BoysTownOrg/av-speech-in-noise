#include "AdaptiveTrack.hpp"

namespace recognition_test {
    void AdaptiveTrack::reset(const Parameters &p) {
        sequenceIndex = 0;
        x_ = p.startingX;
        sameDirectionConsecutiveCount = 0;
        runCounter = 0;
        reversals_ = 0;
        previousDirection = Direction::undefined;
        previousStep = Step::undefined;
        stepSizes.clear();
        for (const auto &sequence : p.runSequences) {
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
        updateConsecutiveCount(Direction::up);
        if (sameDirectionConsecutiveCount == up[sequenceIndex]) {
            updateReversals(Step::fall);
            x_ += stepSizes[sequenceIndex];
            sameDirectionConsecutiveCount = 0;
            previousStep = Step::rise;
        }
        previousDirection = Direction::up;
    }

    void AdaptiveTrack::updateConsecutiveCount(Direction direction) {
        sameDirectionConsecutiveCount = previousDirection == direction
            ? sameDirectionConsecutiveCount + 1
            : 1;
    }

    void AdaptiveTrack::updateReversals(Step step) {
        if (previousStep == step)
            reversal();
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
        updateConsecutiveCount(Direction::down);
        if (sameDirectionConsecutiveCount == down[sequenceIndex]) {
            updateReversals(Step::rise);
            x_ -= stepSizes[sequenceIndex];
            sameDirectionConsecutiveCount = 0;
            previousStep = Step::fall;
        }
        previousDirection = Direction::down;
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
