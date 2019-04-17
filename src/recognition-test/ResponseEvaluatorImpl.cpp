#include "ResponseEvaluatorImpl.hpp"

namespace av_coordinate_response_measure {
    bool ResponseEvaluatorImpl::correct(
        std::string filePath,
        const av_coordinate_response_measure::SubjectResponse &r
    )  {
        auto found = filePath.find(".");
        auto correctNumber = std::stoi(filePath.substr(found-1, 1));
        auto correctColor_ = correctColor(filePath);
        return
            correctNumber == r.number &&
            correctColor_ == r.color;
    }
    
    Color ResponseEvaluatorImpl::correctColor(std::string filePath) {
        auto fileSeparator = filePath.find_last_of("/");
        auto extension = filePath.find(".");
        auto fileNameBeginning = fileSeparator+1;
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
