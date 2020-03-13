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

static auto joinPaths(const std::string &directory, const std::string &file)
    -> std::string {
    return directory + '/' + file;
}

static auto fullPathToLastFile(const std::string &directory,
    const std::vector<std::string> &files) -> std::string {
    return empty(files) ? "" : joinPaths(directory, currentFile(files));
}

static void moveFrontToBack(std::vector<std::string> &files) {
    std::rotate(files.begin(), files.begin() + 1, files.end());
}

static auto allButLast(gsl::span<std::string> s) -> gsl::span<std::string> {
    return s.first(s.size() - 1);
}

RandomizedTargetListWithReplacement::RandomizedTargetListWithReplacement(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetListWithReplacement::loadFromDirectory(std::string d) {
    shuffle(randomizer, files = filesIn(reader, directory = std::move(d)));
}

auto RandomizedTargetListWithReplacement::next() -> std::string {
    if (empty(files))
        return "";

    moveFrontToBack(files);
    shuffle(randomizer, allButLast(files));
    return fullPathToLastFile(directory, files);
}

auto RandomizedTargetListWithReplacement::current() -> std::string {
    return fullPathToLastFile(directory, files);
}

RandomizedTargetListWithoutReplacement::RandomizedTargetListWithoutReplacement(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetListWithoutReplacement::loadFromDirectory(std::string d) {
    shuffle(randomizer, files = filesIn(reader, directory = std::move(d)));
}

auto RandomizedTargetListWithoutReplacement::empty() -> bool {
    return target_list::empty(files);
}

auto RandomizedTargetListWithoutReplacement::next() -> std::string {
    if (target_list::empty(files))
        return "";

    currentFile = files.front();
    files.erase(files.begin());
    return joinPaths(directory, currentFile);
}

auto RandomizedTargetListWithoutReplacement::current() -> std::string {
    return currentFile.empty() ? "" : joinPaths(directory, currentFile);
}

void RandomizedTargetListWithoutReplacement::reinsertCurrent() {
    files.push_back(currentFile);
}

CyclicRandomizedTargetList::CyclicRandomizedTargetList(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void CyclicRandomizedTargetList::loadFromDirectory(std::string d) {
    shuffle(randomizer, files = filesIn(reader, directory = std::move(d)));
}

auto CyclicRandomizedTargetList::next() -> std::string {
    if (empty(files))
        return "";

    moveFrontToBack(files);
    return fullPathToLastFile(directory, files);
}

auto CyclicRandomizedTargetList::current() -> std::string {
    return fullPathToLastFile(directory, files);
}
}
