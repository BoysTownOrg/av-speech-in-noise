#include "DirectoryReaderStub.hpp"
#include "TargetPlaylistStub.hpp"
#include "assert-utility.hpp"
#include <target-playlists/SubdirectoryTargetPlaylistReader.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>

namespace av_speech_in_noise {
namespace {
class TargetPlaylistFactoryStub : public TargetPlaylistFactory {
    std::vector<std::shared_ptr<av_speech_in_noise::TargetPlaylist>> lists_{};

  public:
    auto make() -> std::shared_ptr<av_speech_in_noise::TargetPlaylist> override {
        auto list = lists_.front();
        lists_.erase(lists_.begin());
        return list;
    }

    void setLists(
        std::vector<std::shared_ptr<av_speech_in_noise::TargetPlaylist>> v) {
        lists_ = std::move(v);
    }
};

class SubdirectoryTargetPlaylistReaderTests : public ::testing::Test {
  protected:
    TargetPlaylistFactoryStub targetListFactory;
    DirectoryReaderStub directoryReader;
    SubdirectoryTargetPlaylistReader listReader{
        &targetListFactory, &directoryReader};
    std::vector<std::shared_ptr<av_speech_in_noise::TargetPlaylistStub>>
        targetLists;

    SubdirectoryTargetPlaylistReaderTests() { setListCount(1); }

    void setListCount(int n) {
        targetLists.clear();
        for (int i = 0; i < n; ++i)
            targetLists.push_back(
                std::make_shared<av_speech_in_noise::TargetPlaylistStub>());
        targetListFactory.setLists({targetLists.begin(), targetLists.end()});
    }

    auto read(const std::string &s = {}) { return listReader.read({s}); }

    void setSubDirectories(std::vector<av_speech_in_noise::LocalUrl> v) {
        setListCount(gsl::narrow<int>(v.size() + 1));
        directoryReader.setSubDirectories(std::move(v));
    }

    auto targetList(int n) { return targetLists.at(n); }

    auto targetListDirectory(int n) -> std::string {
        return targetList(n)->directory().path;
    }
};

TEST_F(SubdirectoryTargetPlaylistReaderTests, readLoadsFullPathToEachSubDirectory) {
    setSubDirectories({{"a"}, {"b"}, {"c"}});
    read("d");
    assertEqual("d/a", targetListDirectory(0));
    assertEqual("d/b", targetListDirectory(1));
    assertEqual("d/c", targetListDirectory(2));
}

TEST_F(SubdirectoryTargetPlaylistReaderTests, readPassesDirectory) {
    read("a");
    assertEqual("a", directoryReader.directory());
}

TEST_F(SubdirectoryTargetPlaylistReaderTests, readReturnsReadLists) {
    setSubDirectories(std::vector<av_speech_in_noise::LocalUrl>(3));
    auto actual{read()};
    EXPECT_EQ(3, actual.size());
    EXPECT_EQ(targetList(0), actual.at(0));
    EXPECT_EQ(targetList(1), actual.at(1));
    EXPECT_EQ(targetList(2), actual.at(2));
}

TEST_F(SubdirectoryTargetPlaylistReaderTests,
    readPassesParentDirectoryToFirstListIfNoSubdirectories) {
    setSubDirectories({});
    read("d");
    assertEqual("d", targetListDirectory(0));
}

TEST_F(
    SubdirectoryTargetPlaylistReaderTests, readReturnsFirstListIfNoSubdirectories) {
    setSubDirectories({});
    auto actual{read()};
    EXPECT_EQ(1, actual.size());
    EXPECT_EQ(targetList(0), actual.at(0));
}
}
}
