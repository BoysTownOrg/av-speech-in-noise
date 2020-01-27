#include "DirectoryReaderStub.hpp"
#include "TargetListStub.h"
#include "assert-utility.hpp"
#include <gsl/gsl>
#include <gtest/gtest.h>
#include <target-list/SubdirectoryTargetListReader.hpp>

namespace {
class TargetListFactoryStub : public target_list::TargetListFactory {
    std::vector<std::shared_ptr<av_speech_in_noise::TargetList>> lists_{};

  public:
    auto make() -> std::shared_ptr<av_speech_in_noise::TargetList> override {
        auto list = lists_.front();
        lists_.erase(lists_.begin());
        return list;
    }

    void setLists(
        std::vector<std::shared_ptr<av_speech_in_noise::TargetList>> v) {
        lists_ = std::move(v);
    }
};

class SubdirectoryTargetListReaderTests : public ::testing::Test {
  protected:
    TargetListFactoryStub targetListFactory;
    DirectoryReaderStub directoryReader;
    target_list::SubdirectoryTargetListReader listReader{
        &targetListFactory, &directoryReader};
    std::vector<std::shared_ptr<av_speech_in_noise::tests::TargetListStub>>
        targetLists;

    SubdirectoryTargetListReaderTests() { setListCount(1); }

    void setListCount(int n) {
        targetLists.clear();
        for (int i = 0; i < n; ++i)
            targetLists.push_back(
                std::make_shared<av_speech_in_noise::tests::TargetListStub>());
        targetListFactory.setLists({targetLists.begin(), targetLists.end()});
    }

    auto read(std::string s = {}) { return listReader.read(std::move(s)); }

    void setSubDirectories(std::vector<std::string> v) {
        setListCount(gsl::narrow<int>(v.size() + 1));
        directoryReader.setSubDirectories(std::move(v));
    }

    auto targetList(int n) { return targetLists.at(n); }

    auto targetListDirectory(int n) { return targetList(n)->directory(); }
};

TEST_F(SubdirectoryTargetListReaderTests, readLoadsFullPathToEachSubDirectory) {
    setSubDirectories({"a", "b", "c"});
    read("d");
    assertEqual("d/a", targetListDirectory(0));
    assertEqual("d/b", targetListDirectory(1));
    assertEqual("d/c", targetListDirectory(2));
}

TEST_F(SubdirectoryTargetListReaderTests, readPassesDirectory) {
    read("a");
    assertEqual("a", directoryReader.directory());
}

TEST_F(SubdirectoryTargetListReaderTests, readReturnsReadLists) {
    setSubDirectories(std::vector<std::string>(3));
    auto actual = read();
    EXPECT_EQ(3, actual.size());
    EXPECT_EQ(targetList(0), actual.at(0));
    EXPECT_EQ(targetList(1), actual.at(1));
    EXPECT_EQ(targetList(2), actual.at(2));
}

TEST_F(SubdirectoryTargetListReaderTests,
    readPassesParentDirectoryToFirstListIfNoSubdirectories) {
    setSubDirectories({});
    read("d");
    assertEqual("d", targetListDirectory(0));
}

TEST_F(
    SubdirectoryTargetListReaderTests, readReturnsFirstListIfNoSubdirectories) {
    setSubDirectories({});
    auto actual = read();
    EXPECT_EQ(1, actual.size());
    EXPECT_EQ(targetList(0), actual.at(0));
}
}
