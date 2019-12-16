#include "AdaptiveTrack.hpp"

namespace adaptive_track {
AdaptiveTrack::AdaptiveTrack(const Settings &p)
    : x_{p.startingX}, ceiling_{p.ceiling}, floor_{p.floor},
      bumpLimit_{p.bumpLimit}, bumpCount_{0} {
    for (const auto &sequence : *p.rule)
        if (sequence.runCount != 0) {
            stepSizes.push_back(sequence.stepSize);
            runCounts.push_back(sequence.runCount);
            up_.push_back(sequence.up);
            down_.push_back(sequence.down);
        }
    stepSizes.push_back(0);
}

void AdaptiveTrack::up() {
    update(Direction::up, ceiling_, up_, &AdaptiveTrack::stepUp);
}

void AdaptiveTrack::updateBumpCount(int bumpBoundary) {
    bumpCount_ = x_ == bumpBoundary ? bumpCount_ + 1 : 0;
}

void AdaptiveTrack::update(Direction direction, int bumpBoundary,
    const std::vector<int> &thresholds,
    void (AdaptiveTrack::*whenThresholdMet)()) {
    if (complete_())
        return;

    updateBumpCount(bumpBoundary);
    updateConsecutiveCount(direction);
    callIfConsecutiveCountMet(whenThresholdMet, thresholds.at(sequenceIndex));
    previousDirection = direction;
}

void AdaptiveTrack::callIfConsecutiveCountMet(
    void (AdaptiveTrack::*f)(), int threshold) {
    if (consecutiveCountMet(threshold))
        (this->*f)();
}

auto AdaptiveTrack::consecutiveCountMet(int threshold) -> bool {
    return sameDirectionConsecutiveCount == threshold;
}

void AdaptiveTrack::updateConsecutiveCount(Direction direction) {
    sameDirectionConsecutiveCount =
        previousDirection == direction ? sameDirectionConsecutiveCount + 1 : 1;
}

void AdaptiveTrack::stepUp() {
    updateReversals(Step::fall);
    x_ = std::min(x_ + stepSize(), ceiling_);
    sameDirectionConsecutiveCount = 0;
    previousStep = Step::rise;
}

void AdaptiveTrack::updateReversals(Step step) {
    if (previousStep == step)
        reversal();
}

auto AdaptiveTrack::stepSize() -> int { return stepSizes.at(sequenceIndex); }

void AdaptiveTrack::reversal() {
    ++reversals_;
    if (++runCounter == runCounts.at(sequenceIndex)) {
        runCounter = 0;
        ++sequenceIndex;
    }
}

void AdaptiveTrack::down() {
    update(Direction::down, floor_, down_, &AdaptiveTrack::stepDown);
}

void AdaptiveTrack::stepDown() {
    updateReversals(Step::rise);
    x_ = std::max(x_ - stepSize(), floor_);
    sameDirectionConsecutiveCount = 0;
    previousStep = Step::fall;
}

auto AdaptiveTrack::x() -> int { return x_; }

auto AdaptiveTrack::complete() -> bool { return complete_(); }

auto AdaptiveTrack::complete_() const -> bool {
    return sequenceIndex == runCounts.size() || bumpCount_ == bumpLimit_;
}

auto AdaptiveTrack::reversals() -> int { return reversals_; }
}
