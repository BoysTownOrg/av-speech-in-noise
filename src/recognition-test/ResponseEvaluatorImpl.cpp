#include "ResponseEvaluatorImpl.hpp"

namespace av_coordinate_response_measure {
    int ResponseEvaluatorImpl::invalidNumber = -1;
    
    bool ResponseEvaluatorImpl::correct(
        const std::string &filePath,
        const SubjectResponse &r
    ) {
        return
            correctNumber(filePath) == r.number &&
            correctColor(filePath) == r.color &&
            r.color != Color::notAColor &&
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
    
    Color ResponseEvaluatorImpl::correctColor(const std::string &filePath) {
        auto fileSeparator = filePath.find_last_of("/");
        auto extension = filePath.find(".");
        auto fileNameBeginning = fileSeparator + 1;
        auto beforeExtension = extension - 1;
        auto fileNameLength = beforeExtension - fileNameBeginning;
        auto colorName = filePath.substr(fileNameBeginning, fileNameLength);
        return color(colorName);
    }
    
    Color ResponseEvaluatorImpl::color(const std::string &colorName) {
        if (colorName == "green")
            return Color::green;
        else if (colorName == "blue")
            return Color::blue;
        else if (colorName == "red")
            return Color::red;
        else if (colorName == "white")
            return Color::white;
        else
            return Color::notAColor;
    }
}
