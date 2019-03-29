#include "assert-utility.h"
#include "LogString.h"
#include <recognition-test/OutputFilePathImpl.hpp>
#include <gtest/gtest.h>

namespace {
    class TimeStampStub : public recognition_test::TimeStamp {
        LogString log_{};
        int year_{};
        int month_{};
        int dayOfMonth_{};
        int hour_{};
        int minute_{};
        int second_{};
    public:
        void capture() override {
            log_.insert("capture ");
        }
        
        auto &log() const {
            return log_;
        }
        
        void setYear(int y) {
            year_ = y;
        }
        
        void setMonth(int y) {
            month_ = y;
        }
        
        void setDayOfMonth(int y) {
            dayOfMonth_ = y;
        }
        
        void setHour(int y) {
            hour_ = y;
        }
        
        void setMinute(int y) {
            minute_ = y;
        }
        
        void setSecond(int y) {
            second_ = y;
        }
        
        int year() override {
            log_.insert("year ");
            return year_;
        }
        
        int month() override {
            log_.insert("month ");
            return month_;
        }
        
        int dayOfMonth() override {
            log_.insert("dayOfMonth ");
            return dayOfMonth_;
        }
        
        int hour() override {
            log_.insert("hour ");
            return hour_;
        }
        
        int minute() override {
            log_.insert("minute ");
            return minute_;
        }
        
        int second() override {
            log_.insert("second ");
            return second_;
        }
    };
    
    class FileSystemPathStub : public recognition_test::FileSystemPath {
        std::string homeDirectory_{};
    public:
        void setHomeDirectory(std::string s) {
            homeDirectory_ = s;
        }
        
        std::string homeDirectory() override {
            return homeDirectory_;
        }
    };

    class OutputFilePathTests : public ::testing::Test {
    protected:
        TimeStampStub timeStamp;
        FileSystemPathStub systemPath;
        recognition_test::OutputFilePathImpl path{&timeStamp, &systemPath};
        av_coordinated_response_measure::Model::Test test{};
        
        std::string generateFileName() {
            return path.generateFileName(test);
        }
    };

    TEST_F(OutputFilePathTests, generateFileNameFormatsTestInformationAndTime) {
        test.subjectId = "a";
        test.session = "b";
        test.testerId = "c";
        timeStamp.setYear(1);
        timeStamp.setMonth(2);
        timeStamp.setDayOfMonth(3);
        timeStamp.setHour(4);
        timeStamp.setMinute(5);
        timeStamp.setSecond(6);
        assertEqual(
            "Subject_a_Session_b_Experimenter_c_1-2-3-4-5-6",
            generateFileName()
        );
    }

    TEST_F(OutputFilePathTests, generateFileNameCapturesTimePriorToQueries) {
        generateFileName();
        EXPECT_TRUE(timeStamp.log().beginsWith("capture"));
    }

    TEST_F(OutputFilePathTests, homeDirectoryReturnsFromSystem) {
        systemPath.setHomeDirectory("a");
        assertEqual("a", path.homeDirectory());
    }
}
