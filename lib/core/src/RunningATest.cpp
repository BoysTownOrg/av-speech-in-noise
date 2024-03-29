#include "RunningATest.hpp"
#include "av-speech-in-noise/Model.hpp"

#include <gsl/gsl>

#include <cmath>
#include <functional>
#include <sstream>
#include <string_view>

namespace av_speech_in_noise {
namespace {
class NullTestMethod : public TestMethod {
    auto complete() -> bool override { return {}; }
    auto nextTarget() -> LocalUrl override { return {}; }
    auto currentTarget() -> LocalUrl override { return {}; }
    auto snr() -> SNR override { return SNR{}; }
    void submit(const coordinate_response_measure::Response &) override {}
    void writeLastCoordinateResponse(OutputFile &) override {}
    void writeTestingParameters(OutputFile &) override {}
    void writeTestResult(OutputFile &) override {}
};

class NullObserver : public RunningATest::TestObserver {
    void notifyThatNewTestIsReady(std::string_view session) override {}
    void notifyThatTrialWillBegin(int trialNumber) override {}
    void notifyThatTargetWillPlayAt(const PlayerTimeWithDelay &) override {}
    void notifyThatStimulusHasEnded() override {}
    void notifyThatSubjectHasResponded() override {}
};
}

static NullTestMethod nullTestMethod;

static NullObserver nullObserver;

static void useAllChannels(MaskerPlayer &player) { player.useAllChannels(); }

static void clearChannelDelays(MaskerPlayer &player) {
    player.clearChannelDelays();
}

static void useAllChannels(TargetPlayer &player) { player.useAllChannels(); }

static auto totalrampDuration(MaskerPlayer &player) -> Duration {
    return Duration{2 * player.rampDuration().seconds};
}

static constexpr auto operator+(const Duration &a, const Duration &b)
    -> Duration {
    return Duration{a.seconds + b.seconds};
}

static auto steadyLevelDuration(TargetPlayer &player) -> Duration {
    return player.duration() + RunningATestImpl::targetOnsetFringeDuration +
        RunningATestImpl::targetOffsetFringeDuration;
}

auto trialDuration(TargetPlayer &target, MaskerPlayer &masker) -> Duration {
    return totalrampDuration(masker) + steadyLevelDuration(target);
}

static void useFirstChannelOnly(TargetPlayer &player) {
    player.useFirstChannelOnly();
}

static void setAudioDevice(TargetPlayer &player, const std::string &device) {
    player.setAudioDevice(device);
}

static void setAudioDevice(MaskerPlayer &player, const std::string &device) {
    player.setAudioDevice(device);
}

static void setAudioDevices(MaskerPlayer &maskerPlayer,
    TargetPlayer &targetPlayer, const std::string &device) {
    setAudioDevice(maskerPlayer, device);
    setAudioDevice(targetPlayer, device);
}

static void throwRequestFailureOnInvalidAudioDevice(
    const std::function<void(const std::string &)> &f,
    const std::string &device) {
    try {
        f(device);
    } catch (const InvalidAudioDevice &) {
        throw RunningATest::RequestFailure{
            "'" + device + "' is not a valid audio device."};
    }
}

static void throwRequestFailureIfTrialInProgress(bool f) {
    if (f)
        throw RunningATest::RequestFailure{"Trial in progress."};
}

static void apply(TargetPlayer &player, LevelAmplification x) {
    player.apply(x);
}

static void apply(MaskerPlayer &player, LevelAmplification x) {
    player.apply(x);
}

static void loadFile(
    TargetPlayer &player, const LocalUrl &s, RationalNumber videoScale) {
    player.loadFile(s, videoScale);
}

static void loadFile(MaskerPlayer &player, const LocalUrl &s) {
    player.loadFile(s);
}

static void play(TargetPlayer &player) { player.play(); }

static void play(MaskerPlayer &player) { player.play(); }

static auto currentTarget(TestMethod *testMethod) -> LocalUrl {
    return testMethod->currentTarget();
}

static auto targetName(ResponseEvaluator &evaluator, TestMethod *testMethod)
    -> std::string {
    return evaluator.fileName(currentTarget(testMethod));
}

static void save(OutputFile &file) { file.save(); }

static void tryOpening(OutputFile &file, const TestIdentity &p) {
    file.close();
    try {
        file.openNewFile(p);
    } catch (const OutputFile::OpenFailure &) {
        throw RunningATest::RequestFailure{"Unable to open output file."};
    }
}

constexpr auto operator-(const RealLevel &a, const RealLevel &b)
    -> RealLevelDifference {
    return RealLevelDifference{a.dB_SPL - b.dB_SPL};
}

static constexpr auto operator-(
    const RealLevelDifference &a, const DigitalLevel &b) -> DigitalLevel {
    return DigitalLevel{a.dB - b.dBov};
}

static auto levelAmplification(TargetPlayer &player, const Calibration &p)
    -> LevelAmplification {
    return LevelAmplification{
        DigitalLevel{p.level - p.fullScaleLevel - player.digitalLevel()}.dBov};
}

static auto levelAmplification(MaskerPlayer &player, const Calibration &p)
    -> LevelAmplification {
    return LevelAmplification{
        DigitalLevel{p.level - p.fullScaleLevel - player.digitalLevel()}.dBov};
}

static void show(TargetPlayer &player) { player.showVideo(); }

static void hide(TargetPlayer &player) { player.hideVideo(); }

static auto maskerFileUrl(const Test &test) -> LocalUrl {
    return test.maskerFileUrl;
}

static void throwRequestFailureOnInvalidAudioFile(
    const std::function<void(const LocalUrl &)> &f, const LocalUrl &s) {
    try {
        f(s);
    } catch (const InvalidAudioFile &) {
        throw RunningATest::RequestFailure{"unable to read " + s.path};
    }
}

static auto offsetDuration(
    MaskerPlayer &player, const AudioSampleTimeWithOffset &t) -> Duration {
    return Duration{t.sampleOffset / player.sampleRateHz()};
}

static constexpr auto operator-(const Duration &a, const Duration &b)
    -> Duration {
    return Duration{a.seconds - b.seconds};
}

static void play(TargetPlayer &targetPlayer, const Calibration &calibration,
    RationalNumber videoScale) {
    throwRequestFailureOnInvalidAudioDevice(
        [&](auto device) { setAudioDevice(targetPlayer, device); },
        calibration.audioDevice);
    throwRequestFailureOnInvalidAudioFile(
        [&](auto file) {
            loadFile(targetPlayer, file, videoScale);
            apply(targetPlayer, levelAmplification(targetPlayer, calibration));
        },
        calibration.fileUrl);
    show(targetPlayer);
    play(targetPlayer);
}

static void play(MaskerPlayer &maskerPlayer, const Calibration &calibration) {
    maskerPlayer.stop();
    throwRequestFailureOnInvalidAudioDevice(
        [&](auto device) { setAudioDevice(maskerPlayer, device); },
        calibration.audioDevice);
    throwRequestFailureOnInvalidAudioFile(
        [&](auto file) {
            loadFile(maskerPlayer, file);
            apply(maskerPlayer, levelAmplification(maskerPlayer, calibration));
        },
        calibration.fileUrl);
    play(maskerPlayer);
}

static auto maskerLevelAmplification(MaskerPlayer &maskerPlayer,
    RealLevel maskerLevel, RealLevel fullScaleLevel) -> LevelAmplification {
    return LevelAmplification{
        DigitalLevel{maskerLevel - fullScaleLevel - maskerPlayer.digitalLevel()}
            .dBov};
}

static auto targetLevelAmplification(TestMethod *testMethod,
    MaskerPlayer &maskerPlayer, RealLevel maskerLevel,
    RealLevel fullScaleLevel) {
    return LevelAmplification{
        maskerLevelAmplification(maskerPlayer, maskerLevel, fullScaleLevel).dB +
        testMethod->snr().dB};
}

static void preparePlayersForNextTrial(TestMethod *testMethod,
    Randomizer &randomizer, TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, RealLevel maskerLevel, RealLevel fullScaleLevel,
    RationalNumber videoScale) {
    loadFile(targetPlayer, testMethod->nextTarget(), videoScale);
    apply(targetPlayer,
        targetLevelAmplification(
            testMethod, maskerPlayer, maskerLevel, fullScaleLevel));
    const auto maskerPlayerSeekTimeUpperLimit{
        maskerPlayer.duration() - trialDuration(targetPlayer, maskerPlayer)};
    maskerPlayer.seekSeconds(randomizer.betweenInclusive(
        0., maskerPlayerSeekTimeUpperLimit.seconds));
    maskerPlayer.setSteadyLevelFor(steadyLevelDuration(targetPlayer));
}

static void prepareNextTrialIfNeeded(TestMethod *testMethod, int &trialNumber_,
    OutputFile &outputFile, Randomizer &randomizer, TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, RunningATest::TestObserver *observer,
    RealLevel maskerLevel, RealLevel fullScaleLevel,
    RationalNumber videoScale) {
    observer->notifyThatSubjectHasResponded();
    if (!testMethod->complete()) {
        ++trialNumber_;
        preparePlayersForNextTrial(testMethod, randomizer, targetPlayer,
            maskerPlayer, maskerLevel, fullScaleLevel, videoScale);
    } else {
        testMethod->writeTestResult(outputFile);
        save(outputFile);
    }
}

static void saveOutputFileAndPrepareNextTrialAfter(
    const std::function<void()> &f, TestMethod *testMethod, int &trialNumber_,
    OutputFile &outputFile, Randomizer &randomizer, TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, RunningATest::TestObserver *observer,
    RealLevel maskerLevel, RealLevel fullScaleLevel,
    RationalNumber videoScale) {
    f();
    save(outputFile);
    prepareNextTrialIfNeeded(testMethod, trialNumber_, outputFile, randomizer,
        targetPlayer, maskerPlayer, observer, maskerLevel, fullScaleLevel,
        videoScale);
}

RunningATestImpl::RunningATestImpl(TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, ResponseEvaluator &evaluator,
    OutputFile &outputFile, Randomizer &randomizer, Clock &clock)
    : maskerPlayer{maskerPlayer}, targetPlayer{targetPlayer},
      evaluator{evaluator}, outputFile{outputFile}, randomizer{randomizer},
      clock{clock}, observer{&nullObserver}, testMethod{&nullTestMethod} {
    targetPlayer.attach(this);
    maskerPlayer.attach(this);
}

void RunningATestImpl::attach(RunningATest::Observer *listener) {
    listener_ = listener;
}

void RunningATestImpl::initialize(TestMethod *testMethod_, const Test &test,
    RunningATest::TestObserver *observer_) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);

    if (testMethod_->complete())
        return;

    tryOpening(outputFile, test.identity);
    maskerPlayer.stop();
    throwRequestFailureOnInvalidAudioFile(
        [&](const LocalUrl &file) { maskerPlayer.loadFile(file); },
        maskerFileUrl(test));

    testMethod = testMethod_;
    fullScaleLevel_ = test.fullScaleLevel;
    maskerLevel_ = test.maskerLevel;
    condition = test.condition;
    keepVideoShown = test.keepVideoShown;
    videoScale = test.videoScale;

    hide(targetPlayer);
    maskerPlayer.apply(
        maskerLevelAmplification(maskerPlayer, maskerLevel_, fullScaleLevel_));
    preparePlayersForNextTrial(testMethod, randomizer, targetPlayer,
        maskerPlayer, maskerLevel_, fullScaleLevel_, test.videoScale);
    testMethod->writeTestingParameters(outputFile);
    trialNumber_ = 1;

    useAllChannels(targetPlayer);
    useAllChannels(maskerPlayer);
    clearChannelDelays(maskerPlayer);
    if (test.audioChannelOption == AudioChannelOption::singleSpeaker) {
        useFirstChannelOnly(targetPlayer);
        maskerPlayer.useFirstChannelOnly();
    } else if (test.audioChannelOption == AudioChannelOption::delayedMasker) {
        useFirstChannelOnly(targetPlayer);
        maskerPlayer.setChannelDelaySeconds(0, maskerChannelDelay.seconds);
    }

    if (observer_ == nullptr)
        observer_ = &nullObserver;
    observer = observer_;
    observer->notifyThatNewTestIsReady(test.identity.session);
}

void RunningATestImpl::playTrial(const AudioSettings &settings) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);

    throwRequestFailureOnInvalidAudioDevice(
        [&](auto device) {
            setAudioDevices(maskerPlayer, targetPlayer, device);
        },
        settings.audioDevice);

    playTrialTime_ = clock.time();
    observer->notifyThatTrialWillBegin(trialNumber_);
    if (condition == Condition::audioVisual)
        show(targetPlayer);
    targetPlayer.preRoll();
    trialInProgress_ = true;
}

void RunningATestImpl::notifyThatPreRollHasCompleted() {
    maskerPlayer.fadeIn();
}

void RunningATestImpl::fadeInComplete(const AudioSampleTimeWithOffset &t) {
    PlayerTimeWithDelay timeToPlayWithDelay{};
    timeToPlayWithDelay.playerTime = t.playerTime;
    timeToPlayWithDelay.delay = Delay{
        Duration{offsetDuration(maskerPlayer, t) + targetOnsetFringeDuration}
            .seconds};
    targetPlayer.playAt(timeToPlayWithDelay);
    observer->notifyThatTargetWillPlayAt(timeToPlayWithDelay);
}

void RunningATestImpl::fadeOutComplete() {
    if (!keepVideoShown)
        hide(targetPlayer);
    observer->notifyThatStimulusHasEnded();
    listener_->trialComplete();
    trialInProgress_ = false;
}

void RunningATestImpl::submit(
    const coordinate_response_measure::Response &response) {
    saveOutputFileAndPrepareNextTrialAfter(
        [&]() {
            testMethod->submit(response);
            testMethod->writeLastCoordinateResponse(outputFile);
        },
        testMethod, trialNumber_, outputFile, randomizer, targetPlayer,
        maskerPlayer, observer, maskerLevel_, fullScaleLevel_, videoScale);
}

void RunningATestImpl::prepareNextTrialIfNeeded() {
    av_speech_in_noise::prepareNextTrialIfNeeded(testMethod, trialNumber_,
        outputFile, randomizer, targetPlayer, maskerPlayer, observer,
        maskerLevel_, fullScaleLevel_, videoScale);
}

void RunningATestImpl::playCalibration(const Calibration &calibration) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);
    targetPlayer.useAllChannels();
    play(targetPlayer, calibration, RationalNumber{1, 1});
}

void RunningATestImpl::playLeftSpeakerCalibration(
    const Calibration &calibration) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);
    maskerPlayer.useFirstChannelOnly();
    play(maskerPlayer, calibration);
}

void RunningATestImpl::playRightSpeakerCalibration(
    const Calibration &calibration) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);
    maskerPlayer.useSecondChannelOnly();
    play(maskerPlayer, calibration);
}

auto RunningATestImpl::testComplete() -> bool { return testMethod->complete(); }

auto RunningATestImpl::audioDevices() -> AudioDevices {
    return maskerPlayer.outputAudioDeviceDescriptions();
}

auto RunningATestImpl::trialNumber() -> int { return trialNumber_; }

auto RunningATestImpl::targetFileName() -> std::string {
    return targetName(evaluator, testMethod);
}

auto RunningATestImpl::playTrialTime() -> std::string { return playTrialTime_; }
}
