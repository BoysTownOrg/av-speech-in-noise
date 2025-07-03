#ifndef AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_ADAPTIVETRACKHPP_
#define AV_SPEECH_IN_NOISE_LIB_CORE_INCLUDE_AVSPEECHINNOISE_CORE_ADAPTIVETRACKHPP_

#include "AdaptiveMethod.hpp"
#include <vector>
#include <memory>

namespace adaptive_track {
class AdaptiveTrack : public av_speech_in_noise::Track {
  public:
    explicit AdaptiveTrack(const Settings &);
    auto x() -> double override;
    void up() override;
    void down() override;
    auto complete() -> bool override;
    auto reversals() -> int override;
    void reset() override;
    auto threshold(int reversals) -> double override;
    auto formatResult() -> std::string override { return ""; }

    class Factory : public Track::Factory {
        auto make(const Settings &s) -> std::shared_ptr<Track> override {
            return std::make_shared<AdaptiveTrack>(s);
        }
    };

  private:
    enum class Direction { undefined, up, down };
    enum class Step { undefined, rise, fall };
    void updateBumpCount(int bumpBoundary);
    void update(Direction, int bumpBoundary, const std::vector<int> &,
        void (AdaptiveTrack::*)());
    void callIfConsecutiveCountMet(void (AdaptiveTrack::*)(), int threshold);
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
    Direction previousDirection{Direction::undefined};
    Step previousStep{Step::undefined};
};
}

#endif
