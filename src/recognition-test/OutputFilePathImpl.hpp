#ifndef av_coordinate_response_measure_OutputFilePathImpl_hpp
#define av_coordinate_response_measure_OutputFilePathImpl_hpp

#include "OutputFileImpl.hpp"
#include <av-coordinate-response-measure/Model.h>

namespace av_coordinate_response_measure {
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
        std::string generateFileName(const Test &) override;
        std::string homeDirectory() override;
        std::string outputDirectory() override;
        void setRelativeOutputDirectory(std::string);
    private:
        std::string homeDirectory_();
        std::string outputDirectory_();
        std::string formatTestInformation(const Test &);
        std::string formatTimeStamp();
    };
}

#endif
