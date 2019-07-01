#include "ResponseEvaluatorImpl.hpp"
#include <algorithm>
#include <cctype>

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
        auto fileSeparator = filePath.find_last_of("/");
        auto fileNameBeginning = fileSeparator + 1;
        auto found = std::find_if(filePath.begin() + fileNameBeginning, filePath.end(), [](unsigned char c) { return !std::isalpha(c); });
        auto colorNameLength = std::distance(filePath.begin() + fileNameBeginning, found);
        auto extension = filePath.find(".");
        if (extension == std::string::npos)
            return invalidNumber;
        auto number = filePath.substr(fileNameBeginning + colorNameLength, 1);
        try {
            return std::stoi(number);
        }
        catch (const std::invalid_argument &) {
            return invalidNumber;
        }
    }
    
    coordinate_response_measure::Color ResponseEvaluatorImpl::correctColor(const std::string &filePath) {
        auto fileSeparator = filePath.find_last_of("/");
        auto fileNameBeginning = fileSeparator + 1;
        auto found = std::find_if(filePath.begin() + fileNameBeginning, filePath.end(), [](unsigned char c) { return !std::isalpha(c); });
        auto colorNameLength = std::distance(filePath.begin() + fileNameBeginning, found);
        auto colorName = filePath.substr(fileNameBeginning, colorNameLength);
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
