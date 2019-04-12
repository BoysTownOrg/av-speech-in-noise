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
    void updateConsecutiveCount(AdaptiveTrack::Direction);
    void updateReversals(AdaptiveTrack::Step);
    void reversal();
    bool complete_() const;
};

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

void AdaptiveTrack::updateConsecutiveCount(
    AdaptiveTrack::Direction direction)
{
    sameDirectionConsecutiveCount = previousDirection == direction
        ? sameDirectionConsecutiveCount + 1
        : 1;
}

void AdaptiveTrack::updateReversals(AdaptiveTrack::Step step) {
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

#include <gtest/gtest.h>

namespace {
    class AdaptiveTrackTests : public ::testing::Test {
    protected:
        AdaptiveTrack::Parameters parameters{};
        AdaptiveTrack::Parameters::RunSequence sequence{};
        AdaptiveTrack track{};
        
        void reset() {
            track.reset(parameters);
        }
        
        void push(std::string directions) {
            for (const auto &c : directions)
                if (c == 'd')
                    track.pushDown();
                else if (c == 'u')
                    track.pushUp();
        }
    };

    TEST_F(AdaptiveTrackTests, xEqualToStartingX) {
        parameters.startingX = 1;
        reset();
        EXPECT_EQ(1, track.x());
    }

    TEST_F(AdaptiveTrackTests, noRunSequencesMeansNoStepChanges) {
        parameters.startingX = 5;
        reset();
        track.pushDown();
        EXPECT_EQ(5, track.x());
        track.pushDown();
        EXPECT_EQ(5, track.x());
        track.pushUp();
        EXPECT_EQ(5, track.x());
    }

    TEST_F(AdaptiveTrackTests, exhaustedRunSequencesMeansNoMoreStepChanges) {
        parameters.startingX = 5;
        sequence.runCount = 3;
        sequence.stepSize = 4;
        sequence.up = 1;
        sequence.down = 2;
        parameters.runSequences.push_back(sequence);
        reset();
        track.pushDown();
        EXPECT_EQ(5, track.x());
        track.pushDown();
        EXPECT_EQ(5 - 4, track.x());
        track.pushUp();
        EXPECT_EQ(5 - 4 + 4, track.x());
        track.pushDown();
        EXPECT_EQ(5 - 4 + 4, track.x());
        track.pushDown();
        EXPECT_EQ(5 - 4 + 4 - 4, track.x());
        track.pushUp();
        EXPECT_EQ(5 - 4 + 4 - 4, track.x());
        track.pushDown();
        EXPECT_EQ(5 - 4 + 4 - 4, track.x());
        track.pushDown();
        EXPECT_EQ(5 - 4 + 4 - 4, track.x());
    }

    TEST_F(AdaptiveTrackTests, completeWhenExhausted) {
        parameters.startingX = 5;
        sequence.runCount = 3;
        sequence.stepSize = 4;
        sequence.up = 1;
        sequence.down = 2;
        parameters.runSequences.push_back(sequence);
        reset();
        EXPECT_FALSE(track.complete());
        track.pushDown();
        EXPECT_FALSE(track.complete());
        track.pushDown();
        EXPECT_FALSE(track.complete());
        track.pushUp();
        EXPECT_FALSE(track.complete());
        track.pushDown();
        EXPECT_FALSE(track.complete());
        track.pushDown();
        EXPECT_FALSE(track.complete());
        track.pushUp();
        EXPECT_TRUE(track.complete());
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure4) {
        parameters.startingX = 0;
        sequence.runCount = 8;
        sequence.stepSize = 1;
        sequence.down = 1;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        reset();
        push("dduuuudduuuddddduuudduu");
        EXPECT_EQ(1, track.x());
    }

    // see https://doi.org/10.1121/1.1912375
    TEST_F(AdaptiveTrackTests, LevittFigure5) {
        parameters.startingX = 0;
        sequence.runCount = 5;
        sequence.stepSize = 1;
        sequence.down = 2;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        reset();
        push("dddduduududdddduuuddddd");
        EXPECT_EQ(1, track.x());
    }

    TEST_F(AdaptiveTrackTests, twoSequences) {
        parameters.startingX = 65;
        sequence.runCount = 2;
        sequence.stepSize = 8;
        sequence.down = 2;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        sequence.runCount = 1;
        sequence.stepSize = 4;
        sequence.down = 2;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        reset();
        track.pushDown();
        EXPECT_EQ(65, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4 - 4, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4 - 4, track.x());
    }

    TEST_F(AdaptiveTrackTests, threeSequences) {
        parameters.startingX = 0;
        sequence.runCount = 1;
        sequence.stepSize = 10;
        sequence.down = 3;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        sequence.runCount = 1;
        sequence.stepSize = 5;
        sequence.down = 3;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        sequence.runCount = 6;
        sequence.stepSize = 2;
        sequence.down = 3;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        reset();
        push("ddudddudddddudddddduddd");
        EXPECT_EQ(3, track.x());
    }

    TEST_F(AdaptiveTrackTests, varyingDownUpRule) {
        parameters.startingX = 65;
        sequence.runCount = 2;
        sequence.stepSize = 8;
        sequence.up = 1;
        sequence.down = 2;
        parameters.runSequences.push_back(sequence);
        sequence.runCount = 1;
        sequence.stepSize = 4;
        sequence.up = 2;
        sequence.down = 1;
        parameters.runSequences.push_back(sequence);
        reset();
        track.pushDown();
        EXPECT_EQ(65, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4, track.x());
        track.pushDown();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4 - 4, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4 - 4, track.x());
        track.pushUp();
        EXPECT_EQ(65 - 8 - 8 + 8 + 8 - 4 - 4, track.x());
    }
    TEST_F(AdaptiveTrackTests, reversals) {
        parameters.startingX = 0;
        sequence.runCount = 1000;
        sequence.down = 2;
        sequence.up = 1;
        parameters.runSequences.push_back(sequence);
        reset();
        EXPECT_EQ(int{0}, track.reversals());
        track.pushUp();
        EXPECT_EQ(int{0}, track.reversals());
        track.pushDown();
        EXPECT_EQ(int{0}, track.reversals());
        track.pushDown();
        EXPECT_EQ(1, track.reversals());
        track.pushUp();
        EXPECT_EQ(2, track.reversals());
        track.pushDown();
        EXPECT_EQ(2, track.reversals());
        track.pushDown();
        EXPECT_EQ(3, track.reversals());
    }
}
