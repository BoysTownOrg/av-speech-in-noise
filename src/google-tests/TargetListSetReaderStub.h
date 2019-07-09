#ifndef TargetListSetReaderStub_h
#define TargetListSetReaderStub_h

#include <recognition-test/RecognitionTestModel.hpp>

namespace av_speech_in_noise::tests {
    class TargetListSetReaderStub : public TargetListReader {
        lists_type targetLists_{};
        std::string directory_{};
    public:
        void setTargetLists(lists_type lists) {
            targetLists_ = std::move(lists);
        }
        
        lists_type read(std::string d) override {
            directory_ = std::move(d);
            return targetLists_;
        }
        
        auto directory() const {
            return directory_;
        }
    };
}
#endif
