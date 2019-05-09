#include "TargetListStub.h"
#include "LogString.h"
#include "assert-utility.h"
#include <recognition-test/RecognitionTestModel.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <cmath>

namespace {
    using namespace av_speech_in_noise;
    
    class MaskerPlayerStub : public MaskerPlayer {
        std::vector<std::string> outputAudioDeviceDescriptions_;
        LogString log_;
        std::string filePath_;
        std::string device_;
        double rms_{};
        double level_dB_{};
        double fadeTimeSeconds_{};
        double durationSeconds_{};
        double secondsSeeked_{};
        int deviceIndex_{};
        EventListener *listener_{};
        bool fadeInCalled_{};
        bool fadeOutCalled_{};
        bool playing_{};
        bool setDeviceCalled_{};
        bool throwInvalidAudioDeviceWhenDeviceSet_{};
    public:
        void fadeOutComplete() {
            listener_->fadeOutComplete();
        }
        
        void throwInvalidAudioDeviceWhenDeviceSet() {
            throwInvalidAudioDeviceWhenDeviceSet_ = true;
        }
        
        void setAudioDevice(std::string s) override {
            device_ = std::move(s);
            if (throwInvalidAudioDeviceWhenDeviceSet_)
                throw InvalidAudioDevice{};
        }
        
        auto device() const {
            return device_;
        }
        
        auto setDeviceCalled() const {
            return setDeviceCalled_;
        }
        
        bool playing() override {
            return playing_;
        }
        
        void setPlaying() {
            playing_ = true;
        }
        
        void setOutputAudioDeviceDescriptions(std::vector<std::string> v) {
            outputAudioDeviceDescriptions_ = std::move(v);
        }
        
        auto fadeInCalled() const {
            return fadeInCalled_;
        }
        
        void fadeIn() override {
            fadeInCalled_ = true;
        }
        
        auto fadeOutCalled() const {
            return fadeOutCalled_;
        }
        
        void subscribe(EventListener *e) override {
            listener_ = e;
        }
        
        void fadeOut() override {
            fadeOutCalled_ = true;
        }
        
        void loadFile(std::string filePath) override {
            addToLog("loadFile ");
            filePath_ = filePath;
        }
        
        void addToLog(std::string s) {
            log_.insert(std::move(s));
        }
        
        std::vector<std::string> outputAudioDeviceDescriptions() override {
            return outputAudioDeviceDescriptions_;
        }
        
        auto listener() const {
            return listener_;
        }
        
        void fadeInComplete() {
            listener_->fadeInComplete();
        }
        
        auto filePath() const {
            return filePath_;
        }
        
        void setRms(double x) {
            rms_ = x;
        }
        
        auto level_dB() const {
            return level_dB_;
        }
        
        double rms() override {
            addToLog("rms ");
            return rms_;
        }
        
        void setLevel_dB(double x) override {
            level_dB_ = x;
        }
        
        double durationSeconds() override {
            return durationSeconds_;
        }
        
        void seekSeconds(double x) override {
            secondsSeeked_ = x;
        }
        
        auto secondsSeeked() const {
            return secondsSeeked_;
        }
        
        double fadeTimeSeconds() override {
            return fadeTimeSeconds_;
        }
        
        void setFadeTimeSeconds(double x) {
            fadeTimeSeconds_ = x;
        }
        
        void setDurationSeconds(double x) {
            durationSeconds_ = x;
        }
    };

    class TargetPlayerStub : public TargetPlayer {
        LogString log_;
        std::string filePath_;
        std::string device_;
        double rms_{};
        double level_dB_{};
        double durationSeconds_{};
        int deviceIndex_{};
        EventListener *listener_{};
        bool played_{};
        bool videoHidden_{};
        bool videoShown_{};
        bool throwInvalidAudioDeviceWhenDeviceSet_{};
        bool throwInvalidAudioFileOnRms_{};
        bool setDeviceCalled_{};
        bool playing_{};
        bool playbackCompletionSubscribedTo_{};
    public:
        void subscribeToPlaybackCompletion() override {
            playbackCompletionSubscribedTo_ = true;
        }
        
        bool playing() override {
            return playing_;
        }
        
        auto &log() const {
            return log_;
        }
        
        auto setDeviceCalled() const {
            return setDeviceCalled_;
        }
        
        void throwInvalidAudioDeviceWhenDeviceSet() {
            throwInvalidAudioDeviceWhenDeviceSet_ = true;
        }
        
        void setAudioDevice(std::string s) override {
            setDeviceCalled_ = true;
            device_ = std::move(s);
            if (throwInvalidAudioDeviceWhenDeviceSet_)
                throw InvalidAudioDevice{};
        }
        
        auto device() const {
            return device_;
        }
        
        auto level_dB() const {
            return level_dB_;
        }
        
        void setRms(double x) {
            rms_ = x;
        }
        
        void showVideo() override {
            videoShown_ = true;
        }
        
        auto videoShown() const {
            return videoShown_;
        }
        
        void hideVideo() override {
            videoHidden_ = true;
        }
        
        auto videoHidden() const {
            return videoHidden_;
        }
        
        auto played() const {
            return played_;
        }
        
        void play() override {
            played_ = true;
        }
        
        void subscribe(EventListener *listener) override {
            listener_ = listener;
        }
        
        void loadFile(std::string filePath) override {
            addToLog("loadFile ");
            filePath_ = filePath;
        }
        
        void addToLog(std::string s) {
            log_.insert(std::move(s));
        }
        
        double rms() override {
            addToLog("rms ");
            if (throwInvalidAudioFileOnRms_)
                throw InvalidAudioFile{};
            return rms_;
        }
        
        void setLevel_dB(double x) override {
            level_dB_ = x;
        }
        
        double durationSeconds() override {
            return durationSeconds_;
        }
        
        void playbackComplete() {
            listener_->playbackComplete();
        }
        
        auto listener() const {
            return listener_;
        }
        
        auto filePath() const {
            return filePath_;
        }
        
        auto playbackCompletionSubscribedTo() const {
            return playbackCompletionSubscribedTo_;
        }
        
        void setDurationSeconds(double x) {
            durationSeconds_ = x;
        }
        
        void throwInvalidAudioFileOnRms() {
            throwInvalidAudioFileOnRms_ = true;
        }
    };
    
    class OutputFileStub : public OutputFile {
        coordinate_response_measure::Trial writtenCoordinateResponseTrial_;
        FreeResponseTrial writtenFreeResponseTrial_;
        LogString log_;
        const AdaptiveTest *adaptiveTest_{};
        const FixedLevelTest *fixedLevelTest_{};
        const TestInformation *openNewFileParameters_{};
        bool throwOnOpen_{};
        bool coordinateResponseHeadingWritten_{};
        bool freeResponseTrialHeadingWritten_{};
    public:
        
        void writeTrial(
            const coordinate_response_measure::Trial &trial
        ) override {
            addToLog("writeTrial ");
            writtenCoordinateResponseTrial_ = trial;
        }
        
        void openNewFile(const TestInformation &p) override {
            addToLog("openNewFile ");
            openNewFileParameters_ = &p;
            if (throwOnOpen_)
                throw OpenFailure{};
        }
        
        void close() override {
            addToLog("close ");
        }
        
        void writeCoordinateResponseTrialHeading() override {
            addToLog("writeTrialHeading ");
            coordinateResponseHeadingWritten_ = true;
        }
        
        void writeTest(const AdaptiveTest &test) override {
            addToLog("writeTest ");
            adaptiveTest_ = &test;
        }
        
        void writeTest(const FixedLevelTest &p) override {
            addToLog("writeTest ");
            fixedLevelTest_ = &p;
        }
        
        void writeTrial(const FreeResponseTrial &p) override {
            addToLog("writeTrial ");
            writtenFreeResponseTrial_ = p;
        }
        
        void writeFreeResponseTrialHeading() override {
            addToLog("writeTrialHeading ");
            freeResponseTrialHeadingWritten_ = true;
        }
        
        void addToLog(std::string s) {
            log_.insert(std::move(s));
        }
        
        void throwOnOpen() {
            throwOnOpen_ = true;
        }
        
        auto openNewFileParameters() const {
            return openNewFileParameters_;
        }
        
        auto &log() const {
            return log_;
        }
        
        auto adaptiveTest() const {
            return adaptiveTest_;
        }
        
        auto fixedLevelTest() const {
            return fixedLevelTest_;
        }
        
        auto &writtenCoordinateResponseTrial() const {
            return writtenCoordinateResponseTrial_;
        }
        
        auto &writtenFreeResponseTrial() const {
            return writtenFreeResponseTrial_;
        }
        
        auto freeResponseTrialHeadingWritten() const {
            return freeResponseTrialHeadingWritten_;
        }
        
        auto coordinateResponseTrialHeadingWritten() const {
            return coordinateResponseHeadingWritten_;
        }
    };
    
    class ModelEventListenerStub : public Model::EventListener {
        bool notified_{};
    public:
        void trialComplete() override {
            notified_ = true;
        }
        
        auto notified() const {
            return notified_;
        }
    };
    
    class TrackStub : public Track {
        Settings settings_;
        int x_{};
        int reversals_{};
        bool pushedDown_{};
        bool pushedUp_{};
        bool complete_{};
    public:
        void setReversals(int x) {
            reversals_ = x;
        }
        
        auto pushedUp() const {
            return pushedUp_;
        }
        
        auto pushedDown() const {
            return pushedDown_;
        }
        
        void setX(int x) {
            x_ = x;
        }
        
        void pushDown() override {
            pushedDown_ = true;
        }
        
        void pushUp() override {
            pushedUp_ = true;
        }
        
        int x() override {
            return x_;
        }
        
        bool complete() override {
            return complete_;
        }
        
        int reversals() override {
            return reversals_;
        }
        
        void setComplete() {
            complete_ = true;
        }
    };
    
    class ResponseEvaluatorStub : public ResponseEvaluator {
        std::string correctTarget_;
        std::string correctNumberFilePath_;
        std::string correctColorFilePath_;
        std::string fileName_;
        std::string filePathForFileName_;
        const coordinate_response_measure::SubjectResponse *response_{};
        int correctNumber_{};
        coordinate_response_measure::Color correctColor_{};
        bool correct_{};
    public:
        void setFileName(std::string s) {
            fileName_ = std::move(s);
        }
        
        void setCorrectNumber(int x) {
            correctNumber_ = x;
        }
        
        void setCorrectColor(coordinate_response_measure::Color c) {
            correctColor_ = c;
        }
        
        auto correctNumberFilePath() const {
            return correctNumberFilePath_;
        }
        
        auto correctColorFilePath() const {
            return correctColorFilePath_;
        }
        
        auto correctFilePath() const {
            return correctTarget_;
        }
        
        auto response() const {
            return response_;
        }
        
        void setCorrect() {
            correct_ = true;
        }
        
        void setIncorrect() {
            correct_ = false;
        }
        
        bool correct(
            const std::string &target,
            const coordinate_response_measure::SubjectResponse &p
        ) override {
            correctTarget_ = target;
            response_ = &p;
            return correct_;
        }
        
        coordinate_response_measure::Color correctColor(const std::string &filePath) override {
            correctColorFilePath_ = filePath;
            return correctColor_;
        }
        
        int correctNumber(const std::string &filePath) override {
            correctNumberFilePath_ = filePath;
            return correctNumber_;
        }
        
        std::string fileName(const std::string &filePath) override {
            filePathForFileName_ = filePath;
            return fileName_;
        }
        
        auto filePathForFileName() const {
            return filePathForFileName_;
        }
    };
    
    class RandomizerStub : public Randomizer {
        double lowerBound_{};
        double upperBound_{};
        double randomFloat_{};
        int randomInt_{};
        int lowerIntBound_{};
        int upperIntBound_{};
    public:
        auto lowerIntBound() const {
            return lowerIntBound_;
        }
        
        auto upperIntBound() const {
            return upperIntBound_;
        }
        
        void setRandomInt(int x) {
            randomInt_ = x;
        }
        
        void setRandomFloat(double x) {
            randomFloat_ = x;
        }
        
        auto lowerFloatBound() const {
            return lowerBound_;
        }
        
        auto upperFloatBound() const {
            return upperBound_;
        }
        
        double randomFloatBetween(double a, double b) override {
            lowerBound_ = a;
            upperBound_ = b;
            return randomFloat_;
        }
        
        int randomIntBetween(int a, int b) override {
            lowerIntBound_ = a;
            upperIntBound_ = b;
            return randomInt_;
        }
    };
    
    class TrackFactoryStub : public TrackFactory {
        std::vector<Track::Settings> parameters_;
        std::vector<std::shared_ptr<Track>> tracks_;
    public:
        const auto &parameters() {
            return parameters_;
        }
        
        std::shared_ptr<Track> make(const Track::Settings &s) override {
            parameters_.push_back(s);
            auto track = tracks_.front();
            tracks_.erase(tracks_.begin());
            return track;
        }
        
        void setTracks(std::vector<std::shared_ptr<Track>> t) {
            tracks_ = std::move(t);
        }
    };
    
    class TargetListSetReaderStub : public TargetListReader {
        lists_type targetLists_{};
        std::string directory_{};
    public:
        void setTargetLists(lists_type lists) {
            targetLists_ = std::move(lists);
        }
        
        lists_type read(std::string d) override {
            directory_ = std::move(d);
            return targetLists_;
        }
        
        auto directory() const {
            return directory_;
        }
    };
    
    class FiniteTargetListStub : public FiniteTargetList {
        std::string directory_;
        std::string next_;
        std::string current_;
        bool empty_{};
        bool nextCalled_{};
    public:
        std::string current() override {
            return current_;
        }
        
        void setCurrent(std::string s) {
            current_ = std::move(s);
        }
        
        void loadFromDirectory(std::string directory) override {
            directory_ = std::move(directory);
        }
        
        std::string next() override {
            nextCalled_ = true;
            return next_;
        }
        
        void setNext(std::string s) {
            next_ = std::move(s);
        }
        
        auto directory() const {
            return directory_;
        }
        
        void setEmpty() {
            empty_ = true;
        }
        
        bool empty() override {
            return empty_;
        }
    };
    
    class UseCase {
    public:
        virtual ~UseCase() = default;
        virtual void run(RecognitionTestModel &) = 0;
    };
    
    class SubmittingResponse : public virtual UseCase {
    public:
        virtual bool headingWritten(OutputFileStub &) = 0;
    };
    
    class SubmittingCoordinateResponse : public SubmittingResponse {
        coordinate_response_measure::SubjectResponse response;
    public:
        void run(RecognitionTestModel &m) override {
            m.submitResponse(response);
        }
        
        bool headingWritten(OutputFileStub &file) override {
            return file.coordinateResponseTrialHeadingWritten();
        }
    };
    
    class SubmittingCorrectResponse : public UseCase {
    public:
        void run(RecognitionTestModel &m) override {
            m.submitCorrectResponse();
        }
    };
    
    class SubmittingIncorrectResponse : public UseCase {
    public:
        void run(RecognitionTestModel &m) override {
            m.submitIncorrectResponse();
        }
    };
    
    class SubmittingFreeResponse : public SubmittingResponse {
        FreeResponse response_;
    public:
        void run(RecognitionTestModel &m) override {
            m.submitResponse(response_);
        }
        
        void setResponse(std::string s) {
            response_.response = std::move(s);
        }
        
        bool headingWritten(OutputFileStub &file) override {
            return file.freeResponseTrialHeadingWritten();
        }
    };
    
    class MaskerUseCase : public virtual UseCase {
    public:
        virtual void setMaskerFilePath(std::string) = 0;
    };
    
    class ConditionUseCase : public virtual UseCase {
    public:
        virtual void setAuditoryOnly() = 0;
        virtual void setAudioVisual() = 0;
    };
    
    class InitializingTestUseCase :
        public virtual MaskerUseCase,
        public virtual ConditionUseCase
    {
    public:
        virtual const TestInformation &testInformation() = 0;
    };
    
    class InitializingAdaptiveTest : public InitializingTestUseCase {
        AdaptiveTest test_;
        TrackingRule targetLevelRule_;
    public:
        InitializingAdaptiveTest() {
            test_.targetLevelRule = &targetLevelRule_;
        }
        
        void setTargetListDirectory(std::string s) {
            test_.targetListDirectory = std::move(s);
        }
        
        void setMaskerFilePath(std::string s) override {
            test_.maskerFilePath = std::move(s);
        }
        
        void run(RecognitionTestModel &m) override {
            m.initializeTest(test_);
        }
        
        void setAudioVisual() override {
            test_.condition = Condition::audioVisual;
        }
        
        void setAuditoryOnly() override {
            test_.condition = Condition::auditoryOnly;
        }
        
        void setMaskerLevel_dB_SPL(int x) {
            test_.maskerLevel_dB_SPL = x;
        }
        
        void setStartingSnr_dB(int x) {
            test_.startingSnr_dB = x;
        }
        
        void setFullScaleLevel_dB_SPL(int x) {
            test_.fullScaleLevel_dB_SPL = x;
        }
        
        auto targetLevelRule() const {
            return &targetLevelRule_;
        }
        
        auto &test() const {
            return test_;
        }
        
        const TestInformation &testInformation() override {
            return test_.information;
        }
    };
    
    class InitializingFixedLevelTest : public InitializingTestUseCase {
        FixedLevelTest test_;
    public:
        void run(RecognitionTestModel &m) override {
            m.initializeTest(test_);
        }
        
        void setSnr_dB(int x) {
            test_.snr_dB = x;
        }
        
        void setMaskerLevel_dB_SPL(int x) {
            test_.maskerLevel_dB_SPL = x;
        }
        
        void setFullScaleLevel_dB_SPL(int x) {
            test_.fullScaleLevel_dB_SPL = x;
        }
        
        void setAudioVisual() override {
            test_.condition = Condition::audioVisual;
        }
        
        void setAuditoryOnly() override {
            test_.condition = Condition::auditoryOnly;
        }
        
        void setMaskerFilePath(std::string s) override {
            test_.maskerFilePath = std::move(s);
        }
        
        void setTargetListDirectory(std::string s) {
            test_.targetListDirectory = std::move(s);
        }
        
        auto &test() const {
            return test_;
        }
        
        const TestInformation &testInformation() override {
            return test_.information;
        }
    };
    
    class AudioDeviceUseCase : public virtual UseCase {
    public:
        virtual void setAudioDevice(std::string) = 0;
    };
    
    class PlayingTrial : public AudioDeviceUseCase {
        AudioSettings trial;
    public:
        void setAudioDevice(std::string s) override {
            trial.audioDevice = std::move(s);
        }
        
        void run(RecognitionTestModel &m) override {
            m.playTrial(trial);
        }
    };
    
    class PlayingCalibration :
        public AudioDeviceUseCase,
        public ConditionUseCase
    {
        Calibration calibration;
    public:
        void setAudioDevice(std::string s) override {
            calibration.audioSettings.audioDevice = std::move(s);
        }
        
        void run(RecognitionTestModel &m) override {
            m.playCalibration(calibration);
        }
        
        void setFilePath(std::string s) {
            calibration.filePath = std::move(s);
        }
        
        void setLevel_dB_SPL(int x) {
            calibration.level_dB_SPL = x;
        }
        
        void setFullScaleLevel_dB_SPL(int x) {
            calibration.fullScaleLevel_dB_SPL = x;
        }
        
        void setAudioVisual() override {
            calibration.condition = Condition::audioVisual;
        }
        
        void setAuditoryOnly() override {
            calibration.condition = Condition::auditoryOnly;
        }
    };

    class RecognitionTestModelTests : public testing::Test {
    protected:
        Calibration calibration;
        coordinate_response_measure::SubjectResponse coordinateResponse;
        TargetListSetReaderStub targetListSetReader;
        FiniteTargetListStub finiteTargetList;
        TargetPlayerStub targetPlayer;
        MaskerPlayerStub maskerPlayer;
        OutputFileStub outputFile;
        TrackFactoryStub snrTrackFactory;
        ResponseEvaluatorStub evaluator;
        RandomizerStub randomizer;
        RecognitionTestModel model{
            &targetListSetReader,
            &finiteTargetList,
            &targetPlayer,
            &maskerPlayer,
            &snrTrackFactory,
            &evaluator,
            &outputFile,
            &randomizer
        };
        ModelEventListenerStub listener;
        InitializingAdaptiveTest initializingAdaptiveTest;
        InitializingFixedLevelTest initializingFixedLevelTest;
        PlayingTrial playingTrial;
        PlayingCalibration playingCalibration;
        SubmittingCoordinateResponse submittingCoordinateResponse;
        SubmittingCorrectResponse submittingCorrectResponse;
        SubmittingIncorrectResponse submittingIncorrectResponse;
        SubmittingFreeResponse submittingFreeResponse;
        std::vector<std::shared_ptr<TargetListStub>> targetLists;
        std::vector<std::shared_ptr<TrackStub>> snrTracks;
        
        RecognitionTestModelTests() {
            model.subscribe(&listener);
            setTargetListCount(3);
        }
        
        void initializeAdaptiveTest() {
            run(initializingAdaptiveTest);
        }
        
        void run(UseCase &useCase) {
            useCase.run(model);
        }
        
        void playTrial() {
            run(playingTrial);
        }
        
        void playCalibration() {
            run(playingCalibration);
        }
        
        void submitCoordinateResponse() {
            model.submitResponse(coordinateResponse);
        }
        
        void setOutputAudioDeviceDescriptions(std::vector<std::string> v) {
            maskerPlayer.setOutputAudioDeviceDescriptions(std::move(v));
        }
        
        void assertMaskerPlayerNotPlayed() {
            assertFalse(maskerPlayerFadedIn());
        }
        
        bool maskerPlayerFadedIn() {
            return maskerPlayer.fadeInCalled();
        }
        
        void assertTargetPlayerPlayed() {
            assertTrue(targetPlayerPlayed());
        }
        
        bool targetPlayerPlayed() {
            return targetPlayer.played();
        }
        
        void assertTargetPlayerNotPlayed() {
            assertFalse(targetPlayerPlayed());
        }
        
        void setTrialInProgress() {
            maskerPlayer.setPlaying();
        }
        
        void assertTargetVideoOnlyHidden() {
            assertTrue(targetPlayerVideoHidden());
            assertFalse(targetPlayerVideoShown());
        }
        
        bool targetPlayerVideoHidden() {
            return targetPlayer.videoHidden();
        }
        
        bool targetPlayerVideoShown() {
            return targetPlayer.videoShown();
        }
        
        void assertTargetVideoOnlyShown() {
            assertTargetVideoNotHidden();
            assertTrue(targetPlayerVideoShown());
        }
        
        void assertTargetVideoNotHidden() {
            assertFalse(targetPlayerVideoHidden());
        }
        
        void assertThrowsRequestFailureWhenTrialInProgress(UseCase &useCase) {
            setTrialInProgress();
            assertCallThrowsRequestFailure(useCase, "Trial in progress.");
        }
        
        void assertCallThrowsRequestFailure(
            UseCase &useCase,
            std::string what
        ) {
            try {
                run(useCase);
                FAIL() <<
                    "Expected recognition_test::"
                    "RecognitionTestModel::"
                    "RequestFailure";
            } catch (const RecognitionTestModel::RequestFailure &e) {
                assertEqual(std::move(what), e.what());
            }
        }
        
        void runIgnoringFailure(UseCase &useCase) {
            try {
                run(useCase);
            } catch (const RecognitionTestModel::RequestFailure &) {
            }
        }
        
        template<typename T>
        void assertDevicePassedToPlayer(
            const T &player,
            AudioDeviceUseCase &useCase
        ) {
            useCase.setAudioDevice("a");
            run(useCase);
            assertEqual("a", player.device());
        }
        
        void assertDevicePassedToTargetPlayer(AudioDeviceUseCase &useCase) {
            assertDevicePassedToPlayer(targetPlayer, useCase);
        }
        
        void assertDevicePassedToMaskerPlayer(AudioDeviceUseCase &useCase) {
            assertDevicePassedToPlayer(maskerPlayer, useCase);
        }
        
        void assertThrowsRequestFailureWhenInvalidAudioDevice(
            AudioDeviceUseCase &useCase
        ) {
            throwInvalidAudioDeviceWhenSet();
            useCase.setAudioDevice("a");
            assertCallThrowsRequestFailure(
                useCase,
                "'a' is not a valid audio device."
            );
        }
        
        void throwInvalidAudioDeviceWhenSet() {
            maskerPlayer.throwInvalidAudioDeviceWhenDeviceSet();
            targetPlayer.throwInvalidAudioDeviceWhenDeviceSet();
        }
        
        void assertTargetFileLoadedPriorToRmsQuery(UseCase &useCase) {
            run(useCase);
            assertEqual("loadFile rms ", targetPlayer.log());
        }
    
        double dB(double x) {
            return 20 * std::log10(x);
        }
        
        auto targetPlayerLevel_dB() {
            return targetPlayer.level_dB();
        }
        
        template<typename T>
        void setTargetPlayerRms(T &&x) {
            targetPlayer.setRms(std::forward<T>(x));
        }
        
        auto targetFilePath() {
            return targetPlayer.filePath();
        }
        
        auto blueColor() {
            return coordinate_response_measure::Color::blue;
        }
        
        void playTrialWhenTrialAlreadyInProgressIgnoringFailure() {
            runIgnoringFailureWithTrialInProgress(playingTrial);
        }
        
        void runIgnoringFailureWithTrialInProgress(UseCase &useCase) {
            setTrialInProgress();
            runIgnoringFailure(useCase);
        }
        
        void playCalibrationWhenTrialAlreadyInProgressIgnoringFailure() {
            runIgnoringFailureWithTrialInProgress(playingCalibration);
        }
        
        void assertMaskerFilePathNotPassedToPlayerWhenTrialInProgress(InitializingTestUseCase &useCase) {
            useCase.setMaskerFilePath("a");
            runIgnoringFailureWithTrialInProgress(useCase);
            assertEqual("", maskerPlayer.filePath());
        }
        
        void assertTargetVideoNotHiddenWhenAuditoryOnlyButTrialInProgress(InitializingTestUseCase &useCase) {
            useCase.setAuditoryOnly();
            runIgnoringFailureWithTrialInProgress(useCase);
            assertTargetVideoNotHidden();
        }
        
        void assertTargetFilePathEquals(std::string what) {
            assertEqual(std::move(what), targetFilePath());
        }
        
        auto writtenCoordinateResponseTrial() {
            return outputFile.writtenCoordinateResponseTrial();
        }
        
        auto writtenFreeResponseTrial() {
            return outputFile.writtenFreeResponseTrial();
        }
        
        void setTargetListCount(int n) {
            targetLists.clear();
            snrTracks.clear();
            for (int i = 0; i < n; ++i) {
                targetLists.push_back(std::make_shared<TargetListStub>());
                snrTracks.push_back(std::make_shared<TrackStub>());
            }
            targetListSetReader.setTargetLists({targetLists.begin(), targetLists.end()});
            snrTrackFactory.setTracks({snrTracks.begin(), snrTracks.end()});
        }
        
        void initializeTestWithStartingList(int n) {
            if (gsl::narrow<size_t>(n) >= targetLists.size())
                setTargetListCount(n+1);
            selectList(n);
            initializeAdaptiveTest();
        }
        
        void initializeFixedLevelTest() {
            run(initializingFixedLevelTest);
        }
        
        void selectList(int n) {
            randomizer.setRandomInt(n);
        }
        
        void initializeAdaptiveTestWithListCount(int n) {
            setTargetListCount(n);
            initializeAdaptiveTest();
        }
        
        void assertTargetVideoHiddenWhenAuditoryOnly(ConditionUseCase &useCase) {
            useCase.setAuditoryOnly();
            run(useCase);
            assertTargetVideoOnlyHidden();
        }
        
        void assertTargetVideoShownWhenAudioVisual(ConditionUseCase &useCase) {
            useCase.setAudioVisual();
            run(useCase);
            assertTargetVideoOnlyShown();
        }
        
        auto targetLevelRule() {
            return initializingAdaptiveTest.targetLevelRule();
        }
        
        void assertSettingsContainTargetLevelRule(const Track::Settings &s) {
            assertEqual(targetLevelRule(), s.rule);
        }
        
        void assertSettingsMatchStartingX(const Track::Settings &s, int x) {
            assertEqual(x, s.startingX);
        }
        
        auto &adaptiveTestSettings() const {
            return initializingAdaptiveTest.test();
        }
        
        auto &fixedLevelTestSettings() const {
            return initializingFixedLevelTest.test();
        }
        
        void assertRandomizerPassedIntegerBounds(int a, int b) {
            assertEqual(a, randomizer.lowerIntBound());
            assertEqual(b, randomizer.upperIntBound());
        }
        
        auto snrTrack(int n) {
            return snrTracks.at(n);
        }
        
        auto targetList(int n) {
            return targetLists.at(n);
        }
        
        void setMaskerLevel_dB_SPL(int x) {
            initializingAdaptiveTest.setMaskerLevel_dB_SPL(x);
            initializingFixedLevelTest.setMaskerLevel_dB_SPL(x);
        }
        
        void setSnr_dB(int x) {
            initializingFixedLevelTest.setSnr_dB(x);
        }
        
        void setTestingFullScaleLevel_dB_SPL(int x) {
            initializingAdaptiveTest.setFullScaleLevel_dB_SPL(x);
            initializingFixedLevelTest.setFullScaleLevel_dB_SPL(x);
        }
        
        auto snrTrackFactoryParameters() {
            return snrTrackFactory.parameters();
        }
        
        bool targetPlayerPlaybackCompletionSubscribed() {
            return targetPlayer.playbackCompletionSubscribedTo();
        }
        
        auto maskerPlayerSecondsSeeked() {
            return maskerPlayer.secondsSeeked();
        }
        
        bool trialWrittenCorrect() {
            return writtenCoordinateResponseTrial().correct;
        }
        
        bool snrTrackPushedDown(int n) {
            return snrTrack(n)->pushedDown();
        }
        
        bool snrTrackPushedUp(int n) {
            return snrTrack(n)->pushedUp();
        }
        
        void setCorrectResponse() {
            evaluator.setCorrect();
        }
        
        void setIncorrectResponse() {
            evaluator.setIncorrect();
        }
        
        void setTargetListCurrent(int n, std::string s) {
            targetList(n)->setCurrent(std::move(s));;
        }
        
        void setTargetListNext(int n, std::string s) {
            targetList(n)->setNext(std::move(s));;
        }
        
        void setSnrTrackComplete(int n) {
            snrTrack(n)->setComplete();
        }
        
        void assertTestIncomplete() {
            assertFalse(testComplete());
        }
        
        bool testComplete() {
            return model.testComplete();
        }
        
        void assertTestComplete() {
            assertTrue(testComplete());
        }
        
        void assertPushesSnrTrackDown(UseCase &useCase) {
            initializeTestWithStartingList(1);
            run(useCase);
            assertTrue(snrTrackPushedDown(1));
            assertFalse(snrTrackPushedUp(1));
        }
        
        void assertNextTargetPassedToPlayer(UseCase &useCase) {
            initializeAdaptiveTest();
            setTargetListNext(1, "a");
            selectList(1);
            run(useCase);
            assertTargetFilePathEquals("a");
        }
        
        void assertSelectsRandomListInRangeAfterRemovingCompleteTracks(UseCase &useCase) {
            initializeAdaptiveTestWithListCount(3);
            setSnrTrackComplete(2);
            run(useCase);
            assertRandomizerPassedIntegerBounds(0, 1);
        }
        
        void assertTargetPlayerPlaybackCompletionSubscribed(UseCase &useCase) {
            run(useCase);
            assertTrue(targetPlayerPlaybackCompletionSubscribed());
        }
        
        void assertMaskerPlayerSeekedToRandomTime(UseCase &useCase) {
            randomizer.setRandomFloat(1);
            run(useCase);
            assertEqual(1., maskerPlayerSecondsSeeked());
        }
        
        void assertSelectsListAmongThoseWithIncompleteTracks(UseCase &useCase) {
            initializeAdaptiveTest();
            setTargetListNext(2, "a");
            setSnrTrackComplete(1);
            
            selectList(1);
            run(useCase);
            assertTargetFilePathEquals("a");
        }
        
        void assertPushesSnrTrackUp(UseCase &useCase) {
            initializeTestWithStartingList(1);
            run(useCase);
            assertTrue(snrTrackPushedUp(1));
            assertFalse(snrTrackPushedDown(1));
        }
        
        void assertMaskerFilePathPassedToPlayer(InitializingTestUseCase &useCase) {
            useCase.setMaskerFilePath("a");
            run(useCase);
            assertEqual("a", maskerPlayer.filePath());
        }
        
        void assertFiniteTargetListNextPassedToPlayer(UseCase &useCase) {
            finiteTargetList.setNext("a");
            run(useCase);
            assertTargetFilePathEquals("a");
        }
        
        void assertSeeksToRandomMaskerPositionWithinTrialDuration(UseCase &useCase) {
            targetPlayer.setDurationSeconds(1);
            maskerPlayer.setFadeTimeSeconds(2);
            maskerPlayer.setDurationSeconds(3);
            run(useCase);
            assertEqual(0., randomizer.lowerFloatBound());
            assertEqual(3. - 2 - 1 - 2, randomizer.upperFloatBound());
        }
        
        void assertMaskerPlayerLevelSet(UseCase &useCase) {
            setMaskerLevel_dB_SPL(1);
            setTestingFullScaleLevel_dB_SPL(2);
            maskerPlayer.setRms(3);
            run(useCase);
            assertEqual(1 - 2 - dB(3), maskerPlayer.level_dB());
        }
        
        void assertOutputFilePassedTestInformation(InitializingTestUseCase &useCase) {
            run(useCase);
            assertEqual(outputFile.openNewFileParameters(), &useCase.testInformation());
        }
        
        void assertThrowsRequestFailureWhenOutputFileThrows(UseCase &useCase) {
            outputFile.throwOnOpen();
            assertCallThrowsRequestFailure(useCase, "Unable to open output file.");
        }
        
        void assertOutputFileLog(std::string s) {
            assertEqual(std::move(s), outputFile.log());
        }
        
        void assertHeadingWrittenBeforeTrial(UseCase &useCase) {
            run(useCase);
            assertOutputFileLog("writeTrialHeading writeTrial ");
        }
        
        void assertHeadingWrittenOnceWhenRunTwice(UseCase &useCase) {
            run(useCase);
            run(useCase);
            assertOutputFileLog("writeTrialHeading writeTrial writeTrial ");
        }
        
        void assertHeadingWrittenTwiceWhenRunTwiceNotConsecutively(UseCase &useCase, UseCase &other) {
            run(useCase);
            run(other);
            run(useCase);
            assertOutputFileLog(
                "writeTrialHeading writeTrial "
                "writeTrialHeading writeTrial "
                "writeTrialHeading writeTrial "
            );
        }
        
        void assertHeadingWritten(SubmittingResponse &useCase) {
            run(useCase);
            assertTrue(useCase.headingWritten(outputFile));
        }
    };

    TEST_F(RecognitionTestModelTests, subscribesToPlayerEvents) {
        assertEqual(static_cast<TargetPlayer::EventListener *>(&model), targetPlayer.listener());
        assertEqual(static_cast<MaskerPlayer::EventListener *>(&model), maskerPlayer.listener());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestHidesTargetVideoWhenAuditoryOnly
    ) {
        assertTargetVideoHiddenWhenAuditoryOnly(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestHidesTargetVideoWhenAuditoryOnly
    ) {
        assertTargetVideoHiddenWhenAuditoryOnly(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestShowsTargetVideoWhenAudioVisual
    ) {
        assertTargetVideoShownWhenAudioVisual(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationHidesTargetVideoWhenAuditoryOnly
    ) {
        assertTargetVideoHiddenWhenAuditoryOnly(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationShowsTargetVideoWhenAudioVisual
    ) {
        assertTargetVideoShownWhenAudioVisual(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestCreatesSnrTrackForEachList
    ) {
        initializeAdaptiveTestWithListCount(3);
        assertEqual(3UL, snrTrackFactoryParameters().size());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestCreatesEachSnrTrackWithTargetLevelRule
    ) {
        initializeAdaptiveTestWithListCount(3);
        for (int i = 0; i < 3; ++i)
            assertSettingsContainTargetLevelRule(
                snrTrackFactoryParameters().at(i)
            );
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestCreatesEachSnrTrackWithStartingSnr
    ) {
        initializingAdaptiveTest.setStartingSnr_dB(1);
        initializeAdaptiveTestWithListCount(3);
        for (int i = 0; i < 3; ++i)
            assertSettingsMatchStartingX(snrTrackFactoryParameters().at(i), 1);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestClosesOutputFileOpensAndWritesTestInOrder
    ) {
        run(initializingAdaptiveTest);
        assertOutputFileLog("close openNewFile writeTest ");
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestClosesOutputFileOpensAndWritesTestInOrder
    ) {
        run(initializingFixedLevelTest);
        assertOutputFileLog("close openNewFile writeTest ");
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestOpensNewOutputFilePassingTestInformation
    ) {
        assertOutputFilePassedTestInformation(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestOpensNewOutputFilePassingTestInformation
    ) {
        assertOutputFilePassedTestInformation(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestWritesTestSettings
    ) {
        run(initializingAdaptiveTest);
        assertEqual(outputFile.adaptiveTest(), &adaptiveTestSettings());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestWritesTestSettings
    ) {
        run(initializingFixedLevelTest);
        assertEqual(outputFile.fixedLevelTest(), &fixedLevelTestSettings());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestPassesTargetListDirectory
    ) {
        initializingAdaptiveTest.setTargetListDirectory("a");
        initializeAdaptiveTest();
        assertEqual("a", targetListSetReader.directory());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestPassesTargetListDirectory
    ) {
        initializingFixedLevelTest.setTargetListDirectory("a");
        initializeFixedLevelTest();
        assertEqual("a", finiteTargetList.directory());
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialPassesAudioDeviceToTargetPlayer
    ) {
        assertDevicePassedToTargetPlayer(playingTrial);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationPassesAudioDeviceToTargetPlayer
    ) {
        assertDevicePassedToTargetPlayer(playingCalibration);
    }

    TEST_F(RecognitionTestModelTests, playTrialPassesAudioDeviceToMaskerPlayer) {
        assertDevicePassedToMaskerPlayer(playingTrial);
    }

    TEST_F(RecognitionTestModelTests, playTrialFadesInMasker) {
        playTrial();
        assertTrue(maskerPlayerFadedIn());
    }

    TEST_F(RecognitionTestModelTests, playCalibrationPlaysTarget) {
        playCalibration();
        assertTargetPlayerPlayed();
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitTypedResponseQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestPassesNextTargetToTargetPlayer
    ) {
        setTargetListNext(1, "a");
        initializeTestWithStartingList(1);
        assertTargetFilePathEquals("a");
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestPassesNextTargetToTargetPlayer
    ) {
        assertFiniteTargetListNextPassedToPlayer(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseLoadsNextTarget
    ) {
        assertNextTargetPassedToPlayer(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseLoadsNextTarget
    ) {
        assertNextTargetPassedToPlayer(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseLoadsNextTarget
    ) {
        assertNextTargetPassedToPlayer(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitTypedResponseLoadsNextTarget
    ) {
        initializeFixedLevelTest();
        assertFiniteTargetListNextPassedToPlayer(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestSelectsRandomListInRange
    ) {
        initializeAdaptiveTestWithListCount(3);
        assertRandomizerPassedIntegerBounds(0, 2);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseSelectsRandomListInRangeAfterRemovingCompleteTracks
    ) {
        assertSelectsRandomListInRangeAfterRemovingCompleteTracks(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseSelectsRandomListInRangeAfterRemovingCompleteTracks
    ) {
        assertSelectsRandomListInRangeAfterRemovingCompleteTracks(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseSelectsRandomListInRangeAfterRemovingCompleteTracks
    ) {
        assertSelectsRandomListInRangeAfterRemovingCompleteTracks(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationPassesAudioFileToTargetPlayer
    ) {
        playingCalibration.setFilePath("a");
        playCalibration();
        assertTargetFilePathEquals("a");
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestPassesMaskerFilePathToMaskerPlayer
    ) {
        assertMaskerFilePathPassedToPlayer(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestPassesMaskerFilePathToMaskerPlayer
    ) {
        assertMaskerFilePathPassedToPlayer(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitTypedResponseSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitTypedResponseSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        assertSeeksToRandomMaskerPositionWithinTrialDuration(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitTypedResponseSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestSetsInitialMaskerPlayerLevel
    ) {
        assertMaskerPlayerLevelSet(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestSetsInitialMaskerPlayerLevel
    ) {
        assertMaskerPlayerLevelSet(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestSetsTargetPlayerLevel
    ) {
        snrTrack(1)->setX(2);
        setMaskerLevel_dB_SPL(3);
        setTestingFullScaleLevel_dB_SPL(4);
        setTargetPlayerRms(5);
        initializeTestWithStartingList(1);
        assertEqual(2 + 3 - 4 - dB(5), targetPlayerLevel_dB());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestSetsTargetPlayerLevel
    ) {
        setSnr_dB(2);
        setMaskerLevel_dB_SPL(3);
        setTestingFullScaleLevel_dB_SPL(4);
        setTargetPlayerRms(5);
        initializeFixedLevelTest();
        assertEqual(2 + 3 - 4 - dB(5), targetPlayerLevel_dB());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitTypedResponseSetsTargetPlayerLevel
    ) {
        setSnr_dB(2);
        setMaskerLevel_dB_SPL(3);
        setTestingFullScaleLevel_dB_SPL(4);
        initializeFixedLevelTest();
        setTargetPlayerRms(5);
        run(submittingFreeResponse);
        assertEqual(2 + 3 - 4 - dB(5), targetPlayerLevel_dB());
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationSetsTargetPlayerLevel
    ) {
        playingCalibration.setLevel_dB_SPL(1);
        playingCalibration.setFullScaleLevel_dB_SPL(2);
        setTargetPlayerRms(3);
        playCalibration();
        assertEqual(1 - 2 - dB(3), targetPlayerLevel_dB());
    }

    TEST_F(RecognitionTestModelTests, fadeInCompletePlaysTarget) {
        maskerPlayer.fadeInComplete();
        assertTargetPlayerPlayed();
    }

    TEST_F(RecognitionTestModelTests, targetPlaybackCompleteFadesOutMasker) {
        targetPlayer.playbackComplete();
        assertTrue(maskerPlayer.fadeOutCalled());
    }

    TEST_F(RecognitionTestModelTests, fadeOutCompleteNotifiesTrialComplete) {
        maskerPlayer.fadeOutComplete();
        assertTrue(listener.notified());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitFreeResponseWritesTrialHeading
    ) {
        assertHeadingWritten(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesTrialHeading
    ) {
        assertHeadingWritten(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitFreeResponseWritesTrialHeadingBeforeWritingTrial
    ) {
        assertHeadingWrittenBeforeTrial(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesTrialHeadingBeforeWritingTrial
    ) {
        assertHeadingWrittenBeforeTrial(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitFreeResponseTwiceWritesTrialHeadingOnce
    ) {
        assertHeadingWrittenOnceWhenRunTwice(submittingFreeResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseTwiceWritesTrialHeadingOnce
    ) {
        assertHeadingWrittenOnceWhenRunTwice(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitFreeResponseTwiceWithCoordinateResponseInbetweenWritesTrialHeadingAgain
    ) {
        assertHeadingWrittenTwiceWhenRunTwiceNotConsecutively(
            submittingFreeResponse,
            submittingCoordinateResponse
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseTwiceWithFreeResponseInbetweenWritesTrialHeadingAgain
    ) {
        assertHeadingWrittenTwiceWhenRunTwiceNotConsecutively(
            submittingCoordinateResponse,
            submittingFreeResponse
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        submitFreeResponseWritesResponse
    ) {
        submittingFreeResponse.setResponse("a");
        run(submittingFreeResponse);
        assertEqual("a", writtenFreeResponseTrial().response);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitFreeResponsePassesCurrentTargetToEvaluator
    ) {
        finiteTargetList.setCurrent("a");
        run(submittingFreeResponse);
        assertEqual("a", evaluator.filePathForFileName());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitFreeResponseWritesTarget
    ) {
        evaluator.setFileName("a");
        run(submittingFreeResponse);
        assertEqual("a", writtenFreeResponseTrial().target);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesColor
    ) {
        coordinateResponse.color = blueColor();
        submitCoordinateResponse();
        assertEqual(blueColor(), writtenCoordinateResponseTrial().subjectColor);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesNumber
    ) {
        coordinateResponse.number = 1;
        submitCoordinateResponse();
        assertEqual(1, writtenCoordinateResponseTrial().subjectNumber);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesReversals
    ) {
        initializeTestWithStartingList(1);
        snrTrack(1)->setReversals(2);
        submitCoordinateResponse();
        assertEqual(2, writtenCoordinateResponseTrial().reversals);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectColor
    ) {
        evaluator.setCorrectColor(blueColor());
        submitCoordinateResponse();
        assertEqual(blueColor(), writtenCoordinateResponseTrial().correctColor);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectNumber
    ) {
        evaluator.setCorrectNumber(1);
        submitCoordinateResponse();
        assertEqual(1, writtenCoordinateResponseTrial().correctNumber);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesSnr
    ) {
        initializeTestWithStartingList(1);
        snrTrack(1)->setX(2);
        submitCoordinateResponse();
        assertEqual(2, writtenCoordinateResponseTrial().SNR_dB);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectTrial
    ) {
        setCorrectResponse();
        submitCoordinateResponse();
        assertTrue(trialWrittenCorrect());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesIncorrectTrial
    ) {
        setIncorrectResponse();
        submitCoordinateResponse();
        assertFalse(trialWrittenCorrect());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponsePassesSubjectResponseToEvaluator
    ) {
        submitCoordinateResponse();
        const auto *expected = &coordinateResponse;
        assertEqual(expected, evaluator.response());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponsePassesTargetToEvaluatorForNumberAndColor
    ) {
        initializeTestWithStartingList(1);
        setTargetListCurrent(1, "a");
        submitCoordinateResponse();
        assertEqual("a", evaluator.correctColorFilePath());
        assertEqual("a", evaluator.correctNumberFilePath());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponsePassesTargetToEvaluator
    ) {
        initializeTestWithStartingList(1);
        setTargetListCurrent(1, "a");
        submitCoordinateResponse();
        assertEqual("a", evaluator.correctFilePath());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponsePushesSnrDownWhenEvaluationIsCorrect
    ) {
        setCorrectResponse();
        assertPushesSnrTrackDown(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponsePushesSnrDown
    ) {
        assertPushesSnrTrackDown(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponsePushesSnrUpWhenEvaluationIsIncorrect
    ) {
        setIncorrectResponse();
        assertPushesSnrTrackUp(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponsePushesSnrUp
    ) {
        assertPushesSnrTrackUp(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseSelectsNextListAmongThoseWithIncompleteTracks
    ) {
        assertSelectsListAmongThoseWithIncompleteTracks(submittingCoordinateResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponseSelectsNextListAmongThoseWithIncompleteTracks
    ) {
        assertSelectsListAmongThoseWithIncompleteTracks(submittingCorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponseSelectsNextListAmongThoseWithIncompleteTracks
    ) {
        assertSelectsListAmongThoseWithIncompleteTracks(submittingIncorrectResponse);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestThrowsRequestFailureIfFileFailsToOpen
    ) {
        assertThrowsRequestFailureWhenOutputFileThrows(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestThrowsRequestFailureIfFileFailsToOpen
    ) {
        assertThrowsRequestFailureWhenOutputFileThrows(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationThrowsRequestFailureWhenTargetPlayerThrowsInvalidAudioFile
    ) {
        playingCalibration.setFilePath("a");
        targetPlayer.throwInvalidAudioFileOnRms();
        assertCallThrowsRequestFailure(playingCalibration, "unable to read a");
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialWithInvalidAudioDeviceThrowsRequestFailure
    ) {
        assertThrowsRequestFailureWhenInvalidAudioDevice(playingTrial);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationWithInvalidAudioDeviceThrowsRequestFailure
    ) {
        assertThrowsRequestFailureWhenInvalidAudioDevice(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialDoesNotChangeAudioDeviceWhenTrialInProgress
    ) {
        playTrialWhenTrialAlreadyInProgressIgnoringFailure();
        assertFalse(maskerPlayer.setDeviceCalled());
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationDoesNotChangeAudioDeviceWhenTrialInProgress
    ) {
        playCalibrationWhenTrialAlreadyInProgressIgnoringFailure();
        assertFalse(targetPlayer.setDeviceCalled());
    }

    TEST_F(RecognitionTestModelTests, playTrialDoesNotPlayIfTrialInProgress) {
        playTrialWhenTrialAlreadyInProgressIgnoringFailure();
        assertMaskerPlayerNotPlayed();
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationDoesNotPlayIfTrialInProgress
    ) {
        playCalibrationWhenTrialAlreadyInProgressIgnoringFailure();
        assertTargetPlayerNotPlayed();
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestThrowsRequestFailureIfTrialInProgress
    ) {
        assertThrowsRequestFailureWhenTrialInProgress(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialThrowsRequestFailureIfTrialInProgress
    ) {
        assertThrowsRequestFailureWhenTrialInProgress(playingTrial);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationThrowsRequestFailureIfTrialInProgress
    ) {
        assertThrowsRequestFailureWhenTrialInProgress(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestDoesNotLoadMaskerIfTrialInProgress
    ) {
        assertMaskerFilePathNotPassedToPlayerWhenTrialInProgress(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestDoesNotLoadMaskerIfTrialInProgress
    ) {
        assertMaskerFilePathNotPassedToPlayerWhenTrialInProgress(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeAdaptiveTestDoesNotHideTargetPlayerWhenAuditoryOnlyButTrialInProgress
    ) {
        assertTargetVideoNotHiddenWhenAuditoryOnlyButTrialInProgress(initializingAdaptiveTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeFixedLevelTestDoesNotHideTargetPlayerWhenAuditoryOnlyButTrialInProgress
    ) {
        assertTargetVideoNotHiddenWhenAuditoryOnlyButTrialInProgress(initializingFixedLevelTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        audioDevicesReturnsOutputAudioDeviceDescriptions
    ) {
        setOutputAudioDeviceDescriptions({"a", "b", "c"});
        assertEqual({"a", "b", "c"}, model.audioDevices());
    }

    TEST_F(
        RecognitionTestModelTests,
        testCompleteWhenAllTracksComplete
    ) {
        initializeAdaptiveTestWithListCount(3);
        setSnrTrackComplete(0);
        assertTestIncomplete();
        setSnrTrackComplete(1);
        assertTestIncomplete();
        setSnrTrackComplete(2);
        assertTestComplete();
    }

    TEST_F(
        RecognitionTestModelTests,
        fixedLevelTestCompleteWhenListEmpty
    ) {
        initializeFixedLevelTest();
        assertTestIncomplete();
        finiteTargetList.setEmpty();
        assertTestComplete();
    }
}

