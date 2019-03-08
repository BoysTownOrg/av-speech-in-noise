#include <stimulus-list/RandomizedStimulusList.hpp>
#include <stimulus-list/FileFilterDecorator.hpp>
#include <gtest/gtest.h>

class DirectoryReaderStub : public stimulus_list::DirectoryReader {
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
    
    std::string directory() const {
        return directory_;
    }
};

class RandomizerStub : public stimulus_list::Randomizer {
    std::vector<std::string> toShuffle_;
public:
    std::vector<std::string> toShuffle() const {
        return toShuffle_;
    }
    
    void shuffle(shuffle_iterator begin, shuffle_iterator end) override {
        toShuffle_ = {begin, end};
    }
};

class RandomizedStimulusListTests : public ::testing::Test {
protected:
    DirectoryReaderStub reader{};
    RandomizerStub randomizer{};
    stimulus_list::RandomizedStimulusList list;
    
    RandomizedStimulusListTests() :
        list{&reader, &randomizer} {}
};

static void assertEqual(std::string expected, std::string actual) {
    EXPECT_EQ(expected, actual);
}

TEST_F(
    RandomizedStimulusListTests,
    initializePassesDirectoryToDirectoryReader
) {
    list.initialize("a");
    assertEqual("a", reader.directory());
}

TEST_F(
    RandomizedStimulusListTests,
    testCompleteWhenStimulusFilesExhausted
) {
    reader.setFileNames({ "a", "b", "c" });
    list.initialize({});
    EXPECT_FALSE(list.empty());
    list.next();
    EXPECT_FALSE(list.empty());
    list.next();
    EXPECT_FALSE(list.empty());
    list.next();
    EXPECT_TRUE(list.empty());
}

TEST_F(
    RandomizedStimulusListTests,
    nextReturnsFullPathToFileAtFront
) {
    reader.setFileNames({ "a", "b", "c" });
    list.initialize({"C:"});
    assertEqual("C:/a", list.next());
    assertEqual("C:/b", list.next());
    assertEqual("C:/c", list.next());
}

template<typename T>
void assertEqual(std::vector<T> expected, std::vector<T> actual) {
    EXPECT_EQ(expected.size(), actual.size());
    for (typename std::vector<T>::size_type i = 0; i < expected.size(); ++i)
        EXPECT_EQ(expected.at(i), actual.at(i));
}

TEST_F(
    RandomizedStimulusListTests,
    initializeShufflesFileNames
) {
    reader.setFileNames({ "a", "b", "c" });
    list.initialize({});
    assertEqual({ "a", "b", "c" }, randomizer.toShuffle());
}

TEST(FileFilterDecoratorTests, passesDirectoryToDecorated) {
    DirectoryReaderStub reader;
    stimulus_list::FileFilterDecorator decorator{&reader, {}};
    decorator.filesIn({"a"});
    assertEqual("a", reader.directory());
}

TEST(FileFilterDecoratorTests, returnsFilteredFiles) {
    DirectoryReaderStub reader;
    stimulus_list::FileFilterDecorator decorator{&reader, ".c"};
    reader.setFileNames({ "a", "b.c", "d.e", "f.c", "g.h" });
    assertEqual({ "b.c", "f.c" }, decorator.filesIn({}));
}
