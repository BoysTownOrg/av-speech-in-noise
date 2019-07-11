#include "assert-utility.h"
#include "RecognitionTestModelTests.hpp"
#include <recognition-test/RecognitionTestModel.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests::recognition_test {
    class AdaptiveMethodStub : public IAdaptiveMethod {
        const AdaptiveTest *test_{};
    public:
        void initialize(const AdaptiveTest &t) {
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
    
    class RecognitionTestModel_InternalStub :
        public IRecognitionTestModel_Internal
    {
        std::vector<std::string> audioDevices_{};
        const Model::EventListener *listener_{};
        const Calibration *calibration_{};
        const AudioSettings *playTrialSettings_{};
        const TestInformation *testInformation_{};
        const CommonTest *commonTest_{};
        const TestMethod *testMethod_{};
        const coordinate_response_measure::SubjectResponse *
            coordinateResponse_{};
        bool complete_{};
    public:
        void initialize(
            TestMethod *tm,
            const CommonTest &ct,
            const TestInformation &ti
        ) {
            testMethod_ = tm;
            commonTest_ = &ct;
            testInformation_ = &ti;
        }
        
        void playTrial(const AudioSettings &s) {
            playTrialSettings_ = &s;
        }
        
        void submitResponse(
            const coordinate_response_measure::SubjectResponse &p
        ) {
            coordinateResponse_ = &p;
        }
        
        bool testComplete() { return complete_; }
        
        std::vector<std::string> audioDevices() { return audioDevices_; }
        
        void subscribe(Model::EventListener *e) {
            listener_ = e;
        }
        
        void playCalibration(const Calibration &c) {
            calibration_ = &c;
        }
        
        void submitCorrectResponse()  {}
        
        void submitIncorrectResponse()  {}
        
        void submitResponse(const FreeResponse &)  {}
        
        void throwIfTrialInProgress()  {}
        
        auto coordinateResponse() const {
            return coordinateResponse_;
        }
        
        auto testMethod() const {
            return testMethod_;
        }
        
        auto commonTest() const {
            return commonTest_;
        }
        
        auto testInformation() const {
            return testInformation_;
        }
        
        auto playTrialSettings() const {
            return playTrialSettings_;
        }
        
        auto calibration() const {
            return calibration_;
        }
        
        void setComplete() {
            complete_ = true;
        }
        
        void setAudioDevices(std::vector<std::string> v) {
            audioDevices_ = std::move(v);
        }
        
        auto listener() const {
            return listener_;
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
        AdaptiveTest adaptiveTest;
        FixedLevelTest fixedLevelTest;
        
        void initializeFixedLevelTest() {
            model.initializeTest(fixedLevelTest);
        }
        
        bool testComplete() {
            return model.testComplete();
        }
    };
    
    TEST_F(RecognitionTestModelTests2, submitResponsePassesCoordinateResponse) {
        coordinate_response_measure::SubjectResponse response;
        model.submitResponse(response);
        assertEqual(&std::as_const(response), internalModel.coordinateResponse());
    }
    
    TEST_F(
        RecognitionTestModelTests2,
        initializeFixedLevelTestInitializesFixedLevelMethod
    ) {
        initializeFixedLevelTest();
        assertEqual(&std::as_const(fixedLevelTest), fixedLevelMethod.test());
    }
    
    TEST_F(
        RecognitionTestModelTests2,
        initializeAdaptiveTestInitializesAdaptiveMethod
    ) {
        model.initializeTest(adaptiveTest);
        assertEqual(&std::as_const(adaptiveTest), adaptiveMethod.test());
    }
    
    TEST_F(
        RecognitionTestModelTests2,
        initializeFixedLevelTestInitializesInternalModel
    ) {
        initializeFixedLevelTest();
        assertEqual(
            static_cast<const TestMethod *>(&fixedLevelMethod),
            internalModel.testMethod()
        );
        assertEqual(
            &std::as_const(fixedLevelTest.common),
            internalModel.commonTest()
        );
        assertEqual(
            &std::as_const(fixedLevelTest.information),
            internalModel.testInformation()
        );
    }
    
    TEST_F(RecognitionTestModelTests2, playTrialPassesAudioSettings) {
        AudioSettings settings;
        model.playTrial(settings);
        assertEqual(
            &std::as_const(settings),
            internalModel.playTrialSettings()
        );
    }
    
    TEST_F(RecognitionTestModelTests2, playCalibrationPassesCalibration) {
        Calibration calibration;
        model.playCalibration(calibration);
        assertEqual(
            &std::as_const(calibration),
            internalModel.calibration()
        );
    }
    
    TEST_F(RecognitionTestModelTests2, testCompleteWhenComplete) {
        assertFalse(testComplete());
        internalModel.setComplete();
        assertTrue(testComplete());
    }
    
    TEST_F(RecognitionTestModelTests2, returnsAudioDevices) {
        internalModel.setAudioDevices({"a", "b", "c"});
        assertEqual({"a", "b", "c"}, model.audioDevices());
    }
    
    TEST_F(RecognitionTestModelTests2, subscribesToListener) {
        ModelEventListenerStub listener;
        model.subscribe(&listener);
        assertEqual(
            static_cast<const Model::EventListener *>(&listener),
            internalModel.listener()
        );
    }
}
