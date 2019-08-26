#include "DirectoryReaderStub.h"
#include "assert-utility.h"
#include <target-list/RandomizedTargetList.hpp>
#include <target-list/FileExtensionFilterDecorator.hpp>
#include <gtest/gtest.h>
#include <algorithm>

namespace {
    class RandomizerStub : public target_list::Randomizer {
        std::vector<std::string> toShuffle_;
        std::vector<int> shuffledInts_;
        int rotateToTheLeft_{};
    public:
        auto toShuffle() const {
            return toShuffle_;
        }

        auto shuffledInts() const {
            return shuffledInts_;
        }
        
        void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
            toShuffle_ = {begin, end};
        }
        
        void shuffle(int_shuffle_iterator begin, int_shuffle_iterator end) override {
            shuffledInts_ = {begin, end};
            std::rotate(begin, begin + rotateToTheLeft_, end);
        }

        void rotateToTheLeft(int N) {
            rotateToTheLeft_ = N;
        }
    };

    class RandomizedTargetListTests : public ::testing::Test {
    protected:
        DirectoryReaderStub reader;
        RandomizerStub randomizer;
        target_list::RandomizedTargetList list{&reader, &randomizer};
        
        void loadFromDirectory(std::string s = {}) {
            list.loadFromDirectory(std::move(s));
        }
        
        auto next() {
            return list.next();
        }
        
        void setFileNames(std::vector<std::string> v) {
            reader.setFileNames(std::move(v));
        }
        
        auto shuffled() {
            return randomizer.toShuffle();
        }
        
        void assertHasBeenShuffled(std::vector<std::string> v) {
            assertEqual(std::move(v), shuffled());
        }

        void assertEmpty() {
            assertTrue(empty());
        }

        bool empty() {
            return list.empty();
        }

        void assertNotEmpty() {
            assertFalse(empty());
        }
    };

    TEST_F(RandomizedTargetListTests, emptyOnlyWhenNoFilesLoaded) {
        assertEmpty();
        setFileNames({ "a", "b" });
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

    TEST_F(
        RandomizedTargetListTests,
        loadFromDirectoryShufflesFileNames
    ) {
        setFileNames({ "a", "b", "c" });
        loadFromDirectory();
        assertHasBeenShuffled({ "a", "b", "c" });
    }

    TEST_F(
        RandomizedTargetListTests,
        nextShufflesNextSetNotIncludingCurrent
    ) {
        setFileNames({ "a", "b", "c", "d" });
        loadFromDirectory();
        next();
        assertHasBeenShuffled({ "b", "c", "d" });
    }

    TEST_F(
        RandomizedTargetListTests,
        nextReplacesSecondToLastTarget
    ) {
        setFileNames({ "a", "b", "c", "d", "e" });
        loadFromDirectory();
        next();
        next();
        assertHasBeenShuffled({ "c", "d", "e", "a" });
    }

    TEST_F(
        RandomizedTargetListTests,
        nextReturnsFullPathToFile
    ) {
        setFileNames({ "a", "b", "c" });
        loadFromDirectory("C:");
        assertEqual("C:/a", next());
        assertEqual("C:/b", next());
        assertEqual("C:/c", next());
    }

    TEST_F(
        RandomizedTargetListTests,
        currentReturnsFullPathToFile
    ) {
        setFileNames({ "a", "b", "c" });
        loadFromDirectory("C:");
        next();
        assertEqual("C:/a", list.current());
    }

    TEST_F(
        RandomizedTargetListTests,
        nextReturnsEmptyIfNoFiles
    ) {
        setFileNames({});
        loadFromDirectory();
        assertEqual("", next());
    }

    class FiniteRandomizedTargetListTests : public ::testing::Test {
    protected:
        DirectoryReaderStub reader;
        RandomizerStub randomizer;
        target_list::RandomizedFiniteTargetList list{&reader, &randomizer};
        
        void loadFromDirectory(std::string s = {}) {
            list.loadFromDirectory(std::move(s));
        }
        
        auto next() {
            return list.next();
        }
        
        auto empty() {
            return list.empty();
        }
        
        void setFileNames(std::vector<std::string> v) {
            reader.setFileNames(std::move(v));
        }
        
        void assertNotEmpty() {
            EXPECT_FALSE(empty());
        }
        
        void assertEmpty() {
            EXPECT_TRUE(empty());
        }
    };

    TEST_F(
        FiniteRandomizedTargetListTests,
        loadFromDirectoryPassesDirectoryToDirectoryReader
    ) {
        loadFromDirectory("a");
        assertEqual("a", reader.directory());
    }

    TEST_F(
        FiniteRandomizedTargetListTests,
        emptyWhenStimulusFilesExhausted
    ) {
        setFileNames({ "a", "b", "c" });
        loadFromDirectory();
        assertNotEmpty();
        next();
        assertNotEmpty();
        next();
        assertNotEmpty();
        next();
        assertEmpty();
    }

    TEST_F(
        FiniteRandomizedTargetListTests,
        nextReturnsFullPathToFileAtFront
    ) {
        setFileNames({ "a", "b", "c" });
        loadFromDirectory("C:");
        assertEqual("C:/a", next());
        assertEqual("C:/b", next());
        assertEqual("C:/c", next());
    }

    TEST_F(
        FiniteRandomizedTargetListTests,
        loadFromDirectoryShufflesFileNames
    ) {
        setFileNames({ "a", "b", "c" });
        loadFromDirectory();
        assertEqual({ "a", "b", "c" }, randomizer.toShuffle());
    }

    TEST_F(
        FiniteRandomizedTargetListTests,
        currentReturnsFullPathToFile
    ) {
        setFileNames({ "a", "b", "c" });
        loadFromDirectory("C:");
        next();
        assertEqual("C:/a", list.current());
    }

    TEST_F(
        FiniteRandomizedTargetListTests,
        nextReturnsEmptyIfNoFiles
    ) {
        setFileNames({});
        loadFromDirectory();
        assertEqual("", next());
    }
    
    std::vector<std::string> filesIn(
        target_list::DirectoryReader &reader, 
        std::string directory = {}
    ) {
        return reader.filesIn(std::move(directory));
    }
    
    std::vector<std::string> subDirectories(
        target_list::DirectoryReader &reader, 
        std::string directory = {}
    ) {
        return reader.subDirectories(std::move(directory));
    }

    std::string directory(DirectoryReaderStub &reader) {
        return reader.directory();
    }
    
    class FileExtensionFilterDecoratorTests : public ::testing::Test {
    protected:
        DirectoryReaderStub reader;
        
        target_list::FileExtensionFilterDecorator construct(
            std::vector<std::string> filters = {}
        ) {
            return {&reader, std::move(filters)};
        }
    };

    TEST_F(FileExtensionFilterDecoratorTests, passesDirectoryToDecoratedForFiles) {
        auto decorator = construct();
        filesIn(decorator, "a");
        assertEqual("a", directory(reader));
    }

    TEST_F(FileExtensionFilterDecoratorTests, passesDirectoryToDecoratedForSubdirectories) {
        auto decorator = construct();
        subDirectories(decorator, "a");
        assertEqual("a", directory(reader));
    }

    TEST_F(FileExtensionFilterDecoratorTests, returnsFilteredFiles) {
        auto decorator = construct({".c", ".h"});
        reader.setFileNames({ "a", "b.c", "d.e", "f.c", "g.h" });
        assertEqual({ "b.c", "f.c", "g.h" }, filesIn(decorator));
    }

    TEST_F(FileExtensionFilterDecoratorTests, returnsSubdirectories) {
        auto decorator = construct();
        reader.setSubDirectories({ "a", "b", "c" });
        assertEqual({ "a", "b", "c" }, subDirectories(decorator));
    }
    
    
    class FileIdentifierFilterDecoratorTests : public ::testing::Test {
    protected:
        DirectoryReaderStub reader;
        
        target_list::FileIdentifierFilterDecorator construct(
            std::string indentifier = {}
        ) {
            return {&reader, std::move(indentifier)};
        }
    };

    TEST_F(FileIdentifierFilterDecoratorTests, passesDirectoryToDecoratedForFiles) {
        auto decorator = construct();
        decorator.filesIn("a");
        assertEqual("a", directory(reader));
    }

    TEST_F(FileIdentifierFilterDecoratorTests, passesDirectoryToDecoratedForSubdirectories) {
        auto decorator = construct();
        decorator.subDirectories("a");
        assertEqual("a", directory(reader));
    }

    TEST_F(FileIdentifierFilterDecoratorTests, returnsSubdirectories) {
        auto decorator = construct();
        reader.setSubDirectories({ "a", "b", "c" });
        assertEqual({ "a", "b", "c" }, subDirectories(decorator));
    }

    TEST_F(FileIdentifierFilterDecoratorTests, returnsFilteredFiles) {
        auto decorator = construct("x");
        reader.setFileNames({ "ax.j", "b.c", "d.e", "xf.c", "g.h" });
        assertEqual({ "ax.j", "xf.c" }, filesIn(decorator));
    }
    
    
    class RandomSubsetFilesDecoratorTests : public ::testing::Test {
    protected:
        DirectoryReaderStub reader;
        RandomizerStub randomizer;
        
        target_list::RandomSubsetFilesDecorator construct(
            int N = {}
        ) {
            return {&reader, &randomizer, N};
        }
        
        auto shuffled() {
            return randomizer.shuffledInts();
        }
        
        void assertHasBeenShuffled(std::vector<int> v) {
            assertEqual(std::move(v), shuffled());
        }
    };

    TEST_F(RandomSubsetFilesDecoratorTests, passesDirectoryToDecoratedForFiles) {
        auto decorator = construct();
        filesIn(decorator, "a");
        assertEqual("a", directory(reader));
    }

    TEST_F(RandomSubsetFilesDecoratorTests, passesDirectoryToDecoratedForSubdirectories) {
        auto decorator = construct();
        subDirectories(decorator, "a");
        assertEqual("a", directory(reader));
    }

    TEST_F(RandomSubsetFilesDecoratorTests, returnsSubdirectories) {
        auto decorator = construct();
        reader.setSubDirectories({ "a", "b", "c" });
        assertEqual({ "a", "b", "c" }, subDirectories(decorator));
    }

    TEST_F(RandomSubsetFilesDecoratorTests, passesFileNumberRangeToRandomizer) {
        auto decorator = construct();
        reader.setFileNames({"a", "b", "c"});
        filesIn(decorator);
        assertHasBeenShuffled({ 0, 1, 2 });
    }

    TEST_F(RandomSubsetFilesDecoratorTests, returnsFirstNShuffledIndexedFiles) {
        auto decorator = construct(3);
        reader.setFileNames({"a", "b", "c", "d", "e"});
        randomizer.rotateToTheLeft(2);
        assertEqual({ "c", "d", "e" }, filesIn(decorator));
    }
    
    class DirectoryReaderCompositeTests : public ::testing::Test {
    protected:
        std::vector<DirectoryReaderStub> decorated;

        target_list::DirectoryReaderComposite construct() {
            std::vector<target_list::DirectoryReader *> ptrs;
            for (auto &d : decorated)
                ptrs.push_back(&d);
            return target_list::DirectoryReaderComposite{ptrs};
        }

        void setDecoratedCount(int N) {
            decorated.resize(N);
        }

        void assertEachDecoratedDirectory(const std::string &expected, int N) {
            for (int i = 0; i < N; ++i)
                assertEqual(expected, decorated.at(i).directory());
        }

        auto &decoratedAt(int n) {
            return decorated.at(n);
        }

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
        setFileNamesForDecorated({ "a" }, 0);
        setFileNamesForDecorated({ "b", "c", "d" }, 1);
        setFileNamesForDecorated({ "e", "f" }, 2);
        auto reader = construct();
        assertEqual({"a", "b", "c", "d", "e", "f"}, filesIn(reader));
    }

    TEST_F(DirectoryReaderCompositeTests, passesDirectoryToFirstDecoratedForSubdirectories) {
        setDecoratedCount(3);
        auto reader = construct();
        subDirectories(reader, "a");
        assertEqual("a", directory(decoratedAt(0)));
    }

    TEST_F(DirectoryReaderCompositeTests, returnsSubdirectoriesFromFirstDecorated) {
        setDecoratedCount(3);
        auto reader = construct();
        decoratedAt(0).setSubDirectories({ "a", "b", "c" });
        assertEqual({ "a", "b", "c" }, subDirectories(reader));
    }
}
