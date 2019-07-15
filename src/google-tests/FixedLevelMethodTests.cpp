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
    
    TEST_F(FixedLevelMethodTests, initializePassesTargetList) {
        test.common.targetListDirectory = "a";
        initialize();
        assertEqual("a", targetList.directory());
    }
    
    TEST_F(FixedLevelMethodTests, nextReturnsNextTarget) {
        targetList.setNext("a");
        assertEqual("a", method.next());
    }
    
    TEST_F(FixedLevelMethodTests, snrReturnsInitializedSnr) {
        test.snr_dB = 1;
        initialize();
        assertEqual(1, method.snr_dB());
    }
}
