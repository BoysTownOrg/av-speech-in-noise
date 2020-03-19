#include "RecognitionTestModel.hpp"
#include <gsl/gsl>
#include <cmath>

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
    void writeLastCoordinateResponse(OutputFile *) override {}
    void writeLastCorrectResponse(OutputFile *) override {}
    void writeLastIncorrectResponse(OutputFile *) override {}
    void writeTestingParameters(OutputFile *) override {}
    void writeLastCorrectKeywords(OutputFile *) override {}
    void writeTestResult(OutputFile *) override {}
};
}

static NullTestMethod nullTestMethod;

RecognitionTestModelImpl::RecognitionTestModelImpl(TargetPlayer &targetPlayer,
    MaskerPlayer &maskerPlayer, ResponseEvaluator &evaluator,
    OutputFile &outputFile, Randomizer &randomizer)
    : maskerPlayer{maskerPlayer},
      targetPlayer{targetPlayer}, evaluator{evaluator}, outputFile{outputFile},
      randomizer{randomizer}, testMethod{&nullTestMethod} {
    targetPlayer.subscribe(this);
    maskerPlayer.subscribe(this);
}

static auto auditoryOnly(const Condition &c) -> bool {
    return c == Condition::auditoryOnly;
}

void RecognitionTestModelImpl::subscribe(Model::EventListener *listener) {
    listener_ = listener;
}

void RecognitionTestModelImpl::throwIfTrialInProgress() {
    if (trialInProgress())
        throw Model::RequestFailure{"Trial in progress."};
}

static void useAllChannels(MaskerPlayer &player) { player.useAllChannels(); }

static void clearChannelDelays(MaskerPlayer &player) {
    player.clearChannelDelays();
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
    throwIfTrialInProgress();
    testMethod = testMethod_;
    prepareTest(test);
    trialNumber_ = 1;
}

static void useFirstChannelOnly(TargetPlayer &player) {
    player.useFirstChannelOnly();
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

auto RecognitionTestModelImpl::trialInProgress() -> bool {
    return maskerPlayer.playing();
}

void RecognitionTestModelImpl::prepareTest(const Test &test) {
    storeLevels(test);
    prepareMasker(test.maskerFilePath);
    targetPlayer.hideVideo();
    condition = test.condition;
    if (!testMethod->complete())
        preparePlayersForNextTrial();
    tryOpeningOutputFile(test.identity);
    testMethod->writeTestingParameters(&outputFile);
}

void RecognitionTestModelImpl::storeLevels(const Test &common) {
    fullScaleLevel_dB_SPL = common.fullScaleLevel_dB_SPL;
    maskerLevel_dB_SPL = common.maskerLevel_dB_SPL;
}

void RecognitionTestModelImpl::prepareMasker(const std::string &p) {
    throwInvalidAudioFileOnErrorLoading(
        &RecognitionTestModelImpl::loadMaskerFile, p);
    maskerPlayer.setLevel_dB(maskerLevel_dB());
}

void RecognitionTestModelImpl::throwInvalidAudioFileOnErrorLoading(
    void (RecognitionTestModelImpl::*f)(const std::string &),
    const std::string &file) {
    try {
        (this->*f)(file);
    } catch (const InvalidAudioFile &) {
        throw Model::RequestFailure{"unable to read " + file};
    }
}

void RecognitionTestModelImpl::loadMaskerFile(const std::string &p) {
    maskerPlayer.loadFile(p);
}

static auto dB(double x) -> double { return 20 * std::log10(x); }

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
        targetPlayer.showVideo();
}

void RecognitionTestModelImpl::preparePlayersForNextTrial() {
    prepareTargetPlayer();
    seekRandomMaskerPosition();
}

void RecognitionTestModelImpl::prepareTargetPlayer() {
    loadTargetFile(testMethod->nextTarget());
    setTargetLevel_dB(targetLevel_dB());
    targetPlayer.subscribeToPlaybackCompletion();
}

void RecognitionTestModelImpl::loadTargetFile(std::string s) {
    targetPlayer.loadFile(std::move(s));
}

void RecognitionTestModelImpl::setTargetLevel_dB(double x) {
    targetPlayer.setLevel_dB(x);
}

auto RecognitionTestModelImpl::targetLevel_dB() -> double {
    return maskerLevel_dB() + testMethod->snr_dB();
}

void RecognitionTestModelImpl::seekRandomMaskerPosition() {
    auto upperLimit = maskerPlayer.durationSeconds() -
        2 * maskerPlayer.fadeTimeSeconds() - targetPlayer.durationSeconds();
    maskerPlayer.seekSeconds(randomizer.betweenInclusive(0., upperLimit));
}

void RecognitionTestModelImpl::tryOpeningOutputFile(const TestIdentity &p) {
    outputFile.close();
    tryOpeningOutputFile_(p);
}

void RecognitionTestModelImpl::tryOpeningOutputFile_(const TestIdentity &p) {
    try {
        outputFile.openNewFile(p);
    } catch (const OutputFile::OpenFailure &) {
        throw Model::RequestFailure{"Unable to open output file."};
    }
}

void RecognitionTestModelImpl::playTrial(const AudioSettings &settings) {
    throwIfTrialInProgress();

    preparePlayersToPlay(settings);
    startTrial();
}

void RecognitionTestModelImpl::preparePlayersToPlay(const AudioSettings &p) {
    setAudioDevices(p);
}

void RecognitionTestModelImpl::setAudioDevices(const AudioSettings &p) {
    throwInvalidAudioDeviceOnErrorSettingDevice(
        &RecognitionTestModelImpl::setAudioDevices_, p.audioDevice);
}

void RecognitionTestModelImpl::throwInvalidAudioDeviceOnErrorSettingDevice(
    void (RecognitionTestModelImpl::*f)(const std::string &),
    const std::string &device) {
    try {
        (this->*f)(device);
    } catch (const InvalidAudioDevice &) {
        throw Model::RequestFailure{
            "'" + device + "' is not a valid audio device."};
    }
}

void RecognitionTestModelImpl::setAudioDevices_(const std::string &device) {
    maskerPlayer.setAudioDevice(device);
    setTargetPlayerDevice_(device);
}

void RecognitionTestModelImpl::setTargetPlayerDevice_(
    const std::string &device) {
    targetPlayer.setAudioDevice(device);
}

void RecognitionTestModelImpl::startTrial() {
    if (!auditoryOnly(condition))
        targetPlayer.showVideo();
    maskerPlayer.fadeIn();
}

void RecognitionTestModelImpl::fadeInComplete() { playTarget(); }

void RecognitionTestModelImpl::playTarget() { targetPlayer.play(); }

void RecognitionTestModelImpl::playbackComplete() { maskerPlayer.fadeOut(); }

void RecognitionTestModelImpl::fadeOutComplete() {
    targetPlayer.hideVideo();
    listener_->trialComplete();
}

static auto targetName(ResponseEvaluator &evaluator, TestMethod *testMethod)
    -> std::string {
    return evaluator.fileName(testMethod->currentTarget());
}

static void save(OutputFile &file) { file.save(); }

void RecognitionTestModelImpl::submit(
    const coordinate_response_measure::Response &response) {
    testMethod->submit(response);
    testMethod->writeLastCoordinateResponse(&outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submitCorrectResponse() {
    submitCorrectResponse_();
}

void RecognitionTestModelImpl::submitCorrectResponse_() {
    testMethod->submitCorrectResponse();
    testMethod->writeLastCorrectResponse(&outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::prepareNextTrialIfNeeded() {
    if (!testMethod->complete()) {
        ++trialNumber_;
        preparePlayersForNextTrial();
    } else {
        testMethod->writeTestResult(&outputFile);
        save(outputFile);
    }
}

void RecognitionTestModelImpl::submitIncorrectResponse() {
    submitIncorrectResponse_();
}

void RecognitionTestModelImpl::submitIncorrectResponse_() {
    testMethod->submitIncorrectResponse();
    testMethod->writeLastIncorrectResponse(&outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submit(const FreeResponse &response) {
    write(response);
    testMethod->submit(response);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submit(const CorrectKeywords &correctKeywords) {
    testMethod->submit(correctKeywords);
    testMethod->writeLastCorrectKeywords(&outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::write(const FreeResponse &p) {
    FreeResponseTrial trial;
    trial.response = p.response;
    trial.target = targetName(evaluator, testMethod);
    trial.flagged = p.flagged;
    outputFile.write(trial);
    save(outputFile);
}

void RecognitionTestModelImpl::playCalibration(const Calibration &p) {
    throwIfTrialInProgress();

    playCalibration_(p);
}

void RecognitionTestModelImpl::playCalibration_(const Calibration &p) {
    setTargetPlayerDevice(p);
    loadTargetFile(p.filePath);
    trySettingTargetLevel(p);
    targetPlayer.showVideo();
    playTarget();
}

void RecognitionTestModelImpl::setTargetPlayerDevice(const Calibration &p) {
    throwInvalidAudioDeviceOnErrorSettingDevice(
        &RecognitionTestModelImpl::setTargetPlayerDevice_, p.audioDevice);
}

void RecognitionTestModelImpl::trySettingTargetLevel(const Calibration &p) {
    try {
        setTargetLevel_dB(calibrationLevel_dB(p));
    } catch (const InvalidAudioFile &) {
        throw Model::RequestFailure{"unable to read " + p.filePath};
    }
}

auto RecognitionTestModelImpl::calibrationLevel_dB(const Calibration &p)
    -> double {
    return gsl::narrow_cast<double>(p.level_dB_SPL - p.fullScaleLevel_dB_SPL) -
        unalteredTargetLevel_dB();
}

auto RecognitionTestModelImpl::unalteredTargetLevel_dB() -> double {
    return dB(targetPlayer.rms());
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
