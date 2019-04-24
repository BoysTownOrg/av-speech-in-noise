#ifndef players_RandomizedMaskerPlayer_hpp
#define players_RandomizedMaskerPlayer_hpp

#include "AudioReader.h"
#include <recognition-test/RecognitionTestModel.hpp>
#include <gsl/gsl>

namespace stimulus_players {
    class AudioPlayer {
    public:
        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void fillAudioBuffer(
                const std::vector<gsl::span<float>> &audio) = 0;
        };
        
        virtual ~AudioPlayer() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual void play() = 0;
        virtual void stop() = 0;
        virtual bool playing() = 0;
        virtual void loadFile(std::string) = 0;
        virtual int deviceCount() = 0;
        virtual std::string deviceDescription(int index) = 0;
        virtual bool outputDevice(int index) = 0;
        virtual void setDevice(int index) = 0;
        virtual double sampleRateHz() = 0;
        virtual double durationSeconds() = 0;
        virtual void seekSeconds(double) = 0;
    };
    
    class Timer {
    public:
        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void callback() = 0;
        };
        virtual ~Timer() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual void scheduleCallbackAfterSeconds(double) = 0;
    };

    class MaskerPlayerImpl :
        public av_coordinate_response_measure::MaskerPlayer,
        public AudioPlayer::EventListener,
        public Timer::EventListener
    {
        std::string filePath_{};
        std::atomic<double> levelScalar{1};
        std::atomic<double> fadeInOutSeconds{};
        int hannCounter{};
        int halfWindowLength{};
        AudioPlayer *player;
        AudioReader *reader;
        MaskerPlayer::EventListener *listener{};
        Timer *timer{};
        bool fadingOut_realTime{};
        bool fadingIn_realTime{};
        bool fadingIn_lowPriority{};
        bool fadingOut_lowPriority{};
        std::atomic<bool> fadeOutComplete{};
        std::atomic<bool> fadeInComplete{};
        std::atomic<bool> pleaseFadeOut{};
        std::atomic<bool> pleaseFadeIn{};
    public:
        class AudioBufferFiller : public AudioPlayer::EventListener {
            void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) override;
        };
        
        class TimerImpl : public Timer::EventListener {
            void callback() override;
        };
        
        MaskerPlayerImpl(
            AudioPlayer *,
            AudioReader *,
            Timer *
        );
        void subscribe(MaskerPlayer::EventListener *) override;
        void fadeIn() override;
        void fadeOut() override;
        void loadFile(std::string) override;
        bool playing() override;
        void setAudioDevice(std::string) override;
        void setLevel_dB(double) override;
        void fillAudioBuffer(
            const std::vector<gsl::span<float>> &audio) override;
        void setFadeInOutSeconds(double);
        std::vector<std::string> outputAudioDeviceDescriptions() override;
        double rms() override;
        double durationSeconds() override;
        void seekSeconds(double) override;
        double fadeTimeSeconds() override;
        void callback() override;
    private:
        bool fading();
        std::vector<std::vector<float>> readAudio_();
        std::vector<std::string> audioDeviceDescriptions_();
        int findDeviceIndex(const std::string &device);
        void updateWindowLength();
        void prepareToFadeIn();
        void checkForFadeIn();
        void prepareToFadeOut();
        void checkForFadeOut();
        int levelTransitionSamples();
        void scaleAudio(const std::vector<gsl::span<float>> &);
        bool doneFadingIn();
        void checkForFadeInComplete();
        bool doneFadingOut();
        void checkForFadeOutComplete();
        void advanceCounterIfStillFading();
        void updateFadeState();
        double fadeScalar();
    };
}

#endif
