#include <av-coordinated-response-measure/Model.h>
#include <sstream>

class TimeStamp {
public:
    virtual ~TimeStamp() = default;
    virtual int year() = 0;
    virtual int month() = 0;
    virtual int dayOfMonth() = 0;
    virtual int hour() = 0;
    virtual int minute() = 0;
    virtual int second() = 0;
};

class OutputFilePath {
    TimeStamp *timeStamp;
public:
    OutputFilePath(TimeStamp *timeStamp) : timeStamp{timeStamp} {}
    
    std::string generateFileName(
        const av_coordinated_response_measure::Model::Test &test
    ) {
        std::stringstream stream;
        stream << "Subject_";
        stream << test.subject;
        stream << "_Session_";
        stream << test.session;
        stream << "_Experimenter_";
        stream << test.experimenter;
        stream << "_";
        stream << timeStamp->year();
        stream << "-";
        stream << timeStamp->month();
        stream << "-";
        stream << timeStamp->dayOfMonth();
        stream << "-";
        stream << timeStamp->hour();
        stream << "-";
        stream << timeStamp->minute();
        stream << "-";
        stream << timeStamp->second();
        return stream.str();
    }
};

#include "assert-utility.h"
#include <gtest/gtest.h>

class TimeStampStub : public TimeStamp {
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
    OutputFilePath path{&timeStamp};
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
