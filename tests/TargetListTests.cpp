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

    void assertHasBeenShuffled(std::vector<std::string> v) {
        assertEqual(std::move(v), shuffled());
    }

    void assertEmpty() { assertTrue(empty()); }

    bool empty() { return list.empty(); }

    void assertNotEmpty() { assertFalse(empty()); }

    void reinsertCurrent() {
        list.reinsertCurrent();
    }
};

TEST_F(RandomizedTargetListTests, emptyOnlyWhenNoFilesLoaded) {
    assertEmpty();
    setFileNames({"a", "b"});
    loadFromDirectory();
    assertNotEmpty();
    next();
    assertNotEmpty();
    next();
    assertNotEmpty();
}

TEST_F(
    RandomizedTargetListTests,
    loadFromDirectoryPassesDirectoryToDirectoryReader
) {
    loadFromDirectory("a");
    assertEqual("a", reader.directory());
}

TEST_F(RandomizedTargetListTests, loadFromDirectoryShufflesFileNames) {
    setFileNames({"a", "b", "c"});
    loadFromDirectory();
    assertHasBeenShuffled({"a", "b", "c"});
}

TEST_F(RandomizedTargetListTests, nextShufflesNextSetNotIncludingCurrent) {
    setFileNames({"a", "b", "c", "d"});
    loadFromDirectory();
    next();
    assertHasBeenShuffled({"b", "c", "d"});
}

TEST_F(RandomizedTargetListTests, nextReplacesSecondToLastTarget) {
    setFileNames({"a", "b", "c", "d", "e"});
    loadFromDirectory();
    next();
    next();
    assertHasBeenShuffled({"c", "d", "e", "a"});
}

TEST_F(RandomizedTargetListTests, nextReturnsFullPathToFile) {
    setFileNames({"a", "b", "c"});
    loadFromDirectory("C:");
    assertEqual("C:/a", next());
    assertEqual("C:/b", next());
    assertEqual("C:/c", next());
}

TEST_F(RandomizedTargetListTests, currentReturnsFullPathToFile) {
    setFileNames({"a", "b", "c"});
    loadFromDirectory("C:");
    next();
    assertEqual("C:/a", list.current());
}

TEST_F(RandomizedTargetListTests, nextReturnsEmptyIfNoFiles) {
    setFileNames({});
    loadFromDirectory();
    assertEqual("", next());
}

TEST_F(RandomizedTargetListTests, reinsertCurrent) {
    setFileNames({"a", "b", "c"});
    loadFromDirectory("C:");
    assertEqual("C:/a", next());
    assertEqual("C:/b", next());
    reinsertCurrent();
    assertEqual("C:/c", next());
    assertEqual("C:/a", next());
    assertEqual("C:/b", next());
    assertEqual("C:/b", next());
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

std::vector<std::string> filesIn(
    DirectoryReader &reader, std::string directory = {}) {
    return reader.filesIn(std::move(directory));
}

std::vector<std::string> filter(
    FileFilter &filter_, std::vector<std::string> files = {}) {
    return filter_.filter(std::move(files));
}

std::vector<std::string> subDirectories(
    DirectoryReader &reader, std::string directory = {}) {
    return reader.subDirectories(std::move(directory));
}

std::string directory(DirectoryReaderStub &reader) {
    return reader.directory();
}

class FileFilterStub : public FileFilter {
    std::vector<std::string> filtered_;
    std::vector<std::string> files_;

  public:
    auto files() const { return files_; }

    void setFiltered(std::vector<std::string> f) { filtered_ = std::move(f); }

    std::vector<std::string> filter(std::vector<std::string> f) override {
        files_ = std::move(f);
        return filtered_;
    }
};

class FileFilterDecoratorTests : public ::testing::Test {
  protected:
    DirectoryReaderStub reader;
    FileFilterStub filter;
    FileFilterDecorator decorator{&reader, &filter};
};

TEST_F(FileFilterDecoratorTests, passesDirectoryToDecoratedForFiles) {
    filesIn(decorator, "a");
    assertEqual("a", directory(reader));
}

TEST_F(FileFilterDecoratorTests, passesDirectoryToDecoratedForSubdirectories) {
    subDirectories(decorator, "a");
    assertEqual("a", directory(reader));
}

TEST_F(FileFilterDecoratorTests, passesFilesToFilter) {
    reader.setFileNames({"a", "b", "c"});
    filesIn(decorator);
    assertEqual({"a", "b", "c"}, filter.files());
}

TEST_F(FileFilterDecoratorTests, returnsFilteredFiles) {
    filter.setFiltered({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, filesIn(decorator));
}

TEST_F(FileFilterDecoratorTests, returnsSubdirectories) {
    reader.setSubDirectories({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, subDirectories(decorator));
}

class FileExtensionFilterTests : public ::testing::Test {
  protected:
    static FileExtensionFilter construct(
        std::vector<std::string> filters = {}) {
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
    static FileIdentifierFilter construct(std::string indentifier = {}) {
        return FileIdentifierFilter{std::move(indentifier)};
    }
};

TEST_F(FileIdentifierFilterTests, returnsFilteredFiles) {
    auto decorator = construct("x");
    assertEqual({"ax.j", "xf.c"},
        filter(decorator, {"ax.j", "b.c", "d.e", "xf.c", "g.h"}));
}

TEST_F(FileIdentifierFilterTests, returnsFilesThatEndWithIdentifier) {
    auto decorator = construct("x");
    assertEqual({"ax.j", "fx.c"},
        filter(decorator, {"ax.j", "b.c", "d.e", "fx.c", "g.h"}));
}

class FileIdentifierExcluderFilterTests : public ::testing::Test {
  protected:
    static FileIdentifierExcluderFilter construct(
        std::vector<std::string> indentifiers = {}) {
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

    RandomSubsetFiles construct(int N = {}) { return {&randomizer, N}; }

    auto shuffled() { return randomizer.shuffledInts(); }

    void assertHasBeenShuffled(std::vector<int> v) {
        assertEqual(std::move(v), shuffled());
    }
};

TEST_F(RandomSubsetFilesTests, passesFileNumberRangeToRandomizer) {
    auto decorator = construct();
    filter(decorator, {"a", "b", "c"});
    assertHasBeenShuffled({0, 1, 2});
}

TEST_F(RandomSubsetFilesTests, returnsFirstNShuffledIndexedFiles) {
    auto decorator = construct(3);
    randomizer.rotateToTheLeft(2);
    assertEqual({"c", "d", "e"}, filter(decorator, {"a", "b", "c", "d", "e"}));
}

TEST_F(RandomSubsetFilesTests, returnsAllFilesIfLessThanAvailable) {
    auto decorator = construct(5);
    assertEqual({"a", "b", "c"}, filter(decorator, {"a", "b", "c"}));
}

class DirectoryReaderCompositeTests : public ::testing::Test {
  protected:
    std::vector<DirectoryReaderStub> decorated;

    DirectoryReaderComposite construct() {
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

    auto &decoratedAt(int n) { return decorated.at(n); }

    void setFileNamesForDecorated(std::vector<std::string> v, int n) {
        decoratedAt(n).setFileNames(std::move(v));
    }
};

TEST_F(DirectoryReaderCompositeTests, filesInPassesDirectoryToEach) {
    setDecoratedCount(3);
    auto reader = construct();
    filesIn(reader, "a");
    assertEachDecoratedDirectory("a", 3);
}

TEST_F(DirectoryReaderCompositeTests, filesInPassesCollectsFilesFromEach) {
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

TEST_F(DirectoryReaderCompositeTests, returnsSubdirectoriesFromFirstDecorated) {
    setDecoratedCount(3);
    auto reader = construct();
    decoratedAt(0).setSubDirectories({"a", "b", "c"});
    assertEqual({"a", "b", "c"}, subDirectories(reader));
}
}
}
