#ifndef TESTS_TARGETLISTSTUB_HPP_
#define TESTS_TARGETLISTSTUB_HPP_

#include "LogString.hpp"
#include <recognition-test/Model.hpp>
#include <utility>

namespace av_speech_in_noise {
class TargetPlaylistStub : public virtual TargetPlaylist {
  public:
    auto current() -> LocalUrl override { return {current_}; }

    void setCurrent(std::string s) { current_ = std::move(s); }

    void setCurrentTargetWhenNext(std::string s) {
        currentWhenNext_ = std::move(s);
    }

    void loadFromDirectory(const LocalUrl &directory) override {
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

  private:
    std::stringstream log_{};
    std::string currentWhenNext_{};
    std::string directory_{};
    std::string next_{};
    std::string current_{};
    bool nextCalled_{};
};

class FiniteTargetPlaylistStub : public FiniteTargetPlaylist, public TargetPlaylistStub {
  public:
    void setEmpty() { empty_ = true; }

    auto empty() -> bool override { return empty_; }

    void reinsertCurrent() override { reinsertCurrentCalled_ = true; }

    auto reinsertCurrentCalled() const { return reinsertCurrentCalled_; }

  private:
    bool empty_{};
    bool reinsertCurrentCalled_{};
};
}

#endif
