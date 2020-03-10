#include "OutputFileStub.h"
#include "RandomizerStub.h"
#include "ResponseEvaluatorStub.h"
#include "TargetListSetReaderStub.h"
#include "TargetListStub.h"
#include "TrackStub.h"
#include "assert-utility.h"
#include <recognition-test/AdaptiveMethod.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <algorithm>
#include <functional>

namespace av_speech_in_noise::tests {
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

class SubmittingSufficientCorrectKeywords : public UseCase {
    open_set::CorrectKeywords correctKeywords{};

  public:
    void run(AdaptiveMethodImpl &method) override {
        correctKeywords.count = 2;
        method.submit(correctKeywords);
    }
};

class SubmittingInsufficientCorrectKeywords : public UseCase {
    open_set::CorrectKeywords correctKeywords{};

  public:
    void run(AdaptiveMethodImpl &method) override {
        correctKeywords.count = 1;
        method.submit(correctKeywords);
    }
};

class SubmittingIncorrectResponse : public UseCase {
  public:
    void run(AdaptiveMethodImpl &method) override {
        submitIncorrectResponse(method);
    }
};

class SubmittingCorrectKeywords : public UseCase {
    open_set::CorrectKeywords correctKeywords{};

  public:
    void run(AdaptiveMethodImpl &method) override {
        method.submit(correctKeywords);
    }
};

class WritingResponseUseCase : public virtual UseCase {
  public:
    virtual auto writtenReversals(OutputFileStub &) -> int = 0;
    virtual auto writtenSnr(OutputFileStub &) -> int = 0;
};

class WritingTargetUseCase : public virtual UseCase {
  public:
    virtual auto writtenTarget(OutputFileStub &) -> std::string = 0;
};

auto writtenAdaptiveCoordinateResponseTrial(const OutputFileStub &file)
    -> const coordinate_response_measure::AdaptiveTrial & {
    return file.writtenAdaptiveCoordinateResponseTrial();
}

class WritingCoordinateResponse : public WritingResponseUseCase {
    coordinate_response_measure::Response response{};
    OutputFile &file_;

  public:
    explicit WritingCoordinateResponse(OutputFile &file_) : file_{file_} {}

    void run(AdaptiveMethodImpl &method) override {
        method.submit(response);
        method.writeLastCoordinateResponse(&file_);
    }

    auto writtenReversals(OutputFileStub &file) -> int override {
        return writtenAdaptiveCoordinateResponseTrial(file).reversals;
    }

    auto writtenSnr(OutputFileStub &file) -> int override {
        return writtenAdaptiveCoordinateResponseTrial(file).SNR_dB;
    }
};

auto writtenOpenSetAdaptiveTrial(OutputFileStub &file)
    -> const open_set::AdaptiveTrial & {
    return file.writtenOpenSetAdaptiveTrial();
}

auto writtenReversals(OutputFileStub &file) -> int {
    return writtenOpenSetAdaptiveTrial(file).reversals;
}

auto writtenSnr(OutputFileStub &file) -> int {
    return writtenOpenSetAdaptiveTrial(file).SNR_dB;
}

auto writtenTarget(OutputFileStub &file) -> std::string {
    return writtenOpenSetAdaptiveTrial(file).target;
}

class WritingCorrectResponse : public WritingResponseUseCase,
                               public WritingTargetUseCase {
    OutputFile &file_;

  public:
    explicit WritingCorrectResponse(OutputFile &file_) : file_{file_} {}

    void run(AdaptiveMethodImpl &method) override {
        submitCorrectResponse(method);
        method.writeLastCorrectResponse(&file_);
    }

    auto writtenReversals(OutputFileStub &file) -> int override {
        return av_speech_in_noise::tests::writtenReversals(file);
    }

    auto writtenSnr(OutputFileStub &file) -> int override {
        return av_speech_in_noise::tests::writtenSnr(file);
    }

    auto writtenTarget(OutputFileStub &file) -> std::string override {
        return av_speech_in_noise::tests::writtenTarget(file);
    }
};

class WritingIncorrectResponse : public WritingResponseUseCase,
                                 public WritingTargetUseCase {
    OutputFile &file_;

  public:
    explicit WritingIncorrectResponse(OutputFile &file_) : file_{file_} {}

    void run(AdaptiveMethodImpl &method) override {
        submitIncorrectResponse(method);
        method.writeLastIncorrectResponse(&file_);
    }

    auto writtenReversals(OutputFileStub &file) -> int override {
        return av_speech_in_noise::tests::writtenReversals(file);
    }

    auto writtenSnr(OutputFileStub &file) -> int override {
        return av_speech_in_noise::tests::writtenSnr(file);
    }

    auto writtenTarget(OutputFileStub &file) -> std::string override {
        return av_speech_in_noise::tests::writtenTarget(file);
    }
};

class WritingCorrectKeywords : public WritingResponseUseCase,
                               public WritingTargetUseCase {
    OutputFile &file_;

  public:
    explicit WritingCorrectKeywords(OutputFile &file_) : file_{file_} {}

    void run(AdaptiveMethodImpl &method) override {
        open_set::CorrectKeywords correctKeywords{};
        method.submit(correctKeywords);
        method.writeLastCorrectKeywords(&file_);
    }

    auto writtenReversals(OutputFileStub &file) -> int override {
        return file.writtenCorrectKeywords().reversals;
    }

    auto writtenSnr(OutputFileStub &file) -> int override {
        return file.writtenCorrectKeywords().SNR_dB;
    }

    auto writtenTarget(OutputFileStub &file) -> std::string override {
        return file.writtenCorrectKeywords().target;
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
    method.writeLastCoordinateResponse(&outputFile);
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

class AdaptiveMethodTests : public ::testing::Test {
  protected:
    TrackFactoryStub snrTrackFactory;
    ResponseEvaluatorStub evaluator;
    RandomizerStub randomizer;
    AdaptiveMethodImpl method{&snrTrackFactory, &evaluator, &randomizer};
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
    AdaptiveTest test;
    coordinate_response_measure::Response coordinateResponse{};
    open_set::CorrectKeywords correctKeywords{};
    std::vector<std::shared_ptr<TargetListStub>> lists;
    std::vector<std::shared_ptr<TrackStub>> tracks;

    AdaptiveMethodTests() : lists(listCount), tracks(listCount) {
        std::generate(
            lists.begin(), lists.end(), std::make_shared<TargetListStub>);
        std::generate(
            tracks.begin(), tracks.end(), std::make_shared<TrackStub>);
        targetListReader.setTargetLists({lists.begin(), lists.end()});
        snrTrackFactory.setTracks({tracks.begin(), tracks.end()});
    }

  public:
    void selectList(int n) { randomizer.setRandomInt(n); }

    void assertNextTargetEquals(const std::string &s) {
        assertEqual(s, method.nextTarget());
    }

    void assertRandomizerPassedIntegerBounds(int a, int b) {
        assertEqual(a, randomizer.lowerIntBound());
        assertEqual(b, randomizer.upperIntBound());
    }

    auto track(int n) { return tracks.at(n); }

    void setCurrentForTarget(int n, std::string s) {
        lists.at(n)->setCurrent(std::move(s));
    }

    void assertWritesUpdatedReversals(WritingResponseUseCase &useCase) {
        selectList(1);
        initialize(method, test, targetListReader);
        track(1)->setReversalsWhenUpdated(3);
        selectList(2);
        run(useCase);
        assertEqual(3, useCase.writtenReversals(outputFile));
    }

    void assertWritesPreUpdatedSnr(WritingResponseUseCase &useCase) {
        selectList(1);
        initialize(method, test, targetListReader);
        track(1)->setX(4);
        track(1)->setXWhenUpdated(3);
        selectList(2);
        run(useCase);
        assertEqual(4, useCase.writtenSnr(outputFile));
    }

    static constexpr auto blue{coordinate_response_measure::Color::blue};

    auto writtenCoordinateResponseTrialCorrect() -> bool {
        return writtenAdaptiveCoordinateResponseTrial(outputFile).correct;
    }

    auto snrTrackPushedDown(int n) -> bool { return track(n)->pushedDown(); }

    auto snrTrackPushedUp(int n) -> bool { return track(n)->pushedUp(); }

    void setCorrectCoordinateResponse() { evaluator.setCorrect(); }

    void setIncorrectCoordinateResponse() { evaluator.setIncorrect(); }

    void setSnrTrackComplete(int n) { track(n)->setComplete(); }

    void assertTestIncompleteAfterCoordinateResponse() {
        submit(method, coordinateResponse);
        assertTestIncomplete();
    }

    void assertTestCompleteAfterCoordinateResponse() {
        submit(method, coordinateResponse);
        assertTestComplete();
    }

    void assertTestIncomplete() { assertFalse(testComplete()); }

    auto testComplete() -> bool { return method.complete(); }

    void assertTestComplete() { assertTrue(testComplete()); }

    void run(UseCase &useCase) { useCase.run(method); }

    void assertSelectsListInRangeAfterRemovingCompleteTracks(UseCase &useCase) {
        initialize(method, test, targetListReader);
        setSnrTrackComplete(2);
        run(useCase);
        assertRandomizerPassedIntegerBounds(0, 1);
    }

    void assertNextReturnsNextFilePathAfter(UseCase &useCase) {
        setNext(lists, 1, "a");
        selectList(1);
        run(useCase);
        assertNextTargetEquals("a");
    }

    void assertPushesSnrTrackDown(UseCase &useCase) {
        selectList(1);
        initialize(method, test, targetListReader);
        selectList(2);
        run(useCase);
        assertTrue(snrTrackPushedDown(1));
        assertFalse(snrTrackPushedUp(1));
    }

    void assertPushesSnrTrackUp(UseCase &useCase) {
        selectList(1);
        initialize(method, test, targetListReader);
        selectList(2);
        run(useCase);
        assertFalse(snrTrackPushedDown(1));
        assertTrue(snrTrackPushedUp(1));
    }

    void assertSelectsListAmongThoseWithIncompleteTracks(UseCase &useCase) {
        initialize(method, test, targetListReader);
        setNext(lists, 2, "a");
        setSnrTrackComplete(1);
        selectList(1);
        run(useCase);
        assertNextTargetEquals("a");
    }

    void assertWritesTarget(WritingTargetUseCase &useCase) {
        initialize(method, test, targetListReader);
        evaluator.setFileName("a");
        run(useCase);
        assertEqual("a", useCase.writtenTarget(outputFile));
    }

    void assertPassesCurrentTargetToEvaluatorForFileName(UseCase &useCase) {
        selectList(1);
        initialize(method, test, targetListReader);
        setCurrentForTarget(1, "a");
        selectList(2);
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
    method.writeTestingParameters(&outputFile);
    assertEqual(&std::as_const(test), outputFile.adaptiveTest());
}

ADAPTIVE_METHOD_TEST(initializePassesTargetListDirectory) {
    test.targetListDirectory = "a";
    initialize(method, test, targetListReader);
    assertEqual("a", targetListReader.directory());
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterInitialize) {
    assertNextReturnsNextFilePathAfter(initializing);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterCoordinateResponse) {
    initialize(method, test, targetListReader);
    assertNextReturnsNextFilePathAfter(submittingCoordinateResponse);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterCorrectResponse) {
    initialize(method, test, targetListReader);
    assertNextReturnsNextFilePathAfter(submittingCorrectResponse);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterIncorrectResponse) {
    initialize(method, test, targetListReader);
    assertNextReturnsNextFilePathAfter(submittingIncorrectResponse);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterCorrectKeywords) {
    initialize(method, test, targetListReader);
    assertNextReturnsNextFilePathAfter(submittingCorrectKeywords);
}

ADAPTIVE_METHOD_TEST(randomizerPassedIntegerBoundsOfLists) {
    initialize(method, test, targetListReader);
    assertRandomizerPassedIntegerBounds(0, 2);
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
    track(0)->setX(1);
    selectList(0);
    initialize(method, test, targetListReader);
    assertEqual(1, method.snr_dB());
}

ADAPTIVE_METHOD_TEST(submitCoordinateResponsePassesCurrentToEvaluator) {
    selectList(1);
    initialize(method, test, targetListReader);
    setCurrentForTarget(1, "a");
    selectList(2);
    submit(method, coordinateResponse);
    assertEqual("a", evaluator.correctColorFilePath());
    assertEqual("a", evaluator.correctNumberFilePath());
}

ADAPTIVE_METHOD_TEST(submitCoordinateResponsePassesCorrectFilePathToEvaluator) {
    selectList(1);
    initialize(method, test, targetListReader);
    setCurrentForTarget(1, "a");
    selectList(2);
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
    assertEqual(
        blue, writtenAdaptiveCoordinateResponseTrial(outputFile).subjectColor);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesCorrectColor) {
    initialize(method, test, targetListReader);
    evaluator.setCorrectColor(blue);
    write(method, coordinateResponse, outputFile);
    assertEqual(
        blue, writtenAdaptiveCoordinateResponseTrial(outputFile).correctColor);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesSubjectNumber) {
    initialize(method, test, targetListReader);
    coordinateResponse.number = 1;
    write(method, coordinateResponse, outputFile);
    assertEqual(
        1, writtenAdaptiveCoordinateResponseTrial(outputFile).subjectNumber);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesCorrectNumber) {
    initialize(method, test, targetListReader);
    evaluator.setCorrectNumber(1);
    write(method, coordinateResponse, outputFile);
    assertEqual(
        1, writtenAdaptiveCoordinateResponseTrial(outputFile).correctNumber);
}

ADAPTIVE_METHOD_TEST(writeCorrectKeywordsPassesCorrectKeywords) {
    initialize(method, test, targetListReader);
    correctKeywords.count = 1;
    method.submit(correctKeywords);
    method.writeLastCorrectKeywords(&outputFile);
    assertEqual(1, outputFile.writtenCorrectKeywords().count);
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
    assertTrue(writtenCoordinateResponseTrialCorrect());
}

ADAPTIVE_METHOD_TEST(writeCorrectResponseIsCorrect) {
    initialize(method, test, targetListReader);
    submitCorrectResponse(method);
    method.writeLastCorrectResponse(&outputFile);
    assertTrue(outputFile.writtenOpenSetAdaptiveTrial().correct);
}

ADAPTIVE_METHOD_TEST(writeSufficientCorrectKeywordsIsCorrect) {
    initialize(method, test, targetListReader);
    run(submittingSufficientCorrectKeywords);
    method.writeLastCorrectKeywords(&outputFile);
    assertTrue(outputFile.writtenCorrectKeywords().correct);
}

ADAPTIVE_METHOD_TEST(writeIncorrectCoordinateResponseIsIncorrect) {
    initialize(method, test, targetListReader);
    setIncorrectCoordinateResponse();
    write(method, coordinateResponse, outputFile);
    assertFalse(writtenCoordinateResponseTrialCorrect());
}

ADAPTIVE_METHOD_TEST(writeIncorrectResponseIsIncorrect) {
    initialize(method, test, targetListReader);
    submitIncorrectResponse(method);
    method.writeLastIncorrectResponse(&outputFile);
    assertFalse(outputFile.writtenOpenSetAdaptiveTrial().correct);
}

ADAPTIVE_METHOD_TEST(writeInsufficientCorrectKeywordsIsIncorrect) {
    initialize(method, test, targetListReader);
    run(submittingInsufficientCorrectKeywords);
    method.writeLastCorrectKeywords(&outputFile);
    assertFalse(outputFile.writtenCorrectKeywords().correct);
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
    setSnrTrackComplete(0);
    track(0)->incompleteOnReset();
    initialize(method, test, targetListReader);
    setNext(lists, 0, "a");
    setSnrTrackComplete(1);
    selectList(1);
    method.resetTracks();
    assertNextTargetEquals("a");
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
    setSnrTrackComplete(0);
    assertTestIncompleteAfterCoordinateResponse();
    setSnrTrackComplete(1);
    assertTestIncompleteAfterCoordinateResponse();
    setSnrTrackComplete(2);
    assertTestCompleteAfterCoordinateResponse();
}
}
}
