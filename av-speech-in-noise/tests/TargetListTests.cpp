#include "DirectoryReaderStub.h"
#include "assert-utility.h"
#include <target-list/RandomizedTargetList.hpp>
#include <target-list/FileExtensionFilterDecorator.hpp>
#include <gtest/gtest.h>

namespace {
    class RandomizerStub : public target_list::Randomizer {
        std::vector<std::string> toShuffle_;
    public:
        auto toShuffle() const {
            return toShuffle_;
        }
        
        void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
            toShuffle_ = {begin, end};
        }
    };

    class TargetListTests : public ::testing::Test {
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
    };

    TEST_F(
        TargetListTests,
        loadFromDirectoryPassesDirectoryToDirectoryReader
    ) {
        loadFromDirectory("a");
        assertEqual("a", reader.directory());
    }

    TEST_F(
        TargetListTests,
        loadFromDirectoryShufflesFileNames
    ) {
        setFileNames({ "a", "b", "c" });
        loadFromDirectory();
        assertHasBeenShuffled({ "a", "b", "c" });
    }

    TEST_F(
        TargetListTests,
        nextShufflesNextSetNotIncludingCurrent
    ) {
        setFileNames({ "a", "b", "c", "d" });
        loadFromDirectory();
        next();
        assertHasBeenShuffled({ "b", "c", "d" });
    }

    TEST_F(
        TargetListTests,
        nextReplacesSecondToLastTarget
    ) {
        setFileNames({ "a", "b", "c", "d", "e" });
        loadFromDirectory();
        next();
        next();
        assertHasBeenShuffled({ "c", "d", "e", "a" });
    }

    TEST_F(
        TargetListTests,
        nextReturnsFullPathToFile
    ) {
        setFileNames({ "a", "b", "c" });
        loadFromDirectory("C:");
        assertEqual("C:/a", next());
        assertEqual("C:/b", next());
        assertEqual("C:/c", next());
    }

    TEST_F(
        TargetListTests,
        currentReturnsFullPathToFile
    ) {
        setFileNames({ "a", "b", "c" });
        loadFromDirectory("C:");
        next();
        assertEqual("C:/a", list.current());
    }

    TEST_F(
        TargetListTests,
        nextReturnsEmptyIfNoFiles
    ) {
        setFileNames({});
        loadFromDirectory();
        assertEqual("", next());
    }

    class FiniteTargetListTests : public ::testing::Test {
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
        FiniteTargetListTests,
        loadFromDirectoryPassesDirectoryToDirectoryReader
    ) {
        loadFromDirectory("a");
        assertEqual("a", reader.directory());
    }

    TEST_F(
        FiniteTargetListTests,
        testCompleteWhenStimulusFilesExhausted
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
        FiniteTargetListTests,
        nextReturnsFullPathToFileAtFront
    ) {
        setFileNames({ "a", "b", "c" });
        loadFromDirectory("C:");
        assertEqual("C:/a", next());
        assertEqual("C:/b", next());
        assertEqual("C:/c", next());
    }

    TEST_F(
        FiniteTargetListTests,
        loadFromDirectoryShufflesFileNames
    ) {
        setFileNames({ "a", "b", "c" });
        loadFromDirectory();
        assertEqual({ "a", "b", "c" }, randomizer.toShuffle());
    }

    TEST_F(
        FiniteTargetListTests,
        currentReturnsFullPathToFile
    ) {
        setFileNames({ "a", "b", "c" });
        loadFromDirectory("C:");
        next();
        assertEqual("C:/a", list.current());
    }

    TEST_F(
        FiniteTargetListTests,
        nextReturnsEmptyIfNoFiles
    ) {
        setFileNames({});
        loadFromDirectory();
        assertEqual("", next());
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
        decorator.filesIn({"a"});
        assertEqual("a", reader.directory());
    }

    TEST_F(FileExtensionFilterDecoratorTests, passesDirectoryToDecoratedForSubdirectories) {
        auto decorator = construct();
        decorator.subDirectories({"a"});
        assertEqual("a", reader.directory());
    }

    TEST_F(FileExtensionFilterDecoratorTests, returnsFilteredFiles) {
        auto decorator = construct({".c", ".h"});
        reader.setFileNames({ "a", "b.c", "d.e", "f.c", "g.h" });
        assertEqual({ "b.c", "f.c", "g.h" }, decorator.filesIn({}));
    }

    TEST_F(FileExtensionFilterDecoratorTests, returnsSubdirectories) {
        auto decorator = construct();
        reader.setSubDirectories({ "a", "b", "c" });
        assertEqual({ "a", "b", "c" }, decorator.subDirectories({}));
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
        assertEqual("a", reader.directory());
    }

    TEST_F(FileIdentifierFilterDecoratorTests, passesDirectoryToDecoratedForSubdirectories) {
        auto decorator = construct();
        decorator.subDirectories("a");
        assertEqual("a", reader.directory());
    }
}
