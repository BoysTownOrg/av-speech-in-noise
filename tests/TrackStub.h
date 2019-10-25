#ifndef TESTS_TRACKSTUB_H_
#define TESTS_TRACKSTUB_H_

#include <recognition-test/AdaptiveMethod.hpp>
#include <utility>
#include <vector>

namespace av_speech_in_noise::tests {
class TrackStub : public Track {
    Settings settings_;
    int x_{};
    int reversals_{};
    int reversalsWhenUpdated_{};
    int xWhenUpdated_{};
    bool pushedDown_{};
    bool pushedUp_{};
    bool complete_{};

  public:
    void setXWhenUpdated(int x) { xWhenUpdated_ = x; }

    void setReversalsWhenUpdated(int x) { reversalsWhenUpdated_ = x; }

    void setReversals(int x) { reversals_ = x; }

    auto pushedUp() const { return pushedUp_; }

    auto pushedDown() const { return pushedDown_; }

    void setX(int x) { x_ = x; }

    void down() override {
        pushedDown_ = true;
        reversals_ = reversalsWhenUpdated_;
        x_ = xWhenUpdated_;
    }

    void up() override {
        pushedUp_ = true;
        reversals_ = reversalsWhenUpdated_;
        x_ = xWhenUpdated_;
    }

    int x() override { return x_; }

    bool complete() override { return complete_; }

    int reversals() override { return reversals_; }

    void setComplete() { complete_ = true; }
};

class TrackFactoryStub : public TrackFactory {
    std::vector<Track::Settings> parameters_;
    std::vector<std::shared_ptr<Track>> tracks_;

  public:
    auto &parameters() const { return parameters_; }

    std::shared_ptr<Track> make(const Track::Settings &s) override {
        parameters_.push_back(s);
        auto track = tracks_.front();
        tracks_.erase(tracks_.begin());
        return track;
    }

    void setTracks(std::vector<std::shared_ptr<Track>> t) {
        tracks_ = std::move(t);
    }
};
} // namespace av_speech_in_noise::tests

#endif
