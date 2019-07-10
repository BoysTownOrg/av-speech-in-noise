#ifndef MaskerPlayerStub_h
#define MaskerPlayerStub_h

#include "LogString.h"
#include <recognition-test/RecognitionTestModel.hpp>

namespace av_speech_in_noise::tests {
    class MaskerPlayerStub : public MaskerPlayer {
        std::vector<std::string> outputAudioDeviceDescriptions_;
        LogString log_;
        std::string filePath_;
        std::string device_;
        double rms_{};
        double level_dB_{};
        double fadeTimeSeconds_{};
        double durationSeconds_{};
        double secondsSeeked_{};
        EventListener *listener_{};
        bool fadeInCalled_{};
        bool fadeOutCalled_{};
        bool playing_{};
        bool setDeviceCalled_{};
        bool throwInvalidAudioDeviceWhenDeviceSet_{};
        bool throwInvalidAudioFileOnLoad_{};
    public:
        void throwInvalidAudioFileOnLoad() {
            throwInvalidAudioFileOnLoad_ = true;
        }
        
        void fadeOutComplete() {
            listener_->fadeOutComplete();
        }
        
        void throwInvalidAudioDeviceWhenDeviceSet() {
            throwInvalidAudioDeviceWhenDeviceSet_ = true;
        }
        
        void setAudioDevice(std::string s) override {
            device_ = std::move(s);
            if (throwInvalidAudioDeviceWhenDeviceSet_)
                throw av_speech_in_noise::InvalidAudioDevice{};
        }
        
        auto device() const {
            return device_;
        }
        
        auto setDeviceCalled() const {
            return setDeviceCalled_;
        }
        
        bool playing() override {
            return playing_;
        }
        
        void setPlaying() {
            playing_ = true;
        }
        
        void setOutputAudioDeviceDescriptions(std::vector<std::string> v) {
            outputAudioDeviceDescriptions_ = std::move(v);
        }
        
        auto fadeInCalled() const {
            return fadeInCalled_;
        }
        
        void fadeIn() override {
            fadeInCalled_ = true;
        }
        
        auto fadeOutCalled() const {
            return fadeOutCalled_;
        }
        
        void subscribe(EventListener *e) override {
            listener_ = e;
        }
        
        void fadeOut() override {
            fadeOutCalled_ = true;
        }
        
        void loadFile(std::string filePath) override {
            addToLog("loadFile ");
            filePath_ = std::move(filePath);
            if (throwInvalidAudioFileOnLoad_)
                throw InvalidAudioFile{};
        }
        
        void addToLog(std::string s) {
            log_.insert(std::move(s));
        }
        
        std::vector<std::string> outputAudioDeviceDescriptions() override {
            return outputAudioDeviceDescriptions_;
        }
        
        auto listener() const {
            return listener_;
        }
        
        void fadeInComplete() {
            listener_->fadeInComplete();
        }
        
        auto filePath() const {
            return filePath_;
        }
        
        void setRms(double x) {
            rms_ = x;
        }
        
        auto level_dB() const {
            return level_dB_;
        }
        
        double rms() override {
            addToLog("rms ");
            return rms_;
        }
        
        void setLevel_dB(double x) override {
            level_dB_ = x;
        }
        
        double durationSeconds() override {
            return durationSeconds_;
        }
        
        void seekSeconds(double x) override {
            secondsSeeked_ = x;
        }
        
        auto secondsSeeked() const {
            return secondsSeeked_;
        }
        
        double fadeTimeSeconds() override {
            return fadeTimeSeconds_;
        }
        
        void setFadeTimeSeconds(double x) {
            fadeTimeSeconds_ = x;
        }
        
        void setDurationSeconds(double x) {
            durationSeconds_ = x;
        }
    };
}
#endif
