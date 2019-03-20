#include <recognition-test/Model.hpp>

namespace stimulus_player {
    class StimulusPlayerImpl : public recognition_test::StimulusPlayer {
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
        
        }
        
        double rms() override {
            return 0;
        }
        
        void setLevel_dB(double) override {
        
        }
        
    };
}

#include <gtest/gtest.h>

class StimulusPlayerTests : public ::testing::Test {
protected:
    stimulus_player::StimulusPlayerImpl player;
};
