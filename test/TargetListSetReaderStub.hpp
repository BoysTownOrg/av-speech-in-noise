#ifndef TESTS_TARGETLISTSETREADERSTUB_HPP_
#define TESTS_TARGETLISTSETREADERSTUB_HPP_

#include <recognition-test/AdaptiveMethod.hpp>
#include <utility>

namespace av_speech_in_noise {
class TargetListSetReaderStub : public TargetListReader {
    lists_type targetLists_{};
    std::string directory_{};

  public:
    void setTargetLists(lists_type lists) { targetLists_ = std::move(lists); }

    auto read(const LocalUrl &d) -> lists_type override {
        directory_ = d.path;
        return targetLists_;
    }

    [[nodiscard]] auto directory() const { return directory_; }
};
}

#endif
