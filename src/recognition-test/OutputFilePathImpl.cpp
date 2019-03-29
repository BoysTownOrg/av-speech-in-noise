#include "OutputFilePathImpl.hpp"
#include <sstream>

namespace recognition_test {
    OutputFilePathImpl::OutputFilePathImpl(
        TimeStamp *timeStamp,
        FileSystemPath *systemPath
    ) :
        timeStamp{timeStamp},
        systemPath{systemPath} {}
    
    std::string OutputFilePathImpl::generateFileName(
        const av_coordinated_response_measure::Model::Test &test
    ) {
        timeStamp->capture();
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
    
    std::string OutputFilePathImpl::homeDirectory() { 
        return systemPath->homeDirectory();
    }
}
