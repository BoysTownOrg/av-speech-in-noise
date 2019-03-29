#include "assert-utility.h"
#include <recognition-test/OutputFilePathImpl.hpp>
#include <gtest/gtest.h>

namespace {
    class TimeStampStub : public recognition_test::TimeStamp {
        int year_{};
        int month_{};
        int dayOfMonth_{};
        int hour_{};
        int minute_{};
        int second_{};
    public:
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
            return year_;
        }
        
        int month() override {
            return month_;
        }
        
        int dayOfMonth() override {
            return dayOfMonth_;
        }
        
        int hour() override {
            return hour_;
        }
        
        int minute() override {
            return minute_;
        }
        
        int second() override {
            return second_;
        }
    };

    class OutputFilePathTests : public ::testing::Test {
    protected:
        TimeStampStub timeStamp;
        recognition_test::OutputFilePath path{&timeStamp};
        av_coordinated_response_measure::Model::Test test{};
        
        std::string generateFileName() {
            return path.generateFileName(test);
        }
    };

    TEST_F(OutputFilePathTests, generateFileNameFormatsTestInformationAndTime) {
        test.subject = "a";
        test.session = "b";
        test.experimenter = "c";
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
}
