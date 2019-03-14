#include <recognition-test/Model.hpp>

class VideoPlayer {
public:
    virtual ~VideoPlayer() = default;
    virtual bool playing() = 0;
    virtual void loadFile(std::string) = 0;
    virtual void setDevice(int index) = 0;
    virtual int deviceCount() = 0;
};

class RandomizedMaskerPlayer : public recognition_test::MaskerPlayer {
    VideoPlayer *player;
public:
    RandomizedMaskerPlayer(VideoPlayer *player) : player{player} {}
    
    void subscribe(EventListener *) override {
    
    }
    int deviceCount() override {
        return player->deviceCount();
    }
    std::string deviceDescription(int index) override {
        return {};
    }
    void setDevice(int index) override {
        player->setDevice(index);
    }
    void fadeIn() override {
    
    }
    void fadeOut() override {
    
    }
    void loadFile(std::string filePath) override {
        player->loadFile(std::move(filePath));
    }
    bool playing() override {
        return player->playing();
    }
};

#include "assert-utility.h"
#include <gtest/gtest.h>

class VideoPlayerStub : public VideoPlayer {
    std::string filePath_{};
    int deviceIndex_{};
    int deviceCount_{};
    bool playing_{};
public:
    void setPlaying() {
        playing_ = true;
    }
    
    bool playing() override {
        return playing_;
    }
    
    void loadFile(std::string s) override {
        filePath_ = std::move(s);
    }
    
    void setDevice(int index) override {
        deviceIndex_ = index;
    }
    
    int deviceCount() override {
        return deviceCount_;
    }
    
    auto filePath() const {
        return filePath_;
    }
    
    auto deviceIndex() const {
        return deviceIndex_;
    }
    
    void setDeviceCount(int x) {
        deviceCount_ = x;
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

TEST_F(RandomizedMaskerPlayerTests, loadFileLoadsVideoFile) {
    player.loadFile("a");
    assertEqual("a", videoPlayer.filePath());
}

TEST_F(RandomizedMaskerPlayerTests, setDeviceSetsDevice) {
    player.setDevice(1);
    EXPECT_EQ(1, videoPlayer.deviceIndex());
}

TEST_F(RandomizedMaskerPlayerTests, returnsVideoPlayerDeviceCount) {
    videoPlayer.setDeviceCount(1);
    EXPECT_EQ(1, player.deviceCount());
}
