#include <recognition-test/Model.hpp>

class RandomizedMaskerPlayer : public recognition_test::MaskerPlayer {
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
        return {};
    }
};

#include <gtest/gtest.h>

class RandomizedMaskerPlayerTests : public ::testing::Test {
protected:
    RandomizedMaskerPlayer player;
};

TEST_F(RandomizedMaskerPlayerTests, tbd) {

}
