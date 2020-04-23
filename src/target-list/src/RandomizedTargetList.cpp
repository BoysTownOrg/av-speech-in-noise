#include "RandomizedTargetList.hpp"
#include <algorithm>

namespace target_list {
static auto filesIn(
    DirectoryReader *reader, const av_speech_in_noise::LocalUrl &s)
    -> std::vector<av_speech_in_noise::LocalUrl> {
    return reader->filesIn(s);
}

static auto shuffle(
    Randomizer *randomizer, gsl::span<av_speech_in_noise::LocalUrl> v) {
    randomizer->shuffle(v);
}

static auto empty(const std::vector<av_speech_in_noise::LocalUrl> &files)
    -> bool {
    return files.empty();
}

static auto currentFile(const std::vector<av_speech_in_noise::LocalUrl> &v)
    -> av_speech_in_noise::LocalUrl {
    return empty(v) ? av_speech_in_noise::LocalUrl{""} : v.back();
}

static auto joinPaths(const av_speech_in_noise::LocalUrl &directory,
    const av_speech_in_noise::LocalUrl &file) -> av_speech_in_noise::LocalUrl {
    return {directory.path + '/' + file.path};
}

static auto fullPathToLastFile(const av_speech_in_noise::LocalUrl &directory,
    const std::vector<av_speech_in_noise::LocalUrl> &files)
    -> av_speech_in_noise::LocalUrl {
    return empty(files) ? av_speech_in_noise::LocalUrl{""}
                        : joinPaths(directory, currentFile(files));
}

static void moveFrontToBack(std::vector<av_speech_in_noise::LocalUrl> &files) {
    std::rotate(files.begin(), files.begin() + 1, files.end());
}

static auto allButLast(gsl::span<av_speech_in_noise::LocalUrl> s)
    -> gsl::span<av_speech_in_noise::LocalUrl> {
    return s.first(s.size() - 1);
}

RandomizedTargetListWithReplacement::RandomizedTargetListWithReplacement(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetListWithReplacement::loadFromDirectory(
    const av_speech_in_noise::LocalUrl &d) {
    shuffle(randomizer, files = filesIn(reader, directory_ = d));
}

auto RandomizedTargetListWithReplacement::next()
    -> av_speech_in_noise::LocalUrl {
    if (empty(files))
        return {""};

    moveFrontToBack(files);
    shuffle(randomizer, allButLast(files));
    return fullPathToLastFile(directory_, files);
}

auto RandomizedTargetListWithReplacement::current()
    -> av_speech_in_noise::LocalUrl {
    return fullPathToLastFile(directory_, files);
}

auto RandomizedTargetListWithReplacement::directory()
    -> av_speech_in_noise::LocalUrl {
    return directory_;
}

RandomizedTargetListWithoutReplacement::RandomizedTargetListWithoutReplacement(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetListWithoutReplacement::loadFromDirectory(
    const av_speech_in_noise::LocalUrl &d) {
    shuffle(randomizer, files = filesIn(reader, directory_ = d));
}

auto RandomizedTargetListWithoutReplacement::empty() -> bool {
    return target_list::empty(files);
}

auto RandomizedTargetListWithoutReplacement::next()
    -> av_speech_in_noise::LocalUrl {
    if (target_list::empty(files))
        return {""};

    currentFile = files.front();
    files.erase(files.begin());
    return joinPaths(directory_, currentFile);
}

auto RandomizedTargetListWithoutReplacement::current()
    -> av_speech_in_noise::LocalUrl {
    return currentFile.path.empty() ? av_speech_in_noise::LocalUrl{""}
                                    : joinPaths(directory_, currentFile);
}

auto RandomizedTargetListWithoutReplacement::directory()
    -> av_speech_in_noise::LocalUrl {
    return directory_;
}

void RandomizedTargetListWithoutReplacement::reinsertCurrent() {
    files.push_back(currentFile);
}

CyclicRandomizedTargetList::CyclicRandomizedTargetList(
    DirectoryReader *reader, Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void CyclicRandomizedTargetList::loadFromDirectory(
    const av_speech_in_noise::LocalUrl &d) {
    shuffle(randomizer, files = filesIn(reader, directory_ = d));
}

auto CyclicRandomizedTargetList::next() -> av_speech_in_noise::LocalUrl {
    if (empty(files))
        return {""};

    moveFrontToBack(files);
    return fullPathToLastFile(directory_, files);
}

auto CyclicRandomizedTargetList::current() -> av_speech_in_noise::LocalUrl {
    return fullPathToLastFile(directory_, files);
}

auto CyclicRandomizedTargetList::directory() -> av_speech_in_noise::LocalUrl {
    return directory_;
}
}
