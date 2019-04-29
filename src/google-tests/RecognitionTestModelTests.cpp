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
        std::vector<std::string> outputAudioDeviceDescriptions_{};
        LogString log_{};
        std::string filePath_{};
        std::string device_{};
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
        
        void subscribe(EventListener *listener) override {
            listener_ = listener;
        }
        
        void fadeOut() override {
            fadeOutCalled_ = true;
        }
        
        void loadFile(std::string filePath) override {
            log_.insert("loadFile ");
            filePath_ = filePath;
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
            log_.insert("rms ");
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
        LogString log_{};
        std::string filePath_{};
        std::string device_{};
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
            log_.insert("loadFile ");
            filePath_ = filePath;
        }
        
        double rms() override {
            log_.insert("rms ");
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
        coordinate_response_measure::Trial trialWritten_{};
        LogString log_{};
        const Test *testWritten_{};
        const Test *openNewFileParameters_{};
        bool throwOnOpen_{};
        bool headingWritten_{};
    public:
        auto &log() const {
            return log_;
        }
        
        auto headingWritten() const {
            return headingWritten_;
        }
        
        auto testWritten() const {
            return testWritten_;
        }
        
        auto &trialWritten() const {
            return trialWritten_;
        }
        
        void writeTrial(
            const coordinate_response_measure::Trial &trial
        ) override {
            log_.insert("writeTrial ");
            trialWritten_ = trial;
        }
        
        void openNewFile(
            const Test &p
        ) override {
            log_.insert("openNewFile ");
            openNewFileParameters_ = &p;
            if (throwOnOpen_)
                throw OpenFailure{};
        }
        
        auto openNewFileParameters() const {
            return openNewFileParameters_;
        }
        
        void writeTrialHeading() override {
            log_.insert("writeTrialHeading ");
            headingWritten_ = true;
        }
        
        void writeTest(
            const Test &test
        ) override {
            log_.insert("writeTest ");
            testWritten_ = &test;
        }
        
        void close() override {
            log_.insert("close ");
        }
        
        void throwOnOpen() {
            throwOnOpen_ = true;
        }
    };
    
    class ModelEventListenerStub :
        public Model::EventListener
    {
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
        std::string correctTarget_{};
        std::string correctNumberFilePath_{};
        std::string correctColorFilePath_{};
        const coordinate_response_measure::SubjectResponse *response_{};
        int correctNumber_{};
        coordinate_response_measure::Color correctColor_{};
        bool correct_{};
    public:
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
    
    class UseCase {
    public:
        virtual ~UseCase() = default;
        virtual void run(RecognitionTestModel &) = 0;
    };
    
    class ConditionUseCase : public virtual UseCase {
    public:
        virtual void setAuditoryOnly() = 0;
        virtual void setAudioVisual() = 0;
    };
    
    class SubmittingCoordinateResponse : public UseCase {
        coordinate_response_measure::SubjectResponse response;
    public:
        void run(RecognitionTestModel &m) override {
            m.submitResponse(response);
        }
    };
    
    class SubmittingCorrectResponse : public UseCase {
    public:
        void run(RecognitionTestModel &m) override {
            m.submitCorrectResponse();
        }
    };
    
    class InitializingTest : public ConditionUseCase {
        Test test_;
        TrackingRule targetLevelRule_;
    public:
        InitializingTest() {
            test_.targetLevelRule = &targetLevelRule_;
        }
        
        void setTargetListDirectory(std::string s) {
            test_.targetListDirectory = std::move(s);
        }
        
        void setMaskerFilePath(std::string s) {
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
            calibration.audioDevice = std::move(s);
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
        TargetPlayerStub targetPlayer;
        MaskerPlayerStub maskerPlayer;
        OutputFileStub outputFile;
        TrackFactoryStub snrTrackFactory;
        ResponseEvaluatorStub evaluator;
        RandomizerStub randomizer;
        RecognitionTestModel model{
            &targetListSetReader,
            &targetPlayer,
            &maskerPlayer,
            &snrTrackFactory,
            &evaluator,
            &outputFile,
            &randomizer
        };
        ModelEventListenerStub listener;
        InitializingTest initializingTest;
        PlayingTrial playingTrial;
        PlayingCalibration playingCalibration;
        SubmittingCoordinateResponse submittingCoordinateResponse;
        SubmittingCorrectResponse submittingCorrectResponse;
        std::vector<std::shared_ptr<TargetListStub>> targetLists;
        std::vector<std::shared_ptr<TrackStub>> snrTracks;
        
        RecognitionTestModelTests() {
            model.subscribe(&listener);
            setTargetListCount(3);
        }
        
        void initializeTest() {
            run(initializingTest);
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
            EXPECT_FALSE(maskerPlayerFadedIn());
        }
        
        bool maskerPlayerFadedIn() {
            return maskerPlayer.fadeInCalled();
        }
        
        void assertTargetPlayerPlayed() {
            EXPECT_TRUE(targetPlayerPlayed());
        }
        
        bool targetPlayerPlayed() {
            return targetPlayer.played();
        }
        
        void assertTargetPlayerNotPlayed() {
            EXPECT_FALSE(targetPlayerPlayed());
        }
        
        void setTrialInProgress() {
            maskerPlayer.setPlaying();
        }
        
        void assertTargetVideoOnlyHidden() {
            EXPECT_TRUE(targetPlayerVideoHidden());
            EXPECT_FALSE(targetPlayerVideoShown());
        }
        
        bool targetPlayerVideoHidden() {
            return targetPlayer.videoHidden();
        }
        
        bool targetPlayerVideoShown() {
            return targetPlayer.videoShown();
        }
        
        void assertTargetVideoOnlyShown() {
            assertTargetVideoNotHidden();
            EXPECT_TRUE(targetPlayerVideoShown());
        }
        
        void assertTargetVideoNotHidden() {
            EXPECT_FALSE(targetPlayerVideoHidden());
        }
        
        void assertInitializeTestThrowsRequestFailure(std::string what) {
            assertCallThrowsRequestFailure(initializingTest, std::move(what));
        }
        
        void assertPlayTrialThrowsRequestFailure(std::string what) {
            assertCallThrowsRequestFailure(playingTrial, std::move(what));
        }
        
        void assertPlayCalibrationThrowsRequestFailure(std::string what) {
            assertCallThrowsRequestFailure(playingCalibration, std::move(what));
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
        
        void initializeTestIgnoringFailure() {
            runIgnoringFailure(initializingTest);
        }
        
        void runIgnoringFailure(UseCase &useCase) {
            try {
                run(useCase);
            } catch (const RecognitionTestModel::RequestFailure &) {
            }
        }
        
        void playTrialIgnoringFailure() {
            runIgnoringFailure(playingTrial);
        }
        
        void playCalibrationIgnoringFailure() {
            runIgnoringFailure(playingCalibration);
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
            setTrialInProgress();
            playTrialIgnoringFailure();
        }
        
        void playCalibrationWhenTrialAlreadyInProgressIgnoringFailure() {
            setTrialInProgress();
            playCalibrationIgnoringFailure();
        }
        
        void initializeTestWhenTrialAlreadyInProgressIgnoringFailure() {
            setTrialInProgress();
            initializeTestIgnoringFailure();
        }
        
        void assertTargetFilePathEquals(std::string what) {
            assertEqual(std::move(what), targetFilePath());
        }
        
        auto trialWritten() {
            return outputFile.trialWritten();
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
            initializeTest();
        }
        
        void selectList(int n) {
            randomizer.setRandomInt(n);
        }
        
        void initializeTestWithListCount(int n) {
            setTargetListCount(n);
            initializeTest();
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
            return initializingTest.targetLevelRule();
        }
        
        void assertSettingsContainTargetLevelRule(const Track::Settings &s) {
            EXPECT_EQ(targetLevelRule(), s.rule);
        }
        
        void assertSettingsMatchStartingX(const Track::Settings &s, int x) {
            EXPECT_EQ(x, s.startingX);
        }
        
        auto &testSettings() const {
            return initializingTest.test();
        }
        
        void assertRandomizerPassedIntegerBounds(int a, int b) {
            EXPECT_EQ(a, randomizer.lowerIntBound());
            EXPECT_EQ(b, randomizer.upperIntBound());
        }
        
        auto snrTrack(int n) {
            return snrTracks.at(n);
        }
        
        auto targetList(int n) {
            return targetLists.at(n);
        }
        
        void setMaskerLevel_dB_SPL(int x) {
            initializingTest.setMaskerLevel_dB_SPL(x);
        }
        
        void setTestingFullScaleLevel_dB_SPL(int x) {
            initializingTest.setFullScaleLevel_dB_SPL(x);
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
            return trialWritten().correct;
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
        
        void setSnrTrackComplete(int n) {
            snrTrack(n)->setComplete();
        }
        
        void assertTestIncomplete() {
            EXPECT_FALSE(testComplete());
        }
        
        bool testComplete() {
            return model.testComplete();
        }
        
        void assertTestComplete() {
            EXPECT_TRUE(testComplete());
        }
        
        void assertPushesSnrTrackDown(UseCase &useCase) {
            initializeTestWithStartingList(1);
            run(useCase);
            EXPECT_TRUE(snrTrackPushedDown(1));
            EXPECT_FALSE(snrTrackPushedUp(1));
        }
        
        void assertNextTargetPassedToPlayer(UseCase &useCase) {
            initializeTest();
            targetList(1)->setNext("a");
            selectList(1);
            run(useCase);
            assertTargetFilePathEquals("a");
        }
        
        void assertSelectsRandomListInRangeAfterRemovingCompleteTracks(UseCase &useCase) {
            initializeTestWithListCount(3);
            setSnrTrackComplete(2);
            run(useCase);
            assertRandomizerPassedIntegerBounds(0, 1);
        }
        
        void assertTargetPlayerPlaybackCompletionSubscribed(UseCase &useCase) {
            run(useCase);
            EXPECT_TRUE(targetPlayerPlaybackCompletionSubscribed());
        }
        
        void assertMaskerPlayerSeekedToRandomTime(UseCase &useCase) {
            randomizer.setRandomFloat(1);
            run(useCase);
            EXPECT_EQ(1, maskerPlayerSecondsSeeked());
        }
        
        void assertSelectsListAmongThoseWithIncompleteTracks(UseCase &useCase) {
            initializeTest();
            targetList(2)->setNext("a");
            setSnrTrackComplete(1);
            
            selectList(1);
            run(useCase);
            assertTargetFilePathEquals("a");
        }
    };

    TEST_F(RecognitionTestModelTests, subscribesToPlayerEvents) {
        EXPECT_EQ(&model, targetPlayer.listener());
        EXPECT_EQ(&model, maskerPlayer.listener());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestHidesTargetVideoWhenAuditoryOnly
    ) {
        assertTargetVideoHiddenWhenAuditoryOnly(initializingTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestShowsTargetVideoWhenAudioVisual
    ) {
        assertTargetVideoShownWhenAudioVisual(initializingTest);
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
        initializeTestWritesTrialHeading
    ) {
        initializeTest();
        EXPECT_TRUE(outputFile.headingWritten());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestClosesOutputFileOpensWritesTestAndWritesTrialHeadingInOrder
    ) {
        initializeTest();
        assertEqual(
            "close openNewFile writeTest writeTrialHeading ",
            outputFile.log()
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestCreatesSnrTrackForEachList
    ) {
        initializeTestWithListCount(3);
        EXPECT_EQ(3, snrTrackFactoryParameters().size());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestCreatesEachSnrTrackWithTargetLevelRule
    ) {
        initializeTestWithListCount(3);
        for (int i = 0; i < 3; ++i)
            assertSettingsContainTargetLevelRule(
                snrTrackFactoryParameters().at(i)
            );
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestCreatesEachSnrTrackWithStartingSnr
    ) {
        initializingTest.setStartingSnr_dB(1);
        initializeTestWithListCount(3);
        for (int i = 0; i < 3; ++i)
            assertSettingsMatchStartingX(snrTrackFactoryParameters().at(i), 1);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestOpensNewOutputFilePassingTestSettings
    ) {
        initializeTest();
        EXPECT_EQ(outputFile.openNewFileParameters(), &testSettings());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestWritesTestSettings
    ) {
        initializeTest();
        EXPECT_EQ(outputFile.testWritten(), &testSettings());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestPassesTargetListDirectory
    ) {
        initializingTest.setTargetListDirectory("a");
        initializeTest();
        assertEqual("a", targetListSetReader.directory());
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
        EXPECT_TRUE(maskerPlayerFadedIn());
    }

    TEST_F(RecognitionTestModelTests, playCalibrationPlaysTarget) {
        playCalibration();
        assertTargetPlayerPlayed();
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(initializingTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestPassesNextTargetToTargetPlayer
    ) {
        targetList(1)->setNext("a");
        initializeTestWithStartingList(1);
        assertTargetFilePathEquals("a");
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
        initializeTestSelectsRandomListInRange
    ) {
        initializeTestWithListCount(3);
        assertRandomizerPassedIntegerBounds(0, 2);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponseSelectsRandomListInRangeAfterRemovingCompleteTracks
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
        playCalibrationPassesAudioFileToTargetPlayer
    ) {
        playingCalibration.setFilePath("a");
        playCalibration();
        assertTargetFilePathEquals("a");
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestPassesMaskerFilePathToMaskerPlayer
    ) {
        initializingTest.setMaskerFilePath("a");
        initializeTest();
        assertEqual("a", maskerPlayer.filePath());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestSubscribesToTargetPlaybackCompletionNotification
    ) {
        assertTargetPlayerPlaybackCompletionSubscribed(initializingTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponseSubscribesToTargetPlaybackCompletionNotification
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
        initializeTestSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        targetPlayer.setDurationSeconds(1);
        maskerPlayer.setFadeTimeSeconds(2);
        maskerPlayer.setDurationSeconds(3);
        initializeTest();
        EXPECT_EQ(0., randomizer.lowerFloatBound());
        EXPECT_EQ(3 - 2 - 1 - 2, randomizer.upperFloatBound());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestSeeksToRandomMaskerPosition
    ) {
        assertMaskerPlayerSeekedToRandomTime(initializingTest);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponseSeeksToRandomMaskerPosition
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
        initializeTestSetsInitialMaskerPlayerLevel
    ) {
        setMaskerLevel_dB_SPL(1);
        setTestingFullScaleLevel_dB_SPL(2);
        maskerPlayer.setRms(3);
        initializeTest();
        EXPECT_EQ(1 - 2 - dB(3), maskerPlayer.level_dB());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestSetsTargetPlayerLevel
    ) {
        snrTrack(1)->setX(2);
        setMaskerLevel_dB_SPL(3);
        setTestingFullScaleLevel_dB_SPL(4);
        setTargetPlayerRms(5);
        initializeTestWithStartingList(1);
        EXPECT_EQ(2 + 3 - 4 - dB(5), targetPlayerLevel_dB());
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationSetsTargetPlayerLevel
    ) {
        playingCalibration.setLevel_dB_SPL(1);
        playingCalibration.setFullScaleLevel_dB_SPL(2);
        setTargetPlayerRms(3);
        playCalibration();
        EXPECT_EQ(1 - 2 - dB(3), targetPlayerLevel_dB());
    }

    TEST_F(RecognitionTestModelTests, fadeInCompletePlaysTarget) {
        maskerPlayer.fadeInComplete();
        assertTargetPlayerPlayed();
    }

    TEST_F(RecognitionTestModelTests, targetPlaybackCompleteFadesOutMasker) {
        targetPlayer.playbackComplete();
        EXPECT_TRUE(maskerPlayer.fadeOutCalled());
    }

    TEST_F(RecognitionTestModelTests, fadeOutCompleteNotifiesTrialComplete) {
        maskerPlayer.fadeOutComplete();
        EXPECT_TRUE(listener.notified());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesColor
    ) {
        coordinateResponse.color = blueColor();
        submitCoordinateResponse();
        EXPECT_EQ(blueColor(), trialWritten().subjectColor);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesNumber
    ) {
        coordinateResponse.number = 1;
        submitCoordinateResponse();
        EXPECT_EQ(1, trialWritten().subjectNumber);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesReversals
    ) {
        initializeTestWithStartingList(1);
        snrTrack(1)->setReversals(2);
        submitCoordinateResponse();
        EXPECT_EQ(2, trialWritten().reversals);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectColor
    ) {
        evaluator.setCorrectColor(blueColor());
        submitCoordinateResponse();
        EXPECT_EQ(blueColor(), trialWritten().correctColor);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectNumber
    ) {
        evaluator.setCorrectNumber(1);
        submitCoordinateResponse();
        EXPECT_EQ(1, trialWritten().correctNumber);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesSnr
    ) {
        initializeTestWithStartingList(1);
        snrTrack(1)->setX(2);
        submitCoordinateResponse();
        EXPECT_EQ(2, trialWritten().SNR_dB);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesCorrectTrial
    ) {
        setCorrectResponse();
        submitCoordinateResponse();
        EXPECT_TRUE(trialWrittenCorrect());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCoordinateResponseWritesIncorrectTrial
    ) {
        setIncorrectResponse();
        submitCoordinateResponse();
        EXPECT_FALSE(trialWrittenCorrect());
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
        submitIncorrectResponsePushesSnrUp
    ) {
        initializeTestWithStartingList(1);
        setIncorrectResponse();
        submitCoordinateResponse();
        EXPECT_TRUE(snrTrackPushedUp(1));
        EXPECT_FALSE(snrTrackPushedDown(1));
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
        submitResponsePassesSubjectResponseToEvaluator
    ) {
        submitCoordinateResponse();
        EXPECT_EQ(&coordinateResponse, evaluator.response());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestThrowsRequestFailureIfFileFailsToOpen
    ) {
        outputFile.throwOnOpen();
        assertInitializeTestThrowsRequestFailure("Unable to open output file.");
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
        EXPECT_FALSE(maskerPlayer.setDeviceCalled());
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationDoesNotChangeAudioDeviceWhenTrialInProgress
    ) {
        playCalibrationWhenTrialAlreadyInProgressIgnoringFailure();
        EXPECT_FALSE(targetPlayer.setDeviceCalled());
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
        initializeTestThrowsRequestFailureIfTrialInProgress
    ) {
        setTrialInProgress();
        assertInitializeTestThrowsRequestFailure("Trial in progress.");
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialThrowsRequestFailureIfTrialInProgress
    ) {
        setTrialInProgress();
        assertPlayTrialThrowsRequestFailure("Trial in progress.");
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationThrowsRequestFailureIfTrialInProgress
    ) {
        setTrialInProgress();
        assertPlayCalibrationThrowsRequestFailure("Trial in progress.");
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestDoesNotLoadMaskerIfTrialInProgress
    ) {
        initializingTest.setMaskerFilePath("a");
        initializeTestWhenTrialAlreadyInProgressIgnoringFailure();
        assertEqual("", maskerPlayer.filePath());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestDoesNotHideTargetPlayerWhenAuditoryOnlyButTrialInProgress
    ) {
        initializingTest.setAuditoryOnly();
        initializeTestWhenTrialAlreadyInProgressIgnoringFailure();
        assertTargetVideoNotHidden();
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
        initializeTestWithListCount(3);
        setSnrTrackComplete(0);
        assertTestIncomplete();
        setSnrTrackComplete(1);
        assertTestIncomplete();
        setSnrTrackComplete(2);
        assertTestComplete();
    }
}

