#include "OutputFilePathImpl.hpp"
#include <sstream>

namespace av_coordinate_response_measure {
    OutputFilePathImpl::OutputFilePathImpl(
        TimeStamp *timeStamp,
        FileSystemPath *systemPath
    ) :
        timeStamp{timeStamp},
        systemPath{systemPath} {}
    
    std::string OutputFilePathImpl::generateFileName(const Test &test) {
        std::stringstream stream;
        stream << formatTestInformation(test);
        stream << '_';
        stream << formatTimeStamp();
        return stream.str();
    }
    
    std::string OutputFilePathImpl::formatTestInformation(const Test &test) {
        std::stringstream stream;
        stream << "Subject_";
        stream << test.subjectId;
        stream << "_Session_";
        stream << test.session;
        stream << "_Experimenter_";
        stream << test.testerId;
        return stream.str();
    }
    
    std::string OutputFilePathImpl::formatTimeStamp() {
        timeStamp->capture();
        std::stringstream stream;
        stream << timeStamp->year();
        stream << '-';
        stream << timeStamp->month();
        stream << '-';
        stream << timeStamp->dayOfMonth();
        stream << '-';
        stream << timeStamp->hour();
        stream << '-';
        stream << timeStamp->minute();
        stream << '-';
        stream << timeStamp->second();
        return stream.str();
    }
    
    std::string OutputFilePathImpl::homeDirectory() { 
        return systemPath->homeDirectory();
    }
    
    std::string OutputFilePathImpl::outputDirectory() {
        return outputDirectory_();
    }
    
    std::string OutputFilePathImpl::outputDirectory_() {
        return systemPath->homeDirectory() + "/" + relativePath_;
    }
    
    void OutputFilePathImpl::setRelativeOutputDirectory(std::string s) {
        relativePath_ = std::move(s);
        systemPath->createDirectory(outputDirectory_());
    }
}
