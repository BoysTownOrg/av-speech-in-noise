#include "TargetListStub.h"
#include "DirectoryReaderStub.h"
#include "assert-utility.h"
#include <target-list/SubdirectoryTargetListReader.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>

namespace {
    class TargetListFactoryStub : public target_list::TargetListFactory {
        std::vector<std::shared_ptr<av_speech_in_noise::TargetList>> lists_{};
    public:
        std::shared_ptr<av_speech_in_noise::TargetList> make() override {
            auto list = lists_.front();
            lists_.erase(lists_.begin());
            return list;
        }
        
        void setLists(std::vector<std::shared_ptr<av_speech_in_noise::TargetList>> v) {
            lists_ = std::move(v);
        }
    };
    
    class SubdirectoryTargetListReaderTests : public ::testing::Test {
    protected:
        TargetListFactoryStub targetListFactory;
        DirectoryReaderStub directoryReader;
        target_list::SubdirectoryTargetListReader listReader{
            &targetListFactory,
            &directoryReader
        };
        std::vector<std::shared_ptr<TargetListStub>> targetLists;
        
        SubdirectoryTargetListReaderTests() {
            setListCount(1);
        }
        
        void setListCount(int n) {
            targetLists.clear();
            for (int i = 0; i < n; ++i)
                targetLists.push_back(std::make_shared<TargetListStub>());
            targetListFactory.setLists({targetLists.begin(), targetLists.end()});
        }
        
        auto read(std::string s = {}) {
            return listReader.read(std::move(s));
        }
        
        void setSubDirectories(std::vector<std::string> v) {
            setListCount(gsl::narrow<int>(v.size() + 1));
            directoryReader.setSubDirectories(std::move(v));
        }
        
        auto targetListDirectory(int n) {
            return targetLists.at(n)->directory();
        }
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
        EXPECT_EQ(targetLists.at(0), actual.at(0));
        EXPECT_EQ(targetLists.at(1), actual.at(1));
        EXPECT_EQ(targetLists.at(2), actual.at(2));
    }
    
    TEST_F(
        SubdirectoryTargetListReaderTests,
        readPassesParentDirectoryToFirstListIfNoSubdirectories
    ) {
        setSubDirectories({});
        read("d");
        assertEqual("d", targetListDirectory(0));
    }
    
    TEST_F(
        SubdirectoryTargetListReaderTests,
        readReturnsFirstListIfNoSubdirectories
    ) {
        setSubDirectories({});
        auto actual = read();
        EXPECT_EQ(1, actual.size());
        EXPECT_EQ(targetLists.at(0), actual.at(0));
    }
}
