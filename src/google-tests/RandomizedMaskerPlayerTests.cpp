#include <recognition-test/Model.hpp>

class VideoPlayer {
public:
    virtual ~VideoPlayer() = default;
    virtual bool playing() = 0;
};

class RandomizedMaskerPlayer : public recognition_test::MaskerPlayer {
    VideoPlayer *player;
public:
    RandomizedMaskerPlayer(VideoPlayer *player) : player{player} {}
    
    void subscribe(EventListener *) override {
    
    }
    int deviceCount() override {
        return {};
    }
    std::string deviceDescription(int index) override {
        return {};
    }
    void setDevice(int index) override {
    
    }
    void fadeIn() override {
    
    }
    void fadeOut() override {
    
    }
    void loadFile(std::string filePath) override {
    
    }
    bool playing() override {
        return player->playing();
    }
};

#include <gtest/gtest.h>

class VideoPlayerStub : public VideoPlayer {
    bool playing_{};
public:
    void setPlaying() {
        playing_ = true;
    }
    
    bool playing() override {
        return playing_;
    }
};

class RandomizedMaskerPlayerTests : public ::testing::Test {
protected:
    VideoPlayerStub videoPlayer;
    RandomizedMaskerPlayer player{&videoPlayer};
};

TEST_F(RandomizedMaskerPlayerTests, playingWhenVideoPlayerPlaying) {
    videoPlayer.setPlaying();
    EXPECT_TRUE(player.playing());
}
