#include "ResponseEvaluatorImpl.hpp"

namespace av_speech_in_noise {
    int ResponseEvaluatorImpl::invalidNumber = -1;
    
    bool ResponseEvaluatorImpl::correct(
        const std::string &filePath,
        const coordinate_response_measure::SubjectResponse &r
    ) {
        return
            correctNumber(filePath) == r.number &&
            correctColor(filePath) == r.color &&
            r.color != coordinate_response_measure::Color::notAColor &&
            r.number != invalidNumber;
    }
    int ResponseEvaluatorImpl::correctNumber(const std::string &filePath) {
        auto extension = filePath.find(".");
        if (extension == std::string::npos)
            return invalidNumber;
        auto beforeExtension = extension - 1;
        auto number = filePath.substr(beforeExtension, 1);
        try {
            return std::stoi(number);
        }
        catch (const std::invalid_argument &) {
            return invalidNumber;
        }
    }
    
    coordinate_response_measure::Color ResponseEvaluatorImpl::correctColor(const std::string &filePath) {
        auto fileSeparator = filePath.find_last_of("/");
        auto extension = filePath.find(".");
        auto fileNameBeginning = fileSeparator + 1;
        auto beforeExtension = extension - 1;
        auto fileNameLength = beforeExtension - fileNameBeginning;
        auto colorName = filePath.substr(fileNameBeginning, fileNameLength);
        return color(colorName);
    }
    
    coordinate_response_measure::Color ResponseEvaluatorImpl::color(const std::string &colorName) {
        if (colorName == "green")
            return coordinate_response_measure::Color::green;
        else if (colorName == "blue")
            return coordinate_response_measure::Color::blue;
        else if (colorName == "red")
            return coordinate_response_measure::Color::red;
        else if (colorName == "white")
            return coordinate_response_measure::Color::white;
        else
            return coordinate_response_measure::Color::notAColor;
    }
    
    std::string ResponseEvaluatorImpl::fileName(const std::string &filePath) {
        auto fileSeparator = filePath.find_last_of("/");
        return filePath.substr(fileSeparator+1);
    }
    
}
