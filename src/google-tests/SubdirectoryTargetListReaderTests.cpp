#include "TargetListStub.h"
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
    
    class DirectoryReaderStub : public target_list::DirectoryReader {
        std::vector<std::string> subDirectories_{};
        std::string directory_{};
    public:
        std::vector<std::string> subDirectories(std::string d) override {
            directory_ = std::move(d);
            return subDirectories_;
        }
        
        std::vector<std::string> filesIn(std::string directory) override {
            return {};
        }
        
        auto directory() const {
            return directory_;
        }
        
        void setSubDirectories(std::vector<std::string> v) {
            subDirectories_ = std::move(v);
        }
    };
    
    class SubdirectoryTargetListReaderTests : public ::testing::Test {
    protected:
        TargetListFactoryStub targetListFactory;
        DirectoryReaderStub directoryReader;
        target_list::SubdirectoryTargetListReader listReader{&targetListFactory, &directoryReader};
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
    };
    
    TEST_F(SubdirectoryTargetListReaderTests, readLoadsFullPathToEachSubDirectory) {
        setSubDirectories({"a", "b", "c"});
        read("d");
        assertEqual("d/a", targetLists.at(0)->directory());
        assertEqual("d/b", targetLists.at(1)->directory());
        assertEqual("d/c", targetLists.at(2)->directory());
    }
    
    TEST_F(SubdirectoryTargetListReaderTests, readPassesDirectory) {
        read("a");
        assertEqual("a", directoryReader.directory());
    }
    
    TEST_F(SubdirectoryTargetListReaderTests, readReturnsReadLists) {
        setSubDirectories(std::vector<std::string>(3));
        auto actual = read();
        EXPECT_EQ(3, actual.size());
        EXPECT_EQ(targetLists.at(0).get(), actual.at(0).get());
        EXPECT_EQ(targetLists.at(1).get(), actual.at(1).get());
        EXPECT_EQ(targetLists.at(2).get(), actual.at(2).get());
    }
    
    TEST_F(
        SubdirectoryTargetListReaderTests,
        readPassesParentDirectoryToFirstListIfNoSubdirectories
    ) {
        setSubDirectories({});
        read("d");
        assertEqual("d", targetLists.at(0)->directory());
    }
    
    TEST_F(
        SubdirectoryTargetListReaderTests,
        readReturnsFirstListIfNoSubdirectories
    ) {
        setSubDirectories({});
        auto actual = read();
        EXPECT_EQ(1, actual.size());
        EXPECT_EQ(targetLists.at(0).get(), actual.at(0).get());
    }
}
