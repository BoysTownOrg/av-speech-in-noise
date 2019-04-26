#include "LogString.h"
#include "assert-utility.h"
#include <recognition-test/RecognitionTestModel.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <cmath>

namespace {
    using namespace av_coordinate_response_measure;
    
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

    class TargetListStub : public TargetList {
        std::string directory_{};
        std::string next_{};
        std::string current_{};
        bool empty_{};
        bool nextCalled_{};
    public:
        std::string current() override {
            return current_;
        }
        
        void setCurrent(std::string s) {
            current_ = std::move(s);
        }
        
        auto nextCalled() const {
            return nextCalled_;
        }
        
        auto directory() const {
            return directory_;
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
    };
    
    class OutputFileStub : public OutputFile {
        Trial trialWritten_{};
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
            const Trial &trial
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
        
        auto &settings() const {
            return settings_;
        }
        
        void setX(int x) {
            x_ = x;
        }
        
        void reset(const Settings &p) override {
            settings_ = p;
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
        const SubjectResponse *response_{};
        int correctNumber_{};
        Color correctColor_{};
        bool correct_{};
    public:
        void setCorrectNumber(int x) {
            correctNumber_ = x;
        }
        
        void setCorrectColor(Color c) {
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
            std::string target,
            const SubjectResponse &p
        ) override {
            correctTarget_ = std::move(target);
            response_ = &p;
            return correct_;
        }
        
        Color correctColor(const std::string &filePath) override {
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
    
    class UseCase {
    public:
        virtual ~UseCase() = default;
        virtual void run(RecognitionTestModel &) = 0;
    };
    
    class InitializingTest : public UseCase {
        Test test_;
        TrackingRule targetLevelRule_;
    public:
        InitializingTest() {
            test_.targetLevelRule = &targetLevelRule_;
        }
        
        void setTargetListSetDirectory(std::string s) {
            test_.targetListSetDirectory = std::move(s);
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
        
        void setAudioVisual() {
            test_.condition =
                Condition::audioVisual;
        }
        
        void setAuditoryOnly() {
            test_.condition =
                Condition::auditoryOnly;
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
    
    class AudioDeviceUseCase : public UseCase {
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
    
    class PlayingCalibration : public AudioDeviceUseCase {
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
        
        void setAudioVisual() {
            calibration.condition =
                Condition::audioVisual;
        }
        
        void setAuditoryOnly() {
            calibration.condition =
                Condition::auditoryOnly;
        }
    };
    
    class TrackFactoryStub : public TrackFactory {
        std::vector<Track::Settings> parameters_;
        std::vector<std::shared_ptr<Track>> tracks_{};
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

    class RecognitionTestModelTests : public testing::Test {
    protected:
        Calibration calibration;
        SubjectResponse subjectResponse;
        TargetListStub targetList{};
        TargetListSetReaderStub targetListSetReader;
        TargetPlayerStub targetPlayer{};
        MaskerPlayerStub maskerPlayer{};
        OutputFileStub outputFile{};
        TrackStub snrTrack{};
        TrackFactoryStub snrTrackFactory{};
        ResponseEvaluatorStub evaluator{};
        RandomizerStub randomizer{};
        RecognitionTestModel model{
            &targetListSetReader,
            &targetList,
            &targetPlayer,
            &maskerPlayer,
            &snrTrackFactory,
            &snrTrack,
            &evaluator,
            &outputFile,
            &randomizer
        };
        ModelEventListenerStub listener{};
        InitializingTest initializingTest{};
        PlayingTrial playingTrial{};
        PlayingCalibration playingCalibration{};
        std::vector<std::shared_ptr<TargetListStub>> lists{};
        std::vector<std::shared_ptr<TrackStub>> snrTracks{};
        
        RecognitionTestModelTests() {
            model.subscribe(&listener);
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
        
        void submitResponse() {
            model.submitResponse(subjectResponse);
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
        
        void assertTargetPlayerNotPlayed() {
            EXPECT_FALSE(targetPlayer.played());
        }
        
        void assertListNotAdvanced() {
            EXPECT_FALSE(targetList.nextCalled());
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
            } catch (
                const
                RecognitionTestModel::
                RequestFailure &e
            ) {
                assertEqual(std::move(what), e.what());
            }
        }
        
        void initializeTestIgnoringFailure() {
            runIgnoringFailure(initializingTest);
        }
        
        void runIgnoringFailure(UseCase &useCase) {
            try {
                run(useCase);
            } catch (
                const
                RecognitionTestModel::
                RequestFailure &
            ) {
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
        
        auto snrTrackPushedDown() {
            return snrTrack.pushedDown();
        }
        
        auto snrTrackPushedUp() {
            return snrTrack.pushedUp();
        }
        
        auto targetFilePath() {
            return targetPlayer.filePath();
        }
        
        auto blueColor() {
            return Color::blue;
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
            lists.clear();
            snrTracks.clear();
            for (int i = 0; i < n; ++i) {
                lists.push_back(std::make_shared<TargetListStub>());
                snrTracks.push_back(std::make_shared<TrackStub>());
            }
            targetListSetReader.setTargetLists({lists.begin(), lists.end()});
            snrTrackFactory.setTracks({snrTracks.begin(), snrTracks.end()});
        }
        
        void initializeTestWithStartingList(int n) {
            if (gsl::narrow<size_t>(n) >= lists.size())
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
    };

    TEST_F(RecognitionTestModelTests, subscribesToPlayerEvents) {
        EXPECT_EQ(&model, targetPlayer.listener());
        EXPECT_EQ(&model, maskerPlayer.listener());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestHidesTargetVideoWhenAuditoryOnly
    ) {
        initializingTest.setAuditoryOnly();
        initializeTest();
        assertTargetVideoOnlyHidden();
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestShowsTargetVideoWhenAudioVisual
    ) {
        initializingTest.setAudioVisual();
        initializeTest();
        assertTargetVideoOnlyShown();
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationHidesTargetVideoWhenAuditoryOnly
    ) {
        playingCalibration.setAuditoryOnly();
        playCalibration();
        assertTargetVideoOnlyHidden();
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationShowsTargetVideoWhenAudioVisual
    ) {
        playingCalibration.setAudioVisual();
        playCalibration();
        assertTargetVideoOnlyShown();
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
        initializeTestResetsSnrTrackWithTargetLevelRule
    ) {
        initializeTest();
        EXPECT_EQ(
            initializingTest.targetLevelRule(),
            snrTrack.settings().rule
        );
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestResetsSnrTrackWithStartingSnr
    ) {
        initializingTest.setStartingSnr_dB(1);
        initializeTest();
        EXPECT_EQ(1, snrTrack.settings().startingX);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestCreatesEachSnrTrackWithTargetLevelRule
    ) {
        initializeTestWithListCount(3);
        auto parameters = snrTrackFactory.parameters();
        EXPECT_EQ(3, parameters.size());
        for (auto p : parameters)
            EXPECT_EQ(
                initializingTest.targetLevelRule(),
                p.rule
            );
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestCreatesEachSnrTrackWithStartingSnr
    ) {
        initializingTest.setStartingSnr_dB(1);
        initializeTestWithListCount(3);
        auto parameters = snrTrackFactory.parameters();
        EXPECT_EQ(3, parameters.size());
        for (auto p : parameters)
            EXPECT_EQ(1, p.startingX);
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestOpensNewOutputFile
    ) {
        initializeTest();
        EXPECT_EQ(outputFile.openNewFileParameters(), &initializingTest.test());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestWritesTestInformation
    ) {
        initializeTest();
        EXPECT_EQ(outputFile.testWritten(), &initializingTest.test());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestPassesTargetListDirectoryToTargetList
    ) {
        initializingTest.setTargetListDirectory("a");
        initializeTest();
        assertEqual("a", targetList.directory());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestPassesTargetListSetDirectory
    ) {
        initializingTest.setTargetListSetDirectory("a");
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
        EXPECT_TRUE(targetPlayer.played());
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(playingTrial);
    }

    TEST_F(
        RecognitionTestModelTests,
        playCalibrationQueriesTargetRmsAfterLoadingFile
    ) {
        assertTargetFileLoadedPriorToRmsQuery(playingCalibration);
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialPassesNextTargetToTargetPlayer
    ) {
        targetList.setNext("a");
        playTrial();
        assertTargetFilePathEquals("a");
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialPassesNextTargetToTargetPlayer_2
    ) {
        initializeTestWithStartingList(1);
        lists.at(1)->setNext("a");
        playTrial();
        assertTargetFilePathEquals("a");
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponseSelectsNextList
    ) {
        initializeTestWithListCount(3);
        selectList(1);
        submitResponse();
        lists.at(1)->setNext("a");
        playTrial();
        assertTargetFilePathEquals("a");
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestSelectsRandomListInRange
    ) {
        initializeTestWithListCount(3);
        EXPECT_EQ(int{0}, randomizer.lowerIntBound());
        EXPECT_EQ(2, randomizer.upperIntBound());
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
        playTrialSubscribesToTargetPlaybackCompletionNotification
    ) {
        playTrial();
        EXPECT_TRUE(targetPlayer.playbackCompletionSubscribedTo());
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialSeeksToRandomMaskerPositionWithinTrialDuration
    ) {
        targetPlayer.setDurationSeconds(1);
        maskerPlayer.setFadeTimeSeconds(2);
        maskerPlayer.setDurationSeconds(3);
        playTrial();
        EXPECT_EQ(0., randomizer.lowerFloatBound());
        EXPECT_EQ(3 - 2 - 1 - 2, randomizer.upperFloatBound());
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialSeeksToRandomMaskerPosition
    ) {
        randomizer.setRandomFloat(1);
        playTrial();
        EXPECT_EQ(1, maskerPlayer.secondsSeeked());
    }

    TEST_F(
        RecognitionTestModelTests,
        initializeTestSetsInitialMaskerPlayerLevel
    ) {
        initializingTest.setMaskerLevel_dB_SPL(1);
        initializingTest.setFullScaleLevel_dB_SPL(2);
        maskerPlayer.setRms(3);
        initializeTest();
        EXPECT_EQ(1 - 2 - dB(3), maskerPlayer.level_dB());
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialSetsTargetPlayerLevel
    ) {
        snrTrack.setX(1);
        initializingTest.setMaskerLevel_dB_SPL(2);
        initializingTest.setFullScaleLevel_dB_SPL(3);
        setTargetPlayerRms(4);
        initializeTest();
        playTrial();
        EXPECT_EQ(1 + 2 - 3 - dB(4), targetPlayerLevel_dB());
    }

    TEST_F(
        RecognitionTestModelTests,
        playTrialSetsTargetPlayerLevel_2
    ) {
        initializingTest.setMaskerLevel_dB_SPL(2);
        initializingTest.setFullScaleLevel_dB_SPL(3);
        setTargetPlayerRms(4);
        initializeTestWithStartingList(5);
        snrTracks.at(5)->setX(1);
        playTrial();
        EXPECT_EQ(1 + 2 - 3 - dB(4), targetPlayerLevel_dB());
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
        EXPECT_TRUE(targetPlayer.played());
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
        submitResponseWritesColor
    ) {
        subjectResponse.color = blueColor();
        submitResponse();
        EXPECT_EQ(blueColor(), trialWritten().subjectColor);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponseWritesNumber
    ) {
        subjectResponse.number = 1;
        submitResponse();
        EXPECT_EQ(1, trialWritten().subjectNumber);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponseWritesReversals
    ) {
        snrTrack.setReversals(1);
        submitResponse();
        EXPECT_EQ(1, trialWritten().reversals);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponseWritesCorrectColor
    ) {
        evaluator.setCorrectColor(blueColor());
        submitResponse();
        EXPECT_EQ(blueColor(), trialWritten().correctColor);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponseWritesCorrectNumber
    ) {
        evaluator.setCorrectNumber(1);
        submitResponse();
        EXPECT_EQ(1, trialWritten().correctNumber);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponseWritesSnr
    ) {
        snrTrack.setX(1);
        submitResponse();
        EXPECT_EQ(1, trialWritten().SNR_dB);
    }

    TEST_F(
        RecognitionTestModelTests,
        correctEvaluationPassedToWrite
    ) {
        evaluator.setCorrect();
        submitResponse();
        EXPECT_TRUE(trialWritten().correct);
    }

    TEST_F(
        RecognitionTestModelTests,
        incorrectEvaluationPassedToWrite
    ) {
        evaluator.setIncorrect();
        submitResponse();
        EXPECT_FALSE(trialWritten().correct);
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponsePassesTargetToEvaluatorForNumberAndColor
    ) {
        targetList.setCurrent("a");
        submitResponse();
        assertEqual("a", evaluator.correctColorFilePath());
        assertEqual("a", evaluator.correctNumberFilePath());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponsePassesTargetToEvaluator
    ) {
        targetList.setCurrent("a");
        submitResponse();
        assertEqual("a", evaluator.correctFilePath());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponsePushesSnrDown
    ) {
        evaluator.setCorrect();
        submitResponse();
        EXPECT_TRUE(snrTrackPushedDown());
        EXPECT_FALSE(snrTrackPushedUp());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponsePushesSnrUp
    ) {
        evaluator.setIncorrect();
        submitResponse();
        EXPECT_TRUE(snrTrackPushedUp());
        EXPECT_FALSE(snrTrackPushedDown());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitCorrectResponsePushesSnrDown_2
    ) {
        initializeTestWithStartingList(1);
        evaluator.setCorrect();
        submitResponse();
        EXPECT_TRUE(snrTracks.at(1)->pushedDown());
        EXPECT_FALSE(snrTracks.at(1)->pushedUp());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitIncorrectResponsePushesSnrUp_2
    ) {
        initializeTestWithStartingList(1);
        evaluator.setIncorrect();
        submitResponse();
        EXPECT_TRUE(snrTracks.at(1)->pushedUp());
        EXPECT_FALSE(snrTracks.at(1)->pushedDown());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponseSelectsNextListAmongThoseWithIncompleteTracks
    ) {
        initializeTestWithListCount(3);
        snrTracks.at(1)->setComplete();
        selectList(1);
        submitResponse();
        evaluator.setIncorrect();
        submitResponse();
        EXPECT_TRUE(snrTracks.at(2)->pushedUp());
        EXPECT_FALSE(snrTracks.at(2)->pushedDown());
    }

    TEST_F(
        RecognitionTestModelTests,
        submitResponsePassesSubjectResponseToEvaluator
    ) {
        submitResponse();
        EXPECT_EQ(&subjectResponse, evaluator.response());
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
        playTrialWithInvalidAudioDeviceDoesNotAdvanceTarget
    ) {
        throwInvalidAudioDeviceWhenSet();
        playTrialIgnoringFailure();
        assertListNotAdvanced();
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
        playTrialDoesNotAdvanceListIfTrialInProgress
    ) {
        playTrialWhenTrialAlreadyInProgressIgnoringFailure();
        assertListNotAdvanced();
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
        testCompleteWhenTrackComplete
    ) {
        snrTrack.setComplete();
        EXPECT_TRUE(model.testComplete());
    }
}

