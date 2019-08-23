#ifndef av_speech_in_noise_FixedLevelMethod_hpp
#define av_speech_in_noise_FixedLevelMethod_hpp

#include "RecognitionTestModel.hpp"

namespace av_speech_in_noise {
    class TestConcluder {
    public:
        virtual ~TestConcluder() = default;
        virtual bool complete(TargetList *) = 0;
    };

    class FixedTrialTestConcluder : public TestConcluder {
        int trials_{};
    public:
        void initialize(const FixedLevelTest &p) {
            trials_ = p.trials;
        }

        void submitResponse() {
            --trials_;
        }

        bool complete(TargetList *) override { 
            return trials_ == 0; 
        }
    };

    class FixedLevelMethod : public IFixedLevelMethod {
        coordinate_response_measure::FixedLevelTrial lastTrial{};
        const FixedLevelTest *test{};
        TargetList *targetList;
        ResponseEvaluator *evaluator;
        TestConcluder *concluder;
        int snr_dB_{};
        int trials_{};
        bool complete_{};
    public:
        FixedLevelMethod(TargetList *, ResponseEvaluator *, TestConcluder * = {});
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
