#ifndef av_coordinate_response_measure_ResponseEvaluatorImpl_hpp
#define av_coordinate_response_measure_ResponseEvaluatorImpl_hpp

#include <recognition-test/RecognitionTestModel.hpp>

namespace av_speech_in_noise {
    class ResponseEvaluatorImpl : public ResponseEvaluator {
    public:
        bool correct(
            const std::string &filePath,
            const coordinate_response_measure::SubjectResponse &r) override;
        static int invalidNumber;
        int correctNumber(const std::string &filePath) override;
        coordinate_response_measure::Color correctColor(const std::string &filePath) override;
    private:
        coordinate_response_measure::Color color(const std::string &colorName);
    };
}

#endif
