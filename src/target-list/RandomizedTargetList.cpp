#include "RandomizedTargetList.hpp"

namespace target_list {
    RandomizedTargetList::RandomizedTargetList(
        DirectoryReader *reader,
        Randomizer *randomizer
    ) :
        reader{reader},
        randomizer{randomizer} {}
    
    void RandomizedTargetList::loadFromDirectory(std::string directory) {
        files = reader->filesIn(directory_ = std::move(directory));
        shuffle();
        noFilesGotten = true;
    }
    
    void RandomizedTargetList::shuffle() {
        randomizer->shuffle(files.begin(), files.end());
    }
    
    bool RandomizedTargetList::empty() {
        return empty_();
    }
    
    bool RandomizedTargetList::empty_() {
        return files.empty();
    }
    
    std::string RandomizedTargetList::next() {
        if (empty_())
            return "";
        
        auto nextFile_ = files.front();
        files.erase(files.begin());
        replaceLastFile();
        shuffle();
        return fullPath(currentFile_ = std::move(nextFile_));
    }
    
    void RandomizedTargetList::replaceLastFile() {
        if (!noFilesGotten)
            files.push_back(currentFile_);
        noFilesGotten = false;
    }
    
    std::string RandomizedTargetList::fullPath(std::string file) {
        return directory_ + "/" + file;
    }
    
    std::string RandomizedTargetList::current() {
        return fullPath(currentFile_);
    }
    
}
