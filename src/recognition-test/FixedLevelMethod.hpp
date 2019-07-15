#ifndef av_speech_in_noise_FixedLevelMethod_hpp
#define av_speech_in_noise_FixedLevelMethod_hpp

#include "RecognitionTestModel.hpp"

namespace av_speech_in_noise {
    class FixedLevelMethod : public IFixedLevelMethod {
        coordinate_response_measure::FixedLevelTrial lastTrial{};
        const FixedLevelTest *test{};
        TargetList *targetList;
        ResponseEvaluator *evaluator;
        int snr_dB_{};
        int trials_{};
        bool complete_{};
    public:
        FixedLevelMethod(TargetList *, ResponseEvaluator *);
        void initialize(const FixedLevelTest &) override;
        int snr_dB() override;
        std::string next() override;
        bool complete() override;
        std::string current() override;
        void submitIncorrectResponse() override;
        void submitCorrectResponse() override;
        void writeLastCoordinateResponse(OutputFile *) override;
        void writeTestingParameters(OutputFile *) override;
        void submitResponse(
            const coordinate_response_measure::SubjectResponse &
        ) override;
        void submitResponse(const FreeResponse &) override;
    private:
        void updateCompletion();
    };
}
#endif
