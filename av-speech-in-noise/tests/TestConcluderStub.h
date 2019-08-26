#ifndef TestConcluderStub_h
#define TestConcluderStub_h

#include <recognition-test/RecognitionTestModel.hpp>

namespace av_speech_in_noise::tests {
    class TestConcluderStub : public TestConcluder {
        const FixedLevelTest *test_{};
        TargetList *targetList_{};
        bool complete_{};
        bool responseSubmitted_{};
    public:
        auto targetList() const {
            return targetList_;
        }

        auto responseSubmitted() const {
            return responseSubmitted_;
        }

        auto test() const {
            return test_;
        }

        void setComplete() {
            complete_ = true;
        }

        bool complete(TargetList *t) override {
            targetList_ = t;
            return complete_;
        }

        void initialize(const FixedLevelTest &p) override {
            test_ = &p;
        }

        void submitResponse() override {
            responseSubmitted_ = true;
        }
    };
}

#endif
