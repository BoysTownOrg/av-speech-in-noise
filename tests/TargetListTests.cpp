#include "DirectoryReaderStub.h"
#include "assert-utility.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <target-list/FileFilterDecorator.hpp>
#include <target-list/RandomizedTargetList.hpp>

namespace target_list::tests {
namespace {
class RandomizerStub : public Randomizer {
    std::vector<std::string> toShuffle_;
    std::vector<int> shuffledInts_;
    int rotateToTheLeft_{};

  public:
    [[nodiscard]] auto toShuffle() const { return toShuffle_; }

    [[nodiscard]] auto shuffledInts() const { return shuffledInts_; }

    void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
        toShuffle_ = {begin, end};
    }

    void shuffle(
        int_shuffle_iterator begin, int_shuffle_iterator end) override {
        shuffledInts_ = {begin, end};
        std::rotate(begin, begin + rotateToTheLeft_, end);
    }

    void rotateToTheLeft(int N) { rotateToTheLeft_ = N; }
};

class RandomizedTargetListTests : public ::testing::Test {
  protected:
    DirectoryReaderStub reader;
    RandomizerStub randomizer;
    RandomizedTargetList list{&reader, &randomizer};

    void loadFromDirectory(std::string s = {}) {
        list.loadFromDirectory(std::move(s));
    }

    auto next() { return list.next(); }

    void setFileNames(std::vector<std::string> v) {
        reader.setFileNames(std::move(v));
    }

    auto shuffled() { return randomizer.toShuffle(); }

    void assertHasBeenShuffled(const std::vector<std::string> &v) {
        assertEqual(v, shuffled());
    }

    void assertEmpty() { assertTrue(empty()); }

    auto empty() -> bool { return list.empty(); }

    void assertNotEmpty() { assertFalse(empty()); }

    void reinsertCurrent() { list.reinsertCurrent(); }

    void assertNextEquals(const std::string &s) { assertEqual(s, next()); }
};

#define RANDOMIZED_TARGET_LIST_TEST(a) TEST_F(RandomizedTargetListTests, a)

RANDOMIZED_TARGET_LIST_TEST(emptyOnlyWhenNoFilesLoaded) {
    assertEmpty();
    setFileNames({"a", "b"});
    loadFromDirectory();
    assertNotEmpty();
    next();
    assertNotEmpty();
    next();
    assertNotEmpty();
}

RANDOMIZED_TARGET_LIST_TEST(loadFromDirectoryPassesDirectoryToDirectoryReader) {
    loadFromDirectory("a");
    assertEqual("a", reader.directory());
}

RANDOMIZED_TARGET_LIST_TEST(loadFromDirectoryShufflesFileNames) {
    setFileNames({"a", "b", "c"});
    loadFromDirectory();
    assertHasBeenShuffled({"a", "b", "c"});
}

RANDOMIZED_TARGET_LIST_TEST(nextShufflesNextSetNotIncludingCurrent) {
    setFileNames({"a", "b", "c", "d"});
    loadFromDirectory();
    next();
    assertHasBeenShuffled({"b", "c", "d"});
}

RANDOMIZED_TARGET_LIST_TEST(nextReplacesSecondToLastTarget) {
    setFileNames({"a", "b", "c", "d", "e"});
    loadFromDirectory();
    next();
    next();
    assertHasBeenShuffled({"c", "d", "e", "a"});
}

RANDOMIZED_TARGET_LIST_TEST(nextReturnsFullPathToFile) {
    setFileNames({"a", "b", "c"});
    loadFromDirectory("C:");
    assertNextEquals("C:/a");
    assertNextEquals("C:/b");
    assertNextEquals("C:/c");
}

RANDOMIZED_TARGET_LIST_TEST(currentReturnsFullPathToFile) {
    setFileNames({"a", "b", "c"});
    loadFromDirectory("C:");
    next();
    assertEqual("C:/a", list.current());
}

RANDOMIZED_TARGET_LIST_TEST(nextReturnsEmptyIfNoFiles) {
    setFileNames({});
    loadFromDirectory();
    assertNextEquals("");
}

class FiniteRandomizedTargetListTests : public ::testing::Test {
  protected:
    DirectoryReaderStub reader;
    RandomizerStub randomizer;
    RandomizedFiniteTargetList list{&reader, &randomizer};

    void loadFromDirectory(std::string s = {}) {
        list.loadFromDirectory(std::move(s));
    }

    auto next() { return list.next(); }

    auto empty() { return list.empty(); }

    void setFileNames(std::vector<std::string> v) {
        reader.setFileNames(std::move(v));
    }

    void assertNotEmpty() { EXPECT_FALSE(empty()); }

    void assertEmpty() { EXPECT_TRUE(empty()); }

    void reinsertCurrent() { list.reinsertCurrent(); }

    void assertNextEquals(const std::string &s) { assertEqual(s, next()); }
};

#define FINITE_RANDOMIZED_TARGET_LIST_TEST(a)                                  \
    TEST_F(FiniteRandomizedTargetListTests, a)

FINITE_RANDOMIZED_TARGET_LIST_TEST(
    loadFromDirectoryPassesDirectoryToDirectoryReader) {
    loadFromDirectory("a");
    assertEqual("a", reader.directory());
}

FINITE_RANDOMIZED_TARGET_LIST_TEST(emptyWhenStimulusFilesExhausted) {
    setFileNames({"a", "b", "c"});
    loadFromDirectory();
    assertNotEmpty();
    next();
    assertNotEmpty();
    next();
    assertNotEmpty();
    next();
    assertEmpty();
}

FINITE_RANDOMIZED_TARGET_LIST_TEST(nextReturnsFullPathToFileAtFront) {
    setFileNames({"a", "b", "c"});
    loadFromDirectory("C:");
    assertNextEquals("C:/a");
    assertNextEquals("C:/b");
    assertNextEquals("C:/c");
}

FINITE_RANDOMIZED_TARGET_LIST_TEST(loadFromDirectoryShufflesFileNames) {
    setFileNames({"a", "b", "c"});
    loadFromDirectory();
    assertEqual({"a", "b", "c"}, randomizer.toShuffle());
}

FINITE_RANDOMIZED_TARGET_LIST_TEST(currentReturnsFullPathToFile) {
    setFileNames({"a", "b", "c"});
    loadFromDirectory("C:");
    next();
    assertEqual("C:/a", list.current());
}

FINITE_RANDOMIZED_TARGET_LIST_TEST(nextReturnsEmptyIfNoFiles) {
    setFileNames({});
    loadFromDirectory();
    assertNextEquals("");
}

FINITE_RANDOMIZED_TARGET_LIST_TEST(reinsertCurrent) {
    setFileNames({"a", "b", "c"});
    loadFromDirectory("C:");
    assertNextEquals("C:/a");
    assertNextEquals("C:/b");
    reinsertCurrent();
    assertNextEquals("C:/c");
    assertNextEquals("C:/b");
}

void loadFromDirectory(CyclicRandomizedTargetList &list, std::string s) {
    list.loadFromDirectory(std::move(s));
}

class CyclicRandomizedTargetListTests : public ::testing::Test {
  protected:
    DirectoryReaderStub reader;
    RandomizerStub randomizer;
    CyclicRandomizedTargetList list{&reader, &randomizer};
};

TEST_F(CyclicRandomizedTargetListTests,
    loadFromDirectoryPassesDirectoryToDirectoryReader) {
    loadFromDirectory(list, "a");
    assertEqual("a", reader.directory());
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

auto directory(DirectoryReaderStub &reader) -> std::string {
    return reader.directory();
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
