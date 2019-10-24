#include "RecognitionTestModel_Internal.hpp"
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
        const coordinate_response_measure::SubjectResponse &
    ) override {}
    void submitResponse(const FreeResponse &) override {}
};
}
static NullTestMethod nullTestMethod;

RecognitionTestModel_Internal::RecognitionTestModel_Internal(
    TargetPlayer *targetPlayer,
    MaskerPlayer *maskerPlayer,
    ResponseEvaluator *evaluator,
    OutputFile *outputFile,
    Randomizer *randomizer
) :
    maskerPlayer{maskerPlayer},
    targetPlayer{targetPlayer},
    evaluator{evaluator},
    outputFile{outputFile},
    randomizer{randomizer},
    testMethod{&nullTestMethod}
{
    targetPlayer->subscribe(this);
    maskerPlayer->subscribe(this);
}

void RecognitionTestModel_Internal::subscribe(Model::EventListener *listener) {
    listener_ = listener;
}

void RecognitionTestModel_Internal::throwIfTrialInProgress() {
    if (trialInProgress())
        throw Model::RequestFailure{"Trial in progress."};
}

void RecognitionTestModel_Internal::initialize(
    TestMethod *testMethod_,
    const CommonTest &common,
    const TestInformation &information
) {
    throwIfTrialInProgress();
    testMethod = testMethod_;
    prepareCommonTest(common, information);
}

bool RecognitionTestModel_Internal::trialInProgress() {
    return maskerPlayer->playing();
}

void RecognitionTestModel_Internal::prepareCommonTest(
    const CommonTest &common,
    const TestInformation &information
) {
    storeLevels(common);
    prepareMasker(common.maskerFilePath);
    targetPlayer->hideVideo();
    condition = common.condition;
    if (!testMethod->complete())
        preparePlayersForNextTrial();
    tryOpeningOutputFile(information);
    testMethod->writeTestingParameters(outputFile);
}

void RecognitionTestModel_Internal::storeLevels(const CommonTest &common) {
    fullScaleLevel_dB_SPL = common.fullScaleLevel_dB_SPL;
    maskerLevel_dB_SPL = common.maskerLevel_dB_SPL;
}

void RecognitionTestModel_Internal::prepareMasker(const std::string &p) {
    throwInvalidAudioFileOnErrorLoading(
        &RecognitionTestModel_Internal::loadMaskerFile,
        p
    );
    maskerPlayer->setLevel_dB(maskerLevel_dB());
}

void RecognitionTestModel_Internal::throwInvalidAudioFileOnErrorLoading(
    void (RecognitionTestModel_Internal::*f)(const std::string &),
    const std::string &file
) {
    try {
        (this->*f)(file);
    } catch (const InvalidAudioFile &) {
        throw Model::RequestFailure{"unable to read " + file};
    }
}

void RecognitionTestModel_Internal::loadMaskerFile(const std::string &p) {
    maskerPlayer->loadFile(p);
}

static double dB(double x) {
    return 20 * std::log10(x);
}

double RecognitionTestModel_Internal::maskerLevel_dB() {
    return desiredMaskerLevel_dB() - dB(maskerPlayer->rms());
}

int RecognitionTestModel_Internal::desiredMaskerLevel_dB() {
    return maskerLevel_dB_SPL - fullScaleLevel_dB_SPL;
}

void RecognitionTestModel_Internal::prepareVideo(const Condition &p) {
    if (auditoryOnly(p))
        targetPlayer->hideVideo();
    else
        targetPlayer->showVideo();
}

bool RecognitionTestModel_Internal::auditoryOnly(const Condition &c) {
    return c == Condition::auditoryOnly;
}

void RecognitionTestModel_Internal::preparePlayersForNextTrial() {
    prepareTargetPlayer();
    seekRandomMaskerPosition();
}

void RecognitionTestModel_Internal::prepareTargetPlayer() {
    loadTargetFile(testMethod->next());
    setTargetLevel_dB(targetLevel_dB());
    targetPlayer->subscribeToPlaybackCompletion();
}

void RecognitionTestModel_Internal::loadTargetFile(std::string s) {
    targetPlayer->loadFile(std::move(s));
}

void RecognitionTestModel_Internal::setTargetLevel_dB(double x) {
    targetPlayer->setLevel_dB(x);
}

double RecognitionTestModel_Internal::targetLevel_dB() {
    return maskerLevel_dB() + testMethod->snr_dB();
}

void RecognitionTestModel_Internal::seekRandomMaskerPosition() {
    auto upperLimit =
        maskerPlayer->durationSeconds() -
        2 * maskerPlayer->fadeTimeSeconds() -
        targetPlayer->durationSeconds();
    maskerPlayer->seekSeconds(randomizer->randomFloatBetween(0, upperLimit));
}

void RecognitionTestModel_Internal::tryOpeningOutputFile(
    const TestInformation &p
) {
    outputFile->close();
    tryOpeningOutputFile_(p);
}

void RecognitionTestModel_Internal::tryOpeningOutputFile_(
    const TestInformation &p
) {
    try {
        outputFile->openNewFile(p);
    } catch (const OutputFile::OpenFailure &) {
        throw Model::RequestFailure{"Unable to open output file."};
    }
}

void RecognitionTestModel_Internal::playTrial(const AudioSettings &settings) {
    throwIfTrialInProgress();

    preparePlayersToPlay(settings);
    startTrial();
}

void RecognitionTestModel_Internal::preparePlayersToPlay(
    const AudioSettings &p
) {
    setAudioDevices(p);
}

void RecognitionTestModel_Internal::setAudioDevices(const AudioSettings &p) {
    throwInvalidAudioDeviceOnErrorSettingDevice(
        &RecognitionTestModel_Internal::setAudioDevices_,
        p.audioDevice
    );
}

void RecognitionTestModel_Internal::throwInvalidAudioDeviceOnErrorSettingDevice(
    void (RecognitionTestModel_Internal::*f)(const std::string &),
    const std::string &device
) {
    try {
        (this->*f)(device);
    } catch (const InvalidAudioDevice &) {
        throw Model::RequestFailure{
            "'" + device + "' is not a valid audio device."
        };
    }
}

void RecognitionTestModel_Internal::setAudioDevices_(
    const std::string &device
) {
    maskerPlayer->setAudioDevice(device);
    setTargetPlayerDevice_(device);
}

void RecognitionTestModel_Internal::setTargetPlayerDevice_(
    const std::string &device
) {
    targetPlayer->setAudioDevice(device);
}

void RecognitionTestModel_Internal::startTrial() {
    if (!auditoryOnly(condition))
        targetPlayer->showVideo();
    maskerPlayer->fadeIn();
}

void RecognitionTestModel_Internal::fadeInComplete() {
    playTarget();
}

void RecognitionTestModel_Internal::playTarget() {
    targetPlayer->play();
}

void RecognitionTestModel_Internal::playbackComplete() {
    maskerPlayer->fadeOut();
}

void RecognitionTestModel_Internal::fadeOutComplete() {
    targetPlayer->hideVideo();
    listener_->trialComplete();
}

static std::string targetName(ResponseEvaluator *evaluator, TestMethod *testMethod) {
    return evaluator->fileName(testMethod->current());
}

static void save(OutputFile *file) {
    file->save();
}

void RecognitionTestModel_Internal::submitResponse(
    const coordinate_response_measure::SubjectResponse &response
) {
    testMethod->submitResponse(response);
    testMethod->writeLastCoordinateResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModel_Internal::submitCorrectResponse() {
    submitCorrectResponse_();
}

void RecognitionTestModel_Internal::submitCorrectResponse_() {
    open_set::AdaptiveTrial trial;
    trial.target = targetName(evaluator, testMethod);
    outputFile->writeTrial(trial);
    testMethod->submitCorrectResponse();
    testMethod->writeLastCorrectResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModel_Internal::prepareNextTrialIfNeeded() {
    if (!testMethod->complete())
        preparePlayersForNextTrial();
}

void RecognitionTestModel_Internal::submitIncorrectResponse() {
    submitIncorrectResponse_();
}

void RecognitionTestModel_Internal::submitIncorrectResponse_() {
    testMethod->submitIncorrectResponse();
    testMethod->writeLastIncorrectResponse(outputFile);
    save(outputFile);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModel_Internal::submitResponse(
    const FreeResponse &response
) {
    writeTrial(response);
    testMethod->submitResponse(response);
    prepareNextTrialIfNeeded();
}

void RecognitionTestModel_Internal::writeTrial(const FreeResponse &p) {
    FreeResponseTrial trial;
    trial.response = p.response;
    trial.target = targetName(evaluator, testMethod);
    outputFile->writeTrial(trial);
    save(outputFile);
}

void RecognitionTestModel_Internal::playCalibration(const Calibration &p) {
    throwIfTrialInProgress();

    playCalibration_(p);
}

void RecognitionTestModel_Internal::playCalibration_(const Calibration &p) {
    setTargetPlayerDevice(p);
    loadTargetFile(p.filePath);
    trySettingTargetLevel(p);
    prepareVideo(p.condition);
    playTarget();
}

void RecognitionTestModel_Internal::setTargetPlayerDevice(
    const Calibration &p
) {
    throwInvalidAudioDeviceOnErrorSettingDevice(
        &RecognitionTestModel_Internal::setTargetPlayerDevice_,
        p.audioSettings.audioDevice
    );
}

void RecognitionTestModel_Internal::trySettingTargetLevel(
    const Calibration &p
) {
    try {
        setTargetLevel_dB(calibrationLevel_dB(p));
    } catch (const InvalidAudioFile &) {
        throw Model::RequestFailure{"unable to read " + p.filePath};
    }
}

double RecognitionTestModel_Internal::calibrationLevel_dB(
    const Calibration &p
) {
    return
        p.level_dB_SPL -
        p.fullScaleLevel_dB_SPL -
        unalteredTargetLevel_dB();
}

double RecognitionTestModel_Internal::unalteredTargetLevel_dB() {
    return dB(targetPlayer->rms());
}

bool RecognitionTestModel_Internal::testComplete() {
    return testMethod->complete();
}

std::vector<std::string> RecognitionTestModel_Internal::audioDevices() {
    return maskerPlayer->outputAudioDeviceDescriptions();
}
}
