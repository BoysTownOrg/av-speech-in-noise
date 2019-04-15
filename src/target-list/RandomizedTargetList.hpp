#ifndef target_list_RandomizedStimulusList_hpp
#define target_list_RandomizedStimulusList_hpp

#include <recognition-test/RecognitionTestModel.hpp>
#include <vector>
#include <string>

namespace target_list {
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

    class RandomizedTargetList : public av_coordinate_response_measure::TargetList {
        std::vector<std::string> files{};
        std::string directory_{};
        std::string currentFile_{};
        DirectoryReader *reader;
        Randomizer *randomizer;
        bool first{};
    public:
        RandomizedTargetList(DirectoryReader *, Randomizer *);
        void loadFromDirectory(std::string) override;
        bool empty();
        std::string current_();
        std::string next() override;
        std::string current() override;
    };
}

#endif
