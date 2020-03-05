#include "OutputFileStub.h"
#include "RandomizerStub.h"
#include "ResponseEvaluatorStub.h"
#include "TargetListSetReaderStub.h"
#include "TargetListStub.h"
#include "TrackStub.h"
#include "assert-utility.h"
#include "av-speech-in-noise/Model.hpp"
#include <gtest/gtest.h>
#include <recognition-test/AdaptiveMethod.hpp>
#include <algorithm>

namespace av_speech_in_noise::tests {
namespace {
class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(AdaptiveMethodImpl &) = 0;
};

class Initializing : public UseCase {
    AdaptiveTest test{};

  public:
    void run(AdaptiveMethodImpl &method) override { method.initialize(test); }
};

void submit(AdaptiveMethodImpl &method,
    const coordinate_response_measure::Response &response) {
    method.submitResponse(response);
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
        method.submitResponse(response);
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

class AdaptiveMethodTests : public ::testing::Test {
  protected:
    TargetListSetReaderStub targetListSetReader;
    TrackFactoryStub snrTrackFactory;
    ResponseEvaluatorStub evaluator;
    RandomizerStub randomizer;
    OutputFileStub outputFile;
    AdaptiveMethodImpl method{
        &targetListSetReader, &snrTrackFactory, &evaluator, &randomizer};
    Initializing initializing;
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

    AdaptiveMethodTests() : lists(3), tracks(3) {
        std::generate(
            lists.begin(), lists.end(), std::make_shared<TargetListStub>);
        std::generate(
            tracks.begin(), tracks.end(), std::make_shared<TrackStub>);
        targetListSetReader.setTargetLists({lists.begin(), lists.end()});
        snrTrackFactory.setTracks({tracks.begin(), tracks.end()});
    }

  public:
    auto snrTrackFactoryParameters() const {
        return snrTrackFactory.parameters();
    }

    auto snrTrackFactoryParameters(int x) const {
        return snrTrackFactoryParameters().at(x);
    }

    void initialize() { method.initialize(test); }

    void assertPassedTargetLevelRule(const Track::Settings &s) {
        assertEqual(&std::as_const(test.trackingRule), s.rule);
    }

    void assertStartingXEqualsOne(const Track::Settings &s) {
        assertEqual(1, s.startingX);
    }

    void assertCeilingEqualsOne(const Track::Settings &s) {
        assertEqual(1, s.ceiling);
    }

    void assertFloorEqualsOne(const Track::Settings &s) {
        assertEqual(1, s.floor);
    }

    void assertBumpLimitEqualsOne(const Track::Settings &s) {
        assertEqual(1, s.bumpLimit);
    }

    void applyToSnrTrackFactoryParameters(
        int n, void (AdaptiveMethodTests::*f)(const Track::Settings &)) {
        for (int i = 0; i < n; ++i)
            (this->*f)(snrTrackFactoryParameters(i));
    }

    void selectList(int n) { randomizer.setRandomInt(n); }

    auto next() -> std::string { return method.nextTarget(); }

    void assertNextEquals(const std::string &s) { assertEqual(s, next()); }

    void setNextForList(int n, std::string s) {
        lists.at(n)->setNext(std::move(s));
    }

    void assertRandomizerPassedIntegerBounds(int a, int b) {
        assertEqual(a, randomizer.lowerIntBound());
        assertEqual(b, randomizer.upperIntBound());
    }

    auto track(int n) { return tracks.at(n); }

    void setCurrentForTarget(int n, std::string s) {
        lists.at(n)->setCurrent(std::move(s));
    }

    void writeCoordinateResponse() {
        submit(method, coordinateResponse);
        method.writeLastCoordinateResponse(&outputFile);
    }

    void assertWritesUpdatedReversals(WritingResponseUseCase &useCase) {
        selectList(1);
        initialize();
        track(1)->setReversalsWhenUpdated(3);
        selectList(2);
        run(useCase);
        assertEqual(3, useCase.writtenReversals(outputFile));
    }

    void assertWritesPreUpdatedSnr(WritingResponseUseCase &useCase) {
        selectList(1);
        initialize();
        track(1)->setX(4);
        track(1)->setXWhenUpdated(3);
        selectList(2);
        run(useCase);
        assertEqual(4, useCase.writtenSnr(outputFile));
    }

    static auto blueColor() { return coordinate_response_measure::Color::blue; }

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
        initialize();
        setSnrTrackComplete(2);
        run(useCase);
        assertRandomizerPassedIntegerBounds(0, 1);
    }

    void assertNextReturnsNextFilePathAfter(UseCase &useCase) {
        setNextForList(1, "a");
        selectList(1);
        run(useCase);
        assertNextEquals("a");
    }

    void assertPushesSnrTrackDown(UseCase &useCase) {
        selectList(1);
        initialize();
        selectList(2);
        run(useCase);
        assertTrue(snrTrackPushedDown(1));
        assertFalse(snrTrackPushedUp(1));
    }

    void assertPushesSnrTrackUp(UseCase &useCase) {
        selectList(1);
        initialize();
        selectList(2);
        run(useCase);
        assertFalse(snrTrackPushedDown(1));
        assertTrue(snrTrackPushedUp(1));
    }

    void assertSelectsListAmongThoseWithIncompleteTracks(UseCase &useCase) {
        initialize();
        setNextForList(2, "a");
        setSnrTrackComplete(1);
        selectList(1);
        run(useCase);
        assertNextEquals("a");
    }

    void assertWritesTarget(WritingTargetUseCase &useCase) {
        initialize();
        evaluator.setFileName("a");
        run(useCase);
        assertEqual("a", useCase.writtenTarget(outputFile));
    }

    void assertPassesCurrentTargetToEvaluatorForFileName(UseCase &useCase) {
        selectList(1);
        initialize();
        setCurrentForTarget(1, "a");
        selectList(2);
        run(useCase);
        assertEqual("a", evaluator.filePathForFileName());
    }
};

#define ADAPTIVE_METHOD_TEST(a) TEST_F(AdaptiveMethodTests, a)

ADAPTIVE_METHOD_TEST(initializeCreatesSnrTrackForEachList) {
    initialize();
    assertEqual(std::size_t{3}, snrTrackFactoryParameters().size());
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithTargetLevelRule) {
    initialize();
    applyToSnrTrackFactoryParameters(
        3, &AdaptiveMethodTests::assertPassedTargetLevelRule);
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithSnr) {
    test.startingSnr_dB = 1;
    initialize();
    applyToSnrTrackFactoryParameters(
        3, &AdaptiveMethodTests::assertStartingXEqualsOne);
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithCeiling) {
    test.ceilingSnr_dB = 1;
    initialize();
    applyToSnrTrackFactoryParameters(
        3, &AdaptiveMethodTests::assertCeilingEqualsOne);
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithFloor) {
    test.floorSnr_dB = 1;
    initialize();
    applyToSnrTrackFactoryParameters(
        3, &AdaptiveMethodTests::assertFloorEqualsOne);
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithBumpLimit) {
    test.trackBumpLimit = 1;
    initialize();
    applyToSnrTrackFactoryParameters(
        3, &AdaptiveMethodTests::assertBumpLimitEqualsOne);
}

ADAPTIVE_METHOD_TEST(writeTestParametersPassesToOutputFile) {
    initialize();
    method.writeTestingParameters(&outputFile);
    assertEqual(&std::as_const(test), outputFile.adaptiveTest());
}

ADAPTIVE_METHOD_TEST(initializePassesTargetListDirectory) {
    test.targetListDirectory = "a";
    initialize();
    assertEqual("a", targetListSetReader.directory());
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterInitialize) {
    assertNextReturnsNextFilePathAfter(initializing);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterCoordinateResponse) {
    initialize();
    assertNextReturnsNextFilePathAfter(submittingCoordinateResponse);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterCorrectResponse) {
    initialize();
    assertNextReturnsNextFilePathAfter(submittingCorrectResponse);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterIncorrectResponse) {
    initialize();
    assertNextReturnsNextFilePathAfter(submittingIncorrectResponse);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterCorrectKeywords) {
    initialize();
    assertNextReturnsNextFilePathAfter(submittingCorrectKeywords);
}

ADAPTIVE_METHOD_TEST(randomizerPassedIntegerBoundsOfLists) {
    initialize();
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

ADAPTIVE_METHOD_TEST(snrReturnsThatOfCurrentTrack) {
    track(0)->setX(1);
    selectList(0);
    initialize();
    assertEqual(1, method.snr_dB());
}

ADAPTIVE_METHOD_TEST(submitCoordinateResponsePassesCurrentToEvaluator) {
    selectList(1);
    initialize();
    setCurrentForTarget(1, "a");
    selectList(2);
    submit(method, coordinateResponse);
    assertEqual("a", evaluator.correctColorFilePath());
    assertEqual("a", evaluator.correctNumberFilePath());
}

ADAPTIVE_METHOD_TEST(submitCoordinateResponsePassesCorrectFilePathToEvaluator) {
    selectList(1);
    initialize();
    setCurrentForTarget(1, "a");
    selectList(2);
    submit(method, coordinateResponse);
    assertEqual("a", evaluator.correctFilePath());
}

ADAPTIVE_METHOD_TEST(submitCoordinateResponsePassesResponseToEvaluator) {
    initialize();
    submit(method, coordinateResponse);
    assertEqual(&std::as_const(coordinateResponse), evaluator.response());
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesSubjectColor) {
    initialize();
    coordinateResponse.color = blueColor();
    writeCoordinateResponse();
    assertEqual(blueColor(),
        writtenAdaptiveCoordinateResponseTrial(outputFile).subjectColor);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesCorrectColor) {
    initialize();
    evaluator.setCorrectColor(blueColor());
    writeCoordinateResponse();
    assertEqual(blueColor(),
        writtenAdaptiveCoordinateResponseTrial(outputFile).correctColor);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesSubjectNumber) {
    initialize();
    coordinateResponse.number = 1;
    writeCoordinateResponse();
    assertEqual(
        1, writtenAdaptiveCoordinateResponseTrial(outputFile).subjectNumber);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesCorrectNumber) {
    initialize();
    evaluator.setCorrectNumber(1);
    writeCoordinateResponse();
    assertEqual(
        1, writtenAdaptiveCoordinateResponseTrial(outputFile).correctNumber);
}

ADAPTIVE_METHOD_TEST(writeCorrectKeywordsPassesCorrectKeywords) {
    initialize();
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
    initialize();
    setCorrectCoordinateResponse();
    writeCoordinateResponse();
    assertTrue(writtenCoordinateResponseTrialCorrect());
}

ADAPTIVE_METHOD_TEST(writeCorrectResponseIsCorrect) {
    initialize();
    submitCorrectResponse(method);
    method.writeLastCorrectResponse(&outputFile);
    assertTrue(outputFile.writtenOpenSetAdaptiveTrial().correct);
}

ADAPTIVE_METHOD_TEST(writeSufficientCorrectKeywordsIsCorrect) {
    initialize();
    run(submittingSufficientCorrectKeywords);
    method.writeLastCorrectKeywords(&outputFile);
    assertTrue(outputFile.writtenCorrectKeywords().correct);
}

ADAPTIVE_METHOD_TEST(writeIncorrectCoordinateResponseIsIncorrect) {
    initialize();
    setIncorrectCoordinateResponse();
    writeCoordinateResponse();
    assertFalse(writtenCoordinateResponseTrialCorrect());
}

ADAPTIVE_METHOD_TEST(writeIncorrectResponseIsIncorrect) {
    initialize();
    submitIncorrectResponse(method);
    method.writeLastIncorrectResponse(&outputFile);
    assertFalse(outputFile.writtenOpenSetAdaptiveTrial().correct);
}

ADAPTIVE_METHOD_TEST(writeInsufficientCorrectKeywordsIsIncorrect) {
    initialize();
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
    initialize();
    setSnrTrackComplete(0);
    assertTestIncompleteAfterCoordinateResponse();
    setSnrTrackComplete(1);
    assertTestIncompleteAfterCoordinateResponse();
    setSnrTrackComplete(2);
    assertTestCompleteAfterCoordinateResponse();
}
}
}
