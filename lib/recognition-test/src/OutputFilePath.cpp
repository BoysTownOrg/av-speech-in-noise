#include "OutputFilePath.hpp"
#include <sstream>

namespace av_speech_in_noise {
OutputFilePathImpl::OutputFilePathImpl(
    OutputFileName &outputFileName, FileSystemPath &systemPath)
    : outputFileName{outputFileName}, systemPath{systemPath} {}

static auto format(const TestIdentity &test) -> std::string {
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

DefaultOutputFileName::DefaultOutputFileName(TimeStamp &timeStamp)
    : timeStamp{timeStamp} {}

auto DefaultOutputFileName::generate(const TestIdentity &identity)
    -> std::string {
    std::stringstream stream;
    stream << format(identity);
    stream << '_';
    stream << format(timeStamp);
    return stream.str();
}

MetaConditionOutputFileName::MetaConditionOutputFileName(TimeStamp &timeStamp)
    : timeStamp{timeStamp} {}

auto MetaConditionOutputFileName::generate(const TestIdentity &identity)
    -> std::string {
    std::stringstream stream;
    stream << "condition" << identity.meta << "_" << identity.subjectId;
    stream << '_';
    stream << format(timeStamp);
    return stream.str();
}

auto OutputFilePathImpl::generateFileName(const TestIdentity &identity)
    -> std::string {
    return outputFileName.generate(identity);
}

auto OutputFilePathImpl::outputDirectory() -> std::string {
    return outputDirectory_();
}

auto OutputFilePathImpl::outputDirectory_() -> std::string {
    return systemPath.homeDirectory() / relativeOutputDirectory;
}

void OutputFilePathImpl::setRelativeOutputDirectory(std::filesystem::path s) {
    relativeOutputDirectory = std::move(s);
    systemPath.createDirectory(outputDirectory_());
}
}
