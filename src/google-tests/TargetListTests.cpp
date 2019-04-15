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
        assertEqual({ "a", "b", "c" }, randomizer.toShuffle());
    }

    TEST_F(
        TargetListTests,
        nextShufflesNextSetNotIncludingCurrent
    ) {
        setFileNames({ "a", "b", "c", "d" });
        loadFromDirectory();
        next();
        assertEqual({ "b", "c", "d" }, randomizer.toShuffle());
    }

    TEST_F(
        TargetListTests,
        nextReplacesSecondToLastTarget
    ) {
        setFileNames({ "a", "b", "c", "d", "e" });
        loadFromDirectory();
        next();
        next();
        assertEqual({ "c", "d", "e", "a" }, randomizer.toShuffle());
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
        
        target_list::FileFilterDecorator construct(std::string filter = {}) {
            return {&reader, std::move(filter)};
        }
    };

    TEST_F(FileFilterDecoratorTests, passesDirectoryToDecorated) {
        auto decorator = construct();
        decorator.filesIn({"a"});
        assertEqual("a", reader.directory());
    }

    TEST_F(FileFilterDecoratorTests, returnsFilteredFiles) {
        auto decorator = construct(".c");
        reader.setFileNames({ "a", "b.c", "d.e", "f.c", "g.h" });
        assertEqual({ "b.c", "f.c" }, decorator.filesIn({}));
    }
}
