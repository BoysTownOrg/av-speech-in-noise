#include <recognition-test/RecognitionTestModel.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
    class AdaptiveMethodStub : public IAdaptiveMethod {
    
    };
    
    class FixedLevelMethodStub : public IFixedLevelMethod {
    
    };
    
    class RecognitionTestModelTests2 : public ::testing::Test {
    protected:
        RecognitionTestModel model;
    };
}
