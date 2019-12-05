#ifndef AV_SPEECH_IN_NOISE_ADAPTIVE_TRACK_INCLUDE_ADAPTIVE_TRACK_ADAPTIVETRACK_HPP_
#define AV_SPEECH_IN_NOISE_ADAPTIVE_TRACK_INCLUDE_ADAPTIVE_TRACK_ADAPTIVETRACK_HPP_

#include <recognition-test/AdaptiveMethod.hpp>
#include <vector>
#include <memory>

namespace adaptive_track {
class AdaptiveTrack : public av_speech_in_noise::Track {
    enum class Direction { undefined, up, down };
    enum class Step { undefined, rise, fall };
    std::vector<int> runCounts;
    std::vector<int> stepSizes;
    std::vector<int> up_;
    std::vector<int> down_;
    std::size_t sequenceIndex{};
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

  public:
    explicit AdaptiveTrack(const Settings &);
    int x() override;
    void up() override;
    void down() override;
    bool complete() override;
    int reversals() override;

    class Factory : public Track::Factory {
        std::shared_ptr<av_speech_in_noise::Track> make(
            const av_speech_in_noise::Track::Settings &s) override {
            return std::make_shared<AdaptiveTrack>(s);
        }
    };

  private:
    void updateBumpCount(int bumpBoundary);
    void update(Direction, int bumpBoundary, const std::vector<int> &,
        void (AdaptiveTrack::*)());
    void callIfConsecutiveCountMet(void (AdaptiveTrack::*)(), int threshold);
    bool consecutiveCountMet(int threshold);
    void stepDown();
    void stepUp();
    int stepSize();
    void updateConsecutiveCount(Direction);
    void updateReversals(Step);
    void reversal();
    bool complete_() const;
};
}

#endif
