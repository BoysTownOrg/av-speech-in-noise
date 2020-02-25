#include "RandomizedTargetList.hpp"
#include <algorithm>

namespace target_list {
static auto filesIn(DirectoryReader *reader, std::string s)
    -> std::vector<std::string> {
    return reader->filesIn(std::move(s));
}

static auto shuffle(Randomizer *randomizer, gsl::span<std::string> v) {
    randomizer->shuffle(v);
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

    std::rotate(files.begin(), files.begin() + 1, files.end());
    gsl::span<std::string> files_{files};
    shuffle(randomizer, files_.first(files_.size()-1));
    return fullPath(currentFile_ = files.back());
}

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

CyclicRandomizedTargetList::CyclicRandomizedTargetList(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void CyclicRandomizedTargetList::loadFromDirectory(std::string directory) {
    files = filesIn(reader, directory_ = std::move(directory));
    shuffle(randomizer, files);
}

static auto fullPathToLastFile(const std::string &directory,
    const std::vector<std::string> &files) -> std::string {
    return directory + '/' + files.back();
}

auto CyclicRandomizedTargetList::next() -> std::string {
    std::rotate(files.begin(), files.begin() + 1, files.end());
    return fullPathToLastFile(directory_, files);
}

auto CyclicRandomizedTargetList::current() -> std::string {
    return fullPathToLastFile(directory_, files);
}
}
