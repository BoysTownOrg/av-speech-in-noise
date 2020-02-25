#include "RandomizedTargetList.hpp"
#include <algorithm>

namespace target_list {
static auto filesIn(DirectoryReader *reader, std::string s)
    -> std::vector<std::string> {
    return reader->filesIn(std::move(s));
}

static auto shuffle(Randomizer *randomizer, std::vector<std::string> &v) {
    randomizer->shuffle(v.begin(), v.end());
}

RandomizedTargetListWithReplacement::RandomizedTargetListWithReplacement(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetListWithReplacement::loadFromDirectory(
    std::string directory) {
    files = filesIn(reader, directory_ = std::move(directory));
    shuffle(randomizer, files);
    noFilesGotten = true;
}

auto RandomizedTargetListWithReplacement::next() -> std::string {
    if (empty_())
        return "";

    auto nextFile_ = files.front();
    files.erase(files.begin());
    replaceLastFile();
    shuffle(randomizer, files);
    return fullPath(currentFile_ = std::move(nextFile_));
}

auto RandomizedTargetListWithReplacement::empty() -> bool { return empty_(); }

auto RandomizedTargetListWithReplacement::empty_() -> bool {
    return files.empty();
}

void RandomizedTargetListWithReplacement::replaceLastFile() {
    if (!noFilesGotten)
        files.push_back(currentFile_);
    noFilesGotten = false;
}

auto RandomizedTargetListWithReplacement::fullPath(std::string file)
    -> std::string {
    return directory_ + "/" + std::move(file);
}

auto RandomizedTargetListWithReplacement::current() -> std::string {
    return fullPath(currentFile_);
}

void RandomizedTargetListWithReplacement::reinsertCurrent() {}

RandomizedTargetListWithoutReplacement::RandomizedTargetListWithoutReplacement(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetListWithoutReplacement::loadFromDirectory(
    std::string directory) {
    directory_ = std::move(directory);
    files = filesIn(reader, directory_);
    shuffle(randomizer, files);
}

auto RandomizedTargetListWithoutReplacement::empty() -> bool {
    return empty_();
}

auto RandomizedTargetListWithoutReplacement::empty_() -> bool {
    return files.empty();
}

auto RandomizedTargetListWithoutReplacement::next() -> std::string {
    if (empty_())
        return "";

    currentFile_ = files.front();
    files.erase(files.begin());
    return fullPath(currentFile_);
}

auto RandomizedTargetListWithoutReplacement::fullPath(std::string file)
    -> std::string {
    return directory_ + "/" + std::move(file);
}

auto RandomizedTargetListWithoutReplacement::current() -> std::string {
    return fullPath(currentFile_);
}

void RandomizedTargetListWithoutReplacement::reinsertCurrent() {
    files.push_back(currentFile_);
}

CyclicRandomizedTargetListWithoutReplacement::
    CyclicRandomizedTargetListWithoutReplacement(
        DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void CyclicRandomizedTargetListWithoutReplacement::loadFromDirectory(
    std::string directory) {
    files = filesIn(reader, directory_ = std::move(directory));
    shuffle(randomizer, files);
}

auto CyclicRandomizedTargetListWithoutReplacement::next() -> std::string {
    std::rotate(files.begin(), files.begin() + 1, files.end());
    return directory_ + "/" + files.back();
}
}
