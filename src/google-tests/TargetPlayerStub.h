#ifndef TargetPlayerStub_h
#define TargetPlayerStub_h

#include "LogString.h"
#include <recognition-test/RecognitionTestModel.hpp>

namespace av_speech_in_noise::tests {
    class TargetPlayerStub : public TargetPlayer {
        LogString log_;
        std::string filePath_;
        std::string device_;
        double rms_{};
        double level_dB_{};
        double durationSeconds_{};
        EventListener *listener_{};
        bool played_{};
        bool videoHidden_{};
        bool videoShown_{};
        bool throwInvalidAudioDeviceWhenDeviceSet_{};
        bool throwInvalidAudioFileOnRms_{};
        bool setDeviceCalled_{};
        bool playing_{};
        bool playbackCompletionSubscribedTo_{};
    public:
        void subscribeToPlaybackCompletion() override {
            playbackCompletionSubscribedTo_ = true;
        }
        
        bool playing() override {
            return playing_;
        }
        
        auto &log() const {
            return log_;
        }
        
        auto setDeviceCalled() const {
            return setDeviceCalled_;
        }
        
        void throwInvalidAudioDeviceWhenDeviceSet() {
            throwInvalidAudioDeviceWhenDeviceSet_ = true;
        }
        
        void setAudioDevice(std::string s) override {
            setDeviceCalled_ = true;
            device_ = std::move(s);
            if (throwInvalidAudioDeviceWhenDeviceSet_)
                throw InvalidAudioDevice{};
        }
        
        auto device() const {
            return device_;
        }
        
        auto level_dB() const {
            return level_dB_;
        }
        
        void setRms(double x) {
            rms_ = x;
        }
        
        void showVideo() override {
            videoShown_ = true;
        }
        
        auto videoShown() const {
            return videoShown_;
        }
        
        void hideVideo() override {
            videoHidden_ = true;
        }
        
        auto videoHidden() const {
            return videoHidden_;
        }
        
        auto played() const {
            return played_;
        }
        
        void play() override {
            played_ = true;
        }
        
        void subscribe(EventListener *listener) override {
            listener_ = listener;
        }
        
        void loadFile(std::string filePath) override {
            addToLog("loadFile ");
            filePath_ = filePath;
        }
        
        void addToLog(std::string s) {
            log_.insert(std::move(s));
        }
        
        double rms() override {
            addToLog("rms ");
            if (throwInvalidAudioFileOnRms_)
                throw InvalidAudioFile{};
            return rms_;
        }
        
        void setLevel_dB(double x) override {
            level_dB_ = x;
        }
        
        double durationSeconds() override {
            return durationSeconds_;
        }
        
        void playbackComplete() {
            listener_->playbackComplete();
        }
        
        auto listener() const {
            return listener_;
        }
        
        auto filePath() const {
            return filePath_;
        }
        
        auto playbackCompletionSubscribedTo() const {
            return playbackCompletionSubscribedTo_;
        }
        
        void setDurationSeconds(double x) {
            durationSeconds_ = x;
        }
        
        void throwInvalidAudioFileOnRms() {
            throwInvalidAudioFileOnRms_ = true;
        }
    };
}

#endif
