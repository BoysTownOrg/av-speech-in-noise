#include "DirectoryReaderStub.h"
#include "assert-utility.h"
#include <target-list/FileFilterDecorator.hpp>
#include <target-list/RandomizedTargetList.hpp>
#include <gtest/gtest.h>
#include <algorithm>

namespace target_list {
namespace {
class RandomizerStub : public Randomizer {
  public:
    [[nodiscard]] auto shuffledStrings() const { return shuffledStrings_; }

    [[nodiscard]] auto shuffledInts() const { return shuffledInts_; }

    void shuffle(gsl::span<std::string> s) override {
        shuffledStrings_ = {s.begin(), s.end()};
    }

    void shuffle(gsl::span<int> s) override {
        shuffledInts_ = {s.begin(), s.end()};
        std::rotate(s.begin(), s.begin() + rotateToTheLeft_, s.end());
    }

    void rotateToTheLeft(int N) { rotateToTheLeft_ = N; }

  private:
    std::vector<std::string> shuffledStrings_;
    std::vector<int> shuffledInts_;
    int rotateToTheLeft_{};
};

void loadFromDirectory(
    av_speech_in_noise::TargetList &list, std::string s = {}) {
    list.loadFromDirectory(std::move(s));
}

void setFileNames(DirectoryReaderStub &reader, std::vector<std::string> v) {
    reader.setFileNames(std::move(v));
}

auto next(av_speech_in_noise::TargetList &list) -> std::string {
    return list.next();
}

void assertNextEquals(
    av_speech_in_noise::TargetList &list, const std::string &s) {
    assertEqual(s, next(list));
}

void assertCurrentEquals(
    av_speech_in_noise::TargetList &list, const std::string &s) {
    assertEqual(s, list.current());
}

void assertShuffled(
    RandomizerStub &randomizer, const std::vector<std::string> &s) {
    assertEqual(s, randomizer.shuffledStrings());
}

auto empty(RandomizedTargetListWithoutReplacement &list) {
    return list.empty();
}

void assertNotEmpty(RandomizedTargetListWithoutReplacement &list) {
    assertFalse(empty(list));
}

void assertEmpty(RandomizedTargetListWithoutReplacement &list) {
    assertTrue(empty(list));
}

void reinsertCurrent(RandomizedTargetListWithoutReplacement &list) {
    list.reinsertCurrent();
}

auto directory(DirectoryReaderStub &reader) -> std::string {
    return reader.directory();
}

void assertDirectoryEquals(DirectoryReaderStub &reader, const std::string &s) {
    assertEqual(s, directory(reader));
}

void loadFromDirectoryPassesDirectoryToDirectoryReader(
    av_speech_in_noise::TargetList &list, DirectoryReaderStub &reader) {
    loadFromDirectory(list, "a");
    assertDirectoryEquals(reader, "a");
}

void loadFromDirectoryShufflesFileNames(av_speech_in_noise::TargetList &list,
    DirectoryReaderStub &reader, RandomizerStub &randomizer) {
    setFileNames(reader, {"a", "b", "c"});
    loadFromDirectory(list);
    assertShuffled(randomizer, {"a", "b", "c"});
}

void nextReturnsFullPathToFile(
    av_speech_in_noise::TargetList &list, DirectoryReaderStub &reader) {
    setFileNames(reader, {"a", "b", "c"});
    loadFromDirectory(list, "C:");
    assertNextEquals(list, "C:/a");
    assertNextEquals(list, "C:/b");
    assertNextEquals(list, "C:/c");
}

class RandomizedTargetListWithReplacementTests : public ::testing::Test {
  protected:
    DirectoryReaderStub reader;
    RandomizerStub randomizer;
    RandomizedTargetListWithReplacement list{&reader, &randomizer};
};

class RandomizedTargetListWithoutReplacementTests : public ::testing::Test {
  protected:
    DirectoryReaderStub reader;
    RandomizerStub randomizer;
    RandomizedTargetListWithoutReplacement list{&reader, &randomizer};
};

class CyclicRandomizedTargetListTests : public ::testing::Test {
  protected:
    DirectoryReaderStub reader;
    RandomizerStub randomizer;
    CyclicRandomizedTargetList list{&reader, &randomizer};
};

#define RANDOMIZED_TARGET_LIST_WITH_REPLACEMENT_TEST(a)                        \
    TEST_F(RandomizedTargetListWithReplacementTests, a)

#define RANDOMIZED_TARGET_LIST_WITHOUT_REPLACEMENT_TEST(a)                     \
    TEST_F(RandomizedTargetListWithoutReplacementTests, a)

#define CYCLIC_RANDOMIZED_TARGET_LIST_TEST(a)                                  \
    TEST_F(CyclicRandomizedTargetListTests, a)

RANDOMIZED_TARGET_LIST_WITH_REPLACEMENT_TEST(
    loadFromDirectoryPassesDirectoryToDirectoryReader) {
    loadFromDirectoryPassesDirectoryToDirectoryReader(list, reader);
}

RANDOMIZED_TARGET_LIST_WITHOUT_REPLACEMENT_TEST(
    loadFromDirectoryPassesDirectoryToDirectoryReader) {
    loadFromDirectoryPassesDirectoryToDirectoryReader(list, reader);
}

CYCLIC_RANDOMIZED_TARGET_LIST_TEST(
    loadFromDirectoryPassesDirectoryToDirectoryReader) {
    loadFromDirectoryPassesDirectoryToDirectoryReader(list, reader);
}

RANDOMIZED_TARGET_LIST_WITH_REPLACEMENT_TEST(
    loadFromDirectoryShufflesFileNames) {
    loadFromDirectoryShufflesFileNames(list, reader, randomizer);
}

RANDOMIZED_TARGET_LIST_WITHOUT_REPLACEMENT_TEST(
    loadFromDirectoryShufflesFileNames) {
    loadFromDirectoryShufflesFileNames(list, reader, randomizer);
}

CYCLIC_RANDOMIZED_TARGET_LIST_TEST(loadFromDirectoryShufflesFileNames) {
    loadFromDirectoryShufflesFileNames(list, reader, randomizer);
}

RANDOMIZED_TARGET_LIST_WITH_REPLACEMENT_TEST(
    nextShufflesNextSetNotIncludingCurrent) {
    setFileNames(reader, {"a", "b", "c", "d"});
    loadFromDirectory(list);
    next(list);
    assertShuffled(randomizer, {"b", "c", "d"});
}

RANDOMIZED_TARGET_LIST_WITH_REPLACEMENT_TEST(nextReplacesSecondToLastTarget) {
    setFileNames(reader, {"a", "b", "c", "d", "e"});
    loadFromDirectory(list);
    next(list);
    next(list);
    assertShuffled(randomizer, {"c", "d", "e", "a"});
}

RANDOMIZED_TARGET_LIST_WITH_REPLACEMENT_TEST(nextReturnsFullPathToFile) {
    nextReturnsFullPathToFile(list, reader);
}

RANDOMIZED_TARGET_LIST_WITHOUT_REPLACEMENT_TEST(
    nextReturnsFullPathToFileAtFront) {
    nextReturnsFullPathToFile(list, reader);
}

CYCLIC_RANDOMIZED_TARGET_LIST_TEST(nextReturnsFullPathToFileAtFront) {
    nextReturnsFullPathToFile(list, reader);
}

RANDOMIZED_TARGET_LIST_WITH_REPLACEMENT_TEST(currentReturnsFullPathToFile) {
    setFileNames(reader, {"a", "b", "c"});
    loadFromDirectory(list, "C:");
    next(list);
    assertCurrentEquals(list, "C:/a");
}

RANDOMIZED_TARGET_LIST_WITHOUT_REPLACEMENT_TEST(currentReturnsFullPathToFile) {
    setFileNames(reader, {"a", "b", "c"});
    loadFromDirectory(list, "C:");
    next(list);
    assertCurrentEquals(list, "C:/a");
}

CYCLIC_RANDOMIZED_TARGET_LIST_TEST(currentReturnsFullPathToFile) {
    setFileNames(reader, {"a", "b", "c"});
    loadFromDirectory(list, "C:");
    next(list);
    assertCurrentEquals(list, "C:/a");
}

RANDOMIZED_TARGET_LIST_WITH_REPLACEMENT_TEST(nextReturnsEmptyIfNoFiles) {
    setFileNames(reader, {});
    loadFromDirectory(list);
    assertNextEquals(list, "");
}

RANDOMIZED_TARGET_LIST_WITHOUT_REPLACEMENT_TEST(
    emptyWhenStimulusFilesExhausted) {
    setFileNames(reader, {"a", "b", "c"});
    loadFromDirectory(list);
    assertNotEmpty(list);
    next(list);
    assertNotEmpty(list);
    next(list);
    assertNotEmpty(list);
    next(list);
    assertEmpty(list);
}

RANDOMIZED_TARGET_LIST_WITHOUT_REPLACEMENT_TEST(nextReturnsEmptyIfNoFiles) {
    setFileNames(reader, {});
    loadFromDirectory(list);
    assertNextEquals(list, "");
}

RANDOMIZED_TARGET_LIST_WITHOUT_REPLACEMENT_TEST(reinsertCurrent) {
    setFileNames(reader, {"a", "b", "c"});
    loadFromDirectory(list, "C:");
    assertNextEquals(list, "C:/a");
    assertNextEquals(list, "C:/b");
    reinsertCurrent(list);
    assertNextEquals(list, "C:/c");
    assertNextEquals(list, "C:/b");
}

CYCLIC_RANDOMIZED_TARGET_LIST_TEST(nextCyclesBackToBeginningOfFiles) {
    setFileNames(reader, {"a", "b", "c"});
    loadFromDirectory(list, "C:");
    next(list);
    next(list);
    next(list);
    assertNextEquals(list, "C:/a");
    assertNextEquals(list, "C:/b");
    assertNextEquals(list, "C:/c");
}

auto filesIn(DirectoryReader &reader, std::string directory = {})
    -> std::vector<std::string> {
    return reader.filesIn(std::move(directory));
}

auto filter(FileFilter &filter_, std::vector<std::string> files = {})
    -> std::vector<std::string> {
    return filter_.filter(std::move(files));
}

auto subDirectories(DirectoryReader &reader, std::string directory = {})
    -> std::vector<std::string> {
    return reader.subDirectories(std::move(directory));
}

class FileFilterStub : public FileFilter {
  public:
    [[nodiscard]] auto files() const { return files_; }

    void setFiltered(std::vector<std::string> f) { filtered_ = std::move(f); }

    auto filter(std::vector<std::string> f)
        -> std::vector<std::string> override {
        files_ = std::move(f);
        return filtered_;
    }

  private:
    std::vector<std::string> filtered_;
    std::vector<std::string> files_;
};

class FileFilterDecoratorTests : public ::testing::Test {
  protected:
    DirectoryReaderStub reader;
    FileFilterStub filter;
    FileFilterDecorator decorator{&reader, &filter};
};

#define FILE_FILTER_DECORATOR_TEST(a) TEST_F(FileFilterDecoratorTests, a)

FILE_FILTER_DECORATOR_TEST(passesDirectoryToDecoratedForFiles) {
    filesIn(decorator, "a");
    assertEqual("a", directory(reader));
}

FILE_FILTER_DECORATOR_TEST(passesDirectoryToDecoratedForSubdirectories) {
    subDirectories(decorator, "a");
    assertEqual("a", directory(reader));
}

FILE_FILTER_DECORATOR_TEST(passesFilesToFilter) {
    reader.setFileNames({"a", "b", "c"});
    filesIn(decorator);
    assertEqual({"a", "b", "c"}, filter.files());
}

FILE_FILTER_DECORATOR_TEST(returnsFilteredFiles) {
    filter.setFiltered({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, filesIn(decorator));
}

FILE_FILTER_DECORATOR_TEST(returnsSubdirectories) {
    reader.setSubDirectories({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, subDirectories(decorator));
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
    assertEqual({"b.c", "f.c", "g.h"},
        filter(decorator, {"a", "b.c", "d.e", "f.c", "g.h"}));
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
    assertEqual({"ax.j", "xf.c"},
        filter(decorator, {"ax.j", "b.c", "d.e", "xf.c", "g.h"}));
}

FILE_IDENTIFIER_FILTER_TEST(returnsFilesThatEndWithIdentifier) {
    auto decorator = construct("x");
    assertEqual({"ax.j", "fx.c"},
        filter(decorator, {"ax.j", "b.c", "d.e", "fx.c", "g.h"}));
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
    assertEqual({"ax.j", "d.e"},
        filter(decorator, {"ax.j", "b1.c", "d.e", "fx2.c", "g3.h"}));
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
    filter(decorator, {"a", "b", "c"});
    assertHasBeenShuffled({0, 1, 2});
}

RANDOM_SUBSET_FILES_TEST(returnsFirstNShuffledIndexedFiles) {
    auto decorator = construct(3);
    randomizer.rotateToTheLeft(2);
    assertEqual({"c", "d", "e"}, filter(decorator, {"a", "b", "c", "d", "e"}));
}

RANDOM_SUBSET_FILES_TEST(returnsAllFilesIfLessThanAvailable) {
    auto decorator = construct(5);
    assertEqual({"a", "b", "c"}, filter(decorator, {"a", "b", "c"}));
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
            assertEqual(expected, decorated.at(i).directory());
    }

    auto decoratedAt(int n) -> auto & { return decorated.at(n); }

    void setFileNamesForDecorated(std::vector<std::string> v, int n) {
        decoratedAt(n).setFileNames(std::move(v));
    }
};

#define DIRECTORY_READER_COMPOSITE_TEST(a)                                     \
    TEST_F(DirectoryReaderCompositeTests, a)

DIRECTORY_READER_COMPOSITE_TEST(filesInPassesDirectoryToEach) {
    setDecoratedCount(3);
    auto reader = construct();
    filesIn(reader, "a");
    assertEachDecoratedDirectory("a", 3);
}

DIRECTORY_READER_COMPOSITE_TEST(filesInPassesCollectsFilesFromEach) {
    setDecoratedCount(3);
    setFileNamesForDecorated({"a"}, 0);
    setFileNamesForDecorated({"b", "c", "d"}, 1);
    setFileNamesForDecorated({"e", "f"}, 2);
    auto reader = construct();
    assertEqual({"a", "b", "c", "d", "e", "f"}, filesIn(reader));
}

TEST_F(DirectoryReaderCompositeTests,
    passesDirectoryToFirstDecoratedForSubdirectories) {
    setDecoratedCount(3);
    auto reader = construct();
    subDirectories(reader, "a");
    assertEqual("a", directory(decoratedAt(0)));
}

DIRECTORY_READER_COMPOSITE_TEST(returnsSubdirectoriesFromFirstDecorated) {
    setDecoratedCount(3);
    auto reader = construct();
    decoratedAt(0).setSubDirectories({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, subDirectories(reader));
}
}
}
