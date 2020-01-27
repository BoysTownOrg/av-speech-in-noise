#ifndef TESTS_TESTCONCLUDERSTUB_HPP_
#define TESTS_TESTCONCLUDERSTUB_HPP_

#include "LogString.hpp"
#include <recognition-test/Model.hpp>

namespace av_speech_in_noise {
class TestConcluderStub : public TestConcluder {
    LogString log_;
    const FixedLevelTest *test_{};
    TargetList *targetList_{};
    bool complete_{};
    bool responseSubmitted_{};

  public:
    auto log() const -> const auto & { return log_; }

    auto targetList() const { return targetList_; }

    auto responseSubmitted() const { return responseSubmitted_; }

    auto test() const { return test_; }

    void setComplete() { complete_ = true; }

    void setIncomplete() { complete_ = false; }

    auto complete(TargetList *t) -> bool override {
        log_.insert("complete ");
        targetList_ = t;
        return complete_;
    }

    void initialize(const FixedLevelTest &p) override {
        log_.insert("initialize ");
        test_ = &p;
    }

    void submitResponse() override {
        log_.insert("submitResponse ");
        responseSubmitted_ = true;
    }
};
}

#endif
