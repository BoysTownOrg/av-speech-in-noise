#ifndef target_player_StimulusPlayerImpl_hpp
#define target_player_StimulusPlayerImpl_hpp

#include <recognition-test/RecognitionTestModel.hpp>
#include <gsl/gsl>
#include <vector>

namespace target_player {
    class VideoPlayer {
    public:
        class EventListener {
        public:
            virtual ~EventListener() = default;
            virtual void playbackComplete() = 0;
            virtual void fillAudioBuffer(
                const std::vector<gsl::span<float>> &audio) = 0;
        };
        
        virtual ~VideoPlayer() = default;
        virtual void subscribe(EventListener *) = 0;
        virtual int deviceCount() = 0;
        virtual std::string deviceDescription(int index) = 0;
        virtual void show() = 0;
        virtual void hide() = 0;
        virtual void loadFile(std::string) = 0;
        virtual void play() = 0;
        virtual bool playing() = 0;
        virtual void setDevice(int index) = 0;
        virtual std::vector<std::vector<float>> readAudio(std::string filePath) = 0;
    };
    
    class TargetPlayerImpl :
        public recognition_test::TargetPlayer,
        public VideoPlayer::EventListener
    {
        std::string filePath_{};
        std::atomic<double> audioScale{};
        VideoPlayer *player;
        TargetPlayer::EventListener *listener_{};
    public:
        TargetPlayerImpl(VideoPlayer *);
        void subscribe(TargetPlayer::EventListener *) override;
        void play() override;
        void loadFile(std::string filePath) override;
        void hideVideo() override;
        void showVideo() override;
        double rms() override;
        void setLevel_dB(double) override;
        void setAudioDevice(std::string) override;
        bool playing() override;
        void playbackComplete() override;
        void fillAudioBuffer(
            const std::vector<gsl::span<float> > &audio) override;
        std::vector<std::string> audioDevices();
    };
}

#endif
