#include "ResponseEvaluatorImpl.hpp"

namespace av_coordinate_response_measure {
    bool ResponseEvaluatorImpl::correct(
        std::string filePath,
        const av_coordinate_response_measure::SubjectResponse &r
    )  {
        return
            correctNumber(filePath) == r.number &&
            correctColor(filePath) == r.color;
    }
    int ResponseEvaluatorImpl::correctNumber(const std::string &filePath) {
        auto extension = filePath.find(".");
        auto number = filePath.substr(extension-1, 1);
        return std::stoi(number);
    }
    
    Color ResponseEvaluatorImpl::correctColor(const std::string &filePath) {
        auto fileSeparator = filePath.find_last_of("/");
        auto extension = filePath.find(".");
        auto fileNameBeginning = fileSeparator + 1;
        auto beforeExtension = extension - 1;
        auto fileNameLength = beforeExtension - fileNameBeginning;
        auto colorName = filePath.substr(fileNameBeginning, fileNameLength);
        if (colorName == "green")
            return Color::green;
        else if (colorName == "blue")
            return Color::blue;
        else if (colorName == "red")
            return Color::red;
        else
            return Color::white;
    }
}
