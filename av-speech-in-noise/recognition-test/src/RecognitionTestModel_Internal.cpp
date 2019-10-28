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

RecognitionTestModel::RecognitionTestModel(
    TargetPlayer *targetPlayer, MaskerPlayer *maskerPlayer,
    ResponseEvaluator *evaluator, OutputFile *outputFile,
    Randomizer *randomizer)
    : maskerPlayer{maskerPlayer},
      targetPlayer{targetPlayer}, evaluator{evaluator}, outputFile{outputFile},
      randomizer{randomizer}, testMethod{&nullTestMethod} {
    targetPlayer->subscribe(this);
    maskerPlayer->subscribe(this);
}

void RecognitionTestModel::subscribe(Model::EventListener *listener) {
    listener_ = listener;
}

void RecognitionTestModel::throwIfTrialInProgress() {
    if (trialInProgress())
        throw Model::RequestFailure{"Trial in progress."};
}

void RecognitionTestModel::initialize(TestMethod *testMethod_,
    const Test &common, const TestIdentity &information) {
    throwIfTrialInProgress();
    testMethod = testMethod_;
    prepareCommonTest(common, information);
    trialNumber_ = 1;
}

bool RecognitionTestModel::trialInProgress() {
    return maskerPlayer->playing();
}

void RecognitionTestModel::prepareCommonTest(
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

void RecognitionTestModel::storeLevels(const Test &common) {
    fullScaleLevel_dB_SPL = common.fullScaleLevel_dB_SPL;
    maskerLevel_dB_SPL = common.maskerLevel_dB_SPL;
}

void RecognitionTestModel::prepareMasker(const std::string &p) {
    throwInvalidAudioFileOnErrorLoading(
        &RecognitionTestModel::loadMaskerFile, p);
    maskerPlayer->setLevel_dB(maskerLevel_dB());
}

void RecognitionTestModel::throwInvalidAudioFileOnErrorLoading(
    void (RecognitionTestModel::*f)(const std::string &),
    const std::string &file) {
    try {
        (this->*f)(file);
    } catch (const InvalidAudioFile &) {
        throw Model::RequestFailure{"unable to read " + file};
    }
}

void RecognitionTestModel::loadMaskerFile(const std::string &p) {
    maskerPlayer->loadFile(p);
}

static double dB(double x) { return 20 * std::log10(x); }

double RecognitionTestModel::maskerLevel_dB() {
    return desiredMaskerLevel_dB() - dB(maskerPlayer->rms());
}

int RecognitionTestModel::desiredMaskerLevel_dB() {
    return maskerLevel_dB_SPL - fullScaleLevel_dB_SPL;
}

void RecognitionTestModel::prepareVideo(const Condition &p) {
    if (auditoryOnly(p))
        targetPlayer->hideVideo();
    else
        targetPlayer->showVideo();
}

bool RecognitionTestModel::auditoryOnly(const Condition &c) {
    return c == Condition::auditoryOnly;
}

void RecognitionTestModel::preparePlayersForNextTrial() {
    prepareTargetPlayer();
    seekRandomMaskerPosition();
}

void RecognitionTestModel::prepareTargetPlayer() {
    loadTargetFile(testMethod->next());
    setTargetLevel_dB(targetLevel_dB());
    targetPlayer->subscribeToPlaybackCompletion();
}

void RecognitionTestModel::loadTargetFile(std::string s) {
    targetPlayer->loadFile(std::move(s));
}

void RecognitionTestModel::setTargetLevel_dB(double x) {
    targetPlayer->setLevel_dB(x);
}

double RecognitionTestModel::targetLevel_dB() {
    return maskerLevel_dB() + testMethod->snr_dB();
}

void RecognitionTestModel::seekRandomMaskerPosition() {
    auto upperLimit = maskerPlayer->durationSeconds() -
        2 * maskerPlayer->fadeTimeSeconds() - targetPlayer->durationSeconds();
    maskerPlayer->seekSeconds(randomizer->randomFloatBetween(0, upperLimit));
}

void RecognitionTestModel::tryOpeningOutputFile(
    const TestIdentity &p) {
    outputFile->close();
    tryOpeningOutputFile_(p);
}

void RecognitionTestModel::tryOpeningOutputFile_(
    const TestIdentity &p) {
    try {
        outputFile->openNewFile(p);
    } catch (const OutputFile::OpenFailure &) {
        throw Model::RequestFailure{"Unable to open output file."};
    }
}

void RecognitionTestModel::playTrial(const AudioSettings &settings) {
    throwIfTrialInProgress();

    preparePlayersToPlay(settings);
    startTrial();
}

void RecognitionTestModel::preparePlayersToPlay(
    const AudioSettings &p) {
    setAudioDevices(p);
}

void RecognitionTestModel::setAudioDevices(const AudioSettings &p) {
    throwInvalidAudioDeviceOnErrorSettingDevice(
        &RecognitionTestModel::setAudioDevices_, p.audioDevice);
}

void RecognitionTestModel::throwInvalidAudioDeviceOnErrorSettingDevice(
    void (RecognitionTestModel::*f)(const std::string &),
    const std::string &device) {
    try {
        (this->*f)(device);
    } catch (const InvalidAudioDevice &) {
        throw Model::RequestFailure{
            "'" + device + "' is not a valid audio device."};
    }
}

void RecognitionTestModel::setAudioDevices_(
    const std::string &device) {
    maskerPlayer->setAudioDevice(device);
    setTargetPlayerDevice_(device);
}

void RecognitionTestModel::setTargetPlayerDevice_(
    const std::string &device) {
    targetPlayer->setAudioDevice(device);
}

void RecognitionTestModel::startTrial() {
    if (!auditoryOnly(condition))
        targetPlayer->showVideo();
    maskerPlayer->fadeIn();
}

void RecognitionTestModel::fadeInComplete() { playTarget(); }

void RecognitionTestModel::playTarget() { targetPlayer->play(); }

void RecognitionTestModel::playbackComplete() {
    maskerPlayer->fadeOut();
}

void RecognitionTestModel::fadeOutComplete() {
    targetPlayer->hideVideo();
    listener_->trialComplete();
}

static std::string targetName(
    ResponseEvaluator *evaluator, TestMethod *testMethod) {
    return evaluator->fileName(testMethod->current());
}

static void save(OutputFile *file) { file->save(); }

void RecognitionTestModel::submitResponse(
    const coordinate_response_measure::Response &response) {
    testMethod->submitResponse(response);
    testMethod->writeLastCoordinateResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModel::submitCorrectResponse() {
    submitCorrectResponse_();
}

void RecognitionTestModel::submitCorrectResponse_() {
    testMethod->submitCorrectResponse();
    testMethod->writeLastCorrectResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModel::prepareNextTrialIfNeeded() {
    if (!testMethod->complete()) {
        ++trialNumber_;
        preparePlayersForNextTrial();
    }
}

void RecognitionTestModel::submitIncorrectResponse() {
    submitIncorrectResponse_();
}

void RecognitionTestModel::submitIncorrectResponse_() {
    testMethod->submitIncorrectResponse();
    testMethod->writeLastIncorrectResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModel::submitResponse(
    const FreeResponse &response) {
    writeTrial(response);
    testMethod->submitResponse(response);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModel::writeTrial(const FreeResponse &p) {
    FreeResponseTrial trial;
    trial.response = p.response;
    trial.target = targetName(evaluator, testMethod);
    outputFile->writeTrial(trial);
    save(outputFile);
}

void RecognitionTestModel::playCalibration(const Calibration &p) {
    throwIfTrialInProgress();

    playCalibration_(p);
}

void RecognitionTestModel::playCalibration_(const Calibration &p) {
    setTargetPlayerDevice(p);
    loadTargetFile(p.filePath);
    trySettingTargetLevel(p);
    prepareVideo(p.condition);
    playTarget();
}

void RecognitionTestModel::setTargetPlayerDevice(
    const Calibration &p) {
    throwInvalidAudioDeviceOnErrorSettingDevice(
        &RecognitionTestModel::setTargetPlayerDevice_,
        p.audioSettings.audioDevice);
}

void RecognitionTestModel::trySettingTargetLevel(
    const Calibration &p) {
    try {
        setTargetLevel_dB(calibrationLevel_dB(p));
    } catch (const InvalidAudioFile &) {
        throw Model::RequestFailure{"unable to read " + p.filePath};
    }
}

double RecognitionTestModel::calibrationLevel_dB(
    const Calibration &p) {
    return p.level_dB_SPL - p.fullScaleLevel_dB_SPL - unalteredTargetLevel_dB();
}

double RecognitionTestModel::unalteredTargetLevel_dB() {
    return dB(targetPlayer->rms());
}

bool RecognitionTestModel::testComplete() {
    return testMethod->complete();
}

std::vector<std::string> RecognitionTestModel::audioDevices() {
    return maskerPlayer->outputAudioDeviceDescriptions();
}

int RecognitionTestModel::trialNumber() { return trialNumber_; }
}
