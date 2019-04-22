#include "ResponseEvaluatorImpl.hpp"

namespace av_coordinate_response_measure {
    bool ResponseEvaluatorImpl::correct(
        std::string filePath,
        const SubjectResponse &r
    ) {
        return
            correctNumber(filePath) == r.number &&
            correctColor(filePath) == r.color;
    }
    int ResponseEvaluatorImpl::correctNumber(const std::string &filePath) {
        auto extension = filePath.find(".");
        if (extension == std::string::npos)
            return -1;
        auto number = filePath.substr(extension-1, 1);
        try {
            return std::stoi(number);
        }
        catch (const std::invalid_argument &) {
            return -1;
        }
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
