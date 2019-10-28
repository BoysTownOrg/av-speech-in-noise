#include "RecognitionTestModel.hpp"
#include <cmath>

namespace av_speech_in_noise {
namespace {
class NullTestMethod : public TestMethod {
    bool complete() override { return {}; }
    std::string next() override { return {}; }
    std::string current() override { return {}; }
    int snr_dB() override { return {}; }
    void submitCorrectResponse() override {}
    void submitIncorrectResponse() override {}
    void writeLastCoordinateResponse(OutputFile *) override {}
    void writeLastCorrectResponse(OutputFile *) override {}
    void writeLastIncorrectResponse(OutputFile *) override {}
    void writeTestingParameters(OutputFile *) override {}
    void submitResponse(
        const coordinate_response_measure::Response &) override {}
    void submitResponse(const FreeResponse &) override {}
};
}
static NullTestMethod nullTestMethod;

RecognitionTestModelImpl::RecognitionTestModelImpl(
    TargetPlayer *targetPlayer, MaskerPlayer *maskerPlayer,
    ResponseEvaluator *evaluator, OutputFile *outputFile,
    Randomizer *randomizer)
    : maskerPlayer{maskerPlayer},
      targetPlayer{targetPlayer}, evaluator{evaluator}, outputFile{outputFile},
      randomizer{randomizer}, testMethod{&nullTestMethod} {
    targetPlayer->subscribe(this);
    maskerPlayer->subscribe(this);
}

void RecognitionTestModelImpl::subscribe(Model::EventListener *listener) {
    listener_ = listener;
}

void RecognitionTestModelImpl::throwIfTrialInProgress() {
    if (trialInProgress())
        throw Model::RequestFailure{"Trial in progress."};
}

void RecognitionTestModelImpl::initialize(TestMethod *testMethod_,
    const Test &common, const TestIdentity &information) {
    throwIfTrialInProgress();
    testMethod = testMethod_;
    prepareCommonTest(common, information);
    trialNumber_ = 1;
}

bool RecognitionTestModelImpl::trialInProgress() {
    return maskerPlayer->playing();
}

void RecognitionTestModelImpl::prepareCommonTest(
    const Test &common, const TestIdentity &information) {
    storeLevels(common);
    prepareMasker(common.maskerFilePath);
    targetPlayer->hideVideo();
    condition = common.condition;
    if (!testMethod->complete())
        preparePlayersForNextTrial();
    tryOpeningOutputFile(information);
    testMethod->writeTestingParameters(outputFile);
}

void RecognitionTestModelImpl::storeLevels(const Test &common) {
    fullScaleLevel_dB_SPL = common.fullScaleLevel_dB_SPL;
    maskerLevel_dB_SPL = common.maskerLevel_dB_SPL;
}

void RecognitionTestModelImpl::prepareMasker(const std::string &p) {
    throwInvalidAudioFileOnErrorLoading(
        &RecognitionTestModelImpl::loadMaskerFile, p);
    maskerPlayer->setLevel_dB(maskerLevel_dB());
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
    maskerPlayer->loadFile(p);
}

static double dB(double x) { return 20 * std::log10(x); }

double RecognitionTestModelImpl::maskerLevel_dB() {
    return desiredMaskerLevel_dB() - dB(maskerPlayer->rms());
}

int RecognitionTestModelImpl::desiredMaskerLevel_dB() {
    return maskerLevel_dB_SPL - fullScaleLevel_dB_SPL;
}

void RecognitionTestModelImpl::prepareVideo(const Condition &p) {
    if (auditoryOnly(p))
        targetPlayer->hideVideo();
    else
        targetPlayer->showVideo();
}

bool RecognitionTestModelImpl::auditoryOnly(const Condition &c) {
    return c == Condition::auditoryOnly;
}

void RecognitionTestModelImpl::preparePlayersForNextTrial() {
    prepareTargetPlayer();
    seekRandomMaskerPosition();
}

void RecognitionTestModelImpl::prepareTargetPlayer() {
    loadTargetFile(testMethod->next());
    setTargetLevel_dB(targetLevel_dB());
    targetPlayer->subscribeToPlaybackCompletion();
}

void RecognitionTestModelImpl::loadTargetFile(std::string s) {
    targetPlayer->loadFile(std::move(s));
}

void RecognitionTestModelImpl::setTargetLevel_dB(double x) {
    targetPlayer->setLevel_dB(x);
}

double RecognitionTestModelImpl::targetLevel_dB() {
    return maskerLevel_dB() + testMethod->snr_dB();
}

void RecognitionTestModelImpl::seekRandomMaskerPosition() {
    auto upperLimit = maskerPlayer->durationSeconds() -
        2 * maskerPlayer->fadeTimeSeconds() - targetPlayer->durationSeconds();
    maskerPlayer->seekSeconds(randomizer->randomFloatBetween(0, upperLimit));
}

void RecognitionTestModelImpl::tryOpeningOutputFile(
    const TestIdentity &p) {
    outputFile->close();
    tryOpeningOutputFile_(p);
}

void RecognitionTestModelImpl::tryOpeningOutputFile_(
    const TestIdentity &p) {
    try {
        outputFile->openNewFile(p);
    } catch (const OutputFile::OpenFailure &) {
        throw Model::RequestFailure{"Unable to open output file."};
    }
}

void RecognitionTestModelImpl::playTrial(const AudioSettings &settings) {
    throwIfTrialInProgress();

    preparePlayersToPlay(settings);
    startTrial();
}

void RecognitionTestModelImpl::preparePlayersToPlay(
    const AudioSettings &p) {
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

void RecognitionTestModelImpl::setAudioDevices_(
    const std::string &device) {
    maskerPlayer->setAudioDevice(device);
    setTargetPlayerDevice_(device);
}

void RecognitionTestModelImpl::setTargetPlayerDevice_(
    const std::string &device) {
    targetPlayer->setAudioDevice(device);
}

void RecognitionTestModelImpl::startTrial() {
    if (!auditoryOnly(condition))
        targetPlayer->showVideo();
    maskerPlayer->fadeIn();
}

void RecognitionTestModelImpl::fadeInComplete() { playTarget(); }

void RecognitionTestModelImpl::playTarget() { targetPlayer->play(); }

void RecognitionTestModelImpl::playbackComplete() {
    maskerPlayer->fadeOut();
}

void RecognitionTestModelImpl::fadeOutComplete() {
    targetPlayer->hideVideo();
    listener_->trialComplete();
}

static std::string targetName(
    ResponseEvaluator *evaluator, TestMethod *testMethod) {
    return evaluator->fileName(testMethod->current());
}

static void save(OutputFile *file) { file->save(); }

void RecognitionTestModelImpl::submitResponse(
    const coordinate_response_measure::Response &response) {
    testMethod->submitResponse(response);
    testMethod->writeLastCoordinateResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submitCorrectResponse() {
    submitCorrectResponse_();
}

void RecognitionTestModelImpl::submitCorrectResponse_() {
    testMethod->submitCorrectResponse();
    testMethod->writeLastCorrectResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::prepareNextTrialIfNeeded() {
    if (!testMethod->complete()) {
        ++trialNumber_;
        preparePlayersForNextTrial();
    }
}

void RecognitionTestModelImpl::submitIncorrectResponse() {
    submitIncorrectResponse_();
}

void RecognitionTestModelImpl::submitIncorrectResponse_() {
    testMethod->submitIncorrectResponse();
    testMethod->writeLastIncorrectResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::submitResponse(
    const FreeResponse &response) {
    writeTrial(response);
    testMethod->submitResponse(response);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModelImpl::writeTrial(const FreeResponse &p) {
    FreeResponseTrial trial;
    trial.response = p.response;
    trial.target = targetName(evaluator, testMethod);
    outputFile->writeTrial(trial);
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
    prepareVideo(p.condition);
    playTarget();
}

void RecognitionTestModelImpl::setTargetPlayerDevice(
    const Calibration &p) {
    throwInvalidAudioDeviceOnErrorSettingDevice(
        &RecognitionTestModelImpl::setTargetPlayerDevice_,
        p.audioSettings.audioDevice);
}

void RecognitionTestModelImpl::trySettingTargetLevel(
    const Calibration &p) {
    try {
        setTargetLevel_dB(calibrationLevel_dB(p));
    } catch (const InvalidAudioFile &) {
        throw Model::RequestFailure{"unable to read " + p.filePath};
    }
}

double RecognitionTestModelImpl::calibrationLevel_dB(
    const Calibration &p) {
    return p.level_dB_SPL - p.fullScaleLevel_dB_SPL - unalteredTargetLevel_dB();
}

double RecognitionTestModelImpl::unalteredTargetLevel_dB() {
    return dB(targetPlayer->rms());
}

bool RecognitionTestModelImpl::testComplete() {
    return testMethod->complete();
}

std::vector<std::string> RecognitionTestModelImpl::audioDevices() {
    return maskerPlayer->outputAudioDeviceDescriptions();
}

int RecognitionTestModelImpl::trialNumber() { return trialNumber_; }
}
