#include "RecognitionTestModel_.hpp"
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

RecognitionTestModel_Impl::RecognitionTestModel_Impl(
    TargetPlayer *targetPlayer, MaskerPlayer *maskerPlayer,
    ResponseEvaluator *evaluator, OutputFile *outputFile,
    Randomizer *randomizer)
    : maskerPlayer{maskerPlayer},
      targetPlayer{targetPlayer}, evaluator{evaluator}, outputFile{outputFile},
      randomizer{randomizer}, testMethod{&nullTestMethod} {
    targetPlayer->subscribe(this);
    maskerPlayer->subscribe(this);
}

void RecognitionTestModel_Impl::subscribe(Model::EventListener *listener) {
    listener_ = listener;
}

void RecognitionTestModel_Impl::throwIfTrialInProgress() {
    if (trialInProgress())
        throw Model::RequestFailure{"Trial in progress."};
}

void RecognitionTestModel_Impl::initialize(TestMethod *testMethod_,
    const Test &common, const TestIdentity &information) {
    throwIfTrialInProgress();
    testMethod = testMethod_;
    prepareCommonTest(common, information);
    trialNumber_ = 1;
}

bool RecognitionTestModel_Impl::trialInProgress() {
    return maskerPlayer->playing();
}

void RecognitionTestModel_Impl::prepareCommonTest(
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

void RecognitionTestModel_Impl::storeLevels(const Test &common) {
    fullScaleLevel_dB_SPL = common.fullScaleLevel_dB_SPL;
    maskerLevel_dB_SPL = common.maskerLevel_dB_SPL;
}

void RecognitionTestModel_Impl::prepareMasker(const std::string &p) {
    throwInvalidAudioFileOnErrorLoading(
        &RecognitionTestModel_Impl::loadMaskerFile, p);
    maskerPlayer->setLevel_dB(maskerLevel_dB());
}

void RecognitionTestModel_Impl::throwInvalidAudioFileOnErrorLoading(
    void (RecognitionTestModel_Impl::*f)(const std::string &),
    const std::string &file) {
    try {
        (this->*f)(file);
    } catch (const InvalidAudioFile &) {
        throw Model::RequestFailure{"unable to read " + file};
    }
}

void RecognitionTestModel_Impl::loadMaskerFile(const std::string &p) {
    maskerPlayer->loadFile(p);
}

static double dB(double x) { return 20 * std::log10(x); }

double RecognitionTestModel_Impl::maskerLevel_dB() {
    return desiredMaskerLevel_dB() - dB(maskerPlayer->rms());
}

int RecognitionTestModel_Impl::desiredMaskerLevel_dB() {
    return maskerLevel_dB_SPL - fullScaleLevel_dB_SPL;
}

void RecognitionTestModel_Impl::prepareVideo(const Condition &p) {
    if (auditoryOnly(p))
        targetPlayer->hideVideo();
    else
        targetPlayer->showVideo();
}

bool RecognitionTestModel_Impl::auditoryOnly(const Condition &c) {
    return c == Condition::auditoryOnly;
}

void RecognitionTestModel_Impl::preparePlayersForNextTrial() {
    prepareTargetPlayer();
    seekRandomMaskerPosition();
}

void RecognitionTestModel_Impl::prepareTargetPlayer() {
    loadTargetFile(testMethod->next());
    setTargetLevel_dB(targetLevel_dB());
    targetPlayer->subscribeToPlaybackCompletion();
}

void RecognitionTestModel_Impl::loadTargetFile(std::string s) {
    targetPlayer->loadFile(std::move(s));
}

void RecognitionTestModel_Impl::setTargetLevel_dB(double x) {
    targetPlayer->setLevel_dB(x);
}

double RecognitionTestModel_Impl::targetLevel_dB() {
    return maskerLevel_dB() + testMethod->snr_dB();
}

void RecognitionTestModel_Impl::seekRandomMaskerPosition() {
    auto upperLimit = maskerPlayer->durationSeconds() -
        2 * maskerPlayer->fadeTimeSeconds() - targetPlayer->durationSeconds();
    maskerPlayer->seekSeconds(randomizer->randomFloatBetween(0, upperLimit));
}

void RecognitionTestModel_Impl::tryOpeningOutputFile(
    const TestIdentity &p) {
    outputFile->close();
    tryOpeningOutputFile_(p);
}

void RecognitionTestModel_Impl::tryOpeningOutputFile_(
    const TestIdentity &p) {
    try {
        outputFile->openNewFile(p);
    } catch (const OutputFile::OpenFailure &) {
        throw Model::RequestFailure{"Unable to open output file."};
    }
}

void RecognitionTestModel_Impl::playTrial(const AudioSettings &settings) {
    throwIfTrialInProgress();

    preparePlayersToPlay(settings);
    startTrial();
}

void RecognitionTestModel_Impl::preparePlayersToPlay(
    const AudioSettings &p) {
    setAudioDevices(p);
}

void RecognitionTestModel_Impl::setAudioDevices(const AudioSettings &p) {
    throwInvalidAudioDeviceOnErrorSettingDevice(
        &RecognitionTestModel_Impl::setAudioDevices_, p.audioDevice);
}

void RecognitionTestModel_Impl::throwInvalidAudioDeviceOnErrorSettingDevice(
    void (RecognitionTestModel_Impl::*f)(const std::string &),
    const std::string &device) {
    try {
        (this->*f)(device);
    } catch (const InvalidAudioDevice &) {
        throw Model::RequestFailure{
            "'" + device + "' is not a valid audio device."};
    }
}

void RecognitionTestModel_Impl::setAudioDevices_(
    const std::string &device) {
    maskerPlayer->setAudioDevice(device);
    setTargetPlayerDevice_(device);
}

void RecognitionTestModel_Impl::setTargetPlayerDevice_(
    const std::string &device) {
    targetPlayer->setAudioDevice(device);
}

void RecognitionTestModel_Impl::startTrial() {
    if (!auditoryOnly(condition))
        targetPlayer->showVideo();
    maskerPlayer->fadeIn();
}

void RecognitionTestModel_Impl::fadeInComplete() { playTarget(); }

void RecognitionTestModel_Impl::playTarget() { targetPlayer->play(); }

void RecognitionTestModel_Impl::playbackComplete() {
    maskerPlayer->fadeOut();
}

void RecognitionTestModel_Impl::fadeOutComplete() {
    targetPlayer->hideVideo();
    listener_->trialComplete();
}

static std::string targetName(
    ResponseEvaluator *evaluator, TestMethod *testMethod) {
    return evaluator->fileName(testMethod->current());
}

static void save(OutputFile *file) { file->save(); }

void RecognitionTestModel_Impl::submitResponse(
    const coordinate_response_measure::Response &response) {
    testMethod->submitResponse(response);
    testMethod->writeLastCoordinateResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModel_Impl::submitCorrectResponse() {
    submitCorrectResponse_();
}

void RecognitionTestModel_Impl::submitCorrectResponse_() {
    testMethod->submitCorrectResponse();
    testMethod->writeLastCorrectResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModel_Impl::prepareNextTrialIfNeeded() {
    if (!testMethod->complete()) {
        ++trialNumber_;
        preparePlayersForNextTrial();
    }
}

void RecognitionTestModel_Impl::submitIncorrectResponse() {
    submitIncorrectResponse_();
}

void RecognitionTestModel_Impl::submitIncorrectResponse_() {
    testMethod->submitIncorrectResponse();
    testMethod->writeLastIncorrectResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModel_Impl::submitResponse(
    const FreeResponse &response) {
    writeTrial(response);
    testMethod->submitResponse(response);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModel_Impl::writeTrial(const FreeResponse &p) {
    FreeResponseTrial trial;
    trial.response = p.response;
    trial.target = targetName(evaluator, testMethod);
    outputFile->writeTrial(trial);
    save(outputFile);
}

void RecognitionTestModel_Impl::playCalibration(const Calibration &p) {
    throwIfTrialInProgress();

    playCalibration_(p);
}

void RecognitionTestModel_Impl::playCalibration_(const Calibration &p) {
    setTargetPlayerDevice(p);
    loadTargetFile(p.filePath);
    trySettingTargetLevel(p);
    prepareVideo(p.condition);
    playTarget();
}

void RecognitionTestModel_Impl::setTargetPlayerDevice(
    const Calibration &p) {
    throwInvalidAudioDeviceOnErrorSettingDevice(
        &RecognitionTestModel_Impl::setTargetPlayerDevice_,
        p.audioSettings.audioDevice);
}

void RecognitionTestModel_Impl::trySettingTargetLevel(
    const Calibration &p) {
    try {
        setTargetLevel_dB(calibrationLevel_dB(p));
    } catch (const InvalidAudioFile &) {
        throw Model::RequestFailure{"unable to read " + p.filePath};
    }
}

double RecognitionTestModel_Impl::calibrationLevel_dB(
    const Calibration &p) {
    return p.level_dB_SPL - p.fullScaleLevel_dB_SPL - unalteredTargetLevel_dB();
}

double RecognitionTestModel_Impl::unalteredTargetLevel_dB() {
    return dB(targetPlayer->rms());
}

bool RecognitionTestModel_Impl::testComplete() {
    return testMethod->complete();
}

std::vector<std::string> RecognitionTestModel_Impl::audioDevices() {
    return maskerPlayer->outputAudioDeviceDescriptions();
}

int RecognitionTestModel_Impl::trialNumber() { return trialNumber_; }
}
