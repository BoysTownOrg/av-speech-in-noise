//
//  MersenneTwisterRandomizer.h
//  av-coordinated-response-measure
//
//  Created by Bashford, Seth on 3/12/19.
//

#ifndef MersenneTwisterRandomizer_h
#define MersenneTwisterRandomizer_h

#include <stimulus-list/RandomizedStimulusList.hpp>
#include <random>

class MersenneTwisterRandomizer : public stimulus_list::Randomizer {
    std::mt19937 engine{std::random_device{}()};
public:
    void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
        std::shuffle(begin, end, engine);
    }
};

#endif /* MersenneTwisterRandomizer_h */
