#include "OutputFileStub.hpp"
#include "RandomizerStub.hpp"
#include "ResponseEvaluatorStub.hpp"
#include "TargetPlaylistSetReaderStub.hpp"
#include "TargetPlaylistStub.hpp"
#include "TrackStub.hpp"
#include "assert-utility.hpp"
#include "av-speech-in-noise/Model.hpp"
#include <av-speech-in-noise/core/AdaptiveMethod.hpp>
#include <gtest/gtest.h>
#include <gsl/gsl>
#include <algorithm>
#include <functional>
#include <variant>

namespace av_speech_in_noise {
static auto operator==(const Phi &a, const Phi &b) -> bool {
    return a.alpha == b.alpha && a.beta == b.beta && a.gamma == b.gamma &&
        a.lambda == b.lambda;
}

static auto operator==(const AdaptiveTestResult &a, const AdaptiveTestResult &b)
    -> bool {
    return a.targetsUrl.path == b.targetsUrl.path && a.result == b.result;
}

namespace {
class UseCase {
  public:
    virtual ~UseCase() = default;
    virtual void run(AdaptiveMethodImpl &) = 0;
};

void initialize(AdaptiveMethodImpl &method, const AdaptiveTest &test,
    TargetPlaylistReader &targetListReader, AdaptiveTrack::Factory &factory) {
    method.initialize(test, &targetListReader, &factory);
}

class Initializing : public UseCase {
  public:
    explicit Initializing(
        TargetPlaylistReader &reader, AdaptiveTrack::Factory &factory)
        : reader{reader}, factory{factory} {}

    void run(AdaptiveMethodImpl &method) override {
        initialize(method, test, reader, factory);
    }

  private:
    AdaptiveTest test{};
    TargetPlaylistReader &reader;
    AdaptiveTrack::Factory &factory;
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
        return adaptiveCoordinateResponseTrial(file).snr.dB;
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
    return openSetAdaptiveTrial(file).snr.dB;
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
        return correctKeywordsTrial(file).snr.dB;
    }

    auto target(OutputFileStub &file) -> std::string override {
        return correctKeywordsTrial(file).target;
    }
};

void resetTracks(AdaptiveMethodImpl &method) { method.resetTracks(); }

void assertStartingXEqualsOne(const AdaptiveTrack::Settings &s) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, s.startingX);
}

void assertCeilingEqualsOne(const AdaptiveTrack::Settings &s) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, s.ceiling);
}

void assertFloorEqualsOne(const AdaptiveTrack::Settings &s) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, s.floor);
}

void assertBumpLimitEqualsOne(const AdaptiveTrack::Settings &s) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, s.bumpLimit);
}

void assertTargetLevelRuleEquals(
    const TrackingRule &rule, const AdaptiveTrack::Settings &s) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(&rule, s.rule);
}

void write(AdaptiveMethodImpl &method,
    const coordinate_response_measure::Response &response,
    OutputFile &outputFile) {
    submit(method, response);
    method.writeLastCoordinateResponse(outputFile);
}

auto settings(const TrackFactoryStub &factory)
    -> std::vector<AdaptiveTrack::Settings> {
    return factory.parameters();
}

auto settings(const TrackFactoryStub &factory, gsl::index i)
    -> AdaptiveTrack::Settings {
    return settings(factory).at(i);
}

constexpr auto listCount{3};

void forEachSettings(const TrackFactoryStub &factory,
    const std::function<void(const AdaptiveTrack::Settings &)> &f) {
    for (int i = 0; i < listCount; ++i)
        f(settings(factory, i));
}

void setNext(const std::vector<std::shared_ptr<TargetPlaylistStub>> &lists,
    gsl::index n, std::string s) {
    lists.at(n)->setNext(std::move(s));
}

void setCurrent(const std::vector<std::shared_ptr<TargetPlaylistStub>> &lists,
    gsl::index n, std::string s) {
    lists.at(n)->setCurrent(std::move(s));
}

auto at(const std::vector<std::shared_ptr<TrackStub>> &tracks, gsl::index n)
    -> const std::shared_ptr<TrackStub> & {
    return tracks.at(n);
}

auto complete(AdaptiveMethodImpl &method) -> bool { return method.complete(); }

void assertComplete(AdaptiveMethodImpl &method) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(complete(method));
}

void assertIncomplete(AdaptiveMethodImpl &method) {
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(complete(method));
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
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(s, method.nextTarget().path);
}

auto adaptiveTestResult(OutputFileStub &file) -> AdaptiveTestResults {
    return file.adaptiveTestResult();
}

void assertPassedIntegerBounds(RandomizerStub &randomizer, int a, int b) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(a, randomizer.lowerIntBound());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(b, randomizer.upperIntBound());
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
    AdaptiveMethodImpl method{evaluator, randomizer};
    OutputFileStub outputFile;
    TargetPlaylistSetReaderStub targetListReader;
    Initializing initializing{targetListReader, snrTrackFactory};
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
    std::vector<std::shared_ptr<TargetPlaylistStub>> targetLists;
    std::vector<std::shared_ptr<TrackStub>> tracks;

    AdaptiveMethodTests() : targetLists(listCount), tracks(listCount) {
        std::generate(targetLists.begin(), targetLists.end(),
            std::make_shared<TargetPlaylistStub>);
        std::generate(
            tracks.begin(), tracks.end(), std::make_shared<TrackStub>);
        targetListReader.setTargetPlaylists(
            {targetLists.begin(), targetLists.end()});
        snrTrackFactory.setTracks({tracks.begin(), tracks.end()});
    }

  public:
    void assertWritesUpdatedReversals(WritingResponseUseCase &useCase) {
        selectNextList(randomizer, 1);
        initialize(method, test, targetListReader, snrTrackFactory);
        at(tracks, 1)->setReversalsWhenUpdated(3);
        selectNextList(randomizer, 2);
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(3, useCase.reversals(outputFile));
    }

    void assertWritesPreUpdatedSnr(WritingResponseUseCase &useCase) {
        selectNextList(randomizer, 1);
        initialize(method, test, targetListReader, snrTrackFactory);
        at(tracks, 1)->setX(4);
        at(tracks, 1)->setXWhenUpdated(3);
        selectNextList(randomizer, 2);
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(4, useCase.snr(outputFile));
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
        initialize(method, test, targetListReader, snrTrackFactory);
        setComplete(tracks, 2);
        run(useCase);
        assertPassedIntegerBounds(randomizer, 0, 1);
    }

    void assertNextTargetEqualsNextFromSelectedTargetPlaylistAfter(
        UseCase &useCase) {
        setNext(targetLists, 1, "a");
        selectNextList(randomizer, 1);
        run(useCase);
        assertNextTargetEquals(method, "a");
    }

    void assertPushesSnrTrackDown(UseCase &useCase) {
        selectNextList(randomizer, 1);
        initialize(method, test, targetListReader, snrTrackFactory);
        selectNextList(randomizer, 2);
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(pushedDown(tracks, 1));
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(pushedUp(tracks, 1));
    }

    void assertPushesSnrTrackUp(UseCase &useCase) {
        selectNextList(randomizer, 1);
        initialize(method, test, targetListReader, snrTrackFactory);
        selectNextList(randomizer, 2);
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_FALSE(pushedDown(tracks, 1));
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(pushedUp(tracks, 1));
    }

    void assertSelectsListAmongThoseWithIncompleteTracks(UseCase &useCase) {
        initialize(method, test, targetListReader, snrTrackFactory);
        setNext(targetLists, 2, "a");
        setComplete(tracks, 1);
        selectNextList(randomizer, 1);
        run(useCase);
        assertNextTargetEquals(method, "a");
    }

    void assertWritesTarget(WritingTargetUseCase &useCase) {
        initialize(method, test, targetListReader, snrTrackFactory);
        evaluator.setFileName("a");
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"a"}, useCase.target(outputFile));
    }

    void assertPassesCurrentTargetToEvaluatorForFileName(UseCase &useCase) {
        selectNextList(randomizer, 1);
        initialize(method, test, targetListReader, snrTrackFactory);
        setCurrent(targetLists, 1, "a");
        selectNextList(randomizer, 2);
        run(useCase);
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
            std::string{"a"}, evaluator.filePathForFileName());
    }
};

#define ADAPTIVE_METHOD_TEST(a) TEST_F(AdaptiveMethodTests, a)

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithUmlSettings) {
    test.umlSettings.alpha.space.N = 42;
    initialize(method, test, targetListReader, snrTrackFactory);
    forEachSettings(snrTrackFactory, [&](auto s) {
        AV_SPEECH_IN_NOISE_EXPECT_EQUAL(42, s.umlSettings.alpha.space.N);
    });
}

ADAPTIVE_METHOD_TEST(initializeCreatesSnrTrackForEachList) {
    initialize(method, test, targetListReader, snrTrackFactory);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::size_t{3}, settings(snrTrackFactory).size());
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithTargetLevelRule) {
    initialize(method, test, targetListReader, snrTrackFactory);
    forEachSettings(snrTrackFactory, [&](auto s) {
        assertTargetLevelRuleEquals(test.levittSettings.trackingRule, s);
    });
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithSnr) {
    test.startingSnr.dB = 1;
    initialize(method, test, targetListReader, snrTrackFactory);
    forEachSettings(snrTrackFactory, assertStartingXEqualsOne);
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithCeiling) {
    test.ceilingSnr.dB = 1;
    initialize(method, test, targetListReader, snrTrackFactory);
    forEachSettings(snrTrackFactory, assertCeilingEqualsOne);
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithFloor) {
    test.floorSnr.dB = 1;
    initialize(method, test, targetListReader, snrTrackFactory);
    forEachSettings(snrTrackFactory, assertFloorEqualsOne);
}

ADAPTIVE_METHOD_TEST(initializeCreatesEachSnrTrackWithBumpLimit) {
    test.trackBumpLimit = 1;
    initialize(method, test, targetListReader, snrTrackFactory);
    forEachSettings(snrTrackFactory, assertBumpLimitEqualsOne);
}

ADAPTIVE_METHOD_TEST(writeTestParametersPassesToOutputFile) {
    initialize(method, test, targetListReader, snrTrackFactory);
    method.writeTestingParameters(outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(test), outputFile.adaptiveTest());
}

ADAPTIVE_METHOD_TEST(initializePassesTargetPlaylistDirectory) {
    test.targetsUrl.path = "a";
    initialize(method, test, targetListReader, snrTrackFactory);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, targetListReader.directory());
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterInitialize) {
    assertNextTargetEqualsNextFromSelectedTargetPlaylistAfter(initializing);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterCoordinateResponse) {
    initialize(method, test, targetListReader, snrTrackFactory);
    assertNextTargetEqualsNextFromSelectedTargetPlaylistAfter(
        submittingCoordinateResponse);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterCorrectResponse) {
    initialize(method, test, targetListReader, snrTrackFactory);
    assertNextTargetEqualsNextFromSelectedTargetPlaylistAfter(
        submittingCorrectResponse);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterIncorrectResponse) {
    initialize(method, test, targetListReader, snrTrackFactory);
    assertNextTargetEqualsNextFromSelectedTargetPlaylistAfter(
        submittingIncorrectResponse);
}

ADAPTIVE_METHOD_TEST(nextReturnsNextFilePathAfterCorrectKeywords) {
    initialize(method, test, targetListReader, snrTrackFactory);
    assertNextTargetEqualsNextFromSelectedTargetPlaylistAfter(
        submittingCorrectKeywords);
}

ADAPTIVE_METHOD_TEST(
    randomizerPassedIntegerBoundsOfListsForSelectingListInRange) {
    initialize(method, test, targetListReader, snrTrackFactory);
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
    initialize(method, test, targetListReader, snrTrackFactory);
    resetTracks(method);
    for (auto &track : tracks)
        AV_SPEECH_IN_NOISE_EXPECT_TRUE(track->resetted());
}

ADAPTIVE_METHOD_TEST(snrReturnsThatOfCurrentTrack) {
    at(tracks, 0)->setX(1.2);
    selectNextList(randomizer, 0);
    initialize(method, test, targetListReader, snrTrackFactory);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1.2, method.snr().dB);
}

ADAPTIVE_METHOD_TEST(submitCoordinateResponsePassesCurrentTargetToEvaluator) {
    selectNextList(randomizer, 1);
    initialize(method, test, targetListReader, snrTrackFactory);
    setCurrent(targetLists, 1, "a");
    selectNextList(randomizer, 2);
    submit(method, coordinateResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, evaluator.correctColorFilePath());
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, evaluator.correctNumberFilePath());
}

ADAPTIVE_METHOD_TEST(submitCoordinateResponsePassesCorrectFilePathToEvaluator) {
    selectNextList(randomizer, 1);
    initialize(method, test, targetListReader, snrTrackFactory);
    setCurrent(targetLists, 1, "a");
    selectNextList(randomizer, 2);
    submit(method, coordinateResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, evaluator.correctFilePath());
}

ADAPTIVE_METHOD_TEST(submitCoordinateResponsePassesResponseToEvaluator) {
    initialize(method, test, targetListReader, snrTrackFactory);
    submit(method, coordinateResponse);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(coordinateResponse), evaluator.response());
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesSubjectColor) {
    initialize(method, test, targetListReader, snrTrackFactory);
    coordinateResponse.color = blue;
    write(method, coordinateResponse, outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        blue, adaptiveCoordinateResponseTrial(outputFile).subjectColor);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesCorrectColor) {
    initialize(method, test, targetListReader, snrTrackFactory);
    evaluator.setCorrectColor(blue);
    write(method, coordinateResponse, outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        blue, adaptiveCoordinateResponseTrial(outputFile).correctColor);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesSubjectNumber) {
    initialize(method, test, targetListReader, snrTrackFactory);
    coordinateResponse.number = 1;
    write(method, coordinateResponse, outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        1, adaptiveCoordinateResponseTrial(outputFile).subjectNumber);
}

ADAPTIVE_METHOD_TEST(writeCoordinateResponsePassesCorrectNumber) {
    initialize(method, test, targetListReader, snrTrackFactory);
    evaluator.setCorrectNumber(1);
    write(method, coordinateResponse, outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        1, adaptiveCoordinateResponseTrial(outputFile).correctNumber);
}

ADAPTIVE_METHOD_TEST(writeCorrectKeywordsPassesCorrectKeywords) {
    initialize(method, test, targetListReader, snrTrackFactory);
    correctKeywords.count = 1;
    method.submit(correctKeywords);
    method.writeLastCorrectKeywords(outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(1, correctKeywordsTrial(outputFile).count);
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
    initialize(method, test, targetListReader, snrTrackFactory);
    setCorrectCoordinateResponse();
    write(method, coordinateResponse, outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(coordinateResponseTrialCorrect(outputFile));
}

ADAPTIVE_METHOD_TEST(writeCorrectResponseIsCorrect) {
    initialize(method, test, targetListReader, snrTrackFactory);
    submitCorrectResponse(method);
    method.writeLastCorrectResponse(outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(outputFile.openSetAdaptiveTrial().correct);
}

ADAPTIVE_METHOD_TEST(writeSufficientCorrectKeywordsIsCorrect) {
    initialize(method, test, targetListReader, snrTrackFactory);
    run(submittingSufficientCorrectKeywords);
    method.writeLastCorrectKeywords(outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(correctKeywordsTrial(outputFile).correct);
}

ADAPTIVE_METHOD_TEST(writeIncorrectCoordinateResponseIsIncorrect) {
    initialize(method, test, targetListReader, snrTrackFactory);
    setIncorrectCoordinateResponse();
    write(method, coordinateResponse, outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(coordinateResponseTrialCorrect(outputFile));
}

ADAPTIVE_METHOD_TEST(writeIncorrectResponseIsIncorrect) {
    initialize(method, test, targetListReader, snrTrackFactory);
    submitIncorrectResponse(method);
    method.writeLastIncorrectResponse(outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(outputFile.openSetAdaptiveTrial().correct);
}

ADAPTIVE_METHOD_TEST(writeInsufficientCorrectKeywordsIsIncorrect) {
    initialize(method, test, targetListReader, snrTrackFactory);
    run(submittingInsufficientCorrectKeywords);
    method.writeLastCorrectKeywords(outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_FALSE(correctKeywordsTrial(outputFile).correct);
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
    initialize(method, test, targetListReader, snrTrackFactory);
    at(tracks, 0)->setThreshold(11.);
    targetLists.at(0)->setDirectory("a");
    at(tracks, 1)->setThreshold(22.);
    targetLists.at(1)->setDirectory("b");
    at(tracks, 2)->setThreshold(33.);
    targetLists.at(2)->setDirectory("c");
    method.writeTestResult(outputFile);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        11., std::get<Threshold>(adaptiveTestResult(outputFile).at(0).result));
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"a"}, adaptiveTestResult(outputFile).at(0).targetsUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        22., std::get<Threshold>(adaptiveTestResult(outputFile).at(1).result));
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"b"}, adaptiveTestResult(outputFile).at(1).targetsUrl.path);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        33., std::get<Threshold>(adaptiveTestResult(outputFile).at(2).result));
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        std::string{"c"}, adaptiveTestResult(outputFile).at(2).targetsUrl.path);
}

ADAPTIVE_METHOD_TEST(testResults) {
    initialize(method, test, targetListReader, snrTrackFactory);
    at(tracks, 0)->setThreshold(11.);
    targetLists.at(0)->setDirectory("a");
    at(tracks, 1)->setThreshold(22.);
    targetLists.at(1)->setDirectory("b");
    at(tracks, 2)->setThreshold(33.);
    targetLists.at(2)->setDirectory("c");
    assertEqual({{{"a"}, std::variant<Threshold, Phi>{11.}},
                    {{"b"}, std::variant<Threshold, Phi>{22.}},
                    {{"c"}, std::variant<Threshold, Phi>{33.}}},
        method.testResults());
}

ADAPTIVE_METHOD_TEST(passesThresholdReversals) {
    test.thresholdReversals = 1;
    initialize(method, test, targetListReader, snrTrackFactory);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        snrTrackFactory.parameters()[0].thresholdReversals, 1);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        snrTrackFactory.parameters()[1].thresholdReversals, 1);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        snrTrackFactory.parameters()[2].thresholdReversals, 1);
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
    initialize(method, test, targetListReader, snrTrackFactory);
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
    initialize(method, test, targetListReader, snrTrackFactory);
    setComplete(tracks, 0);
    assertTestIncompleteAfterCoordinateResponse();
    setComplete(tracks, 1);
    assertTestIncompleteAfterCoordinateResponse();
    setComplete(tracks, 2);
    assertTestCompleteAfterCoordinateResponse();
}
}
}
