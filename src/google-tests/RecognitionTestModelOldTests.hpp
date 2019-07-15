#ifndef RecognitionTestModelTests_h
#define RecognitionTestModelTests_h

#include "TargetListStub.h"
#include "TargetListSetReaderStub.h"
#include "TrackStub.h"
#include "MaskerPlayerStub.h"
#include "TargetPlayerStub.h"
#include "OutputFileStub.h"
#include "ResponseEvaluatorStub.h"
#include "RandomizerStub.h"
#include "LogString.h"
#include "assert-utility.h"
#include <recognition-test/FixedLevelMethod.hpp>
#include <recognition-test/AdaptiveMethod.hpp>
#include <recognition-test/RecognitionTestModel.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <cmath>

namespace av_speech_in_noise::tests::recognition_test {
    class UseCase {
    public:
        virtual ~UseCase() = default;
        virtual void run(RecognitionTestModel &) = 0;
        virtual void run(RecognitionTestModel_Internal &) {}
    };

    class SubmittingResponse : public virtual UseCase {
    };
}
#endif
