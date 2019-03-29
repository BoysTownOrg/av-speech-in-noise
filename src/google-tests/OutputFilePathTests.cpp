#include <av-coordinated-response-measure/Model.h>

class TimeStamp {
public:
    virtual ~TimeStamp() = default;
};

class OutputFilePath {
public:
    OutputFilePath(TimeStamp *) {}
    
    std::string generateFileName(
        const av_coordinated_response_measure::Model::Test &
    ) {
        return "Subject_a_Session_b_Experimenter_c_1-2-3-4-5-6";
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
