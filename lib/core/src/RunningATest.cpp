#include "RunningATest.hpp"

#include <gsl/gsl>

#include <functional>

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
}

static NullTestMethod nullTestMethod;

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

static auto maskerLevelAmplification(
    MaskerPlayer &maskerPlayer, const Test &test) -> LevelAmplification {
    return LevelAmplification{DigitalLevel{
        test.maskerLevel - test.fullScaleLevel - maskerPlayer.digitalLevel()}
                                  .dBov};
}

static auto targetLevelAmplification(
    TestMethod *testMethod, MaskerPlayer &maskerPlayer, const Test &test) {
    return LevelAmplification{
        maskerLevelAmplification(maskerPlayer, test).dB + testMethod->snr().dB};
}

static void preparePlayersForNextTrial(TestMethod *testMethod,
    Randomizer &randomizer, TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, const Test &test) {
    loadFile(targetPlayer, testMethod->nextTarget(), test.videoScale);
    apply(
        targetPlayer, targetLevelAmplification(testMethod, maskerPlayer, test));
    const auto maskerPlayerSeekTimeUpperLimit{
        maskerPlayer.duration() - trialDuration(targetPlayer, maskerPlayer)};
    maskerPlayer.seekSeconds(randomizer.betweenInclusive(
        0., maskerPlayerSeekTimeUpperLimit.seconds));
    maskerPlayer.setSteadyLevelFor(steadyLevelDuration(targetPlayer));
}

static void prepareNextTrialIfNeeded(TestMethod *testMethod, int &trialNumber_,
    OutputFile &outputFile, Randomizer &randomizer, TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer,
    const std::vector<std::reference_wrapper<RunningATest::TestObserver>>
        &observers,
    const Test &test) {
    for (auto observer : observers)
        observer.get().notifyThatSubjectHasResponded();
    if (!testMethod->complete()) {
        ++trialNumber_;
        preparePlayersForNextTrial(
            testMethod, randomizer, targetPlayer, maskerPlayer, test);
    } else {
        testMethod->writeTestResult(outputFile);
        save(outputFile);
    }
}

static void saveOutputFileAndPrepareNextTrialAfter(
    const std::function<void()> &f, TestMethod *testMethod, int &trialNumber_,
    OutputFile &outputFile, Randomizer &randomizer, TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer,
    const std::vector<std::reference_wrapper<RunningATest::TestObserver>>
        &observer,
    const Test &test) {
    f();
    save(outputFile);
    prepareNextTrialIfNeeded(testMethod, trialNumber_, outputFile, randomizer,
        targetPlayer, maskerPlayer, observer, test);
}

RunningATestImpl::RunningATestImpl(TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, ResponseEvaluator &evaluator,
    OutputFile &outputFile, Randomizer &randomizer, Clock &clock)
    : maskerPlayer{maskerPlayer}, targetPlayer{targetPlayer},
      evaluator{evaluator}, outputFile{outputFile}, randomizer{randomizer},
      clock{clock}, testMethod{&nullTestMethod} {
    targetPlayer.attach(this);
    maskerPlayer.attach(this);
}

void RunningATestImpl::attach(RunningATest::RequestObserver *listener) {
    requestObserver = listener;
}

void RunningATestImpl::initialize(TestMethod *testMethod_, const Test &test,
    std::vector<std::reference_wrapper<TestObserver>> observers_) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);

    if (testMethod_->complete())
        return;

    tryOpening(outputFile, test.identity);
    maskerPlayer.stop();
    throwRequestFailureOnInvalidAudioFile(
        [&](const LocalUrl &file) { maskerPlayer.loadFile(file); },
        maskerFileUrl(test));

    testMethod = testMethod_;
    this->test = test;

    hide(targetPlayer);
    maskerPlayer.apply(maskerLevelAmplification(maskerPlayer, test));
    preparePlayersForNextTrial(
        testMethod, randomizer, targetPlayer, maskerPlayer, test);
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

    testObservers = observers_;
    for (auto observer : testObservers)
        observer.get().notifyThatNewTestIsReady(test.identity.session);
}

void RunningATestImpl::playTrial(const AudioSettings &settings) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);

    throwRequestFailureOnInvalidAudioDevice(
        [&](auto device) {
            setAudioDevices(maskerPlayer, targetPlayer, device);
        },
        settings.audioDevice);

    playTrialTime_ = clock.time();
    for (auto observer : testObservers)
        observer.get().notifyThatTrialWillBegin(trialNumber_);
    if (test.condition == Condition::audioVisual)
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
    for (auto observer : testObservers)
        observer.get().notifyThatTargetWillPlayAt(timeToPlayWithDelay);
}

void RunningATestImpl::fadeOutComplete() {
    if (!test.keepVideoShown)
        hide(targetPlayer);
    for (auto observer : testObservers)
        observer.get().notifyThatStimulusHasEnded();
    requestObserver->notifyThatPlayTrialHasCompleted();
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
        maskerPlayer, testObservers, test);
}

void RunningATestImpl::prepareNextTrialIfNeeded() {
    av_speech_in_noise::prepareNextTrialIfNeeded(testMethod, trialNumber_,
        outputFile, randomizer, targetPlayer, maskerPlayer, testObservers,
        test);
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
