#include <recognition-test/Model.hpp>

namespace stimulus_player {
    class VideoPlayer {
    public:
        virtual ~VideoPlayer() = default;
        virtual void show() = 0;
    };
    
    class StimulusPlayerImpl : public recognition_test::StimulusPlayer {
        VideoPlayer *player;
    public:
        StimulusPlayerImpl(VideoPlayer *player) : player{player} {}
        
        void subscribe(EventListener *) override {
        
        }
        
        void setDevice(int index) override {
        
        }
        
        void play() override {
        
        }
        
        void loadFile(std::string filePath) override {
        
        }
        
        void hideVideo() override {
        
        }
        
        void showVideo() override {
            player->show();
        }
        
        double rms() override {
            return 0;
        }
        
        void setLevel_dB(double) override {
        
        }
    };
}

#include <gtest/gtest.h>

class VideoPlayerStub : public stimulus_player::VideoPlayer {
    bool shown_{};
public:
    auto shown() const {
        return shown_;
    }
    
    void show() override {
        shown_ = true;
    }
};

class StimulusPlayerTests : public ::testing::Test {
protected:
    VideoPlayerStub videoPlayer;
    stimulus_player::StimulusPlayerImpl player{&videoPlayer};
};

TEST_F(StimulusPlayerTests, showVideoShowsVideo) {
    player.showVideo();
    EXPECT_TRUE(videoPlayer.shown());
}
