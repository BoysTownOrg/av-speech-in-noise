#ifndef MersenneTwisterRandomizer_h
#define MersenneTwisterRandomizer_h

#include <target-list/RandomizedTargetList.hpp>
#include <random>

class MersenneTwisterRandomizer : public target_list::Randomizer {
    std::mt19937 engine{std::random_device{}()};
public:
    void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
        std::shuffle(begin, end, engine);
    }
};

#endif
