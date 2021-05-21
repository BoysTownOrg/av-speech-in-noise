#include "DirectoryReaderStub.hpp"
#include "assert-utility.hpp"
#include <av-speech-in-noise/playlist/FileFilterDecorator.hpp>
#include <av-speech-in-noise/playlist/RandomizedTargetPlaylists.hpp>
#include <gtest/gtest.h>
#include <algorithm>

namespace av_speech_in_noise {
static auto operator==(const LocalUrl &a, const LocalUrl &b) -> bool {
    return a.path == b.path;
}
}

namespace av_speech_in_noise {
namespace {
class RandomizerStub : public target_list::Randomizer {
  public:
    [[nodiscard]] auto shuffledStrings() const { return shuffledStrings_; }

    [[nodiscard]] auto shuffledInts() const { return shuffledInts_; }

    void shuffle(gsl::span<av_speech_in_noise::LocalUrl> s) override {
        shuffledStrings_ = {s.begin(), s.end()};
        ++shuffledCount_;
    }

    void shuffle(gsl::span<int> s) override {
        shuffledInts_ = {s.begin(), s.end()};
        std::rotate(s.begin(), s.begin() + rotateToTheLeft_, s.end());
    }

    void rotateToTheLeft(int N) { rotateToTheLeft_ = N; }

    auto shuffledCount() -> gsl::index { return shuffledCount_; }

  private:
    std::vector<av_speech_in_noise::LocalUrl> shuffledStrings_;
    std::vector<int> shuffledInts_;
    gsl::index shuffledCount_{};
    int rotateToTheLeft_{};
};

void loadFromDirectory(TargetPlaylist &list, const std::string &s = {}) {
    list.loadFromDirectory({s});
}

void setFileNames(
    DirectoryReaderStub &reader, std::vector<av_speech_in_noise::LocalUrl> v) {
    reader.setFileNames(std::move(v));
}

auto next(TargetPlaylist &list) -> std::string { return list.next().path; }

void assertNextEquals(TargetPlaylist &list, const std::string &s) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, next(list));
}

void assertCurrentEquals(TargetPlaylist &list, const std::string &s) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, list.current().path);
}

void assertShuffled(RandomizerStub &randomizer,
    const std::vector<av_speech_in_noise::LocalUrl> &s) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, randomizer.shuffledStrings());
}

auto empty(FiniteTargetPlaylist &list) { return list.empty(); }

void assertNotEmpty(FiniteTargetPlaylist &list) {
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(empty(list));
}

void assertEmpty(FiniteTargetPlaylist &list) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(empty(list));
}

void reinsertCurrent(RandomizedTargetPlaylistWithoutReplacement &list) {
    list.reinsertCurrent();
}

auto directory(DirectoryReaderStub &reader) -> std::string {
    return reader.directory();
}

void assertDirectoryEquals(DirectoryReaderStub &reader, const std::string &s) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, directory(reader));
}

void loadFromDirectoryPassesDirectoryToDirectoryReader(
    TargetPlaylist &list, DirectoryReaderStub &reader) {
    loadFromDirectory(list, "a");
    assertDirectoryEquals(reader, "a");
}

void loadFromDirectoryShufflesFileNames(TargetPlaylist &list,
    DirectoryReaderStub &reader, RandomizerStub &randomizer) {
    setFileNames(reader, {{"a"}, {"b"}, {"c"}});
    loadFromDirectory(list);
    assertShuffled(randomizer, {{"a"}, {"b"}, {"c"}});
}

void nextReturnsFullPathToFile(
    TargetPlaylist &list, DirectoryReaderStub &reader) {
    setFileNames(reader, {{"a"}, {"b"}, {"c"}});
    loadFromDirectory(list, "C:");
    assertNextEquals(list, "C:/a");
    assertNextEquals(list, "C:/b");
    assertNextEquals(list, "C:/c");
}

void currentReturnsFullPathToFile(
    TargetPlaylist &list, DirectoryReaderStub &reader) {
    setFileNames(reader, {{"a"}, {"b"}, {"c"}});
    loadFromDirectory(list, "C:");
    next(list);
    assertCurrentEquals(list, "C:/a");
}

void directoryReturnsDirectory(TargetPlaylist &list) {
    loadFromDirectory(list, "a");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, list.directory().path);
}

void nextReturnsEmptyIfNoFiles(
    TargetPlaylist &list, DirectoryReaderStub &reader) {
    setFileNames(reader, {});
    loadFromDirectory(list);
    assertNextEquals(list, "");
}

void currentReturnsEmptyIfNoFiles(
    TargetPlaylist &list, DirectoryReaderStub &reader) {
    setFileNames(reader, {});
    loadFromDirectory(list);
    assertCurrentEquals(list, "");
}

class RandomizedTargetPlaylistWithReplacementTests : public ::testing::Test {
  protected:
    DirectoryReaderStub reader;
    RandomizerStub randomizer;
    RandomizedTargetPlaylistWithReplacement list{&reader, &randomizer};
};

class RandomizedTargetPlaylistWithoutReplacementTests : public ::testing::Test {
  protected:
    DirectoryReaderStub reader;
    RandomizerStub randomizer;
    RandomizedTargetPlaylistWithoutReplacement list{&reader, &randomizer};
};

class CyclicRandomizedTargetPlaylistTests : public ::testing::Test {
  protected:
    DirectoryReaderStub reader;
    RandomizerStub randomizer;
    CyclicRandomizedTargetPlaylist list{&reader, &randomizer};
};

class EachTargetPlayedOnceThenShuffleAndRepeatTests : public ::testing::Test {
  protected:
    DirectoryReaderStub reader;
    RandomizerStub randomizer;
    EachTargetPlayedOnceThenShuffleAndRepeat list{&reader, &randomizer};
};

#define RANDOMIZED_TARGET_PLAYLIST_WITH_REPLACEMENT_TEST(a)                    \
    TEST_F(RandomizedTargetPlaylistWithReplacementTests, a)

#define RANDOMIZED_TARGET_PLAYLIST_WITHOUT_REPLACEMENT_TEST(a)                 \
    TEST_F(RandomizedTargetPlaylistWithoutReplacementTests, a)

#define CYCLIC_RANDOMIZED_TARGET_PLAYLIST_TEST(a)                              \
    TEST_F(CyclicRandomizedTargetPlaylistTests, a)

#define EACH_TARGET_PLAYED_ONCE_THEN_SHUFFLE_AND_REPEAT_TEST(a)                \
    TEST_F(EachTargetPlayedOnceThenShuffleAndRepeatTests, a)

RANDOMIZED_TARGET_PLAYLIST_WITH_REPLACEMENT_TEST(
    loadFromDirectoryPassesDirectoryToDirectoryReader) {
    loadFromDirectoryPassesDirectoryToDirectoryReader(list, reader);
}

RANDOMIZED_TARGET_PLAYLIST_WITHOUT_REPLACEMENT_TEST(
    loadFromDirectoryPassesDirectoryToDirectoryReader) {
    loadFromDirectoryPassesDirectoryToDirectoryReader(list, reader);
}

CYCLIC_RANDOMIZED_TARGET_PLAYLIST_TEST(
    loadFromDirectoryPassesDirectoryToDirectoryReader) {
    loadFromDirectoryPassesDirectoryToDirectoryReader(list, reader);
}

EACH_TARGET_PLAYED_ONCE_THEN_SHUFFLE_AND_REPEAT_TEST(
    loadFromDirectoryPassesDirectoryToDirectoryReader) {
    loadFromDirectoryPassesDirectoryToDirectoryReader(list, reader);
}

RANDOMIZED_TARGET_PLAYLIST_WITH_REPLACEMENT_TEST(
    loadFromDirectoryShufflesFileNames) {
    loadFromDirectoryShufflesFileNames(list, reader, randomizer);
}

RANDOMIZED_TARGET_PLAYLIST_WITHOUT_REPLACEMENT_TEST(
    loadFromDirectoryShufflesFileNames) {
    loadFromDirectoryShufflesFileNames(list, reader, randomizer);
}

CYCLIC_RANDOMIZED_TARGET_PLAYLIST_TEST(loadFromDirectoryShufflesFileNames) {
    loadFromDirectoryShufflesFileNames(list, reader, randomizer);
}

EACH_TARGET_PLAYED_ONCE_THEN_SHUFFLE_AND_REPEAT_TEST(
    loadFromDirectoryShufflesFileNames) {
    loadFromDirectoryShufflesFileNames(list, reader, randomizer);
}

RANDOMIZED_TARGET_PLAYLIST_WITH_REPLACEMENT_TEST(nextReturnsFullPathToFile) {
    nextReturnsFullPathToFile(list, reader);
}

RANDOMIZED_TARGET_PLAYLIST_WITHOUT_REPLACEMENT_TEST(
    nextReturnsFullPathToFileAtFront) {
    nextReturnsFullPathToFile(list, reader);
}

CYCLIC_RANDOMIZED_TARGET_PLAYLIST_TEST(nextReturnsFullPathToFileAtFront) {
    nextReturnsFullPathToFile(list, reader);
}

EACH_TARGET_PLAYED_ONCE_THEN_SHUFFLE_AND_REPEAT_TEST(
    nextReturnsFullPathToFileAtFront) {
    nextReturnsFullPathToFile(list, reader);
}

RANDOMIZED_TARGET_PLAYLIST_WITH_REPLACEMENT_TEST(currentReturnsFullPathToFile) {
    currentReturnsFullPathToFile(list, reader);
}

RANDOMIZED_TARGET_PLAYLIST_WITHOUT_REPLACEMENT_TEST(
    currentReturnsFullPathToFile) {
    currentReturnsFullPathToFile(list, reader);
}

CYCLIC_RANDOMIZED_TARGET_PLAYLIST_TEST(currentReturnsFullPathToFile) {
    currentReturnsFullPathToFile(list, reader);
}

EACH_TARGET_PLAYED_ONCE_THEN_SHUFFLE_AND_REPEAT_TEST(
    currentReturnsFullPathToFile) {
    currentReturnsFullPathToFile(list, reader);
}

RANDOMIZED_TARGET_PLAYLIST_WITH_REPLACEMENT_TEST(directoryReturnsDirectory) {
    directoryReturnsDirectory(list);
}

RANDOMIZED_TARGET_PLAYLIST_WITHOUT_REPLACEMENT_TEST(directoryReturnsDirectory) {
    directoryReturnsDirectory(list);
}

CYCLIC_RANDOMIZED_TARGET_PLAYLIST_TEST(directoryReturnsDirectory) {
    directoryReturnsDirectory(list);
}

EACH_TARGET_PLAYED_ONCE_THEN_SHUFFLE_AND_REPEAT_TEST(
    directoryReturnsDirectory) {
    directoryReturnsDirectory(list);
}

RANDOMIZED_TARGET_PLAYLIST_WITH_REPLACEMENT_TEST(nextReturnsEmptyIfNoFiles) {
    nextReturnsEmptyIfNoFiles(list, reader);
}

RANDOMIZED_TARGET_PLAYLIST_WITHOUT_REPLACEMENT_TEST(nextReturnsEmptyIfNoFiles) {
    nextReturnsEmptyIfNoFiles(list, reader);
}

CYCLIC_RANDOMIZED_TARGET_PLAYLIST_TEST(nextReturnsEmptyIfNoFiles) {
    nextReturnsEmptyIfNoFiles(list, reader);
}

EACH_TARGET_PLAYED_ONCE_THEN_SHUFFLE_AND_REPEAT_TEST(
    nextReturnsEmptyIfNoFiles) {
    nextReturnsEmptyIfNoFiles(list, reader);
}

RANDOMIZED_TARGET_PLAYLIST_WITH_REPLACEMENT_TEST(currentReturnsEmptyIfNoFiles) {
    currentReturnsEmptyIfNoFiles(list, reader);
}

RANDOMIZED_TARGET_PLAYLIST_WITHOUT_REPLACEMENT_TEST(
    currentReturnsEmptyIfNoFiles) {
    currentReturnsEmptyIfNoFiles(list, reader);
}

CYCLIC_RANDOMIZED_TARGET_PLAYLIST_TEST(currentReturnsEmptyIfNoFiles) {
    currentReturnsEmptyIfNoFiles(list, reader);
}

EACH_TARGET_PLAYED_ONCE_THEN_SHUFFLE_AND_REPEAT_TEST(
    currentReturnsEmptyIfNoFiles) {
    currentReturnsEmptyIfNoFiles(list, reader);
}

RANDOMIZED_TARGET_PLAYLIST_WITH_REPLACEMENT_TEST(
    nextShufflesNextSetNotIncludingCurrent) {
    setFileNames(reader, {{"a"}, {"b"}, {"c"}, {"d"}});
    loadFromDirectory(list);
    next(list);
    assertShuffled(randomizer, {{"b"}, {"c"}, {"d"}});
}

EACH_TARGET_PLAYED_ONCE_THEN_SHUFFLE_AND_REPEAT_TEST(
    nextShufflesAllWhenExhaustedAndSetToRepeat) {
    list.setRepeats(1);
    setFileNames(reader, {{"a"}, {"b"}, {"c"}, {"d"}});
    loadFromDirectory(list);
    next(list);
    next(list);
    next(list);
    next(list);
    assertShuffled(randomizer, {{"a"}, {"b"}, {"c"}, {"d"}});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(gsl::index{2}, randomizer.shuffledCount());
}

RANDOMIZED_TARGET_PLAYLIST_WITH_REPLACEMENT_TEST(
    nextReplacesSecondToLastTarget) {
    setFileNames(reader, {{"a"}, {"b"}, {"c"}, {"d"}, {"e"}});
    loadFromDirectory(list);
    next(list);
    next(list);
    assertShuffled(randomizer, {{"c"}, {"d"}, {"e"}, {"a"}});
}

RANDOMIZED_TARGET_PLAYLIST_WITHOUT_REPLACEMENT_TEST(
    emptyWhenStimulusFilesExhausted) {
    setFileNames(reader, {{"a"}, {"b"}, {"c"}});
    loadFromDirectory(list);
    assertNotEmpty(list);
    next(list);
    assertNotEmpty(list);
    next(list);
    assertNotEmpty(list);
    next(list);
    assertEmpty(list);
}

EACH_TARGET_PLAYED_ONCE_THEN_SHUFFLE_AND_REPEAT_TEST(
    emptyWhenStimulusFilesExhausted) {
    setFileNames(reader, {{"a"}, {"b"}, {"c"}});
    loadFromDirectory(list);
    assertNotEmpty(list);
    next(list);
    assertNotEmpty(list);
    next(list);
    assertNotEmpty(list);
    next(list);
    assertEmpty(list);
}

EACH_TARGET_PLAYED_ONCE_THEN_SHUFFLE_AND_REPEAT_TEST(
    emptyWhenStimulusFilesExhaustedWithRepeats) {
    list.setRepeats(1);
    setFileNames(reader, {{"a"}, {"b"}, {"c"}});
    loadFromDirectory(list);
    next(list);
    next(list);
    next(list);
    assertNotEmpty(list);
    next(list);
    assertNotEmpty(list);
    next(list);
    assertNotEmpty(list);
    next(list);
    assertEmpty(list);
}

EACH_TARGET_PLAYED_ONCE_THEN_SHUFFLE_AND_REPEAT_TEST(
    emptyWhenStimulusFilesExhaustedWithRepeatsAfterReload) {
    list.setRepeats(1);
    setFileNames(reader, {{"a"}, {"b"}, {"c"}});
    loadFromDirectory(list);
    next(list);
    next(list);
    next(list);
    next(list);
    next(list);
    next(list);
    loadFromDirectory(list);
    assertNotEmpty(list);
}

EACH_TARGET_PLAYED_ONCE_THEN_SHUFFLE_AND_REPEAT_TEST(
    nextReturnsFullPathToFileAtFrontAfterReload) {
    setFileNames(reader, {{"a"}, {"b"}, {"c"}});
    loadFromDirectory(list);
    next(list);
    next(list);
    loadFromDirectory(list);
    nextReturnsFullPathToFile(list, reader);
}

RANDOMIZED_TARGET_PLAYLIST_WITHOUT_REPLACEMENT_TEST(reinsertCurrent) {
    setFileNames(reader, {{"a"}, {"b"}, {"c"}});
    loadFromDirectory(list, "C:");
    assertNextEquals(list, "C:/a");
    assertNextEquals(list, "C:/b");
    reinsertCurrent(list);
    assertNextEquals(list, "C:/c");
    assertNextEquals(list, "C:/b");
}

CYCLIC_RANDOMIZED_TARGET_PLAYLIST_TEST(nextCyclesBackToBeginningOfFiles) {
    setFileNames(reader, {{"a"}, {"b"}, {"c"}});
    loadFromDirectory(list, "C:");
    next(list);
    next(list);
    next(list);
    assertNextEquals(list, "C:/a");
    assertNextEquals(list, "C:/b");
    assertNextEquals(list, "C:/c");
}

auto filesIn(DirectoryReader &reader, const LocalUrl &directory = {})
    -> std::vector<av_speech_in_noise::LocalUrl> {
    return reader.filesIn(directory);
}

auto filter(FileFilter &filter_,
    const std::vector<av_speech_in_noise::LocalUrl> &files = {})
    -> std::vector<av_speech_in_noise::LocalUrl> {
    return filter_.filter(files);
}

auto subDirectories(DirectoryReader &reader, const LocalUrl &directory = {})
    -> std::vector<av_speech_in_noise::LocalUrl> {
    return reader.subDirectories(directory);
}

class FileFilterStub : public FileFilter {
  public:
    [[nodiscard]] auto files() const { return files_; }

    void setFiltered(std::vector<av_speech_in_noise::LocalUrl> f) {
        filtered_ = std::move(f);
    }

    auto filter(std::vector<av_speech_in_noise::LocalUrl> f)
        -> std::vector<av_speech_in_noise::LocalUrl> override {
        files_ = std::move(f);
        return filtered_;
    }

  private:
    std::vector<av_speech_in_noise::LocalUrl> filtered_;
    std::vector<av_speech_in_noise::LocalUrl> files_;
};

class FileFilterDecoratorTests : public ::testing::Test {
  protected:
    DirectoryReaderStub reader;
    FileFilterStub filter;
    FileFilterDecorator decorator{&reader, &filter};
};

#define FILE_FILTER_DECORATOR_TEST(a) TEST_F(FileFilterDecoratorTests, a)

FILE_FILTER_DECORATOR_TEST(passesDirectoryToDecoratedForFiles) {
    filesIn(decorator, {"a"});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, directory(reader));
}

FILE_FILTER_DECORATOR_TEST(passesDirectoryToDecoratedForSubdirectories) {
    subDirectories(decorator, {"a"});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"a"}, directory(reader));
}

FILE_FILTER_DECORATOR_TEST(passesFilesToFilter) {
    reader.setFileNames({{"a"}, {"b"}, {"c"}});
    filesIn(decorator);
    assertEqual({{"a"}, {"b"}, {"c"}}, filter.files());
}

FILE_FILTER_DECORATOR_TEST(returnsFilteredFiles) {
    filter.setFiltered({{"a"}, {"b"}, {"c"}});
    assertEqual({{"a"}, {"b"}, {"c"}}, filesIn(decorator));
}

FILE_FILTER_DECORATOR_TEST(returnsSubdirectories) {
    reader.setSubDirectories({{"a"}, {"b"}, {"c"}});
    assertEqual({{"a"}, {"b"}, {"c"}}, subDirectories(decorator));
}

class FileExtensionFilterTests : public ::testing::Test {
  protected:
    static auto construct(std::vector<std::string> filters = {})
        -> FileExtensionFilter {
        return FileExtensionFilter{std::move(filters)};
    }
};

TEST_F(FileExtensionFilterTests, returnsFilteredFiles) {
    auto decorator = construct({".c", ".h"});
    assertEqual({{"b.c"}, {"f.c"}, {"g.h"}},
        filter(decorator, {{"a"}, {"b.c"}, {"d.e"}, {"f.c"}, {"g.h"}}));
}

class FileIdentifierFilterTests : public ::testing::Test {
  protected:
    static auto construct(std::string indentifier = {})
        -> FileIdentifierFilter {
        return FileIdentifierFilter{std::move(indentifier)};
    }
};

#define FILE_IDENTIFIER_FILTER_TEST(a) TEST_F(FileIdentifierFilterTests, a)

FILE_IDENTIFIER_FILTER_TEST(returnsFilteredFiles) {
    auto decorator = construct("x");
    assertEqual({{"ax.j"}, {"xf.c"}},
        filter(decorator, {{"ax.j"}, {"b.c"}, {"d.e"}, {"xf.c"}, {"g.h"}}));
}

FILE_IDENTIFIER_FILTER_TEST(returnsFilesThatEndWithIdentifier) {
    auto decorator = construct("x");
    assertEqual({{"ax.j"}, {"fx.c"}},
        filter(decorator, {{"ax.j"}, {"b.c"}, {"d.e"}, {"fx.c"}, {"g.h"}}));
}

class FileIdentifierExcluderFilterTests : public ::testing::Test {
  protected:
    static auto construct(std::vector<std::string> indentifiers = {})
        -> FileIdentifierExcluderFilter {
        return FileIdentifierExcluderFilter{std::move(indentifiers)};
    }
};

TEST_F(
    FileIdentifierExcluderFilterTests, returnsFilesThatDontEndWithIdentifiers) {
    auto decorator = construct({"1", "2", "3"});
    assertEqual({{"ax.j"}, {"d.e"}},
        filter(decorator, {{"ax.j"}, {"b1.c"}, {"d.e"}, {"fx2.c"}, {"g3.h"}}));
}

class RandomSubsetFilesTests : public ::testing::Test {
  protected:
    RandomizerStub randomizer;

    auto construct(int N = {}) -> RandomSubsetFiles { return {&randomizer, N}; }

    auto shuffled() { return randomizer.shuffledInts(); }

    void assertHasBeenShuffled(const std::vector<int> &v) {
        assertEqual(v, shuffled());
    }
};

#define RANDOM_SUBSET_FILES_TEST(a) TEST_F(RandomSubsetFilesTests, a)

RANDOM_SUBSET_FILES_TEST(passesFileNumberRangeToRandomizer) {
    auto decorator = construct();
    filter(decorator, {{"a"}, {"b"}, {"c"}});
    assertHasBeenShuffled({0, 1, 2});
}

RANDOM_SUBSET_FILES_TEST(returnsFirstNShuffledIndexedFiles) {
    auto decorator = construct(3);
    randomizer.rotateToTheLeft(2);
    assertEqual({{"c"}, {"d"}, {"e"}},
        filter(decorator, {{"a"}, {"b"}, {"c"}, {"d"}, {"e"}}));
}

RANDOM_SUBSET_FILES_TEST(returnsAllFilesIfLessThanAvailable) {
    auto decorator = construct(5);
    assertEqual(
        {{"a"}, {"b"}, {"c"}}, filter(decorator, {{"a"}, {"b"}, {"c"}}));
}

class DirectoryReaderCompositeTests : public ::testing::Test {
  protected:
    std::vector<DirectoryReaderStub> decorated;

    auto construct() -> DirectoryReaderComposite {
        std::vector<DirectoryReader *> ptrs;
        for (auto &d : decorated)
            ptrs.push_back(&d);
        return DirectoryReaderComposite{ptrs};
    }

    void setDecoratedCount(int N) { decorated.resize(N); }

    void assertEachDecoratedDirectory(const std::string &expected, int N) {
        for (int i = 0; i < N; ++i)
            AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
                expected, decorated.at(i).directory());
    }

    auto decoratedAt(int n) -> auto & { return decorated.at(n); }

    void setFileNamesForDecorated(
        std::vector<av_speech_in_noise::LocalUrl> v, int n) {
        decoratedAt(n).setFileNames(std::move(v));
    }
};

#define DIRECTORY_READER_COMPOSITE_TEST(a)                                     \
    TEST_F(DirectoryReaderCompositeTests, a)

DIRECTORY_READER_COMPOSITE_TEST(filesInPassesDirectoryToEach) {
    setDecoratedCount(3);
    auto reader = construct();
    filesIn(reader, {"a"});
    assertEachDecoratedDirectory({"a"}, 3);
}

DIRECTORY_READER_COMPOSITE_TEST(filesInPassesCollectsFilesFromEach) {
    setDecoratedCount(3);
    setFileNamesForDecorated({{"a"}}, 0);
    setFileNamesForDecorated({{"b"}, {"c"}, {"d"}}, 1);
    setFileNamesForDecorated({{"e"}, {"f"}}, 2);
    auto reader = construct();
    assertEqual({{"a"}, {"b"}, {"c"}, {"d"}, {"e"}, {"f"}}, filesIn(reader));
}

TEST_F(DirectoryReaderCompositeTests,
    passesDirectoryToFirstDecoratedForSubdirectories) {
    setDecoratedCount(3);
    auto reader = construct();
    subDirectories(reader, {"a"});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, directory(decoratedAt(0)));
}

DIRECTORY_READER_COMPOSITE_TEST(returnsSubdirectoriesFromFirstDecorated) {
    setDecoratedCount(3);
    auto reader = construct();
    decoratedAt(0).setSubDirectories({{"a"}, {"b"}, {"c"}});
    assertEqual({{"a"}, {"b"}, {"c"}}, subDirectories(reader));
}
}
}
