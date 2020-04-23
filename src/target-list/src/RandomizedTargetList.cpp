#include "RandomizedTargetList.hpp"
#include <algorithm>

namespace av_speech_in_noise {
static auto filesIn(DirectoryReader *reader, const LocalUrl &s) -> LocalUrls {
    return reader->filesIn(s);
}

static auto shuffle(target_list::Randomizer *randomizer,
    gsl::span<av_speech_in_noise::LocalUrl> v) {
    randomizer->shuffle(v);
}

static auto empty(const LocalUrls &files) -> bool { return files.empty(); }

static auto currentFile(const LocalUrls &v) -> LocalUrl {
    return empty(v) ? av_speech_in_noise::LocalUrl{""} : v.back();
}

static auto joinPaths(const LocalUrl &directory, const LocalUrl &file)
    -> LocalUrl {
    return {directory.path + '/' + file.path};
}

static auto fullPathToLastFile(
    const LocalUrl &directory, const LocalUrls &files) -> LocalUrl {
    return empty(files) ? av_speech_in_noise::LocalUrl{""}
                        : joinPaths(directory, currentFile(files));
}

static void moveFrontToBack(LocalUrls &files) {
    std::rotate(files.begin(), files.begin() + 1, files.end());
}

static auto allButLast(gsl::span<av_speech_in_noise::LocalUrl> s)
    -> gsl::span<av_speech_in_noise::LocalUrl> {
    return s.first(s.size() - 1);
}

RandomizedTargetListWithReplacement::RandomizedTargetListWithReplacement(
    DirectoryReader *reader, target_list::Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetListWithReplacement::loadFromDirectory(const LocalUrl &d) {
    shuffle(randomizer, files = filesIn(reader, directory_ = d));
}

auto RandomizedTargetListWithReplacement::next() -> LocalUrl {
    if (empty(files))
        return {""};

    moveFrontToBack(files);
    shuffle(randomizer, allButLast(files));
    return fullPathToLastFile(directory_, files);
}

auto RandomizedTargetListWithReplacement::current() -> LocalUrl {
    return fullPathToLastFile(directory_, files);
}

auto RandomizedTargetListWithReplacement::directory() -> LocalUrl {
    return directory_;
}

RandomizedTargetListWithoutReplacement::RandomizedTargetListWithoutReplacement(
    DirectoryReader *reader, target_list::Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetListWithoutReplacement::loadFromDirectory(
    const LocalUrl &d) {
    shuffle(randomizer, files = filesIn(reader, directory_ = d));
}

auto RandomizedTargetListWithoutReplacement::empty() -> bool {
    return av_speech_in_noise::empty(files);
}

auto RandomizedTargetListWithoutReplacement::next() -> LocalUrl {
    if (av_speech_in_noise::empty(files))
        return {""};

    currentFile = files.front();
    files.erase(files.begin());
    return joinPaths(directory_, currentFile);
}

auto RandomizedTargetListWithoutReplacement::current() -> LocalUrl {
    return currentFile.path.empty() ? av_speech_in_noise::LocalUrl{""}
                                    : joinPaths(directory_, currentFile);
}

auto RandomizedTargetListWithoutReplacement::directory() -> LocalUrl {
    return directory_;
}

void RandomizedTargetListWithoutReplacement::reinsertCurrent() {
    files.push_back(currentFile);
}

CyclicRandomizedTargetList::CyclicRandomizedTargetList(
    DirectoryReader *reader, target_list::Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void CyclicRandomizedTargetList::loadFromDirectory(const LocalUrl &d) {
    shuffle(randomizer, files = filesIn(reader, directory_ = d));
}

auto CyclicRandomizedTargetList::next() -> LocalUrl {
    if (empty(files))
        return {""};

    moveFrontToBack(files);
    return fullPathToLastFile(directory_, files);
}

auto CyclicRandomizedTargetList::current() -> LocalUrl {
    return fullPathToLastFile(directory_, files);
}

auto CyclicRandomizedTargetList::directory() -> LocalUrl { return directory_; }
}
