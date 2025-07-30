#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_LEVITTTRACKHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_LEVITTTRACKHPP_

#include "IAdaptiveMethod.hpp"
#include <vector>

namespace av_speech_in_noise {
struct TrackingSequence {
    int runCount{};
    int stepSize{};
    int down{};
    int up{};
};

using TrackingRule = typename std::vector<TrackingSequence>;

struct LevittSettings {
    TrackingRule trackingRule;
    int thresholdReversals;
};

class LevittTrack : public AdaptiveTrack {
  public:
    explicit LevittTrack(const LevittSettings &, const Settings &);
    auto x() -> double override;
    void up() override;
    void down() override;
    auto complete() -> bool override;
    auto reversals() -> int override;
    void reset() override;
    auto result() -> std::variant<Threshold, Phi> override;

  private:
    enum class Direction { undefined, up, down };
    enum class Step { undefined, rise, fall };
    void updateBumpCount(int bumpBoundary);
    void update(Direction, int bumpBoundary, const std::vector<int> &,
        void (LevittTrack::*)());
    void callIfConsecutiveCountMet(void (LevittTrack::*)(), int threshold);
    auto consecutiveCountMet(int threshold) -> bool;
    void stepDown();
    void stepUp();
    auto stepSize() -> int;
    void updateConsecutiveCount(Direction);
    void updateReversals(Step);
    void reversal();
    auto complete_() const -> bool;

    std::vector<int> runCounts;
    std::vector<int> stepSizes;
    std::vector<int> up_;
    std::vector<int> down_;
    std::vector<int> reversalX;
    std::size_t sequenceIndex{};
    int startingX_;
    int x_;
    int ceiling_;
    int floor_;
    int bumpLimit_;
    int bumpCount_;
    int sameDirectionConsecutiveCount{};
    int runCounter{};
    int reversals_{};
    int thresholdReversals{};
    Direction previousDirection{Direction::undefined};
    Step previousStep{Step::undefined};
};
}

#endif
