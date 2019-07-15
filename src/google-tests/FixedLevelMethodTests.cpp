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
        coordinate_response_measure::SubjectResponse coordinateResponse;
        
        void initialize() {
            method.initialize(test);
        }
        
        void writeCoordinateResponse() {
            submitCoordinateResponse();
            writeLastCoordinateResponse();
        }
        
        void submitCoordinateResponse() {
            method.submitResponse(coordinateResponse);
        }
        
        void writeLastCoordinateResponse() {
            method.writeLastCoordinateResponse(&outputFile);
        }
        
        auto blueColor() {
            return coordinate_response_measure::Color::blue;
        }
        
        auto writtenFixedLevelTrial() {
            return outputFile.writtenFixedLevelTrial2();
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
    
    TEST_F(FixedLevelMethodTests, writeCoordinateResponsePassesSubjectColor) {
        coordinateResponse.color = blueColor();
        writeCoordinateResponse();
        assertEqual(blueColor(), writtenFixedLevelTrial().subjectColor);
    }
    
    TEST_F(FixedLevelMethodTests, writeCoordinateResponsePassesSubjectNumber) {
        coordinateResponse.number = 1;
        writeCoordinateResponse();
        assertEqual(1, writtenFixedLevelTrial().subjectNumber);
    }
}
