#ifndef av_coordinate_response_measure_ResponseEvaluatorImpl_hpp
#define av_coordinate_response_measure_ResponseEvaluatorImpl_hpp

#include <recognition-test/RecognitionTestModel.hpp>

namespace av_coordinate_response_measure {
    class ResponseEvaluatorImpl : public ResponseEvaluator {
    public:
        bool correct(std::string filePath, const SubjectResponse &r) override;
        static int invalidNumber;
        int correctNumber(const std::string &filePath) override;
        Color correctColor(const std::string &filePath) override;
    private:
        Color color(const std::string &colorName);
    };
}

#endif
