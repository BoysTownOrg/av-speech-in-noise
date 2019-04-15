#include "RandomizedTargetList.hpp"

namespace target_list {
    RandomizedTargetList::RandomizedTargetList(
        DirectoryReader *reader,
        Randomizer *randomizer
    ) :
        reader{reader},
        randomizer{randomizer} {}
    
    void RandomizedTargetList::loadFromDirectory(std::string directory) {
        directory_ = std::move(directory);
        files = reader->filesIn(directory_);
        randomizer->shuffle(files.begin(), files.end());
    }
    
    bool RandomizedTargetList::empty() {
        return files.empty();
    }
    
    std::string RandomizedTargetList::next() {
        auto next_ = files.front();
        files.erase(files.begin());
        randomizer->shuffle(files.begin(), files.end());
        return directory_ + "/" + next_;
    }
    
    std::string RandomizedTargetList::current() { 
        return {};
    }
    
}
