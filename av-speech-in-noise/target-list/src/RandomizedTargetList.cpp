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

static auto empty(const std::vector<std::string> &files) -> bool {
    return files.empty();
}

static auto currentFile(const std::vector<std::string> &v) -> std::string {
    return empty(v) ? "" : v.back();
}

static auto fullPathToLastFile(const std::string &directory,
    const std::vector<std::string> &files) -> std::string {
    return empty(files) ? "" : directory + '/' + currentFile(files);
}

static void rotate(std::vector<std::string> &files) {
    std::rotate(files.begin(), files.begin() + 1, files.end());
}

RandomizedTargetListWithReplacement::RandomizedTargetListWithReplacement(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetListWithReplacement::loadFromDirectory(
    std::string directory) {
    files = filesIn(reader, directory_ = std::move(directory));
    shuffle(randomizer, files);
}

auto RandomizedTargetListWithReplacement::next() -> std::string {
    if (target_list::empty(files))
        return "";

    rotate(files);
    gsl::span<std::string> files_{files};
    shuffle(randomizer, files_.first(files_.size() - 1));
    return fullPathToLastFile(directory_, files);
}

auto RandomizedTargetListWithReplacement::current() -> std::string {
    return fullPathToLastFile(directory_, files);
}

RandomizedTargetListWithoutReplacement::RandomizedTargetListWithoutReplacement(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetListWithoutReplacement::loadFromDirectory(
    std::string directory) {
    files = filesIn(reader, directory_ = std::move(directory));
    shuffle(randomizer, files);
}

auto RandomizedTargetListWithoutReplacement::empty() -> bool {
    return target_list::empty(files);
}

auto RandomizedTargetListWithoutReplacement::next() -> std::string {
    if (target_list::empty(files))
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
    return currentFile_.empty() ? "" : fullPath(currentFile_);
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

auto CyclicRandomizedTargetList::next() -> std::string {
    if (target_list::empty(files))
        return "";

    rotate(files);
    return fullPathToLastFile(directory_, files);
}

auto CyclicRandomizedTargetList::current() -> std::string {
    return fullPathToLastFile(directory_, files);
}
}
