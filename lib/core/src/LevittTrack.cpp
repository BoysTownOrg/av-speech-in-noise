#include "LevittTrack.hpp"
#include <gsl/gsl>
#include <algorithm>

namespace av_speech_in_noise {
LevittTrack::LevittTrack(const Settings &p)
    : startingX_{p.startingX}, x_{p.startingX}, ceiling_{p.ceiling},
      floor_{p.floor}, bumpLimit_{p.bumpLimit}, bumpCount_{0},
      thresholdReversals{p.thresholdReversals} {
    for (const auto &sequence : *p.rule)
        if (sequence.runCount != 0) {
            stepSizes.push_back(sequence.stepSize);
            runCounts.push_back(sequence.runCount);
            up_.push_back(sequence.up);
            down_.push_back(sequence.down);
        }
    stepSizes.push_back(0);
}

void LevittTrack::up() {
    update(Direction::up, ceiling_, up_, &LevittTrack::stepUp);
}

void LevittTrack::updateBumpCount(int bumpBoundary) {
    bumpCount_ = x_ == bumpBoundary ? bumpCount_ + 1 : 0;
}

void LevittTrack::update(Direction direction, int bumpBoundary,
    const std::vector<int> &thresholds,
    void (LevittTrack::*whenThresholdMet)()) {
    if (complete_())
        return;

    updateBumpCount(bumpBoundary);
    updateConsecutiveCount(direction);
    callIfConsecutiveCountMet(whenThresholdMet, thresholds.at(sequenceIndex));
    previousDirection = direction;
}

void LevittTrack::callIfConsecutiveCountMet(
    void (LevittTrack::*f)(), int threshold) {
    if (consecutiveCountMet(threshold))
        (this->*f)();
}

auto LevittTrack::consecutiveCountMet(int threshold) -> bool {
    return sameDirectionConsecutiveCount == threshold;
}

void LevittTrack::updateConsecutiveCount(Direction direction) {
    sameDirectionConsecutiveCount =
        previousDirection == direction ? sameDirectionConsecutiveCount + 1 : 1;
}

void LevittTrack::stepUp() {
    updateReversals(Step::fall);
    x_ = std::min(x_ + stepSize(), ceiling_);
    sameDirectionConsecutiveCount = 0;
    previousStep = Step::rise;
}

void LevittTrack::updateReversals(Step step) {
    if (previousStep == step)
        reversal();
}

auto LevittTrack::stepSize() -> int { return stepSizes.at(sequenceIndex); }

void LevittTrack::reversal() {
    ++reversals_;
    reversalX.push_back(x_);
    if (++runCounter == runCounts.at(sequenceIndex)) {
        runCounter = 0;
        ++sequenceIndex;
    }
}

void LevittTrack::down() {
    update(Direction::down, floor_, down_, &LevittTrack::stepDown);
}

void LevittTrack::stepDown() {
    updateReversals(Step::rise);
    x_ = std::max(x_ - stepSize(), floor_);
    sameDirectionConsecutiveCount = 0;
    previousStep = Step::fall;
}

auto LevittTrack::x() -> double { return x_; }

auto LevittTrack::complete() -> bool { return complete_(); }

auto LevittTrack::complete_() const -> bool {
    return sequenceIndex == runCounts.size() || bumpCount_ == bumpLimit_;
}

auto LevittTrack::reversals() -> int { return reversals_; }

void LevittTrack::reset() {
    x_ = startingX_;
    sameDirectionConsecutiveCount = 0;
    reversals_ = 0;
    previousStep = Step::undefined;
    sequenceIndex = 0;
    runCounter = 0;
    bumpCount_ = 0;
}

static auto size(const std::vector<int> &v) -> gsl::index { return v.size(); }

static auto bounded(int reversals, const std::vector<int> &reversalX)
    -> gsl::index {
    return std::max(
        std::min<gsl::index>(reversals, size(reversalX)), gsl::index{0});
}

auto LevittTrack::result() -> std::variant<Threshold, Phi> {
    return std::accumulate(reversalX.rbegin(),
               reversalX.rbegin() + bounded(thresholdReversals, reversalX), 0) /
        (bounded(thresholdReversals, reversalX) * 1.);
}
}
