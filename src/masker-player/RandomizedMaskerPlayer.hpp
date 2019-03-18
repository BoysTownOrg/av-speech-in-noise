#ifndef RandomizedMaskerPlayer_hpp
#define RandomizedMaskerPlayer_hpp

#include <recognition-test/Model.hpp>
#include <gsl/gsl>
#include <cmath>

namespace masker_player {
    class VideoPlayer {
    public:
        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) = 0;
        };
        
        virtual ~VideoPlayer() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual bool playing() = 0;
        virtual void loadFile(std::string) = 0;
        virtual void setDevice(int index) = 0;
        virtual int deviceCount() = 0;
        virtual std::string deviceDescription(int index) = 0;
        virtual void play() = 0;
        virtual double sampleRateHz() = 0;
    };

    class RandomizedMaskerPlayer :
        public recognition_test::MaskerPlayer,
        public VideoPlayer::EventListener
    {
        double audioScale{};
        int levelTransitionSamples{};
        int hannCounter{};
        VideoPlayer *player;
        bool smoothingMasker{};
    public:
        RandomizedMaskerPlayer(VideoPlayer *);
        void subscribe(MaskerPlayer::EventListener *) override;
        int deviceCount() override;
        std::string deviceDescription(int index) override;
        void setDevice(int index) override;
        void fadeIn() override;
        void fadeOut() override;
        void loadFile(std::string) override;
        bool playing() override;
        void setLevel_dB(double);
        void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) override;
        void setFadeInSeconds(double);
    private:
        double getSmoothingScalar();
    };
}


#endif
