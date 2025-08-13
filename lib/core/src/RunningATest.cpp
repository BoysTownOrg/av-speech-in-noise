#include "RunningATest.hpp"
#include "Configuration.hpp"
#include "IOutputFile.hpp"
#include "av-speech-in-noise/Model.hpp"

#include <algorithm>
#include <gsl/gsl>

#include <functional>
#include <vector>

namespace av_speech_in_noise {
namespace {
class NullTestMethod : public TestMethod {
    auto complete() -> bool override { return {}; }
    auto nextTarget() -> LocalUrl override { return {}; }
    auto currentTarget() -> LocalUrl override { return {}; }
    auto snr() -> FloatSNR override { return FloatSNR{}; }
    void submit(const coordinate_response_measure::Response &) override {}
    void writeLastCoordinateResponse(OutputFile &) override {}
    void writeTestResult(OutputFile &) override {}
    void write(std::ostream &) override {}
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

static constexpr auto operator-(const RealLevel &a, const RealLevel &b)
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
        [&](const auto &device) { setAudioDevice(targetPlayer, device); },
        calibration.audioDevice);
    throwRequestFailureOnInvalidAudioFile(
        [&](const auto &file) {
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
        [&](const auto &device) { setAudioDevice(maskerPlayer, device); },
        calibration.audioDevice);
    throwRequestFailureOnInvalidAudioFile(
        [&](const auto &file) {
            loadFile(maskerPlayer, file);
            apply(maskerPlayer, levelAmplification(maskerPlayer, calibration));
        },
        calibration.fileUrl);
    play(maskerPlayer);
}

static auto maskerLevelAmplification(MaskerPlayer &maskerPlayer,
    RealLevel maskerLevel, const Test &test) -> LevelAmplification {
    return LevelAmplification{DigitalLevel{
        maskerLevel - test.fullScaleLevel - maskerPlayer.digitalLevel()}
            .dBov};
}

static auto targetLevelAmplification(TestMethod *testMethod,
    MaskerPlayer &maskerPlayer, RealLevel maskerLevel, const Test &test) {
    return LevelAmplification{
        maskerLevelAmplification(maskerPlayer, maskerLevel, test).dB +
        testMethod->snr().dB};
}

static void preparePlayersForNextTrial(TestMethod *testMethod,
    Randomizer &randomizer, TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, RealLevel maskerLevel, const Test &test,
    RationalNumber videoScale) {
    loadFile(targetPlayer, testMethod->nextTarget(), videoScale);
    apply(targetPlayer,
        targetLevelAmplification(testMethod, maskerPlayer, maskerLevel, test));
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
    RealLevel maskerLevel, const Test &test, RationalNumber videoScale) {
    for (auto observer : observers)
        observer.get().notifyThatSubjectHasResponded();
    if (!testMethod->complete()) {
        ++trialNumber_;
        preparePlayersForNextTrial(testMethod, randomizer, targetPlayer,
            maskerPlayer, maskerLevel, test, videoScale);
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
    RealLevel maskerLevel, const Test &test, RationalNumber videoScale) {
    f();
    save(outputFile);
    prepareNextTrialIfNeeded(testMethod, trialNumber_, outputFile, randomizer,
        targetPlayer, maskerPlayer, observer, maskerLevel, test, videoScale);
}

RunningATestImpl::RunningATestImpl(TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, ResponseEvaluator &evaluator,
    OutputFile &outputFile, Randomizer &randomizer, Clock &clock,
    ConfigurationRegistry &registry)
    : maskerPlayer{maskerPlayer}, targetPlayer{targetPlayer},
      evaluator{evaluator}, outputFile{outputFile}, randomizer{randomizer},
      clock{clock}, testMethod{&nullTestMethod} {
    registry.subscribe(*this, "video scale denominator");
    registry.subscribe(*this, "video scale numerator");
    registry.subscribe(*this, "keep video shown");
    registry.subscribe(*this, "subject ID");
    registry.subscribe(*this, "tester ID");
    registry.subscribe(*this, "session");
    registry.subscribe(*this, "RME setting");
    registry.subscribe(*this, "transducer");
    registry.subscribe(*this, "meta");
    registry.subscribe(*this, "method");
    registry.subscribe(*this, "masker");
    registry.subscribe(*this, "masker level (dB SPL)");

    targetPlayer.attach(this);
    maskerPlayer.attach(this);
}

void RunningATestImpl::attach(RunningATest::RequestObserver *listener) {
    requestObserver = listener;
}

void RunningATestImpl::initialize(TestMethod *testMethod, const Test &test) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);

    if (testMethod->complete())
        return;

    this->testMethod = testMethod;
    this->test = test;
    trialNumber_ = 1;

    tryOpening(outputFile, testIdentity);
    maskerPlayer.stop();
    throwRequestFailureOnInvalidAudioFile(
        [&](const LocalUrl &file) { maskerPlayer.loadFile(file); },
        maskerFileUrl);

    hide(targetPlayer);
    maskerPlayer.apply(
        maskerLevelAmplification(maskerPlayer, maskerLevel, test));
    preparePlayersForNextTrial(testMethod, randomizer, targetPlayer,
        maskerPlayer, maskerLevel, test, videoScale);
    outputFile.write(*this);

    useAllChannels(targetPlayer);
    useAllChannels(maskerPlayer);
    clearChannelDelays(maskerPlayer);
    if (audioChannelOption == AudioChannelOption::singleSpeaker) {
        useFirstChannelOnly(targetPlayer);
        maskerPlayer.useFirstChannelOnly();
    } else if (audioChannelOption == AudioChannelOption::delayedMasker) {
        useFirstChannelOnly(targetPlayer);
        maskerPlayer.setChannelDelaySeconds(0, maskerChannelDelay.seconds);
    }

    if (test.enableVibrotactileStimulus)
        maskerPlayer.enableVibrotactileStimulus();
    else
        maskerPlayer.disableVibrotactileStimulus();

    for (auto observer : testObservers)
        observer.get().notifyThatNewTestIsReady(testIdentity.session);
}

void RunningATestImpl::playTrial(const AudioSettings &settings) {
    throwRequestFailureIfTrialInProgress(trialInProgress_);

    throwRequestFailureOnInvalidAudioDevice(
        [&](const auto &device) {
            setAudioDevices(maskerPlayer, targetPlayer, device);
        },
        settings.audioDevice);

    playTrialTime_ = clock.time();
    for (auto observer : testObservers)
        observer.get().notifyThatTrialWillBegin(trialNumber_);
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
    for (auto observer : testObservers)
        observer.get().notifyThatTargetWillPlayAt(timeToPlayWithDelay);
}

void RunningATestImpl::fadeOutComplete() {
    if (!keepVideoShown)
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
        maskerPlayer, testObservers, maskerLevel, test, videoScale);
}

void RunningATestImpl::prepareNextTrialIfNeeded() {
    av_speech_in_noise::prepareNextTrialIfNeeded(testMethod, trialNumber_,
        outputFile, randomizer, targetPlayer, maskerPlayer, testObservers,
        maskerLevel, test, videoScale);
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

static auto insertSubjectId(std::ostream &stream, const TestIdentity &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "subject", p.subjectId);
}

static auto insertTester(std::ostream &stream, const TestIdentity &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "tester", p.testerId);
}

static auto insertSession(std::ostream &stream, const TestIdentity &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "session", p.session);
}

static auto insertMethod(std::ostream &stream, const TestIdentity &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "method", p.method);
}

static auto insertRmeSetting(std::ostream &stream, const TestIdentity &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "RME setting", p.rmeSetting);
}

static auto insertTransducer(std::ostream &stream, const TestIdentity &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "transducer", p.transducer);
}

static auto operator<<(std::ostream &stream, const TestIdentity &identity)
    -> std::ostream & {
    return insertTransducer(
        insertRmeSetting(
            insertMethod(
                insertSession(
                    insertTester(insertSubjectId(stream, identity), identity),
                    identity),
                identity),
            identity),
        identity);
}

void RunningATestImpl::write(std::ostream &stream) {
    stream << testIdentity;
    insertLabeledLine(stream, "masker", maskerFileUrl.path);
    insertLabeledLine(stream, "masker level (dB SPL)", maskerLevel.dB_SPL);
    insertLabeledLine(stream, "condition", name(condition));
    testMethod->write(stream);
    insertNewLine(stream);
}

void RunningATestImpl::configure(
    const std::string &key, const std::string &value) {
    if (key == "subject ID")
        testIdentity.subjectId = value;
    else if (key == "tester ID")
        testIdentity.testerId = value;
    else if (key == "session")
        testIdentity.session = value;
    else if (key == "RME setting")
        testIdentity.rmeSetting = value;
    else if (key == "transducer")
        testIdentity.transducer = value;
    else if (key == "meta")
        testIdentity.meta = value;
    else if (key == "method") {
        testIdentity.method = value;
        audioChannelOption = AudioChannelOption::all;
        if (contains(value, "not spatial"))
            audioChannelOption = AudioChannelOption::singleSpeaker;
        else if (contains(value, "spatial"))
            audioChannelOption = AudioChannelOption::delayedMasker;
    } else if (key == "video scale denominator")
        videoScale.denominator = integer(value);
    else if (key == "video scale numerator")
        videoScale.numerator = integer(value);
    else if (key == "keep video shown")
        keepVideoShown = boolean(value);
    else if (key == "masker")
        maskerFileUrl.path = value;
    else if (key == "masker level (dB SPL)")
        maskerLevel.dB_SPL = integer(value);
    else if (key == "condition")
        for (auto c : {Condition::auditoryOnly, Condition::audioVisual})
            if (value == name(c))
                condition = c;
}

void RunningATestImpl::add(TestObserver &t) {
    testObservers.emplace_back(std::ref(t));
}

void RunningATestImpl::remove(TestObserver &t) {
    testObservers.erase(
        std::remove_if(testObservers.begin(), testObservers.end(),
            [&t](const std::reference_wrapper<TestObserver> &to) {
                return &to.get() == &t;
            }),
        testObservers.end());
}
}
