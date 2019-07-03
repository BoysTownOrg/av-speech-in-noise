#include "RecognitionTestModel.hpp"
#include <gsl/gsl>
#include <cmath>

namespace av_speech_in_noise {
    class NullTrack : public Track {
        void pushDown() override {}
        void pushUp() override {}
        int x() override { return {}; }
        bool complete() override { return {}; }
        int reversals() override { return {}; }
    };
    
    class NullTargetList : public TargetList {
        void loadFromDirectory(std::string) override {}
        std::string next() override { return {}; }
        std::string current() override { return {}; }
    };
    
    static NullTrack nullTrack;
    static NullTargetList nullTargetList;
    
    RecognitionTestModel::RecognitionTestModel(
        TargetListReader *targetListSetReader,
        FiniteTargetList *finiteTargetList,
        TargetPlayer *targetPlayer,
        MaskerPlayer *maskerPlayer,
        TrackFactory *snrTrackFactory,
        ResponseEvaluator *evaluator,
        OutputFile *outputFile,
        Randomizer *randomizer
    ) :
        targetListSetReader{targetListSetReader},
        finiteTargetList{finiteTargetList},
        maskerPlayer{maskerPlayer},
        targetPlayer{targetPlayer},
        snrTrackFactory{snrTrackFactory},
        evaluator{evaluator},
        outputFile{outputFile},
        randomizer{randomizer},
        currentSnrTrack{&nullTrack},
        currentTargetList{&nullTargetList}
    {
        targetPlayer->subscribe(this);
        maskerPlayer->subscribe(this);
    }
    
    void RecognitionTestModel::subscribe(Model::EventListener *listener) {
        listener_ = listener;
    }
    
    void RecognitionTestModel::initializeTest(const FixedLevelTest &p) {
        throwIfTrialInProgress();
        
        currentTargetList = finiteTargetList;
        
        auto common = p.common;
        currentTargetList->loadFromDirectory(common.targetListDirectory);
        tryOpeningOutputFile(p.information);
        outputFile->writeTest(p);
        prepareCommonTest(common);
        snr_dB = p.snr_dB;
        preparePlayersForNextTrial();
        fixedLevelTest = true;
    }
    
    void RecognitionTestModel::prepareCommonTest(const CommonTest &common) {
        storeLevels(common);
        prepareMasker(common.maskerFilePath);
        prepareVideo(common.condition);
    }
    
    void RecognitionTestModel::storeLevels(const CommonTest &common) {
        fullScaleLevel_dB_SPL = common.fullScaleLevel_dB_SPL;
        maskerLevel_dB_SPL = common.maskerLevel_dB_SPL;
    }
    
    void RecognitionTestModel::initializeTest(const AdaptiveTest &p) {
        throwIfTrialInProgress();
        
        auto common = p.common;
        
        readTargetLists(p);
        prepareSnrTracks(p);
        tryOpeningOutputFile(p.information);
        outputFile->writeTest(p);
        prepareCommonTest(common);
        prepareNextAdaptiveTrial();
        fixedLevelTest = false;
    }
    
    void RecognitionTestModel::throwIfTrialInProgress() {
        if (trialInProgress())
            throw RequestFailure{"Trial in progress."};
    }
    
    bool RecognitionTestModel::trialInProgress() {
        return maskerPlayer->playing();
    }
    
    void RecognitionTestModel::readTargetLists(const AdaptiveTest &p) {
        lists = targetListSetReader->read(p.common.targetListDirectory);
    }
    
    void RecognitionTestModel::prepareSnrTracks(const AdaptiveTest &p) {
        targetListsWithTracks.clear();
        for (auto list : lists)
            makeTrackWithList(list.get(), p);
    }
    
    void RecognitionTestModel::makeTrackWithList(
        TargetList *list,
        const AdaptiveTest &p
    ) {
        Track::Settings s;
        s.rule = p.targetLevelRule;
        s.startingX = p.startingSnr_dB;
        s.ceiling = p.ceilingSnr_dB;
        targetListsWithTracks.push_back({
            list,
            snrTrackFactory->make(s)
        });
    }
    
    void RecognitionTestModel::tryOpeningOutputFile(const TestInformation &p) {
        outputFile->close();
        tryOpeningOutputFile_(p);
    }
    
    void RecognitionTestModel::tryOpeningOutputFile_(const TestInformation &p) {
        try {
            outputFile->openNewFile(p);
        } catch (const OutputFile::OpenFailure &) {
            throw RequestFailure{"Unable to open output file."};
        }
    }
    
    void RecognitionTestModel::prepareMasker(const std::string &p) {
        loadMaskerFile(p);
        maskerPlayer->setLevel_dB(maskerLevel_dB());
    }
    
    void RecognitionTestModel::loadMaskerFile(const std::string &p) {
        maskerPlayer->loadFile(p);
    }
    
    static double dB(double x) {
        return 20 * std::log10(x);
    }
    
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
    
    void RecognitionTestModel::prepareNextAdaptiveTrial() {
        selectNextList();
        snr_dB = adaptiveSnr_dB();
        preparePlayersForNextTrial();
    }
    
    void RecognitionTestModel::preparePlayersForNextTrial() {
        prepareTargetPlayer();
        seekRandomMaskerPosition();
    }
    
    void RecognitionTestModel::selectNextList() {
        auto remainingListCount = gsl::narrow<int>(targetListsWithTracks.size());
        size_t n = randomizer->randomIntBetween(0, remainingListCount - 1);
        if (n < targetListsWithTracks.size()) {
            currentSnrTrack = targetListsWithTracks.at(n).track.get();
            currentTargetList = targetListsWithTracks.at(n).list;
        }
    }
    
    void RecognitionTestModel::prepareTargetPlayer() {
        loadTargetFile(currentTargetList->next());
        setTargetLevel_dB(targetLevel_dB());
        targetPlayer->subscribeToPlaybackCompletion();
    }
    
    void RecognitionTestModel::loadTargetFile(std::string s) {
        targetPlayer->loadFile(std::move(s));;
    }
    
    void RecognitionTestModel::setTargetLevel_dB(double x) {
        targetPlayer->setLevel_dB(x);
    }
    
    double RecognitionTestModel::targetLevel_dB() {
        return
            desiredMaskerLevel_dB() +
            snr_dB -
            unalteredTargetLevel_dB();
    }
    
    int RecognitionTestModel::adaptiveSnr_dB() {
        return currentSnrTrack->x();
    }
    
    double RecognitionTestModel::unalteredTargetLevel_dB() {
        return dB(targetPlayer->rms());
    }
    
    void RecognitionTestModel::seekRandomMaskerPosition() {
        auto upperLimit =
            maskerPlayer->durationSeconds() -
            2 * maskerPlayer->fadeTimeSeconds() -
            targetPlayer->durationSeconds();
        maskerPlayer->seekSeconds(randomizer->randomFloatBetween(0, upperLimit));
    }
    
    void RecognitionTestModel::playTrial(const AudioSettings &settings) {
        throwIfTrialInProgress();
        
        preparePlayersToPlay(settings);
        startTrial();
    }
    
    void RecognitionTestModel::preparePlayersToPlay(const AudioSettings &p) {
        setAudioDevices(p);
    }
    
    void RecognitionTestModel::setAudioDevices(const AudioSettings &p) {
        throwInvalidAudioDeviceOnErrorSettingDevice(
            &RecognitionTestModel::setAudioDevices_,
            p.audioDevice
        );
    }
    
    void RecognitionTestModel::throwInvalidAudioDeviceOnErrorSettingDevice(
        void (RecognitionTestModel::*f)(const std::string &),
        const std::string &device
    ) {
        try {
            (this->*f)(device);
        } catch (const InvalidAudioDevice &) {
            throw RequestFailure{
                "'" + device + "' is not a valid audio device."
            };
        }
    }
    
    void RecognitionTestModel::setAudioDevices_(const std::string &device) {
        maskerPlayer->setAudioDevice(device);
        setTargetPlayerDevice_(device);
    }
    
    void RecognitionTestModel::setTargetPlayerDevice_(const std::string &device) {
        targetPlayer->setAudioDevice(device);
    }
    
    void RecognitionTestModel::startTrial() {
        maskerPlayer->fadeIn();
    }
    
    void RecognitionTestModel::fadeInComplete() {
        playTarget();
    }
    
    void RecognitionTestModel::playTarget() {
        targetPlayer->play();
    }
    
    void RecognitionTestModel::playbackComplete() {
        maskerPlayer->fadeOut();
    }
    
    void RecognitionTestModel::fadeOutComplete() {
        listener_->trialComplete();
    }
    
    void RecognitionTestModel::submitResponse(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        writeTrial(response);
        updateSnr(response);
        prepareNextAdaptiveTrialAfterRemovingCompleteTracks();
    }
    
    void RecognitionTestModel::writeTrial(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        if (!justWroteCoordinateResponseTrial)
            outputFile->writeCoordinateResponseTrialHeading();
        coordinate_response_measure::Trial trial;
        trial.subjectColor = response.color;
        trial.subjectNumber = response.number;
        trial.reversals = currentSnrTrack->reversals();
        trial.correctColor = evaluator->correctColor(currentTarget());
        trial.correctNumber = evaluator->correctNumber(currentTarget());
        trial.SNR_dB = adaptiveSnr_dB();
        trial.correct = correct(response);
        outputFile->writeTrial(trial);
        outputFile->save();
        justWroteCoordinateResponseTrial = true;
        justWroteFreeResponseTrial = false;
    }
    
    void RecognitionTestModel::updateSnr(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        if (correct(response))
            currentSnrTrack->pushDown();
        else
            currentSnrTrack->pushUp();
    }
    
    bool RecognitionTestModel::correct(
        const coordinate_response_measure::SubjectResponse &response
    ) {
        return evaluator->correct(currentTarget(), response);
    }
    
    std::string RecognitionTestModel::currentTarget() {
        return currentTargetList->current();
    }
    
    void RecognitionTestModel::prepareNextAdaptiveTrialAfterRemovingCompleteTracks() {
        removeCompleteTracks();
        prepareNextAdaptiveTrial();
    }
    
    void RecognitionTestModel::removeCompleteTracks() {
        targetListsWithTracks.erase(
            std::remove_if(
                targetListsWithTracks.begin(),
                targetListsWithTracks.end(),
                [](const TargetListWithTrack &t) {
                    return t.track->complete();
                }
            ),
            targetListsWithTracks.end()
        );
    }
    
    void RecognitionTestModel::submitCorrectResponse() {
        currentSnrTrack->pushDown();
        prepareNextAdaptiveTrialAfterRemovingCompleteTracks();
    }
    
    void RecognitionTestModel::submitIncorrectResponse() {
        currentSnrTrack->pushUp();
        prepareNextAdaptiveTrialAfterRemovingCompleteTracks();
    }
    
    void RecognitionTestModel::submitResponse(const FreeResponse &p) {
        if (!justWroteFreeResponseTrial)
            outputFile->writeFreeResponseTrialHeading();
        FreeResponseTrial trial;
        trial.response = p.response;
        trial.target = evaluator->fileName(finiteTargetList->current());
        outputFile->writeTrial(trial);
        outputFile->save();
        justWroteFreeResponseTrial = true;
        justWroteCoordinateResponseTrial = false;
        
        prepareTargetPlayer();
        seekRandomMaskerPosition();
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
    
    void RecognitionTestModel::setTargetPlayerDevice(const Calibration &p) {
        throwInvalidAudioDeviceOnErrorSettingDevice(
            &RecognitionTestModel::setTargetPlayerDevice_,
            p.audioSettings.audioDevice
        );
    }
    
    void RecognitionTestModel::trySettingTargetLevel(const Calibration &p) {
        try {
            setTargetLevel_dB(calibrationLevel_dB(p));
        } catch (const InvalidAudioFile &) {
            throw RequestFailure{"unable to read " + p.filePath};
        }
    }
    
    double RecognitionTestModel::calibrationLevel_dB(const Calibration &p) {
        return
            p.level_dB_SPL -
            p.fullScaleLevel_dB_SPL -
            unalteredTargetLevel_dB();
    }

    bool RecognitionTestModel::testComplete() {
        return fixedLevelTest ?
            finiteTargetList->empty() :
            std::all_of(
                targetListsWithTracks.begin(),
                targetListsWithTracks.end(),
                [](const TargetListWithTrack &t) {
                    return t.track->complete();
                }
            );
    }
    
    std::vector<std::string> RecognitionTestModel::audioDevices() {
        return maskerPlayer->outputAudioDeviceDescriptions();
    }
}

