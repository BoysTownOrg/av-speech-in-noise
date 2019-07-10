#ifndef TargetListStub_h
#define TargetListStub_h

#include "LogString.h"
#include <recognition-test/RecognitionTestModel.hpp>

namespace av_speech_in_noise::tests {
    class TargetListStub : public TargetList {
        LogString log_{};
        std::string currentWhenNext_{};
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
        
        void setNext(std::string s) {
            next_ = std::move(s);
        }
        
        auto directory() {
            return directory_;
        }
        
        auto &log() const {
            return log_;
        }
    };
}
#endif
