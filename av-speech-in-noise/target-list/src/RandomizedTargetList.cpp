#include "RandomizedTargetList.hpp"

namespace target_list {
RandomizedTargetList::RandomizedTargetList(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetList::loadFromDirectory(std::string directory) {
    files = reader->filesIn(directory_ = std::move(directory));
    shuffle();
    noFilesGotten = true;
}

void RandomizedTargetList::shuffle() {
    randomizer->shuffle(files.begin(), files.end());
}

auto RandomizedTargetList::next() -> std::string {
    if (empty_())
        return "";

    auto nextFile_ = files.front();
    files.erase(files.begin());
    replaceLastFile();
    shuffle();
    return fullPath(currentFile_ = std::move(nextFile_));
}

auto RandomizedTargetList::empty() -> bool { return empty_(); }

auto RandomizedTargetList::empty_() -> bool { return files.empty(); }

void RandomizedTargetList::replaceLastFile() {
    if (!noFilesGotten)
        files.push_back(currentFile_);
    noFilesGotten = false;
}

auto RandomizedTargetList::fullPath(std::string file) -> std::string {
    return directory_ + "/" + std::move(file);
}

auto RandomizedTargetList::current() -> std::string {
    return fullPath(currentFile_);
}

void RandomizedTargetList::reinsertCurrent() {}

RandomizedFiniteTargetList::RandomizedFiniteTargetList(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedFiniteTargetList::loadFromDirectory(std::string directory) {
    directory_ = std::move(directory);
    files = reader->filesIn(directory_);
    randomizer->shuffle(files.begin(), files.end());
}

auto RandomizedFiniteTargetList::empty() -> bool { return empty_(); }

auto RandomizedFiniteTargetList::empty_() -> bool { return files.empty(); }

auto RandomizedFiniteTargetList::next() -> std::string {
    if (empty_())
        return "";

    currentFile_ = files.front();
    files.erase(files.begin());
    return fullPath(currentFile_);
}

auto RandomizedFiniteTargetList::fullPath(std::string file) -> std::string {
    return directory_ + "/" + std::move(file);
}

auto RandomizedFiniteTargetList::current() -> std::string {
    return fullPath(currentFile_);
}

void RandomizedFiniteTargetList::reinsertCurrent() {
    files.push_back(currentFile_);
}
}
