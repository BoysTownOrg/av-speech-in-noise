#include "OutputFileStub.hpp"
#include "RandomizerStub.hpp"
#include "ResponseEvaluatorStub.hpp"
#include "TargetListSetReaderStub.hpp"
#include "TargetListStub.hpp"
#include "TrackStub.hpp"
#include "assert-utility.hpp"
#include <recognition-test/AdaptiveMethod.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <algorithm>
#include <functional>

namespace av_speech_in_noise {
static auto operator==(const AdaptiveTestResult &a, const AdaptiveTestResult &b)
    -> bool {
    return a.targetListDirectory == b.targetListDirectory &&
        a.threshold == b.threshold;
}

namespace {
class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(AdaptiveMethodImpl &) = 0;
};

void initialize(AdaptiveMethodImpl &method, const AdaptiveTest &test,
    TargetListReader &targetListReader) {
    method.initialize(test, &targetListReader);
}

class Initializing : public UseCase {
  public:
    explicit Initializing(TargetListReader &reader) : reader{reader} {}

    void run(AdaptiveMethodImpl &method) override {
        initialize(method, test, reader);
    }

  private:
    AdaptiveTest test{};
    TargetListReader &reader;
};

void submit(AdaptiveMethodImpl &method,
    const coordinate_response_measure::Response &response) {
    method.submit(response);
}

class SubmittingCoordinateResponse : public UseCase {
    coordinate_response_measure::Response response{};

  public:
    void run(AdaptiveMethodImpl &method) override { submit(method, response); }
};

class SubmittingCorrectCoordinateResponse : public UseCase {
    coordinate_response_measure::Response response{};
    ResponseEvaluatorStub &evaluator;

  public:
    explicit SubmittingCorrectCoordinateResponse(
        ResponseEvaluatorStub &evaluator)
        : evaluator{evaluator} {}

    void run(AdaptiveMethodImpl &method) override {
        evaluator.setCorrect();
        submit(method, response);
    }
};

class SubmittingIncorrectCoordinateResponse : public UseCase {
    coordinate_response_measure::Response response{};
    ResponseEvaluatorStub &evaluator;

  public:
    explicit SubmittingIncorrectCoordinateResponse(
        ResponseEvaluatorStub &evaluator)
        : evaluator{evaluator} {}

    void run(AdaptiveMethodImpl &method) override {
        evaluator.setIncorrect();
        submit(method, response);
    }
};

void submitCorrectResponse(AdaptiveMethodImpl &method) {
    method.submitCorrectResponse();
}

void submitIncorrectResponse(AdaptiveMethodImpl &method) {
    method.submitIncorrectResponse();
}

class SubmittingCorrectResponse : public UseCase {
  public:
    void run(AdaptiveMethodImpl &method) override {
        submitCorrectResponse(method);
    }
};

void submit(AdaptiveMethodImpl &method, CorrectKeywords &keywords) {
    method.submit(keywords);
}

class SubmittingSufficientCorrectKeywords : public UseCase {
    CorrectKeywords correctKeywords{};

  public:
    void run(AdaptiveMethodImpl &method) override {
        correctKeywords.count = 2;
        submit(method, correctKeywords);
    }
};

class SubmittingInsufficientCorrectKeywords : public UseCase {
    CorrectKeywords correctKeywords{};

  public:
    void run(AdaptiveMethodImpl &method) override {
        correctKeywords.count = 1;
        submit(method, correctKeywords);
    }
};

class SubmittingIncorrectResponse : public UseCase {
  public:
    void run(AdaptiveMethodImpl &method) override {
        submitIncorrectResponse(method);
    }
};

class SubmittingCorrectKeywords : public UseCase {
    CorrectKeywords correctKeywords{};

  public:
    void run(AdaptiveMethodImpl &method) override {
        submit(method, correctKeywords);
    }
};

class WritingResponseUseCase : public virtual UseCase {
  public:
    virtual auto reversals(OutputFileStub &) -> int = 0;
    virtual auto snr(OutputFileStub &) -> int = 0;
};

class WritingTargetUseCase : public virtual UseCase {
  public:
    virtual auto target(OutputFileStub &) -> std::string = 0;
};

auto adaptiveCoordinateResponseTrial(const OutputFileStub &file)
    -> const coordinate_response_measure::AdaptiveTrial & {
    return file.adaptiveCoordinateResponseTrial();
}

class WritingCoordinateResponse : public WritingResponseUseCase {
    coordinate_response_measure::Response response{};
    OutputFile &file_;

  public:
    explicit WritingCoordinateResponse(OutputFile &file_) : file_{file_} {}

    void run(AdaptiveMethodImpl &method) override {
        method.submit(response);
        method.writeLastCoordinateResponse(file_);
    }

    auto reversals(OutputFileStub &file) -> int override {
        return adaptiveCoordinateResponseTrial(file).reversals;
    }

    auto snr(OutputFileStub &file) -> int override {
        return adaptiveCoordinateResponseTrial(file).SNR_dB;
    }
};

auto openSetAdaptiveTrial(OutputFileStub &file)
    -> const open_set::AdaptiveTrial & {
    return file.openSetAdaptiveTrial();
}

auto openSetReversals(OutputFileStub &file) -> int {
    return openSetAdaptiveTrial(file).reversals;
}

auto openSetSnr(OutputFileStub &file) -> int {
    return openSetAdaptiveTrial(file).SNR_dB;
}

auto openSetTarget(OutputFileStub &file) -> std::string {
    return openSetAdaptiveTrial(file).target;
}

class WritingCorrectResponse : public WritingResponseUseCase,
                               public WritingTargetUseCase {
    OutputFile &file_;

  public:
    explicit WritingCorrectResponse(OutputFile &file_) : file_{file_} {}

    void run(AdaptiveMethodImpl &method) override {
        submitCorrectResponse(method);
        method.writeLastCorrectResponse(file_);
    }

    auto reversals(OutputFileStub &file) -> int override {
        return openSetReversals(file);
    }

    auto snr(OutputFileStub &file) -> int override { return openSetSnr(file); }

    auto target(OutputFileStub &file) -> std::string override {
        return openSetTarget(file);
    }
};

class WritingIncorrectResponse : public WritingResponseUseCase,
                                 public WritingTargetUseCase {
    OutputFile &file_;

  public:
    explicit WritingIncorrectResponse(OutputFile &file_) : file_{file_} {}

    void run(AdaptiveMethodImpl &method) override {
        submitIncorrectResponse(method);
        method.writeLastIncorrectResponse(file_);
    }

    auto reversals(OutputFileStub &file) -> int override {
        return openSetReversals(file);
    }

    auto snr(OutputFileStub &file) -> int override { return openSetSnr(file); }

    auto target(OutputFileStub &file) -> std::string override {
        return openSetTarget(file);
    }
};

auto correctKeywordsTrial(OutputFileStub &file)
    -> const CorrectKeywordsTrial & {
    return file.correctKeywordsTrial();
}

class WritingCorrectKeywords : public WritingResponseUseCase,
                               public WritingTargetUseCase {
    OutputFile &file_;

  public:
    explicit WritingCorrectKeywords(OutputFile &file_) : file_{file_} {}

    void run(AdaptiveMethodImpl &method) override {
        CorrectKeywords correctKeywords{};
        method.submit(correctKeywords);
        method.writeLastCorrectKeywords(file_);
    }

    auto reversals(OutputFileStub &file) -> int override {
        return correctKeywordsTrial(file).reversals;
    }

    auto snr(OutputFileStub &file) -> int override {
        return correctKeywordsTrial(file).SNR_dB;
    }

    auto target(OutputFileStub &file) -> std::string override {
        return correctKeywordsTrial(file).target;
    }
};

void resetTracks(AdaptiveMethodImpl &method) { method.resetTracks(); }

void assertStartingXEqualsOne(const Track::Settings &s) {
    assertEqual(1, s.startingX);
}

void assertCeilingEqualsOne(const Track::Settings &s) {
    assertEqual(1, s.ceiling);
}

void assertFloorEqualsOne(const Track::Settings &s) { assertEqual(1, s.floor); }

void assertBumpLimitEqualsOne(const Track::Settings &s) {
    assertEqual(1, s.bumpLimit);
}

void assertTargetLevelRuleEquals(
    const TrackingRule &rule, const Track::Settings &s) {
    assertEqual(&rule, s.rule);
}

void write(AdaptiveMethodImpl &method,
    const coordinate_response_measure::Response &response,
    OutputFile &outputFile) {
    submit(method, response);
    method.writeLastCoordinateResponse(outputFile);
}

auto settings(const TrackFactoryStub &factory) -> std::vector<Track::Settings> {
    return factory.parameters();
}

auto settings(const TrackFactoryStub &factory, gsl::index i)
    -> Track::Settings {
    return settings(factory).at(i);
}

constexpr auto listCount{3};

void forEachSettings(const TrackFactoryStub &factory,
    const std::function<void(const Track::Settings &)> &f) {
    for (int i = 0; i < listCount; ++i)
        f(settings(factory, i));
}

void setNext(const std::vector<std::shared_ptr<TargetListStub>> &lists,
    gsl::index n, std::string s) {
    lists.at(n)->setNext(std::move(s));
}

void setCurrent(const std::vector<std::shared_ptr<TargetListStub>> &lists,
    gsl::index n, std::string s) {
    lists.at(n)->setCurrent(std::move(s));
}

auto at(const std::vector<std::shared_ptr<TrackStub>> &tracks, gsl::index n)
    -> const std::shared_ptr<TrackStub> & {
    return tracks.at(n);
}

auto complete(AdaptiveMethodImpl &method) -> bool { return method.complete(); }

void assertComplete(AdaptiveMethodImpl &method) {
    assertTrue(complete(method));
}

void assertIncomplete(AdaptiveMethodImpl &method) {
    assertFalse(complete(method));
}

auto pushedDown(const std::vector<std::shared_ptr<TrackStub>> &tracks,
    gsl::index n) -> bool {
    return at(tracks, n)->pushedDown();
}

auto pushedUp(const std::vector<std::shared_ptr<TrackStub>> &tracks,
    gsl::index n) -> bool {
    return at(tracks, n)->pushedUp();
}

void setComplete(
    const std::vector<std::shared_ptr<TrackStub>> &tracks, gsl::index n) {
    at(tracks, n)->setComplete();
}

void selectNextList(RandomizerStub &randomizer, int n) {
    randomizer.setRandomInt(n);
}

void assertNextTargetEquals(AdaptiveMethodImpl &method, const std::string &s) {
    assertEqual(s, method.nextTarget());
}

auto adaptiveTestResult(OutputFileStub &file) -> AdaptiveTestResults {
    return file.adaptiveTestResult();
}

void assertPassedIntegerBounds(RandomizerStub &randomizer, int a, int b) {
    assertEqual(a, randomizer.lowerIntBound());
    assertEqual(b, randomizer.upperIntBound());
}

auto coordinateResponseTrialCorrect(OutputFileStub &file) -> bool {
    return adaptiveCoordinateResponseTrial(file).correct;
}

constexpr auto blue{coordinate_response_measure::Color::blue};

class AdaptiveMethodTests : public ::testing::Test {
  protected:
    TrackFactoryStub snrTrackFactory;
    ResponseEvaluatorStub evaluator;
    RandomizerStub randomizer;
    AdaptiveMethodImpl method{snrTrackFactory, evaluator, randomizer};
    OutputFileStub outputFile;
    TargetListSetReaderStub targetListReader;
    Initializing initializing{targetListReader};
    SubmittingCoordinateResponse submittingCoordinateResponse;
    SubmittingCorrectCoordinateResponse submittingCorrectCoordinateResponse{
        evaluator};
    SubmittingIncorrectCoordinateResponse submittingIncorrectCoordinateResponse{
        evaluator};
    SubmittingCorrectResponse submittingCorrectResponse;
    SubmittingSufficientCorrectKeywords submittingSufficientCorrectKeywords;
    SubmittingIncorrectResponse submittingIncorrectResponse;
    SubmittingInsufficientCorrectKeywords submittingInsufficientCorrectKeywords;
    SubmittingCorrectKeywords submittingCorrectKeywords;
    WritingCoordinateResponse writingCoordinateResponse{outputFile};
    WritingCorrectResponse writingCorrectResponse{outputFile};
    WritingIncorrectResponse writingIncorrectResponse{outputFile};
    WritingCorrectKeywords writingCorrectKeywords{outputFile};
    AdaptiveTest test{};
    coordinate_response_measure::Response coordinateResponse{};
    CorrectKeywords correctKeywords{};
    std::vector<std::shared_ptr<TargetListStub>> targetLists;
    std::vector<std::shared_ptr<TrackStub>> tracks;

    AdaptiveMethodTests() : targetLists(listCount), tracks(listCount) {
        std::generate(targetLists.begin(), targetLists.end(),
            std::make_shared<TargetListStub>);
        std::generate(
            tracks.begin(), tracks.end(), std::make_shared<TrackStub>);
        targetListReader.setTargetLists(
            {targetLists.begin(), targetLists.end()});
        snrTrackFactory.setTracks({tracks.begin(), tracks.end()});
    }

  public:
    void assertWritesUpdatedReversals(WritingResponseUseCase &useCase) {
        selectNextList(randomizer, 1);
        initialize(method, test, targetListReader);
        at(tracks, 1)->setReversalsWhenUpdated(3);
        selectNextList(randomizer, 2);
        run(useCase);
        assertEqual(3, useCase.reversals(outputFile));
    }

    void assertWritesPreUpdatedSnr(WritingResponseUseCase &useCase) {
        selectNextList(randomizer, 1);
        initialize(method, test, targetListReader);
        at(tracks, 1)->setX(4);
        at(tracks, 1)->setXWhenUpdated(3);
        selectNextList(randomizer, 2);
        run(useCase);
        assertEqual(4, useCase.snr(outputFile));
    }

    void setCorrectCoordinateResponse() { evaluator.setCorrect(); }

    void setIncorrectCoordinateResponse() { evaluator.setIncorrect(); }

    void assertTestIncompleteAfterCoordinateResponse() {
        submit(method, coordinateResponse);
        assertIncomplete(method);
    }

    void assertTestCompleteAfterCoordinateResponse() {
        submit(method, coordinateResponse);
        assertComplete(method);
    }

    void run(UseCase &useCase) { useCase.run(method); }

    void assertSelectsListInRangeAfterRemovingCompleteTracks(UseCase &useCase) {
        initialize(method, test, targetListReader);
        setComplete(tracks, 2);
        run(useCase);
        assertPassedIntegerBounds(randomizer, 0, 1);
    }

    void assertNextTargetEqualsNextFromSelectedTargetListAfter(
        UseCase &useCase) {
        setNext(targetLists, 1, "a");
        selectNextList(randomizer, 1);
        run(useCase);
        assertNextTargetEquals(method, "a");
    }

    void assertPushesSnrTrackDown(UseCase &useCase) {
        selectNextList(randomizer, 1);
        initialize(method, test, targetListReader);
        selectNextList(randomizer, 2);
        run(useCase);
        assertTrue(pushedDown(tracks, 1));
        assertFalse(pushedUp(tracks, 1));
    }

    void assertPushesSnrTrackUp(UseCase &useCase) {
        selectNextList(randomizer, 1);
        initialize(method, test, targetListReader);
        selectNextList(randomizer, 2);
        run(useCase);
        assertFalse(pushedDown(tracks, 1));
        assertTrue(pushedUp(tracks, 1));
    }

    void assertSelectsListAmongThoseWithIncompleteTracks(UseCase &useCase) {
        initialize(method, test, targetListReader);
        setNext(targetLists, 2, "a");
        setComplete(tracks, 1);
        selectNextList(randomizer, 1);
        run(useCase);
        assertNextTargetEquals(method, "a");
    }

    void assertWritesTarget(WritingTargetUseCase &useCase) {
        initialize(method, test, targetListReader);
        evaluator.setFileName("a");
        run(useCase);
        assertEqual("a", useCase.target(outputFile));
    }

    void assertPassesCurrentTargetToEvaluatorForFileName(UseCase &useCase) {
        selectNextList(randomizer, 1);
        initialize(method, test, targetListReader);
        setCurrent(targetLists, 1, "a");
        selectNextList(randomizer, 2);
        run(useCase);
        assertEqual("a", evaluator.filePathForFileName());
    }
};

#define ADAPTIVE_METHOD_TEST(a) TEST_F(AdaptiveMethodTests, a)

ADAPTIVE_METHOD_TEST(initializeCreatesSnrTrackForEachList) {
    initialize(method, test, targetListReader);
    assertEqual(std::size_t{3}, settings(snrTrackFactory).size());
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithTargetLevelRule) {
    initialize(method, test, targetListReader);
    forEachSettings(snrTrackFactory,
        [&](auto s) { assertTargetLevelRuleEquals(test.trackingRule, s); });
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithSnr) {
    test.startingSnr_dB = 1;
    initialize(method, test, targetListReader);
    forEachSettings(snrTrackFactory, assertStartingXEqualsOne);
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithCeiling) {
    test.ceilingSnr_dB = 1;
    initialize(method, test, targetListReader);
    forEachSettings(snrTrackFactory, assertCeilingEqualsOne);
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithFloor) {
    test.floorSnr_dB = 1;
    initialize(method, test, targetListReader);
    forEachSettings(snrTrackFactory, assertFloorEqualsOne);
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithBumpLimit) {
    test.trackBumpLimit = 1;
    initialize(method, test, targetListReader);
    forEachSettings(snrTrackFactory, assertBumpLimitEqualsOne);
}

ADAPTIVE_METHOD_TEST(writeTestParametersPassesToOutputFile) {
    initialize(method, test, targetListReader);
    method.writeTestingParameters(outputFile);
    assertEqual(&std::as_const(test), outputFile.adaptiveTest());
}

ADAPTIVE_METHOD_TEST(initializePassesTargetListDirectory) {
    test.targetListDirectory = "a";
    initialize(method, test, targetListReader);
    assertEqual("a", targetListReader.directory());
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterInitialize) {
    assertNextTargetEqualsNextFromSelectedTargetListAfter(initializing);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterCoordinateResponse) {
    initialize(method, test, targetListReader);
    assertNextTargetEqualsNextFromSelectedTargetListAfter(
        submittingCoordinateResponse);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterCorrectResponse) {
    initialize(method, test, targetListReader);
    assertNextTargetEqualsNextFromSelectedTargetListAfter(
        submittingCorrectResponse);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterIncorrectResponse) {
    initialize(method, test, targetListReader);
    assertNextTargetEqualsNextFromSelectedTargetListAfter(
        submittingIncorrectResponse);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterCorrectKeywords) {
    initialize(method, test, targetListReader);
    assertNextTargetEqualsNextFromSelectedTargetListAfter(
        submittingCorrectKeywords);
}

ADAPTIVE_METHOD_TEST(
    randomizerPassedIntegerBoundsOfListsForSelectingListInRange) {
    initialize(method, test, targetListReader);
    assertPassedIntegerBounds(randomizer, 0, 2);
}

ADAPTIVE_METHOD_TEST(
    submitCoordinateResponseSelectsListInRangeAfterRemovingCompleteTracks) {
    assertSelectsListInRangeAfterRemovingCompleteTracks(
        submittingCoordinateResponse);
}

ADAPTIVE_METHOD_TEST(
    submitCorrectResponseSelectsListInRangeAfterRemovingCompleteTracks) {
    assertSelectsListInRangeAfterRemovingCompleteTracks(
        submittingCorrectResponse);
}

ADAPTIVE_METHOD_TEST(
    submitIncorrectResponseSelectsListInRangeAfterRemovingCompleteTracks) {
    assertSelectsListInRangeAfterRemovingCompleteTracks(
        submittingIncorrectResponse);
}

ADAPTIVE_METHOD_TEST(
    submitCorrectKeywordsSelectsListInRangeAfterRemovingCompleteTracks) {
    assertSelectsListInRangeAfterRemovingCompleteTracks(
        submittingCorrectKeywords);
}

ADAPTIVE_METHOD_TEST(resetTracksResetsEachTrack) {
    initialize(method, test, targetListReader);
    resetTracks(method);
    for (auto &track : tracks)
        assertTrue(track->resetted());
}

ADAPTIVE_METHOD_TEST(snrReturnsThatOfCurrentTrack) {
    at(tracks, 0)->setX(1);
    selectNextList(randomizer, 0);
    initialize(method, test, targetListReader);
    assertEqual(1, method.snr_dB());
}

ADAPTIVE_METHOD_TEST(submitCoordinateResponsePassesCurrentTargetToEvaluator) {
    selectNextList(randomizer, 1);
    initialize(method, test, targetListReader);
    setCurrent(targetLists, 1, "a");
    selectNextList(randomizer, 2);
    submit(method, coordinateResponse);
    assertEqual("a", evaluator.correctColorFilePath());
    assertEqual("a", evaluator.correctNumberFilePath());
}

ADAPTIVE_METHOD_TEST(submitCoordinateResponsePassesCorrectFilePathToEvaluator) {
    selectNextList(randomizer, 1);
    initialize(method, test, targetListReader);
    setCurrent(targetLists, 1, "a");
    selectNextList(randomizer, 2);
    submit(method, coordinateResponse);
    assertEqual("a", evaluator.correctFilePath());
}

ADAPTIVE_METHOD_TEST(submitCoordinateResponsePassesResponseToEvaluator) {
    initialize(method, test, targetListReader);
    submit(method, coordinateResponse);
    assertEqual(&std::as_const(coordinateResponse), evaluator.response());
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesSubjectColor) {
    initialize(method, test, targetListReader);
    coordinateResponse.color = blue;
    write(method, coordinateResponse, outputFile);
    assertEqual(blue, adaptiveCoordinateResponseTrial(outputFile).subjectColor);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesCorrectColor) {
    initialize(method, test, targetListReader);
    evaluator.setCorrectColor(blue);
    write(method, coordinateResponse, outputFile);
    assertEqual(blue, adaptiveCoordinateResponseTrial(outputFile).correctColor);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesSubjectNumber) {
    initialize(method, test, targetListReader);
    coordinateResponse.number = 1;
    write(method, coordinateResponse, outputFile);
    assertEqual(1, adaptiveCoordinateResponseTrial(outputFile).subjectNumber);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesCorrectNumber) {
    initialize(method, test, targetListReader);
    evaluator.setCorrectNumber(1);
    write(method, coordinateResponse, outputFile);
    assertEqual(1, adaptiveCoordinateResponseTrial(outputFile).correctNumber);
}

ADAPTIVE_METHOD_TEST(writeCorrectKeywordsPassesCorrectKeywords) {
    initialize(method, test, targetListReader);
    correctKeywords.count = 1;
    method.submit(correctKeywords);
    method.writeLastCorrectKeywords(outputFile);
    assertEqual(1, correctKeywordsTrial(outputFile).count);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesReversalsAfterUpdatingTrack) {
    assertWritesUpdatedReversals(writingCoordinateResponse);
}

ADAPTIVE_METHOD_TEST(writeCorrectResponsePassesReversalsAfterUpdatingTrack) {
    assertWritesUpdatedReversals(writingCorrectResponse);
}

ADAPTIVE_METHOD_TEST(writeIncorrectResponsePassesReversalsAfterUpdatingTrack) {
    assertWritesUpdatedReversals(writingIncorrectResponse);
}

ADAPTIVE_METHOD_TEST(writeCorrectKeywordsPassesReversalsAfterUpdatingTrack) {
    assertWritesUpdatedReversals(writingCorrectKeywords);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesSnrBeforeUpdatingTrack) {
    assertWritesPreUpdatedSnr(writingCoordinateResponse);
}

ADAPTIVE_METHOD_TEST(writeCorrectResponsePassesSnrBeforeUpdatingTrack) {
    assertWritesPreUpdatedSnr(writingCorrectResponse);
}

ADAPTIVE_METHOD_TEST(writeIncorrectResponsePassesSnrBeforeUpdatingTrack) {
    assertWritesPreUpdatedSnr(writingIncorrectResponse);
}

ADAPTIVE_METHOD_TEST(writeCorrectKeywordsPassesSnrBeforeUpdatingTrack) {
    assertWritesPreUpdatedSnr(writingCorrectKeywords);
}

ADAPTIVE_METHOD_TEST(writeCorrectCoordinateResponseIsCorrect) {
    initialize(method, test, targetListReader);
    setCorrectCoordinateResponse();
    write(method, coordinateResponse, outputFile);
    assertTrue(coordinateResponseTrialCorrect(outputFile));
}

ADAPTIVE_METHOD_TEST(writeCorrectResponseIsCorrect) {
    initialize(method, test, targetListReader);
    submitCorrectResponse(method);
    method.writeLastCorrectResponse(outputFile);
    assertTrue(outputFile.openSetAdaptiveTrial().correct);
}

ADAPTIVE_METHOD_TEST(writeSufficientCorrectKeywordsIsCorrect) {
    initialize(method, test, targetListReader);
    run(submittingSufficientCorrectKeywords);
    method.writeLastCorrectKeywords(outputFile);
    assertTrue(correctKeywordsTrial(outputFile).correct);
}

ADAPTIVE_METHOD_TEST(writeIncorrectCoordinateResponseIsIncorrect) {
    initialize(method, test, targetListReader);
    setIncorrectCoordinateResponse();
    write(method, coordinateResponse, outputFile);
    assertFalse(coordinateResponseTrialCorrect(outputFile));
}

ADAPTIVE_METHOD_TEST(writeIncorrectResponseIsIncorrect) {
    initialize(method, test, targetListReader);
    submitIncorrectResponse(method);
    method.writeLastIncorrectResponse(outputFile);
    assertFalse(outputFile.openSetAdaptiveTrial().correct);
}

ADAPTIVE_METHOD_TEST(writeInsufficientCorrectKeywordsIsIncorrect) {
    initialize(method, test, targetListReader);
    run(submittingInsufficientCorrectKeywords);
    method.writeLastCorrectKeywords(outputFile);
    assertFalse(correctKeywordsTrial(outputFile).correct);
}

ADAPTIVE_METHOD_TEST(writeCorrectResponseWritesTarget) {
    assertWritesTarget(writingCorrectResponse);
}

ADAPTIVE_METHOD_TEST(writeIncorrectResponseWritesTarget) {
    assertWritesTarget(writingIncorrectResponse);
}

ADAPTIVE_METHOD_TEST(writeCorrectKeywordsWritesTarget) {
    assertWritesTarget(writingCorrectKeywords);
}

ADAPTIVE_METHOD_TEST(writeTestResult) {
    initialize(method, test, targetListReader);
    at(tracks, 0)->setThreshold(11.);
    targetLists.at(0)->setDirectory("a");
    at(tracks, 1)->setThreshold(22.);
    targetLists.at(1)->setDirectory("b");
    at(tracks, 2)->setThreshold(33.);
    targetLists.at(2)->setDirectory("c");
    method.writeTestResult(outputFile);
    assertEqual(11., adaptiveTestResult(outputFile).at(0).threshold);
    assertEqual("a", adaptiveTestResult(outputFile).at(0).targetListDirectory);
    assertEqual(22., adaptiveTestResult(outputFile).at(1).threshold);
    assertEqual("b", adaptiveTestResult(outputFile).at(1).targetListDirectory);
    assertEqual(33., adaptiveTestResult(outputFile).at(2).threshold);
    assertEqual("c", adaptiveTestResult(outputFile).at(2).targetListDirectory);
}

ADAPTIVE_METHOD_TEST(testResults) {
    initialize(method, test, targetListReader);
    at(tracks, 0)->setThreshold(11.);
    targetLists.at(0)->setDirectory("a");
    at(tracks, 1)->setThreshold(22.);
    targetLists.at(1)->setDirectory("b");
    at(tracks, 2)->setThreshold(33.);
    targetLists.at(2)->setDirectory("c");
    assertEqual({{"a", 11.}, {"b", 22.}, {"c", 33.}}, method.testResults());
}

ADAPTIVE_METHOD_TEST(writeTestResultPassThresholdReversals) {
    test.thresholdReversals = 1;
    initialize(method, test, targetListReader);
    method.writeTestResult(outputFile);
    assertEqual(1, at(tracks, 0)->thresholdReversals());
    assertEqual(1, at(tracks, 1)->thresholdReversals());
    assertEqual(1, at(tracks, 2)->thresholdReversals());
}

ADAPTIVE_METHOD_TEST(submitCorrectResponsePassesCurrentToEvaluator) {
    assertPassesCurrentTargetToEvaluatorForFileName(submittingCorrectResponse);
}

ADAPTIVE_METHOD_TEST(submitIncorrectResponsePassesCurrentToEvaluator) {
    assertPassesCurrentTargetToEvaluatorForFileName(
        submittingIncorrectResponse);
}

ADAPTIVE_METHOD_TEST(submitCorrectKeywordsPassesCurrentToEvaluator) {
    assertPassesCurrentTargetToEvaluatorForFileName(submittingCorrectKeywords);
}

ADAPTIVE_METHOD_TEST(submitCorrectCoordinateResponsePushesSnrTrackDown) {
    assertPushesSnrTrackDown(submittingCorrectCoordinateResponse);
}

ADAPTIVE_METHOD_TEST(submitCorrectResponsePushesSnrTrackDown) {
    assertPushesSnrTrackDown(submittingCorrectResponse);
}

ADAPTIVE_METHOD_TEST(submitSufficientCorrectKeywordsPushesSnrTrackDown) {
    assertPushesSnrTrackDown(submittingSufficientCorrectKeywords);
}

ADAPTIVE_METHOD_TEST(submitIncorrectCoordinateResponsePushesSnrTrackUp) {
    assertPushesSnrTrackUp(submittingIncorrectCoordinateResponse);
}

ADAPTIVE_METHOD_TEST(submitIncorrectResponsePushesSnrTrackDown) {
    assertPushesSnrTrackUp(submittingIncorrectResponse);
}

ADAPTIVE_METHOD_TEST(submitInsufficientCorrectKeywordsPushesSnrTrackDown) {
    assertPushesSnrTrackUp(submittingInsufficientCorrectKeywords);
}

ADAPTIVE_METHOD_TEST(resettingTracksSelectsListAmongThoseWithIncompleteTracks) {
    setComplete(tracks, 0);
    at(tracks, 0)->incompleteOnReset();
    initialize(method, test, targetListReader);
    setNext(targetLists, 0, "a");
    setComplete(tracks, 1);
    selectNextList(randomizer, 1);
    method.resetTracks();
    assertNextTargetEquals(method, "a");
}

ADAPTIVE_METHOD_TEST(
    submitCoordinateResponseSelectsListAmongThoseWithIncompleteTracks) {
    assertSelectsListAmongThoseWithIncompleteTracks(
        submittingCoordinateResponse);
}

ADAPTIVE_METHOD_TEST(
    submitCorrectResponseSelectsListAmongThoseWithIncompleteTracks) {
    assertSelectsListAmongThoseWithIncompleteTracks(submittingCorrectResponse);
}

ADAPTIVE_METHOD_TEST(
    submitIncorrectResponseSelectsListAmongThoseWithIncompleteTracks) {
    assertSelectsListAmongThoseWithIncompleteTracks(
        submittingIncorrectResponse);
}

ADAPTIVE_METHOD_TEST(
    submitCorrectKeywordsSelectsListAmongThoseWithIncompleteTracks) {
    assertSelectsListAmongThoseWithIncompleteTracks(submittingCorrectKeywords);
}

ADAPTIVE_METHOD_TEST(completeWhenAllTracksComplete) {
    initialize(method, test, targetListReader);
    setComplete(tracks, 0);
    assertTestIncompleteAfterCoordinateResponse();
    setComplete(tracks, 1);
    assertTestIncompleteAfterCoordinateResponse();
    setComplete(tracks, 2);
    assertTestCompleteAfterCoordinateResponse();
}
}
}
