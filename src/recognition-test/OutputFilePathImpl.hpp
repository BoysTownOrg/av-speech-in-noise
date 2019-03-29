#ifndef recognition_test_OutputFilePathImpl_hpp
#define recognition_test_OutputFilePathImpl_hpp

#include "OutputFileImpl.hpp"
#include <av-coordinated-response-measure/Model.h>

namespace recognition_test {
    class TimeStamp {
    public:
        virtual ~TimeStamp() = default;
        virtual int year() = 0;
        virtual int month() = 0;
        virtual int dayOfMonth() = 0;
        virtual int hour() = 0;
        virtual int minute() = 0;
        virtual int second() = 0;
        virtual void capture() = 0;
    };
    
    class FileSystemPath {
    public:
        virtual ~FileSystemPath() = default;
        virtual std::string homeDirectory() = 0;
    };

    class OutputFilePathImpl : public OutputFilePath {
        TimeStamp *timeStamp;
        FileSystemPath *systemPath;
    public:
        OutputFilePathImpl(TimeStamp *, FileSystemPath *);
        std::string generateFileName(
            const av_coordinated_response_measure::Model::Test &
        ) override;
        std::string homeDirectory() override;
    };
}
#endif
