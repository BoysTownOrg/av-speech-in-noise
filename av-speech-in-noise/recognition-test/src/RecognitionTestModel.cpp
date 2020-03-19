#include "RecognitionTestModel.hpp"
#include <gsl/gsl>
#include <cmath>
#include <functional>

namespace av_speech_in_noise {
namespace {
class NullTestMethod : public TestMethod {
    auto complete() -> bool override { return {}; }
    auto nextTarget() -> std::string override { return {}; }
    auto currentTarget() -> std::string override { return {}; }
    auto snr_dB() -> int override { return {}; }
    void submit(const coordinate_response_measure::Response &) override {}
    void submit(const FreeResponse &) override {}
    void submit(const CorrectKeywords &) override {}
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void writeLastCoordinateResponse(OutputFile &) override {}
    void writeLastCorrectResponse(OutputFile &) override {}
    void writeLastIncorrectResponse(OutputFile &) override {}
    void writeTestingParameters(OutputFile &) override {}
    void writeLastCorrectKeywords(OutputFile &) override {}
    void writeTestResult(OutputFile &) override {}
};
}

static NullTestMethod nullTestMethod;

static auto auditoryOnly(const Condition &c) -> bool {
    return c == Condition::auditoryOnly;
}

static void useAllChannels(MaskerPlayer &player) { player.useAllChannels(); }

static void clearChannelDelays(MaskerPlayer &player) {
    player.clearChannelDelays();
}

static void useFirstChannelOnly(TargetPlayer &player) {
    player.useFirstChannelOnly();
}

static auto trialInProgress(MaskerPlayer &player) -> bool {
    return player.playing();
}

static void setAudioDevice(TargetPlayer &player, const std::string &device) {
    player.setAudioDevice(device);
}

static void setAudioDevices(MaskerPlayer &maskerPlayer,
    TargetPlayer &targetPlayer, const std::string &device) {
    maskerPlayer.setAudioDevice(device);
    setAudioDevice(targetPlayer, device);
}

static void throwInvalidAudioDeviceOnError(
    const std::function<void(const std::string &)> &f,
    const std::string &device) {
    try {
        f(device);
    } catch (const InvalidAudioDevice &) {
        throw Model::RequestFailure{
            "'" + device + "' is not a valid audio device."};
    }
}

static void throwIfTrialInProgress(MaskerPlayer &player) {
    if (trialInProgress(player))
        throw Model::RequestFailure{"Trial in progress."};
}

static void setLevel_dB(TargetPlayer &player, double x) {
    player.setLevel_dB(x);
}

static void loadFile(TargetPlayer &player, std::string s) {
    player.loadFile(std::move(s));
}

static void play(TargetPlayer &player) { player.play(); }

static auto dB(double x) -> double { return 20 * std::log10(x); }

static auto targetName(ResponseEvaluator &evaluator, TestMethod *testMethod)
    -> std::string {
    return evaluator.fileName(testMethod->currentTarget());
}

static void save(OutputFile &file) { file.save(); }

static void tryOpening(OutputFile &file, const TestIdentity &p) {
    file.close();
    try {
        file.openNewFile(p);
    } catch (const OutputFile::OpenFailure &) {
        throw Model::RequestFailure{"Unable to open output file."};
    }
}

static auto level_dB(TargetPlayer &player, const Calibration &p) -> double {
    return gsl::narrow_cast<double>(p.level_dB_SPL - p.fullScaleLevel_dB_SPL) -
        dB(player.rms());
}

static void show(TargetPlayer &player) { player.showVideo(); }

RecognitionTestModelImpl::RecognitionTestModelImpl(TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, ResponseEvaluator &evaluator,
    OutputFile &outputFile, Randomizer &randomizer)
    : maskerPlayer{maskerPlayer},
      targetPlayer{targetPlayer}, evaluator{evaluator}, outputFile{outputFile},
      randomizer{randomizer}, testMethod{&nullTestMethod} {
    targetPlayer.subscribe(this);
    maskerPlayer.subscribe(this);
}

void RecognitionTestModelImpl::subscribe(Model::EventListener *listener) {
    listener_ = listener;
}

void RecognitionTestModelImpl::initialize(
    TestMethod *testMethod_, const Test &test) {
    initialize_(testMethod_, test);
    targetPlayer.useAllChannels();
    useAllChannels(maskerPlayer);
    clearChannelDelays(maskerPlayer);
}

void RecognitionTestModelImpl::initialize_(
    TestMethod *testMethod_, const Test &test) {
    throwIfTrialInProgress(maskerPlayer);
    testMethod = testMethod_;
    prepareTest(test);
    trialNumber_ = 1;
}

void RecognitionTestModelImpl::initializeWithSingleSpeaker(
    TestMethod *testMethod_, const Test &test) {
    initialize_(testMethod_, test);
    useFirstChannelOnly(targetPlayer);
    maskerPlayer.useFirstChannelOnly();
    clearChannelDelays(maskerPlayer);
}

void RecognitionTestModelImpl::initializeWithDelayedMasker(
    TestMethod *testMethod_, const Test &test) {
    initialize_(testMethod_, test);
    useFirstChannelOnly(targetPlayer);
    useAllChannels(maskerPlayer);
    maskerPlayer.setChannelDelaySeconds(0, maskerChannelDelaySeconds);
}

void RecognitionTestModelImpl::prepareTest(const Test &test) {
    storeLevels(test);
    prepareMasker(test.maskerFilePath);
    targetPlayer.hideVideo();
    condition = test.condition;
    if (!testMethod->complete())
        preparePlayersForNextTrial();
    tryOpening(outputFile, test.identity);
    testMethod->writeTestingParameters(outputFile);
}

void RecognitionTestModelImpl::storeLevels(const Test &test) {
    fullScaleLevel_dB_SPL = test.fullScaleLevel_dB_SPL;
    maskerLevel_dB_SPL = test.maskerLevel_dB_SPL;
}

void RecognitionTestModelImpl::prepareMasker(const std::string &file) {
    try {
        maskerPlayer.loadFile(file);
    } catch (const InvalidAudioFile &) {
        throw Model::RequestFailure{"unable to read " + file};
    }
    maskerPlayer.setLevel_dB(maskerLevel_dB());
}

auto RecognitionTestModelImpl::maskerLevel_dB() -> double {
    return desiredMaskerLevel_dB() - dB(maskerPlayer.rms());
}

auto RecognitionTestModelImpl::desiredMaskerLevel_dB() -> int {
    return maskerLevel_dB_SPL - fullScaleLevel_dB_SPL;
}

void RecognitionTestModelImpl::prepareVideo(const Condition &p) {
    if (auditoryOnly(p))
        targetPlayer.hideVideo();
    else
        show(targetPlayer);
}

void RecognitionTestModelImpl::preparePlayersForNextTrial() {
    prepareTargetPlayer();
    seekRandomMaskerPosition();
}

void RecognitionTestModelImpl::prepareTargetPlayer() {
    loadFile(targetPlayer, testMethod->nextTarget());
    setLevel_dB(targetPlayer, targetLevel_dB());
    targetPlayer.subscribeToPlaybackCompletion();
}

auto RecognitionTestModelImpl::targetLevel_dB() -> double {
    return maskerLevel_dB() + testMethod->snr_dB();
}

void RecognitionTestModelImpl::seekRandomMaskerPosition() {
    auto upperLimit = maskerPlayer.durationSeconds() -
        2 * maskerPlayer.fadeTimeSeconds() - targetPlayer.durationSeconds();
    maskerPlayer.seekSeconds(randomizer.betweenInclusive(0., upperLimit));
}

void RecognitionTestModelImpl::playTrial(const AudioSettings &settings) {
    throwIfTrialInProgress(maskerPlayer);

    throwInvalidAudioDeviceOnError(
        [&](auto device) {
            setAudioDevices(maskerPlayer, targetPlayer, device);
        },
        settings.audioDevice);
    if (!auditoryOnly(condition))
        show(targetPlayer);
    maskerPlayer.fadeIn();
}

void RecognitionTestModelImpl::fadeInComplete() { play(targetPlayer); }

void RecognitionTestModelImpl::playbackComplete() { maskerPlayer.fadeOut(); }

void RecognitionTestModelImpl::fadeOutComplete() {
    targetPlayer.hideVideo();
    listener_->trialComplete();
}

void RecognitionTestModelImpl::prepareNextTrialIfNeeded() {
    if (!testMethod->complete()) {
        ++trialNumber_;
        preparePlayersForNextTrial();
    } else {
        testMethod->writeTestResult(outputFile);
        save(outputFile);
    }
}

void RecognitionTestModelImpl::submitCorrectResponse() {
    testMethod->submitCorrectResponse();
    testMethod->writeLastCorrectResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submitIncorrectResponse() {
    testMethod->submitIncorrectResponse();
    testMethod->writeLastIncorrectResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submit(
    const coordinate_response_measure::Response &response) {
    testMethod->submit(response);
    testMethod->writeLastCoordinateResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submit(const FreeResponse &response) {
    testMethod->submit(response);
    FreeResponseTrial trial;
    trial.response = response.response;
    trial.target = targetName(evaluator, testMethod);
    trial.flagged = response.flagged;
    outputFile.write(trial);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submit(const CorrectKeywords &correctKeywords) {
    testMethod->submit(correctKeywords);
    testMethod->writeLastCorrectKeywords(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::playCalibration(const Calibration &p) {
    throwIfTrialInProgress(maskerPlayer);

    playCalibration_(p);
}

void RecognitionTestModelImpl::playCalibration_(const Calibration &p) {
    throwInvalidAudioDeviceOnError(
        [&](auto device) { setAudioDevice(targetPlayer, device); },
        p.audioDevice);
    loadFile(targetPlayer, p.filePath);
    try {
        setLevel_dB(targetPlayer, level_dB(targetPlayer, p));
    } catch (const InvalidAudioFile &) {
        throw Model::RequestFailure{"unable to read " + p.filePath};
    }
    show(targetPlayer);
    play(targetPlayer);
}

auto RecognitionTestModelImpl::testComplete() -> bool {
    return testMethod->complete();
}

auto RecognitionTestModelImpl::audioDevices() -> AudioDevices {
    return maskerPlayer.outputAudioDeviceDescriptions();
}

auto RecognitionTestModelImpl::trialNumber() -> int { return trialNumber_; }

auto RecognitionTestModelImpl::targetFileName() -> std::string {
    return targetName(evaluator, testMethod);
}
}
