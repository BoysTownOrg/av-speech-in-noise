#include <recognition-test/Model.hpp>
#include <gsl/gsl>
#include <cmath>

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

#include "assert-utility.h"
#include <gtest/gtest.h>

namespace {
    class VideoPlayerStub : public VideoPlayer {
        std::string filePath_{};
        std::string deviceDescription_{};
        int deviceIndex_{};
        int deviceCount_{};
        int deviceDescriptionDeviceIndex_{};
        EventListener *listener_{};
        bool playing_{};
        bool played_{};
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
        
        std::string deviceDescription(int index) override {
            deviceDescriptionDeviceIndex_ = index;
            return deviceDescription_;
        }
        
        void play() override {
            played_ = true;
        }
        
        void subscribe(EventListener * listener) override {
            listener_ = listener;
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
        
        void setDeviceDescription(std::string s) {
            deviceDescription_ = std::move(s);
        }
        
        auto deviceDescriptionDeviceIndex() const {
            return deviceDescriptionDeviceIndex_;
        }
        
        auto played() const {
            return played_;
        }
        
        void fillAudioBuffer(const std::vector<gsl::span<float>> &audio) {
            listener_->fillAudioBuffer(audio);
        }
    };

    class RandomizedMaskerPlayerTests : public ::testing::Test {
    protected:
        std::vector<float> leftChannel{};
        VideoPlayerStub videoPlayer;
        RandomizedMaskerPlayer player{&videoPlayer};
        
        void fillAudioBuffer() {
            std::vector<gsl::span<float>> audio{ leftChannel };
            videoPlayer.fillAudioBuffer(audio);
        }
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

    TEST_F(RandomizedMaskerPlayerTests, returnsVideoPlayerDeviceDescription) {
        videoPlayer.setDeviceDescription("a");
        assertEqual("a", player.deviceDescription({}));
    }

    TEST_F(RandomizedMaskerPlayerTests, passesDeviceIndexToDeviceDescription) {
        player.deviceDescription(1);
        EXPECT_EQ(1, videoPlayer.deviceDescriptionDeviceIndex());
    }

    TEST_F(RandomizedMaskerPlayerTests, fadeInPlaysVideoPlayer) {
        player.fadeIn();
        EXPECT_TRUE(videoPlayer.played());
    }

    TEST_F(RandomizedMaskerPlayerTests, twentydBMultipliesSignalByTen) {
        player.setLevel_dB(20);
        leftChannel = { 1, 2, 3 };
        fillAudioBuffer();
        assertEqual({ 10, 20, 30 }, leftChannel);
    }
}
