#include "RandomizedTargetPlaylists.hpp"
#include "SubdirectoryTargetPlaylistReader.hpp"

#include <algorithm>
#include <filesystem>

namespace av_speech_in_noise {
static auto filesIn(DirectoryReader *reader, const LocalUrl &s) -> LocalUrls {
    try {
        return reader->filesIn(s);
    } catch (const DirectoryReader::CannotRead &) {
        return {};
    }
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
    return {std::filesystem::path{directory.path} / file.path};
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

RandomizedTargetPlaylistWithReplacement::
    RandomizedTargetPlaylistWithReplacement(
        DirectoryReader *reader, target_list::Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetPlaylistWithReplacement::load(const LocalUrl &d) {
    shuffle(randomizer, files = filesIn(reader, directory_ = d));
}

auto RandomizedTargetPlaylistWithReplacement::next() -> LocalUrl {
    if (empty(files))
        return {""};

    moveFrontToBack(files);
    shuffle(randomizer, allButLast(files));
    return fullPathToLastFile(directory_, files);
}

auto RandomizedTargetPlaylistWithReplacement::current() -> LocalUrl {
    return fullPathToLastFile(directory_, files);
}

auto RandomizedTargetPlaylistWithReplacement::directory() -> LocalUrl {
    return directory_;
}

RandomizedTargetPlaylistWithoutReplacement::
    RandomizedTargetPlaylistWithoutReplacement(
        DirectoryReader *reader, target_list::Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void RandomizedTargetPlaylistWithoutReplacement::load(const LocalUrl &d) {
    shuffle(randomizer, files = filesIn(reader, directory_ = d));
}

auto RandomizedTargetPlaylistWithoutReplacement::empty() -> bool {
    return av_speech_in_noise::empty(files);
}

auto RandomizedTargetPlaylistWithoutReplacement::next() -> LocalUrl {
    if (av_speech_in_noise::empty(files))
        return {""};

    currentFile = files.front();
    files.erase(files.begin());
    return joinPaths(directory_, currentFile);
}

auto RandomizedTargetPlaylistWithoutReplacement::current() -> LocalUrl {
    return currentFile.path.empty() ? av_speech_in_noise::LocalUrl{""}
                                    : joinPaths(directory_, currentFile);
}

auto RandomizedTargetPlaylistWithoutReplacement::directory() -> LocalUrl {
    return directory_;
}

void RandomizedTargetPlaylistWithoutReplacement::reinsertCurrent() {
    files.push_back(currentFile);
}

CyclicRandomizedTargetPlaylist::CyclicRandomizedTargetPlaylist(
    DirectoryReader *reader, target_list::Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void CyclicRandomizedTargetPlaylist::load(const LocalUrl &d) {
    shuffle(randomizer, files = filesIn(reader, directory_ = d));
}

auto CyclicRandomizedTargetPlaylist::next() -> LocalUrl {
    if (empty(files))
        return {""};

    moveFrontToBack(files);
    return fullPathToLastFile(directory_, files);
}

auto CyclicRandomizedTargetPlaylist::current() -> LocalUrl {
    return fullPathToLastFile(directory_, files);
}

auto CyclicRandomizedTargetPlaylist::directory() -> LocalUrl {
    return directory_;
}

EachTargetPlayedOnceThenShuffleAndRepeat::
    EachTargetPlayedOnceThenShuffleAndRepeat(
        DirectoryReader *reader, target_list::Randomizer *randomizer)
    : reader{reader}, randomizer{randomizer} {}

void EachTargetPlayedOnceThenShuffleAndRepeat::load(const LocalUrl &d) {
    shuffle(randomizer, files = filesIn(reader, directory_ = d));
    endOfPlaylistCount = 0;
    currentIterator = files.begin();
}

auto EachTargetPlayedOnceThenShuffleAndRepeat::next() -> LocalUrl {
    if (av_speech_in_noise::empty(files))
        return {""};

    currentFile = *currentIterator;
    if (++currentIterator == files.end()) {
        ++endOfPlaylistCount;
        shuffle(randomizer, files);
        currentIterator = files.begin();
    }
    return joinPaths(directory_, currentFile);
}

auto EachTargetPlayedOnceThenShuffleAndRepeat::current() -> LocalUrl {
    return currentFile.path.empty() ? av_speech_in_noise::LocalUrl{""}
                                    : joinPaths(directory_, currentFile);
}

auto EachTargetPlayedOnceThenShuffleAndRepeat::directory() -> LocalUrl {
    return directory_;
}

auto EachTargetPlayedOnceThenShuffleAndRepeat::empty() -> bool {
    return endOfPlaylistCount > repeats;
}

void EachTargetPlayedOnceThenShuffleAndRepeat::setRepeats(gsl::index n) {
    repeats = n;
}
}
