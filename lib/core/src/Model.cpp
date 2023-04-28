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

static void initialize(FixedLevelMethod &method,
    const FixedLevelFixedTrialsTest &test, TargetPlaylist &targets) {
    method.initialize(test, &targets);
}

static void initialize(FixedLevelMethod &method,
    const FixedLevelTestWithEachTargetNTimes &test,
    FiniteTargetPlaylist &targets) {
    method.initialize(test, &targets);
}

static void initialize(FixedLevelMethod &method, const FixedLevelTest &test,
    FiniteTargetPlaylistWithRepeatables &targets) {
    method.initialize(test, &targets);
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

void RunningATestFacadeImpl::initialize(const AdaptiveTest &test) {
    av_speech_in_noise::initialize(
        adaptiveMethod, test, targetsWithReplacementReader);
    initializeTestWithPossiblePeripheral(adaptiveMethod, test);
}

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
