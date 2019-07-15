#include "ResponseEvaluatorStub.h"
#include "TargetListStub.h"
#include "OutputFileStub.h"
#include "assert-utility.h"
#include <recognition-test/FixedLevelMethod.hpp>
#include <gtest/gtest.h>

namespace av_speech_in_noise::tests {
    class FixedLevelMethodTests : public ::testing::Test {
    protected:
        ResponseEvaluatorStub evaluator{};
        TargetListStub targetList{};
        OutputFileStub outputFile;
        FixedLevelMethod method{&targetList, &evaluator};
        FixedLevelTest test;
        
        void initialize() {
            method.initialize(test);
        }
    };
    
    TEST_F(FixedLevelMethodTests, writeTestPassesSettings) {
        initialize();
        method.writeTestingParameters(&outputFile);
        assertEqual(&std::as_const(test), outputFile.fixedLevelTest());
    }
}
