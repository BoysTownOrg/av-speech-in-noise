#ifndef av_coordinate_response_measure_ResponseEvaluatorImpl_hpp
#define av_coordinate_response_measure_ResponseEvaluatorImpl_hpp

#include <recognition-test/RecognitionTestModel.hpp>

namespace av_coordinate_response_measure {
    class ResponseEvaluatorImpl :
        public av_coordinate_response_measure::ResponseEvaluator
    {
    public:
        bool correct(
            std::string filePath,
            const av_coordinate_response_measure::SubjectResponse &r
        ) override {
            auto found = filePath.find(".");
            auto correctNumber = std::stoi(filePath.substr(found-1, 1));
            auto correctColor_ = correctColor(filePath);
            return
                correctNumber == r.number &&
                correctColor_ == r.color;
        }
        
        av_coordinate_response_measure::Color correctColor(std::string filePath) {
            auto fileSeparator = filePath.find_last_of("/");
            auto extension = filePath.find(".");
            auto fileNameBeginning = fileSeparator+1;
            auto beforeExtension = extension - 1;
            auto fileNameLength = beforeExtension - fileNameBeginning;
            auto colorName = filePath.substr(fileNameBeginning, fileNameLength);
            if (colorName == "green")
                return av_coordinate_response_measure::Color::green;
            else if (colorName == "blue")
                return av_coordinate_response_measure::Color::blue;
            else if (colorName == "red")
                return av_coordinate_response_measure::Color::red;
            else
                return av_coordinate_response_measure::Color::white;
        }
    };
}
#endif
