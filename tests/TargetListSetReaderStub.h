#ifndef TESTS_TARGETLISTSETREADERSTUB_H_
#define TESTS_TARGETLISTSETREADERSTUB_H_

#include <recognition-test/AdaptiveMethod.hpp>
#include <utility>

namespace av_speech_in_noise::tests {
class TargetListSetReaderStub : public TargetListReader {
    lists_type targetLists_{};
    std::string directory_{};

  public:
    void setTargetLists(lists_type lists) { targetLists_ = std::move(lists); }

    lists_type read(std::string d) override {
        directory_ = std::move(d);
        return targetLists_;
    }

    [[nodiscard]] auto directory() const { return directory_; }
};
}

#endif
