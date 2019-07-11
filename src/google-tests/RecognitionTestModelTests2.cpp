#include <recognition-test/RecognitionTestModel.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
    class AdaptiveMethodStub : public IAdaptiveMethod {
    
        void initialize(const av_speech_in_noise::AdaptiveTest &)  {}
        
        bool complete()  {return {};}
        
        std::string next()  {return {};}
        
        std::string current()  {return {};}
        
        int snr_dB()  {return {};}
        
        void submitCorrectResponse()  {}
        
        void submitIncorrectResponse()  {}
        
        void submitResponse(const av_speech_in_noise::FreeResponse &)  {}
        
        void writeTestingParameters(av_speech_in_noise::OutputFile *)  {}
        
        void writeLastCoordinateResponse(av_speech_in_noise::OutputFile *)  {}
        
        void submitResponse(const coordinate_response_measure::SubjectResponse &)  {}
        
    };
    
    class FixedLevelMethodStub : public IFixedLevelMethod {
    
        void initialize(const av_speech_in_noise::FixedLevelTest &)  {}
        
        bool complete()  {return {};}
        
        std::string next()  {return {};}
        
        std::string current()  {return {};}
        
        int snr_dB()  {return {};}
        
        void submitCorrectResponse()  {}
        
        void submitIncorrectResponse()  {}
        
        void submitResponse(const av_speech_in_noise::FreeResponse &)  {}
        
        void writeTestingParameters(av_speech_in_noise::OutputFile *)  {}
        
        void writeLastCoordinateResponse(av_speech_in_noise::OutputFile *)  {}
        
        void submitResponse(const coordinate_response_measure::SubjectResponse &)  {}
        
    };
    
    class RecognitionTestModel_InternalStub : public IRecognitionTestModel_Internal {
        const coordinate_response_measure::SubjectResponse *coordinateResponse_{};
    public:
        void initialize(av_speech_in_noise::TestMethod *, const av_speech_in_noise::CommonTest &, const av_speech_in_noise::TestInformation &)  {}
        
        void playTrial(const av_speech_in_noise::AudioSettings &)  {}
        
        void submitResponse(const coordinate_response_measure::SubjectResponse &p) {
            coordinateResponse_ = &p;
        }
        
        bool testComplete()  {return {};}
        
        std::vector<std::string> audioDevices() {return {};}
        
        void subscribe(Model::EventListener *)  {}
        
        void playCalibration(const av_speech_in_noise::Calibration &)  {}
        
        void submitCorrectResponse()  {}
        
        void submitIncorrectResponse()  {}
        
        void submitResponse(const av_speech_in_noise::FreeResponse &)  {}
        
        void throwIfTrialInProgress()  {}
        
        const coordinate_response_measure::SubjectResponse *coordinateResponse() const {
            return coordinateResponse_;
        }
    };
    
    class RecognitionTestModelTests2 : public ::testing::Test {
    protected:
        AdaptiveMethodStub adaptiveMethod;
        FixedLevelMethodStub fixedLevelMethod;
        RecognitionTestModel_InternalStub internalModel;
        RecognitionTestModel model{
            &adaptiveMethod,
            &fixedLevelMethod,
            &internalModel
        };
    };
    
    TEST_F(RecognitionTestModelTests2, passesCoordinateResponse) {
        coordinate_response_measure::SubjectResponse response;
        model.submitResponse(response);
        EXPECT_EQ(&response, internalModel.coordinateResponse());
    }
}
