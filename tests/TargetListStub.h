#ifndef TESTS_TARGETLISTSTUB_H_
#define TESTS_TARGETLISTSTUB_H_

#include "LogString.h"
#include <recognition-test/Model.hpp>

namespace av_speech_in_noise::tests {
class TargetListStub : public TargetList {
    LogString log_{};
    std::string currentWhenNext_{};
    std::string directory_{};
    std::string next_{};
    std::string current_{};
    bool nextCalled_{};
    bool empty_{};

  public:
    void setEmpty() { empty_ = true; }

    bool empty() override { return empty_; }

    std::string current() override { return current_; }

    void setCurrent(std::string s) { current_ = std::move(s); }

    void setCurrentTargetWhenNext(std::string s) {
        currentWhenNext_ = std::move(s);
    }

    void loadFromDirectory(std::string directory) override {
        log_.insert("loadFromDirectory ");
        directory_ = std::move(directory);
    }

    std::string next() override {
        log_.insert("next ");
        nextCalled_ = true;
        current_ = currentWhenNext_;
        return next_;
    }

    void setNext(std::string s) { next_ = std::move(s); }

    auto directory() { return directory_; }

    auto &log() const { return log_; }
};
} // namespace av_speech_in_noise::tests

#endif
