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
    };

    class RandomizedMaskerPlayer :
        public recognition_test::MaskerPlayer,
        public VideoPlayer::EventListener
    {
        double audioScale{};
        VideoPlayer *player;
    public:
        RandomizedMaskerPlayer(VideoPlayer *player) : player{player}
        {
            player->subscribe(this);
        }
        
        void subscribe(MaskerPlayer::EventListener *) override {
        
        }
        
        int deviceCount() override {
            return player->deviceCount();
        }
        
        std::string deviceDescription(int index) override {
            return player->deviceDescription(index);
        }
        
        void setDevice(int index) override {
            player->setDevice(index);
        }
        
        void fadeIn() override {
            player->play();
        }
        
        void fadeOut() override {
        
        }
        
        void loadFile(std::string filePath) override {
            player->loadFile(std::move(filePath));
        }
        
        bool playing() override {
            return player->playing();
        }
        
        void setLevel_dB(double x) {
            audioScale = std::pow(10, x/20);
        }
        
        void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) override {
            for (auto channel : audio)
                for (auto &x : channel)
                    x *= audioScale;
        }
    };
}


#endif
