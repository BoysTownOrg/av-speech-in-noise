#include "OutputFilePath.hpp"
#include <sstream>

namespace av_speech_in_noise {
OutputFilePathImpl::OutputFilePathImpl(
    TimeStamp *timeStamp, FileSystemPath *systemPath)
    : outputFileName{*timeStamp}, systemPath{systemPath} {}

static auto formatTestIdentity(const TestIdentity &test) -> std::string {
    std::stringstream stream;
    stream << "Subject_";
    stream << test.subjectId;
    stream << "_Session_";
    stream << test.session;
    stream << "_Experimenter_";
    stream << test.testerId;
    return stream.str();
}

static auto format(TimeStamp &timeStamp) -> std::string {
    timeStamp.capture();
    std::stringstream stream;
    stream << timeStamp.year();
    stream << '-';
    stream << timeStamp.month();
    stream << '-';
    stream << timeStamp.dayOfMonth();
    stream << '-';
    stream << timeStamp.hour();
    stream << '-';
    stream << timeStamp.minute();
    stream << '-';
    stream << timeStamp.second();
    return stream.str();
}

OutputFileName::OutputFileName(TimeStamp &timeStamp) : timeStamp{timeStamp} {}

auto OutputFileName::generate(const TestIdentity &identity) -> std::string {
    std::stringstream stream;
    stream << formatTestIdentity(identity);
    stream << '_';
    stream << format(timeStamp);
    return stream.str();
}

auto OutputFilePathImpl::generateFileName(const TestIdentity &test)
    -> std::string {
    return outputFileName.generate(test);
}

auto OutputFilePathImpl::homeDirectory() -> std::string {
    return homeDirectory_();
}

auto OutputFilePathImpl::homeDirectory_() -> std::string {
    return systemPath->homeDirectory();
}

auto OutputFilePathImpl::outputDirectory() -> std::string {
    return outputDirectory_();
}

auto OutputFilePathImpl::outputDirectory_() -> std::string {
    return homeDirectory_() + "/" + relativePath_;
}

void OutputFilePathImpl::setRelativeOutputDirectory(std::string s) {
    relativePath_ = std::move(s);
    systemPath->createDirectory(outputDirectory_());
}
}
