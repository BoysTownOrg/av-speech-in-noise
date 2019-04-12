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
    virtual int x() const = 0;
    virtual bool complete() const = 0;
    virtual int reversals() const = 0;
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
    std::vector<int> runCounts;
    std::vector<int> stepSizes;
    std::vector<int> up;
    std::vector<int> down;
    std::size_t sequenceIndex;
    int _x;
    int sameDirectionConsecutiveCount;
    int runCounter;
    int _reversals;
    Direction previousDirection;
    Step previousStep;
public:
    int x() const override;
    void pushUp() override;
    void pushDown() override;
    bool complete() const override;
    int reversals() const override;
    void reset(const Parameters &) override;
    
private:
    void updateConsecutiveCount(AdaptiveTrack::Direction);
    void updateReversals(AdaptiveTrack::Step);
    void reversal();
    bool _complete() const;
};

void AdaptiveTrack::reset(const Parameters &p) {
    sequenceIndex = 0;
    _x = p.startingX;
    sameDirectionConsecutiveCount = 0;
    runCounter = 0;
    _reversals = 0;
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

int AdaptiveTrack::x() const {
    return _x;
}

void AdaptiveTrack::pushUp() {
    if (_complete())
        return;
    updateConsecutiveCount(Direction::up);
    if (sameDirectionConsecutiveCount == up[sequenceIndex]) {
        updateReversals(Step::fall);
        _x += stepSizes[sequenceIndex];
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
    ++_reversals;
    if (++runCounter == runCounts[sequenceIndex]) {
        runCounter = 0;
        ++sequenceIndex;
    }
}

void AdaptiveTrack::pushDown() {
    if (_complete())
        return;
    updateConsecutiveCount(Direction::down);
    if (sameDirectionConsecutiveCount == down[sequenceIndex]) {
        updateReversals(Step::rise);
        _x -= stepSizes[sequenceIndex];
        sameDirectionConsecutiveCount = 0;
        previousStep = Step::fall;
    }
    previousDirection = Direction::down;
}

bool AdaptiveTrack::complete() const {
    return _complete();
}

bool AdaptiveTrack::_complete() const {
    return sequenceIndex == runCounts.size();
}

int AdaptiveTrack::reversals() const {
    return _reversals;
}

#include <gtest/gtest.h>

namespace {
    class AdaptiveTrackTests : public ::testing::Test {
    protected:
        AdaptiveTrack::Parameters parameters{};
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
        AdaptiveTrack::Parameters::RunSequence sequence{};
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
        AdaptiveTrack::Parameters::RunSequence sequence{};
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
        AdaptiveTrack::Parameters::RunSequence sequence{};
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
        AdaptiveTrack::Parameters::RunSequence sequence{};
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
        std::vector<AdaptiveTrack::Parameters::RunSequence> sequences(2);
        sequences[0].runCount = 2;
        sequences[0].stepSize = 8;
        sequences[0].down = 2;
        sequences[0].up = 1;
        sequences[1].runCount = 1;
        sequences[1].stepSize = 4;
        sequences[1].down = 2;
        sequences[1].up = 1;
        parameters.runSequences = sequences;
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
        std::vector<AdaptiveTrack::Parameters::RunSequence> sequences(3);
        sequences[0].runCount = 1;
        sequences[0].stepSize = 10;
        sequences[0].down = 3;
        sequences[0].up = 1;
        sequences[1].runCount = 1;
        sequences[1].stepSize = 5;
        sequences[1].down = 3;
        sequences[1].up = 1;
        sequences[2].runCount = 6;
        sequences[2].stepSize = 2;
        sequences[2].down = 3;
        sequences[2].up = 1;
        parameters.runSequences = sequences;
        reset();
        push("ddudddudddddudddddduddd");
        EXPECT_EQ(3, track.x());
    }

    TEST_F(AdaptiveTrackTests, varyingDownUpRule) {
        parameters.startingX = 65;
        std::vector<AdaptiveTrack::Parameters::RunSequence> sequences(2);
        sequences[0].runCount = 2;
        sequences[0].stepSize = 8;
        sequences[0].up = 1;
        sequences[0].down = 2;
        sequences[1].runCount = 1;
        sequences[1].stepSize = 4;
        sequences[1].up = 2;
        sequences[1].down = 1;
        parameters.runSequences = sequences;
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
        AdaptiveTrack::Parameters::RunSequence sequence{};
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
