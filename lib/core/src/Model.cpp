#include "Model.hpp"
#include "IRecognitionTestModel.hpp"

namespace av_speech_in_noise {
static void initialize(
    RunningATest &model, TestMethod &method, const Test &test) {
    model.initialize(&method, test);
}

static void initialize(RunningATest &model, TestMethod &method,
    const Test &test, RunningATest::Observer *observer) {
    model.initialize(&method, test, observer);
}

static void initialize(AdaptiveMethod &method, const AdaptiveTest &test,
    TargetPlaylistReader &reader) {
    method.initialize(test, &reader);
}

RunningATestFacadeImpl::RunningATestFacadeImpl(AdaptiveMethod &adaptiveMethod,
    FixedLevelMethod &fixedLevelMethod,
    TargetPlaylistReader &targetsWithReplacementReader,
    TargetPlaylistReader &cyclicTargetsReader,
    TargetPlaylist &targetsWithReplacement,
    FiniteTargetPlaylistWithRepeatables &silentIntervalTargets,
    FiniteTargetPlaylistWithRepeatables &everyTargetOnce,
    RepeatableFiniteTargetPlaylist &eachTargetNTimes,
    FiniteTargetPlaylistWithRepeatables &predeterminedTargets,
    RunningATest &model, OutputFile &outputFile,
    RunningATest::Observer &audioRecording, RunningATest::Observer &eyeTracking)
    : audioRecording{audioRecording}, eyeTracking{eyeTracking},
      adaptiveMethod{adaptiveMethod}, fixedLevelMethod{fixedLevelMethod},
      targetsWithReplacementReader{targetsWithReplacementReader},
      cyclicTargetsReader{cyclicTargetsReader},
      targetsWithReplacement{targetsWithReplacement},
      silentIntervalTargets{silentIntervalTargets},
      everyTargetOnce{everyTargetOnce},
      predeterminedTargets{predeterminedTargets},
      eachTargetNTimes{eachTargetNTimes}, runningATest{model},
      outputFile{outputFile} {}

void RunningATestFacadeImpl::initializeWithCyclicTargets(
    const AdaptiveTest &test) {
    av_speech_in_noise::initialize(adaptiveMethod, test, cyclicTargetsReader);
    initializeTestWithPossiblePeripheral(adaptiveMethod, test);
}

void RunningATestFacadeImpl::initializeTestWithPossiblePeripheral(
    TestMethod &method, const Test &test) {
    switch (test.peripheral) {
    case TestPeripheral::none:
        av_speech_in_noise::initialize(runningATest, method, test);
        break;
    case TestPeripheral::eyeTracking:
        av_speech_in_noise::initialize(
            runningATest, method, test, &eyeTracking);
        break;
    case TestPeripheral::audioRecording:
        av_speech_in_noise::initialize(
            runningATest, method, test, &audioRecording);
        break;
    }
}
}
