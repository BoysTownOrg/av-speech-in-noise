#ifndef masker_player_RandomizedMaskerPlayer_hpp
#define masker_player_RandomizedMaskerPlayer_hpp

#include <recognition-test/RecognitionTestModel.hpp>
#include <gsl/gsl>

namespace masker_player {
    class AudioPlayer {
    public:
        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void fillAudioBuffer(
                const std::vector<gsl::span<float>> &audio) = 0;
            virtual void timerCallback() = 0;
        };
        
        virtual ~AudioPlayer() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual bool playing() = 0;
        virtual void loadFile(std::string) = 0;
        virtual void setDevice(int index) = 0;
        virtual int deviceCount() = 0;
        virtual std::string deviceDescription(int index) = 0;
        virtual void play() = 0;
        virtual double sampleRateHz() = 0;
        virtual void stop() = 0;
        virtual void scheduleCallbackAfterSeconds(double) = 0;
    };

    class RandomizedMaskerPlayer :
        public recognition_test::MaskerPlayer,
        public AudioPlayer::EventListener
    {
        std::atomic<double> audioScale{1};
        std::atomic<double> fadeInOutSeconds{};
        int hannCounter{};
        int halfWindowLength{};
        AudioPlayer *player;
        MaskerPlayer::EventListener *listener{};
        bool fadingOut{};
        bool fadingIn{};
        std::atomic<bool> fadeOutComplete{};
        std::atomic<bool> fadeInComplete{};
        std::atomic<bool> pleaseFadeOut{};
        std::atomic<bool> pleaseFadeIn{};
    public:
        RandomizedMaskerPlayer(AudioPlayer *);
        void subscribe(MaskerPlayer::EventListener *) override;
        void fadeIn() override;
        void fadeOut() override;
        void loadFile(std::string) override;
        bool playing() override;
        void setAudioDevice(std::string) override;
        void setLevel_dB(double);
        void fillAudioBuffer(
            const std::vector<gsl::span<float>> &audio) override;
        void setFadeInOutSeconds(double);
        std::vector<std::string> audioDeviceDescriptions() override;
        void timerCallback() override;
    private:
        void checkForFadeIn();
        void checkForFadeOut();
        int levelTransitionSamples();
        void scaleAudio(const std::vector<gsl::span<float>> &);
        void checkForFadeInComplete();
        void checkForFadeOutComplete();
        void updateState();
        double transitionScale();
        double nextScale();
    };
}

#endif
