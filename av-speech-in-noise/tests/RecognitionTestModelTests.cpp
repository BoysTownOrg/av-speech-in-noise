#include "ModelEventListenerStub.h"
#include "TargetListStub.h"
#include "assert-utility.h"
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
        TargetList *targetList_{};
    public:
        void initialize(const FixedLevelTest &t, TargetList *list) {
            targetList_ = list;
            test_ = &t;
        }

        auto targetList() const {
            return targetList_;
        }
        
        auto test() const {
            return test_;
        }
        
        bool complete()  {return {};}
        std::string next()  {return {};}
        std::string current()  {return {};}
        int snr_dB()  {return {};}
        void submitCorrectResponse() {}
        void submitIncorrectResponse() {}
        void submitResponse(const FreeResponse &) {}
        void writeTestingParameters(OutputFile *) {}
        void writeLastCoordinateResponse(OutputFile *) {}
        void submitResponse(const coordinate_response_measure::SubjectResponse &) {}
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
    
    namespace internal_ {
        class InitializingTestUseCase {
        public:
            virtual ~InitializingTestUseCase() = default;
            virtual void run(RecognitionTestModel &) = 0;
            virtual const CommonTest &commonTest() = 0;
            virtual const TestInformation &testInformation() = 0;
            virtual const TestMethod *testMethod() = 0;
        };
        
        class InitializingAdaptiveTest : public InitializingTestUseCase {
            AdaptiveTest test;
            AdaptiveMethodStub *method;
        public:
            explicit InitializingAdaptiveTest(AdaptiveMethodStub *method) :
                method{method} {}
            
            void run(RecognitionTestModel &model) override {
                model.initializeTest(test);
            }
            
            const CommonTest &commonTest() override {
                return test.common;
            }
            
            const TestInformation &testInformation() override {
                return test.information;
            }
            
            const TestMethod *testMethod() override {
                return method;
            }
        };
        
        class InitializingFixedLevelTest : public InitializingTestUseCase {
            FixedLevelTest test;
            FixedLevelMethodStub *method;
        public:
            explicit InitializingFixedLevelTest(FixedLevelMethodStub *method) :
                method{method} {}
            
            void run(RecognitionTestModel &model) override {
                model.initializeTest(test);
            }
            
            const CommonTest &commonTest() override {
                return test.common;
            }
            
            const TestInformation &testInformation() override {
                return test.information;
            }
            
            const TestMethod *testMethod() override {
                return method;
            }
        };
        
        class InitializingFixedLevelTestWithFiniteTargets : public InitializingTestUseCase {
            FixedLevelTest test;
            FixedLevelMethodStub *method;
        public:
            explicit InitializingFixedLevelTestWithFiniteTargets(FixedLevelMethodStub *method) :
                method{method} {}
            
            void run(RecognitionTestModel &model) override {
                model.initializeTestWithFiniteTargets(test);
            }
            
            const CommonTest &commonTest() override {
                return test.common;
            }
            
            const TestInformation &testInformation() override {
                return test.information;
            }
            
            const TestMethod *testMethod() override {
                return method;
            }
        };
    }
    
    class RecognitionTestModelTests : public ::testing::Test {
    protected:
        AdaptiveMethodStub adaptiveMethod;
        FixedLevelMethodStub fixedLevelMethod;
        TargetListStub infiniteTargetList;
        FixedLevelMethodStub fixedLevelMethodWithFiniteTargets;
        RecognitionTestModel_InternalStub internalModel;
        RecognitionTestModel model{
            &adaptiveMethod,
            &fixedLevelMethod,
            &infiniteTargetList,
            &fixedLevelMethodWithFiniteTargets,
            &internalModel
        };
        AdaptiveTest adaptiveTest;
        FixedLevelTest fixedLevelTest;
        internal_::InitializingAdaptiveTest initializingAdaptiveTest{
            &adaptiveMethod
        };
        internal_::InitializingFixedLevelTest initializingFixedLevelTest{
            &fixedLevelMethod
        };
        internal_::InitializingFixedLevelTestWithFiniteTargets
            initializingFixedLevelTestWithFiniteTargets{&fixedLevelMethodWithFiniteTargets};
        
        void initializeFixedLevelTest() {
            model.initializeTest(fixedLevelTest);
        }

        void initializeFixedLevelTestWithFiniteTargets() {
            model.initializeTestWithFiniteTargets(fixedLevelTest);
        }
        
        void initializeAdaptiveTest() {
            model.initializeTest(adaptiveTest);
        }
        
        bool testComplete() {
            return model.testComplete();
        }
        
        void run(internal_::InitializingTestUseCase &useCase) {
            useCase.run(model);
        }
        
        void assertInitializesInternalModel(
            internal_::InitializingTestUseCase &useCase
        ) {
            run(useCase);
            assertEqual(
                useCase.testMethod(),
                internalModel.testMethod()
            );
            assertEqual(
                &useCase.commonTest(),
                internalModel.commonTest()
            );
            assertEqual(
                &useCase.testInformation(),
                internalModel.testInformation()
            );
        }
    };
    
    TEST_F(RecognitionTestModelTests, submitResponsePassesCoordinateResponse) {
        coordinate_response_measure::SubjectResponse response;
        model.submitResponse(response);
        assertEqual(&std::as_const(response), internalModel.coordinateResponse());
    }
    
    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestInitializesFixedLevelMethod
    ) {
        initializeFixedLevelTest();
        assertEqual(&std::as_const(fixedLevelTest), fixedLevelMethod.test());
    }
    
    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestInitializesWithInfiniteTargetList
    ) {
        initializeFixedLevelTest();
        assertEqual(
            static_cast<TargetList *>(&infiniteTargetList), 
            fixedLevelMethod.targetList()
        );
    }
    
    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestWithFiniteTargetsInitializesFixedLevelMethodWithFiniteTargets
    ) {
        initializeFixedLevelTestWithFiniteTargets();
        assertEqual(&std::as_const(fixedLevelTest), fixedLevelMethodWithFiniteTargets.test());
    }
    
    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestInitializesAdaptiveMethod
    ) {
        initializeAdaptiveTest();
        assertEqual(&std::as_const(adaptiveTest), adaptiveMethod.test());
    }
    
    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestInitializesInternalModel
    ) {
        assertInitializesInternalModel(initializingFixedLevelTest);
    }
    
    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestWithFiniteTargetsInitializesInternalModel
    ) {
        assertInitializesInternalModel(initializingFixedLevelTestWithFiniteTargets);
    }
    
    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestInitializesInternalModel
    ) {
        assertInitializesInternalModel(initializingAdaptiveTest);
    }
    
    TEST_F(RecognitionTestModelTests, playTrialPassesAudioSettings) {
        AudioSettings settings;
        model.playTrial(settings);
        assertEqual(
            &std::as_const(settings),
            internalModel.playTrialSettings()
        );
    }
    
    TEST_F(RecognitionTestModelTests, playCalibrationPassesCalibration) {
        Calibration calibration;
        model.playCalibration(calibration);
        assertEqual(
            &std::as_const(calibration),
            internalModel.calibration()
        );
    }
    
    TEST_F(RecognitionTestModelTests, testCompleteWhenComplete) {
        assertFalse(testComplete());
        internalModel.setComplete();
        assertTrue(testComplete());
    }
    
    TEST_F(RecognitionTestModelTests, returnsAudioDevices) {
        internalModel.setAudioDevices({"a", "b", "c"});
        assertEqual({"a", "b", "c"}, model.audioDevices());
    }
    
    TEST_F(RecognitionTestModelTests, subscribesToListener) {
        ModelEventListenerStub listener;
        model.subscribe(&listener);
        assertEqual(
            static_cast<const Model::EventListener *>(&listener),
            internalModel.listener()
        );
    }
}
