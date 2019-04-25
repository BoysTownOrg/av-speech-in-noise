#include "assert-utility.h"
#include <recognition-test/MultiTrackTargetList.hpp>
#include <gtest/gtest.h>

namespace {
    using namespace av_coordinate_response_measure;

    class TargetListStub : public TargetList {
        std::string directory_{};
        std::string next_{};
        std::string current_{};
        bool empty_{};
        bool nextCalled_{};
    public:
        std::string current() override {
            return current_;
        }
        /*
        void setCurrent(std::string s) {
            current_ = std::move(s);
        }
        
        auto nextCalled() const {
            return nextCalled_;
        }
        
        void setNext(std::string s) {
            next_ = std::move(s);
        }*/
        
        auto directory() const {
            return directory_;
        }
        
        void loadFromDirectory(std::string directory) override {
            directory_ = std::move(directory);
        }
        
        std::string next() override {
            nextCalled_ = true;
            return next_;
        }
    };
    
    class TargetListFactoryStub : public TargetListFactory {
        std::vector<std::shared_ptr<TargetList>> lists_{};
    public:
        std::shared_ptr<TargetList> make() override {
            auto list = lists_.front();
            lists_.erase(lists_.begin());
            return list;
        }
        
        void setLists(std::vector<std::shared_ptr<TargetList>> v) {
            lists_ = std::move(v);
        }
    };
    
    class DirectoryReaderStub : public DirectoryReader {
        std::vector<std::string> subDirectories_{};
    public:
        std::vector<std::string> subDirectories(std::string directory) override {
            return subDirectories_;
        }
        
        void setSubDirectories(std::vector<std::string> v) {
            subDirectories_ = std::move(v);
        }
    };
    
    class MultiTrackTargetListTests : public ::testing::Test {
    protected:
        TargetListFactoryStub targetListFactory;
        DirectoryReaderStub directoryReader;
        MultiTrackTargetList list{&targetListFactory, &directoryReader};
        std::vector<std::shared_ptr<TargetListStub>> lists;
        
        void setListCount(int n) {
            lists.clear();
            for (int i = 0; i < n; ++i)
                lists.push_back(std::make_shared<TargetListStub>());
            targetListFactory.setLists({lists.begin(), lists.end()});
        }
    };
    
    TEST_F(MultiTrackTargetListTests, loadFromDirectoryLoadsEachSubDirectory) {
        setListCount(3);
        directoryReader.setSubDirectories({"a", "b", "c"});
        list.loadFromDirectory({});
        assertEqual("a", lists.at(0)->directory());
        assertEqual("b", lists.at(1)->directory());
        assertEqual("c", lists.at(2)->directory());
    }
}
