#ifndef stimulus_list_RandomizedStimulusList_hpp
#define stimulus_list_RandomizedStimulusList_hpp

#include <recognition-test/Model.hpp>
#include <vector>
#include <string>

namespace stimulus_list {
    class DirectoryReader {
    public:
        virtual ~DirectoryReader() = default;
        virtual std::vector<std::string> filesIn(std::string directory) = 0;
    };
    
    class Randomizer {
    public:
        virtual ~Randomizer() = default;
        using shuffle_iterator = std::vector<std::string>::iterator;
        virtual void shuffle(shuffle_iterator begin, shuffle_iterator end) = 0;
    };
    

    class RandomizedStimulusList : public recognition_test::StimulusList {
        std::vector<std::string> files{};
        std::string directory_{};
        DirectoryReader *reader;
        Randomizer *randomizer;
    public:
        RandomizedStimulusList(DirectoryReader *reader, Randomizer *randomizer);
        void initialize(std::string directory) override;
        bool empty() override;
        std::string next() override;
    };
}



#endif
