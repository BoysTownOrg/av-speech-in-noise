#ifndef TargetListStub_h
#define TargetListStub_h

#include <recognition-test/RecognitionTestModel.hpp>

namespace av_speech_in_noise::tests {
    class TargetListStub : public TargetList {
        std::string directory_{};
        std::string next_{};
        std::string current_{};
        bool nextCalled_{};
    public:
        std::string current() override {
            return current_;
        }
        
        void setCurrent(std::string s) {
            current_ = std::move(s);
        }
        
        void loadFromDirectory(std::string directory) override {
            directory_ = std::move(directory);
        }
        
        std::string next() override {
            nextCalled_ = true;
            return next_;
        }
        
        void setNext(std::string s) {
            next_ = std::move(s);
        }
        
        auto directory() {
            return directory_;
        }
    };
}
#endif
