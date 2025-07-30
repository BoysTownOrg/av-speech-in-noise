#ifndef TESTS_TARGETLISTSTUB_HPP_
#define TESTS_TARGETLISTSTUB_HPP_

#include "LogString.hpp"

#include <av-speech-in-noise/core/TargetPlaylist.hpp>

#include <utility>

namespace av_speech_in_noise {
class TargetPlaylistStub : public virtual TargetPlaylist {
  public:
    auto current() -> LocalUrl override { return {current_}; }

    void setCurrent(std::string s) { current_ = std::move(s); }

    void setCurrentTargetWhenNext(std::string s) {
        currentWhenNext_ = std::move(s);
    }

    void load(const LocalUrl &directory) override {
        insert(log_, "loadFromDirectory ");
        directory_ = directory.path;
    }

    auto next() -> LocalUrl override {
        insert(log_, "next ");
        nextCalled_ = true;
        current_ = currentWhenNext_;
        return {next_};
    }

    void setNext(std::string s) { next_ = std::move(s); }

    auto directory() -> LocalUrl override { return {directory_}; }

    void setDirectory(std::string s) { directory_ = std::move(s); }

    auto log() const -> const std::stringstream & { return log_; }

  protected:
    std::stringstream log_{};
    std::string currentWhenNext_{};
    std::string directory_{};
    std::string next_{};
    std::string current_{};
    bool nextCalled_{};
};

class FiniteTargetPlaylistStub : public virtual FiniteTargetPlaylist,
                                 public TargetPlaylistStub {
  public:
    void setEmpty() { empty_ = true; }

    auto empty() -> bool override {
        insert(log_, "empty ");
        return empty_;
    }

  private:
    bool empty_{};
};

class FiniteTargetPlaylistWithRepeatablesStub
    : public virtual FiniteTargetPlaylistWithRepeatables,
      public FiniteTargetPlaylistStub {
  public:
    void reinsertCurrent() override {
        insert(log_, "reinsertCurrent ");
        reinsertCurrentCalled_ = true;
    }

    auto reinsertCurrentCalled() const -> bool {
        return reinsertCurrentCalled_;
    }

  private:
    bool reinsertCurrentCalled_{};
};

class FailingTargetPlaylistStub : public virtual TargetPlaylist {
  public:
    auto current() -> LocalUrl override { return {}; }

    void load(const LocalUrl &) override { throw LoadFailure{}; }

    auto next() -> LocalUrl override { return {}; }

    auto directory() -> LocalUrl override { return {}; }
};
}

#endif
