#ifndef AV_SPEECH_IN_NOISE_MAIN_MACOS_MAIN_H_
#define AV_SPEECH_IN_NOISE_MAIN_MACOS_MAIN_H_

#include "MacOsTestSetupViewFactory.h"
#include <recognition-test/RecognitionTestModel.hpp>
#include <stimulus-players/TargetPlayerImpl.hpp>

namespace av_speech_in_noise {
void main(EyeTracker &, MacOsTestSetupViewFactory *, SignalProcessor * = {});
}

#endif
