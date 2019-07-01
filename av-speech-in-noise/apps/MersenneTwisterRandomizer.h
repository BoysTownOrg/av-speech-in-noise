#ifndef MersenneTwisterRandomizer_h
#define MersenneTwisterRandomizer_h

#include <target-list/RandomizedTargetList.hpp>
#include <recognition-test/RecognitionTestModel.hpp>
#include <random>

class MersenneTwisterRandomizer :
    public target_list::Randomizer,
    public av_speech_in_noise::Randomizer
{
    std::mt19937 engine{std::random_device{}()};
public:
    void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
        std::shuffle(begin, end, engine);
    }
    
    double randomFloatBetween(double a, double b) override {
        std::uniform_real_distribution<> distribution{a, b};
        return distribution(engine);
    }
    
    int randomIntBetween(int a, int b) override {
        std::uniform_int_distribution<> distribution{a, b};
        return distribution(engine);
    }
    
};

#endif
