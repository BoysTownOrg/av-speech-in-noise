#ifndef TESTS_TESTCONCLUDERSTUB_H_
#define TESTS_TESTCONCLUDERSTUB_H_

#include "LogString.h"
#include <recognition-test/Model.hpp>

namespace av_speech_in_noise::tests {
class TestConcluderStub : public TestConcluder {
    LogString log_;
    const FixedLevelTest *test_{};
    TargetList *targetList_{};
    bool complete_{};
    bool responseSubmitted_{};

  public:
    const auto &log() const {
        return log_;
    }

    auto targetList() const { return targetList_; }

    auto responseSubmitted() const { return responseSubmitted_; }

    auto test() const { return test_; }

    void setComplete() { complete_ = true; }

    bool complete(TargetList *t) override {
        log_.insert("complete ");
        targetList_ = t;
        return complete_;
    }

    void initialize(const FixedLevelTest &p) override { test_ = &p; }

    void submitResponse() override {
        log_.insert("submitResponse ");
        responseSubmitted_ = true;
    }
};
}

#endif
