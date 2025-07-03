#ifndef TESTS_TRACKSTUB_HPP_
#define TESTS_TRACKSTUB_HPP_

#include <av-speech-in-noise/core/IAdaptiveMethod.hpp>
#include <utility>
#include <vector>

namespace av_speech_in_noise {
class TrackStub : public Track {
    Settings settings_;
    double thresholdWhenUpdated_{};
    double threshold_{};
    int x_{};
    int reversals_{};
    int reversalsWhenUpdated_{};
    int xWhenUpdated_{};
    int thresholdReversals_{};
    bool pushedDown_{};
    bool pushedUp_{};
    bool complete_{};
    bool resetted_{};
    bool incompleteOnReset_{};

  public:
    void setXWhenUpdated(int x) { xWhenUpdated_ = x; }

    void setReversalsWhenUpdated(int x) { reversalsWhenUpdated_ = x; }

    void setThresholdWhenUpdated(double x) { thresholdWhenUpdated_ = x; }

    void setThreshold(double x) { threshold_ = x; }

    void setReversals(int x) { reversals_ = x; }

    [[nodiscard]] auto pushedUp() const { return pushedUp_; }

    [[nodiscard]] auto pushedDown() const { return pushedDown_; }

    void setX(int x) { x_ = x; }

    void down() override {
        pushedDown_ = true;
        reversals_ = reversalsWhenUpdated_;
        x_ = xWhenUpdated_;
        threshold_ = thresholdWhenUpdated_;
    }

    void up() override {
        pushedUp_ = true;
        reversals_ = reversalsWhenUpdated_;
        x_ = xWhenUpdated_;
        threshold_ = thresholdWhenUpdated_;
    }

    auto x() -> double override { return x_; }

    auto complete() -> bool override { return complete_; }

    auto reversals() -> int override { return reversals_; }

    void setComplete() { complete_ = true; }

    void reset() override {
        resetted_ = true;
        if (incompleteOnReset_)
            complete_ = false;
    }

    void incompleteOnReset() { incompleteOnReset_ = true; }

    [[nodiscard]] auto resetted() const -> bool { return resetted_; }

    auto threshold(int reversals) -> double override {
        thresholdReversals_ = reversals;
        return threshold_;
    }

    [[nodiscard]] auto thresholdReversals() const -> int {
        return thresholdReversals_;
    }
};

class TrackFactoryStub : public Track::Factory {
  public:
    [[nodiscard]] auto parameters() const -> auto & { return parameters_; }

    auto make(const Track::Settings &s) -> std::shared_ptr<Track> override {
        parameters_.push_back(s);
        auto track = tracks_.front();
        tracks_.erase(tracks_.begin());
        return track;
    }

    void setTracks(std::vector<std::shared_ptr<Track>> t) {
        tracks_ = std::move(t);
    }

  private:
    std::vector<Track::Settings> parameters_;
    std::vector<std::shared_ptr<Track>> tracks_;
};
}

#endif
