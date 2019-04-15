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
        first = true;
    }
    
    bool RandomizedTargetList::empty() {
        return files.empty();
    }
    
    std::string RandomizedTargetList::next() {
        auto next_ = files.front();
        files.erase(files.begin());
        if (!first)
            files.push_back(current_);
        first = false;
        randomizer->shuffle(files.begin(), files.end());
        current_ = next_;
        return directory_ + "/" + next_;
    }
    
    std::string RandomizedTargetList::current() { 
        return directory_ + "/" + current_;
    }
    
}
