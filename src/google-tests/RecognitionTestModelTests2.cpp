#include <recognition-test/RecognitionTestModel.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
    class AdaptiveMethodStub : public IAdaptiveMethod {
    
        void initialize(const AdaptiveTest &)  {}
        
        bool complete()  {return {};}
        
        std::string next()  {return {};}
        
        std::string current()  {return {};}
        
        int snr_dB()  {return {};}
        
        void submitCorrectResponse()  {}
        
        void submitIncorrectResponse()  {}
        
        void submitResponse(const FreeResponse &)  {}
        
        void writeTestingParameters(OutputFile *)  {}
        
        void writeLastCoordinateResponse(OutputFile *)  {}
        
        void submitResponse(const coordinate_response_measure::SubjectResponse &)  {}
        
    };
    
    class FixedLevelMethodStub : public IFixedLevelMethod {
        const FixedLevelTest *test_{};
    public:
        void initialize(const FixedLevelTest &t) {
            test_ = &t;
        }
        
        auto test() const {
            return test_;
        }
        
        bool complete()  {return {};}
        
        std::string next()  {return {};}
        
        std::string current()  {return {};}
        
        int snr_dB()  {return {};}
        
        void submitCorrectResponse()  {}
        
        void submitIncorrectResponse()  {}
        
        void submitResponse(const FreeResponse &)  {}
        
        void writeTestingParameters(OutputFile *)  {}
        
        void writeLastCoordinateResponse(OutputFile *)  {}
        
        void submitResponse(const coordinate_response_measure::SubjectResponse &)  {}
        
    };
    
    class RecognitionTestModel_InternalStub : public IRecognitionTestModel_Internal {
        const coordinate_response_measure::SubjectResponse *coordinateResponse_{};
    public:
        void initialize(TestMethod *, const CommonTest &, const TestInformation &)  {}
        
        void playTrial(const AudioSettings &)  {}
        
        void submitResponse(const coordinate_response_measure::SubjectResponse &p) {
            coordinateResponse_ = &p;
        }
        
        bool testComplete()  {return {};}
        
        std::vector<std::string> audioDevices() {return {};}
        
        void subscribe(Model::EventListener *)  {}
        
        void playCalibration(const Calibration &)  {}
        
        void submitCorrectResponse()  {}
        
        void submitIncorrectResponse()  {}
        
        void submitResponse(const FreeResponse &)  {}
        
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
    
    TEST_F(RecognitionTestModelTests2, initializeFixedLevelTestInitializesFixedLevelMethod) {
        FixedLevelTest test;
        model.initializeTest(test);
        EXPECT_EQ(&test, fixedLevelMethod.test());
    }
}
