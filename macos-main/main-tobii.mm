#include "main.h"
#include "TobiiEyeTracker.hpp"

int main() {
    av_speech_in_noise::TobiiEyeTracker eyeTracker;
    av_speech_in_noise::main(eyeTracker);
}
