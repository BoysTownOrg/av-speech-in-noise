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
        virtual void createDirectory(std::string) = 0;
    };

    class OutputFilePathImpl : public OutputFilePath {
        std::string relativePath_{};
        TimeStamp *timeStamp;
        FileSystemPath *systemPath;
    public:
        OutputFilePathImpl(TimeStamp *, FileSystemPath *);
        std::string generateFileName(
            const av_coordinated_response_measure::Test &
        ) override;
        std::string homeDirectory() override;
        std::string outputDirectory() override;
        void setRelativeOutputDirectory(std::string);
    private:
        std::string outputDirectory_();
        std::string formatTestInformation(
            const av_coordinated_response_measure::Test &
        );
        std::string formatTimeStamp();
    };
}

#endif
