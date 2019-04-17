#include "assert-utility.h"
#include <target-list/RandomizedTargetList.hpp>
#include <target-list/FileFilterDecorator.hpp>
#include <gtest/gtest.h>

namespace {
    class DirectoryReaderStub : public target_list::DirectoryReader {
        std::vector<std::string> fileNames_{};
        std::string directory_{};
    public:
        void setFileNames(std::vector<std::string> files) {
            fileNames_ = files;
        }
        
        std::vector<std::string> filesIn(std::string directory) override {
            directory_ = directory;
            return fileNames_;
        }
        
        auto directory() const {
            return directory_;
        }
    };

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
        DirectoryReaderStub reader{};
        RandomizerStub randomizer{};
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
    
    class FileFilterDecoratorTests : public ::testing::Test {
    protected:
        DirectoryReaderStub reader{};
        
        target_list::FileFilterDecorator construct(
            std::vector<std::string> filters = {}
        ) {
            return {&reader, std::move(filters)};
        }
    };

    TEST_F(FileFilterDecoratorTests, passesDirectoryToDecorated) {
        auto decorator = construct();
        decorator.filesIn({"a"});
        assertEqual("a", reader.directory());
    }

    TEST_F(FileFilterDecoratorTests, returnsFilteredFiles) {
        auto decorator = construct({".c", ".h"});
        reader.setFileNames({ "a", "b.c", "d.e", "f.c", "g.h" });
        assertEqual({ "b.c", "f.c", "g.h" }, decorator.filesIn({}));
    }
}
