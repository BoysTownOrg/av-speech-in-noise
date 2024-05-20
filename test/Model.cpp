#include "MaskerPlayerStub.hpp"
#include "OutputFileStub.hpp"
#include "RandomizerStub.hpp"
#include "assert-utility.hpp"

#include <av-speech-in-noise/core/SubmittingKeyPress.hpp>
#include <av-speech-in-noise/core/SubmittingConsonant.hpp>
#include <av-speech-in-noise/core/SubmittingFreeResponse.hpp>
#include <av-speech-in-noise/core/SubmittingPassFail.hpp>
#include <av-speech-in-noise/core/SubmittingKeywords.hpp>
#include <av-speech-in-noise/core/SubmittingNumberKeywords.hpp>
#include <av-speech-in-noise/core/SubmittingSyllable.hpp>

#include <gtest/gtest.h>

#include <sstream>
#include <utility>

namespace av_speech_in_noise {
static auto operator==(const AdaptiveTestResult &a, const AdaptiveTestResult &b)
    -> bool {
    return a.targetsUrl.path == b.targetsUrl.path && a.threshold == b.threshold;
}

namespace {
class AdaptiveMethodStub : public AdaptiveMethod {
  public:
    [[nodiscard]] auto tracksResetted() const -> bool {
        return tracksResetted_;
    }

    void initialize(
        const AdaptiveTest &t, TargetPlaylistReader *reader) override {
        test_ = &t;
        targetListReader_ = reader;
    }

    [[nodiscard]] auto test() const { return test_; }

    [[nodiscard]] auto targetListReader() const -> TargetPlaylistReader * {
        return targetListReader_;
    }

    void setTestResults(AdaptiveTestResults v) { testResults_ = std::move(v); }

    auto testResults() -> AdaptiveTestResults override { return testResults_; }

    void resetTracks() override { tracksResetted_ = true; }

    auto log() -> const std::stringstream & { return log_; }

    void log(const std::string &s) { log_ << s; }

    auto complete() -> bool override { return {}; }
    auto nextTarget() -> LocalUrl override {
        log_ << "nextTarget ";
        return {};
    }
    auto currentTarget() -> LocalUrl override { return {}; }
    auto snr() -> SNR override { return SNR{}; }
    void submitCorrectResponse() override { log_ << "submitCorrectResponse "; }
    void submitIncorrectResponse() override {
        log_ << "submitIncorrectResponse ";
    }
    void submit(const CorrectKeywords &) override {
        log_ << "submitCorrectKeywords ";
    }
    void writeTestingParameters(OutputFile &) override {}
    void writeLastCoordinateResponse(OutputFile &) override {}
    void writeLastCorrectResponse(OutputFile &) override {
        log_ << "writeLastCorrectResponse ";
    }
    void writeLastIncorrectResponse(OutputFile &) override {
        log_ << "writeLastIncorrectResponse ";
    }
    void writeLastCorrectKeywords(OutputFile &) override {
        log_ << "writeLastCorrectKeywords ";
    }
    void writeTestResult(OutputFile &) override {}
    void submit(const coordinate_response_measure::Response &) override {}

  private:
    AdaptiveTestResults testResults_;
    std::stringstream log_{};
    const AdaptiveTest *test_{};
    TargetPlaylistReader *targetListReader_{};
    bool tracksResetted_{};
};

class FixedLevelMethodStub : public FixedLevelMethod {
  public:
    void initialize(
        const FixedLevelFixedTrialsTest &t, TargetPlaylist *list) override {
        targetList_ = list;
        fixedTrialsTest_ = &t;
    }

    void initialize(const FixedLevelTest &t,
        FiniteTargetPlaylistWithRepeatables *list) override {
        targetList_ = list;
        test_ = &t;
    }

    void initialize(
        const FixedLevelTest &t, FiniteTargetPlaylist *list) override {
        targetList_ = list;
        test_ = &t;
    }

    [[nodiscard]] auto targetList() const { return targetList_; }

    [[nodiscard]] auto test() const { return test_; }

    auto fixedTrialsTest() -> const FixedLevelFixedTrialsTest * {
        return fixedTrialsTest_;
    }

    auto submittedConsonant() const -> bool { return submittedConsonant_; }

    auto log() -> const std::stringstream & { return log_; }

    void log(const std::string &s) { log_ << s; }

    auto complete() -> bool override { return {}; }

    auto nextTarget() -> LocalUrl override {
        log_ << "nextTarget ";
        return {};
    }

    auto currentTarget() -> LocalUrl override { return currentTarget_; }

    void setCurrentTargetPath(std::string s) {
        currentTarget_.path = std::move(s);
    }

    auto snr() -> SNR override { return SNR{}; }

    void submit(const Flaggable &) override { submittedFlaggable = true; }

    void submit(const ConsonantResponse &) override {
        submittedConsonant_ = true;
        log_ << "submitConsonant ";
    }

    void writeTestingParameters(OutputFile &) override {}

    void writeLastCoordinateResponse(OutputFile &) override {}

    void writeLastConsonant(OutputFile &) override {
        log_ << "writeLastConsonant ";
    }

    void writeTestResult(OutputFile &) override {}

    void submit(const coordinate_response_measure::Response &) override {}

    auto keywordsTestResults() -> KeywordsTestResults override {
        return keywordsTestResults_;
    }

    void setKeywordsTestResults(const KeywordsTestResults &r) {
        keywordsTestResults_ = r;
    }

    void submit(const ThreeKeywordsResponse &r) override {
        threeKeywords_ = &r;
    }

    auto threeKeywords() -> const ThreeKeywordsResponse * {
        return threeKeywords_;
    }

    bool submittedFlaggable{};

  private:
    KeywordsTestResults keywordsTestResults_{};
    std::stringstream log_{};
    LocalUrl currentTarget_;
    const ThreeKeywordsResponse *threeKeywords_{};
    const FixedLevelTest *test_{};
    const FixedLevelFixedTrialsTest *fixedTrialsTest_{};
    TargetPlaylist *targetList_{};
    bool submittedConsonant_{};
};

class RunningATestStub : public RunningATest {
  public:
    explicit RunningATestStub(AdaptiveMethodStub &adaptiveMethod,
        FixedLevelMethodStub &fixedLevelMethodStub)
        : adaptiveMethod{adaptiveMethod},
          fixedLevelMethodStub{fixedLevelMethodStub} {}

    [[nodiscard]] auto nextTrialPreparedIfNeeded() const -> bool {
        return nextTrialPreparedIfNeeded_;
    }

    void prepareNextTrialIfNeeded() override {
        nextTrialPreparedIfNeeded_ = true;
        fixedLevelMethodStub.setCurrentTargetPath("TOOLATE");
        adaptiveMethod.log("TOOLATE ");
        fixedLevelMethodStub.log("TOOLATE ");
    }

    void initialize(TestMethod *method, const Test &test,
        std::vector<std::reference_wrapper<TestObserver>> observer) override {
        testMethod_ = method;
        test_ = &test;
        this->observer = observer;
    }

    auto trialNumber() -> int override { return trialNumber_; }

    void setTrialNumber(int n) { trialNumber_ = n; }

    void setTargetFileName(std::string s) { targetFileName_ = std::move(s); }

    auto targetFileName() -> std::string override { return targetFileName_; }

    void playTrial(const AudioSettings &s) override { playTrialSettings_ = &s; }

    void submit(const coordinate_response_measure::Response &p) override {
        coordinateResponse_ = &p;
    }

    auto testComplete() -> bool override { return complete_; }

    auto audioDevices() -> std::vector<std::string> override {
        return audioDevices_;
    }

    void attach(RunningATest::Observer *e) override { listener_ = e; }

    void playCalibration(const Calibration &c) override { calibration_ = &c; }

    void playLeftSpeakerCalibration(const Calibration &c) override {
        leftSpeakerCalibration_ = &c;
    }

    void playRightSpeakerCalibration(const Calibration &c) override {
        rightSpeakerCalibration_ = &c;
    }

    [[nodiscard]] auto coordinateResponse() const {
        return coordinateResponse_;
    }

    [[nodiscard]] auto testMethod() const { return testMethod_; }

    [[nodiscard]] auto test() const { return test_; }

    [[nodiscard]] auto playTrialSettings() const { return playTrialSettings_; }

    [[nodiscard]] auto calibration() const { return calibration_; }

    auto leftSpeakerCalibration() -> const Calibration * {
        return leftSpeakerCalibration_;
    }

    auto rightSpeakerCalibration() -> const Calibration * {
        return rightSpeakerCalibration_;
    }

    [[nodiscard]] auto listener() const { return listener_; }

    void setComplete() { complete_ = true; }

    void setAudioDevices(std::vector<std::string> v) {
        audioDevices_ = std::move(v);
    }

    auto playTrialTime() -> std::string override { return playTrialTime_; }

    void setPlayTrialTime(std::string s) { playTrialTime_ = std::move(s); }

    std::vector<std::reference_wrapper<TestObserver>> observer{};

  private:
    std::vector<std::string> audioDevices_{};
    std::string targetFileName_{};
    std::string playTrialTime_;
    AdaptiveMethodStub &adaptiveMethod;
    FixedLevelMethodStub &fixedLevelMethodStub;
    const RunningATest::Observer *listener_{};
    const Calibration *calibration_{};
    const Calibration *leftSpeakerCalibration_{};
    const Calibration *rightSpeakerCalibration_{};
    const AudioSettings *playTrialSettings_{};
    const Test *test_{};
    const TestMethod *testMethod_{};
    const coordinate_response_measure::Response *coordinateResponse_{};
    int trialNumber_{};
    bool complete_{};
    bool nextTrialPreparedIfNeeded_{};
};

class SubmittingFreeResponseTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub adaptiveTestMethod;
    FixedLevelMethodStub testMethod;
    RunningATestStub model{adaptiveTestMethod, testMethod};
    OutputFileStub outputFile;
    submitting_free_response::InteractorImpl interactor{
        testMethod, model, outputFile};
    FreeResponse freeResponse;
};

class SubmittingConsonantTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub adaptiveTestMethod;
    FixedLevelMethodStub testMethod;
    RunningATestStub model{adaptiveTestMethod, testMethod};
    OutputFileStub outputFile;
    submitting_consonant::InteractorImpl interactor{
        testMethod, model, outputFile};
    ConsonantResponse response;
};

class SubmittingPassFailTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub testMethod;
    FixedLevelMethodStub fixedLevelMethod;
    RunningATestStub model{testMethod, fixedLevelMethod};
    OutputFileStub outputFile;
    submitting_pass_fail::InteractorImpl interactor{
        testMethod, model, outputFile};
};

class SubmittingKeywordsTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub testMethod;
    FixedLevelMethodStub fixedLevelMethod;
    RunningATestStub model{testMethod, fixedLevelMethod};
    OutputFileStub outputFile;
    submitting_keywords::InteractorImpl interactor{
        fixedLevelMethod, model, outputFile};
    ThreeKeywordsResponse threeKeywords;
};

class SubmittingNumberKeywordsTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub testMethod;
    FixedLevelMethodStub fixedLevelMethod;
    RunningATestStub model{testMethod, fixedLevelMethod};
    OutputFileStub outputFile;
    submitting_number_keywords::InteractorImpl interactor{
        testMethod, model, outputFile};
    CorrectKeywords correctKeywords;
};

class SubmittingSyllableTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub testMethod;
    FixedLevelMethodStub fixedLevelMethod;
    RunningATestStub model{testMethod, fixedLevelMethod};
    OutputFileStub outputFile;
    submitting_syllable::InteractorImpl interactor{
        fixedLevelMethod, model, outputFile};
    SyllableResponse syllableResponse;
};

class SubmittingKeypressTests : public ::testing::Test {
  protected:
    AdaptiveMethodStub adaptiveTestMethod;
    FixedLevelMethodStub testMethod;
    RunningATestStub model{adaptiveTestMethod, testMethod};
    OutputFileStub outputFile;
    MaskerPlayerStub maskerPlayer;
    RandomizerStub randomizer;
    submitting_keypress::InteractorImpl interactor{
        testMethod, model, outputFile, maskerPlayer, randomizer};
    KeyPressResponse response;
};

#define SUBMITTING_FREE_RESPONSE_TEST(a) TEST_F(SubmittingFreeResponseTests, a)

#define SUBMITTING_PASS_FAIL_TEST(a) TEST_F(SubmittingPassFailTests, a)

#define SUBMITTING_KEYWORDS_TEST(a) TEST_F(SubmittingKeywordsTests, a)

#define SUBMITTING_CONSONANT_TEST(a) TEST_F(SubmittingConsonantTests, a)

#define SUBMITTING_KEYPRESS_TEST(a) TEST_F(SubmittingKeypressTests, a)

#define SUBMITTING_NUMBER_KEYWORDS_TEST(a)                                     \
    TEST_F(SubmittingNumberKeywordsTests, a)

#define SUBMITTING_SYLLABLE(a) TEST_F(SubmittingSyllableTests, a)

SUBMITTING_FREE_RESPONSE_TEST(submittingFreeResponsePreparesNextTrialIfNeeded) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_FREE_RESPONSE_TEST(
    submitFreeResponseSavesOutputFileAfterWritingTrial) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_FREE_RESPONSE_TEST(submitFreeResponseWritesResponse) {
    freeResponse.response = "a";
    interactor.submit(freeResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, outputFile.freeResponseTrial().response);
}

SUBMITTING_FREE_RESPONSE_TEST(submitFreeResponseWritesFlagged) {
    freeResponse.flagged = true;
    interactor.submit(freeResponse);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputFile.freeResponseTrial().flagged);
}

SUBMITTING_FREE_RESPONSE_TEST(submitFreeResponseWritesWithoutFlag) {
    interactor.submit(freeResponse);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(outputFile.freeResponseTrial().flagged);
}

SUBMITTING_FREE_RESPONSE_TEST(submitWritesTime) {
    model.setPlayTrialTime("a");
    interactor.submit(freeResponse);
    assertEqual("a", outputFile.freeResponseTrial().time);
}

SUBMITTING_FREE_RESPONSE_TEST(submitFreeResponseWritesTarget) {
    testMethod.setCurrentTargetPath("a/b/c.txt");
    interactor.submit(freeResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c.txt"}, outputFile.freeResponseTrial().target);
}

SUBMITTING_FREE_RESPONSE_TEST(submitFreeResponseSubmitsResponse) {
    interactor.submit(freeResponse);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testMethod.submittedFlaggable);
}

SUBMITTING_PASS_FAIL_TEST(
    submitCorrectResponseWritesTrialAfterSubmittingResponse) {
    interactor.submitCorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(contains(
        testMethod.log(), "submitCorrectResponse writeLastCorrectResponse "));
}

SUBMITTING_PASS_FAIL_TEST(
    submitIncorrectResponseWritesTrialAfterSubmittingResponse) {
    interactor.submitIncorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(contains(testMethod.log(),
        "submitIncorrectResponse writeLastIncorrectResponse "));
}

SUBMITTING_PASS_FAIL_TEST(
    submitCorrectResponseQueriesNextTargetAfterWritingResponse) {
    interactor.submitCorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(testMethod.log(), "writeLastCorrectResponse TOOLATE "));
}

SUBMITTING_PASS_FAIL_TEST(
    submitIncorrectResponseQueriesNextTargetAfterWritingResponse) {
    interactor.submitIncorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(testMethod.log(), "writeLastIncorrectResponse TOOLATE "));
}

SUBMITTING_PASS_FAIL_TEST(submittingCorrectPreparesNextTrialIfNeeded) {
    interactor.submitCorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_PASS_FAIL_TEST(submittingIncorrectPreparesNextTrialIfNeeded) {
    interactor.submitIncorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_PASS_FAIL_TEST(submitCorrectSavesOutputFileAfterWritingTrial) {
    interactor.submitCorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_PASS_FAIL_TEST(submitIncorrectSavesOutputFileAfterWritingTrial) {
    interactor.submitIncorrectResponse();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_KEYWORDS_TEST(preparesNextTrialIfNeeded) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_KEYWORDS_TEST(savesOutputFileAfterWritingTrial) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_KEYWORDS_TEST(writesFlagged) {
    threeKeywords.flagged = true;
    interactor.submit(threeKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputFile.threeKeywordsTrial().flagged);
}

SUBMITTING_KEYWORDS_TEST(writesWithoutFlag) {
    interactor.submit(threeKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(outputFile.threeKeywordsTrial().flagged);
}

SUBMITTING_KEYWORDS_TEST(writesTarget) {
    fixedLevelMethod.setCurrentTargetPath("a/b/c.txt");
    interactor.submit(threeKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c.txt"}, outputFile.threeKeywordsTrial().target);
}

SUBMITTING_KEYWORDS_TEST(writesEachKeywordEvaluation) {
    threeKeywords.firstCorrect = true;
    threeKeywords.secondCorrect = false;
    threeKeywords.thirdCorrect = true;
    interactor.submit(threeKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        outputFile.threeKeywordsTrial().firstCorrect);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(
        outputFile.threeKeywordsTrial().secondCorrect);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        outputFile.threeKeywordsTrial().thirdCorrect);
}

SUBMITTING_KEYWORDS_TEST(submitThreeKeywordsSubmitsToTestMethod) {
    interactor.submit(threeKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &threeKeywords, fixedLevelMethod.threeKeywords());
}

SUBMITTING_NUMBER_KEYWORDS_TEST(preparesNextTrialIfNeeded) {
    interactor.submit(correctKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_NUMBER_KEYWORDS_TEST(writesTrialAfterSubmittingResponse) {
    interactor.submit(correctKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(contains(
        testMethod.log(), "submitCorrectKeywords writeLastCorrectKeywords "));
}

SUBMITTING_NUMBER_KEYWORDS_TEST(queriesNextTargetAfterWritingResponse) {
    interactor.submit(correctKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(testMethod.log(), "writeLastCorrectKeywords TOOLATE "));
}

SUBMITTING_NUMBER_KEYWORDS_TEST(savesOutputFileAfterWritingTrial) {
    interactor.submit(correctKeywords);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_SYLLABLE(preparesNextTrialIfNeeded) {
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_SYLLABLE(savesOutputFileAfterWritingTrial) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_SYLLABLE(writesSubjectSyllable) {
    syllableResponse.syllable = Syllable::gi;
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Syllable::gi, outputFile.syllableTrial().subjectSyllable);
}

SUBMITTING_SYLLABLE(writesCorrectSyllable) {
    fixedLevelMethod.setCurrentTargetPath("a/b/say_vi_2-25ao.mov");
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        Syllable::vi, outputFile.syllableTrial().correctSyllable);
}

SUBMITTING_SYLLABLE(writesFlagged) {
    syllableResponse.flagged = true;
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputFile.syllableTrial().flagged);
}

SUBMITTING_SYLLABLE(writesTarget) {
    fixedLevelMethod.setCurrentTargetPath("a/b/c.txt");
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c.txt"}, outputFile.syllableTrial().target);
}

SUBMITTING_SYLLABLE(submitsToTestMethod) {
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(fixedLevelMethod.submittedFlaggable);
}

SUBMITTING_SYLLABLE(submitCorrectSyllable) {
    fixedLevelMethod.setCurrentTargetPath("a/b/say_dji_3-25av.mov");
    syllableResponse.syllable = Syllable::dji;
    interactor.submit(syllableResponse);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputFile.syllableTrial().correct);
}

SUBMITTING_CONSONANT_TEST(submitConsonantSubmitsResponse) {
    ConsonantResponse r;
    interactor.submit(r);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testMethod.submittedConsonant());
}

SUBMITTING_CONSONANT_TEST(submitConsonantWritesTrialAfterSubmittingResponse) {
    ConsonantResponse r;
    interactor.submit(r);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(testMethod.log(), "submitConsonant writeLastConsonant "));
}

SUBMITTING_CONSONANT_TEST(queriesNextTargetAfterWritingResponse) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(testMethod.log(), "writeLastConsonant TOOLATE "));
}

SUBMITTING_CONSONANT_TEST(preparesNextTrialIfNeeded) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_CONSONANT_TEST(savesOutputFileAfterWritingTrial) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_KEYPRESS_TEST(savesReactionTime) {
    maskerPlayer.setNanosecondsFromPlayerTime(123456789);
    PlayerTimeWithDelay playerTime;
    playerTime.delay.seconds = .4;
    interactor.notifyThatTargetWillPlayAt(playerTime);
    response.seconds = .9;
    interactor.submit(response);
    const auto start{(123456789 + 400000000 + 500000) / 1000000};
    const auto end{900};
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        end - start, outputFile.keypressTrial.rt.milliseconds);
}

SUBMITTING_KEYPRESS_TEST(preparesNextTrialIfNeeded) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(model.nextTrialPreparedIfNeeded());
}

SUBMITTING_KEYPRESS_TEST(savesOutputFileAfterWritingTrial) {
    interactor.submit({});
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(outputFile.log(), "save "));
}

SUBMITTING_KEYPRESS_TEST(submitFreeResponseWritesResponse) {
    response.key = KeyPressed::second;
    interactor.submit(response);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        KeyPressed::second, outputFile.keypressTrial.key);
}

SUBMITTING_KEYPRESS_TEST(submitsResponseToTestMethod) {
    interactor.submit(response);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(testMethod.submittedFlaggable);
}

SUBMITTING_KEYPRESS_TEST(writesTarget) {
    testMethod.setCurrentTargetPath("a/b/c.txt");
    interactor.submit(response);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c.txt"}, outputFile.keypressTrial.target);
}

SUBMITTING_KEYPRESS_TEST(selectsRandomVibrotactileStimulus) {
    randomizer.randomInts.push(3); // 60, 110, 160, 190
    randomizer.randomInts.push(1); // 100, 250
    interactor.notifyThatTrialWillBegin({});
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        0.190 + RunningATest::targetOnsetFringeDuration.seconds,
        maskerPlayer.vibrotactileStimulus.delay.seconds);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        0.250, maskerPlayer.vibrotactileStimulus.duration.seconds);
}
}
}
