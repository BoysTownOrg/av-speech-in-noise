#ifndef recognition_test_OutputFilePathImpl_hpp
#define recognition_test_OutputFilePathImpl_hpp

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

#endif
