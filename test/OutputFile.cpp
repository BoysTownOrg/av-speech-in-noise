#include "LogString.hpp"
#include "assert-utility.hpp"
#include "av-speech-in-noise/Model.hpp"

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/core/OutputFile.hpp>

#include <gtest/gtest.h>

#include <gsl/gsl>

#include <optional>
#include <vector>
#include <map>
#include <iostream>

namespace av_speech_in_noise {
namespace {
class WriterStub : public Writer {
  public:
    void save() override { saved_ = true; }

    void close() override { closed_ = true; }

    void open(const std::string &f) override { filePath_ = f; }

    void write(const std::string &s) override { insert(written_, s); }

    auto failed() -> bool override { return {}; }

    auto saved() const -> bool { return saved_; }

    auto filePath() const -> std::string { return filePath_; }

    auto closed() const -> bool { return closed_; }

    auto written() const -> const std::stringstream & { return written_; }

    auto writable() -> const Writable * { return writable_; }

    void write(Writable &w) override { writable_ = &w; }

  private:
    std::stringstream written_;
    std::string filePath_;
    const Writable *writable_{};
    bool closed_{};
    bool saved_{};
};

class WritableStub : public Writable {
  public:
    void write(std::ostream &stream) override { stream << "writable"; }
};

class OutputFilePathStub : public OutputFilePath {
  public:
    auto outputDirectory() -> std::string override { return outputDirectory_; }

    auto generateFileName(const TestIdentity &p) -> std::string override {
        testIdentity_ = &p;
        return fileName_;
    }

    void setFileName(std::string s) { fileName_ = std::move(s); }

    [[nodiscard]] auto testIdentity() const -> const TestIdentity * {
        return testIdentity_;
    }

    void setOutputDirectory(std::string s) { outputDirectory_ = std::move(s); }

    auto relativeOutputDirectory() -> std::string {
        return relativeOutputDirectory_;
    }

  private:
    std::string fileName_;
    std::string outputDirectory_;
    std::string relativeOutputDirectory_;
    const TestIdentity *testIdentity_{};
};

class UseCase {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(UseCase);
    virtual void run(OutputFileImpl &) = 0;
};

class WritingTrial : public virtual UseCase {
  public:
    virtual auto headingLabels() -> std::map<HeadingItem, gsl::index> = 0;
    virtual void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &, gsl::index line) = 0;
};

class WritingEvaluatedTrial : public virtual WritingTrial {
  public:
    virtual void incorrect() = 0;
    virtual void correct() = 0;
    virtual auto evaluationEntryIndex() -> gsl::index = 0;
};

class WritingFlaggableTrial : public virtual WritingTrial {
  public:
    virtual void flag() = 0;
    virtual void clearFlag() = 0;
};

void setCorrect(coordinate_response_measure::Trial &trial) {
    trial.correct = true;
}

void setIncorrect(coordinate_response_measure::Trial &trial) {
    trial.correct = false;
}

void run(UseCase &useCase, OutputFileImpl &file) { useCase.run(file); }

auto written(WriterStub &writer) -> const std::stringstream & {
    return writer.written();
}

auto writtenString(WriterStub &writer) -> std::string {
    return string(written(writer));
}

void openNewFile(OutputFileImpl &file, const TestIdentity &identity = {}) {
    file.openNewFile(identity);
}

void write(OutputFileImpl &file, const BinocularGazeSamples &gazeSamples) {
    file.write(gazeSamples);
}

void writeTargetStartTimeNanoseconds(OutputFileImpl &file, std::uintmax_t t) {
    file.write(TargetStartTime{t});
}

void assertEndsWith(WriterStub &writer, const std::string &s) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(endsWith(written(writer), s));
}

auto find_nth_element(const std::string &content, gsl::index n, char what)
    -> std::string::size_type {
    auto found{std::string::npos};
    for (int i = 0; i < n; ++i)
        found = content.find(what, found + 1U);
    return found;
}

auto upUntilFirstOfAny(const std::string &content, std::vector<char> v)
    -> std::string {
    return content.substr(0, content.find_first_of({v.begin(), v.end()}));
}

auto nthCommaDelimitedEntryOfLine(
    WriterStub &writer, gsl::index n, gsl::index line) -> std::string {
    const auto precedingNewLine{
        find_nth_element(writtenString(writer), line - 1, '\n')};
    const auto line_{writtenString(writer).substr(precedingNewLine + 1)};
    const auto precedingComma{find_nth_element(line_, n - 1, ',')};
    const auto entryBeginning =
        (precedingComma == std::string::npos) ? 0U : precedingComma + 2;
    return upUntilFirstOfAny(line_.substr(entryBeginning), {',', '\n'});
}

void assertContainsColonDelimitedEntry(
    WriterStub &writer, const std::string &label, const std::string &what) {
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(
        contains(written(writer), label + ": " + what + '\n'));
}

void assertNthCommaDelimitedEntryOfLine(WriterStub &writer,
    const std::string &what, gsl::index n, gsl::index line) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        what, nthCommaDelimitedEntryOfLine(writer, n, line));
}

void assertNthCommaDelimitedEntryOfLine(WriterStub &writer,
    coordinate_response_measure::Color what, gsl::index n, gsl::index line) {
    assertNthCommaDelimitedEntryOfLine(writer, name(what), n, line);
}

void assertNthCommaDelimitedEntryOfLine(
    WriterStub &writer, HeadingItem item, gsl::index n, gsl::index line) {
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        name(item), nthCommaDelimitedEntryOfLine(writer, n, line));
}

void assertNthEntryOfSecondLine(
    WriterStub &writer, const std::string &what, gsl::index n) {
    assertNthCommaDelimitedEntryOfLine(writer, what, n, 2);
}

auto at(const std::map<HeadingItem, gsl::index> &m, HeadingItem item)
    -> gsl::index {
    return m.at(item);
}

class WritingAdaptiveCoordinateResponseTrial : public WritingEvaluatedTrial {
  public:
    WritingAdaptiveCoordinateResponseTrial() {
        trial.snr.dB = 1;
        trial.correctNumber = 2;
        trial.subjectNumber = 3;
        trial.correctColor = coordinate_response_measure::Color::green;
        trial.subjectColor = coordinate_response_measure::Color::red;
        trial.reversals = 4;
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "1", at(headingLabels_, HeadingItem::snr_dB), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "2", at(headingLabels_, HeadingItem::correctNumber), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "3", at(headingLabels_, HeadingItem::subjectNumber), line);
        assertNthCommaDelimitedEntryOfLine(writer,
            coordinate_response_measure::Color::green,
            at(headingLabels_, HeadingItem::correctColor), line);
        assertNthCommaDelimitedEntryOfLine(writer,
            coordinate_response_measure::Color::red,
            at(headingLabels_, HeadingItem::subjectColor), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "4", at(headingLabels_, HeadingItem::reversals), line);
    }

    void incorrect() override { setIncorrect(trial); }

    void correct() override { setCorrect(trial); }

    auto evaluationEntryIndex() -> gsl::index override {
        return at(headingLabels_, HeadingItem::evaluation);
    }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

  private:
    coordinate_response_measure::AdaptiveTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{{HeadingItem::snr_dB, 1},
        {HeadingItem::correctNumber, 2}, {HeadingItem::subjectNumber, 3},
        {HeadingItem::correctColor, 4}, {HeadingItem::subjectColor, 5},
        {HeadingItem::evaluation, 6}, {HeadingItem::reversals, 7}};
};

class WritingFixedLevelCoordinateResponseTrial : public WritingEvaluatedTrial {
  public:
    WritingFixedLevelCoordinateResponseTrial() {
        trial.correctNumber = 2;
        trial.subjectNumber = 3;
        trial.correctColor = coordinate_response_measure::Color::green;
        trial.subjectColor = coordinate_response_measure::Color::red;
        trial.target = "a";
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "2", at(headingLabels_, HeadingItem::correctNumber), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "3", at(headingLabels_, HeadingItem::subjectNumber), line);
        assertNthCommaDelimitedEntryOfLine(writer,
            coordinate_response_measure::Color::green,
            at(headingLabels_, HeadingItem::correctColor), line);
        assertNthCommaDelimitedEntryOfLine(writer,
            coordinate_response_measure::Color::red,
            at(headingLabels_, HeadingItem::subjectColor), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "a", at(headingLabels_, HeadingItem::target), line);
    }

    void incorrect() override { setIncorrect(trial); }

    void correct() override { setCorrect(trial); }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto evaluationEntryIndex() -> gsl::index override {
        return at(headingLabels_, HeadingItem::evaluation);
    }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

  private:
    coordinate_response_measure::FixedLevelTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{
        {HeadingItem::correctNumber, 1}, {HeadingItem::subjectNumber, 2},
        {HeadingItem::correctColor, 3}, {HeadingItem::subjectColor, 4},
        {HeadingItem::evaluation, 5}, {HeadingItem::target, 6}};
};

class WritingOpenSetAdaptiveTrial : public WritingEvaluatedTrial {
  public:
    WritingOpenSetAdaptiveTrial() {
        trial.snr.dB = 11.1;
        trial.target = "a";
        trial.reversals = 22;
        trial.phi = Phi{1.2, 2.3, 3.4, 4.5};
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "11.1", at(headingLabels_, HeadingItem::snr_dB), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "a", at(headingLabels_, HeadingItem::target), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "22", at(headingLabels_, HeadingItem::reversals), line);
        assertNthCommaDelimitedEntryOfLine(writer, "1.2 2.3 3.4 4.5",
            at(headingLabels_, HeadingItem::phi), line);
    }

    void incorrect() override { trial.correct = false; }

    void correct() override { trial.correct = true; }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto evaluationEntryIndex() -> gsl::index override {
        return at(headingLabels_, HeadingItem::evaluation);
    }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

  private:
    open_set::AdaptiveTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{{HeadingItem::snr_dB, 1},
        {HeadingItem::target, 2}, {HeadingItem::evaluation, 3},
        {HeadingItem::reversals, 4}, {HeadingItem::phi, 5}};
};

class WritingCorrectKeywordsTrial : public WritingEvaluatedTrial {
  public:
    WritingCorrectKeywordsTrial() {
        trial.snr.dB = 11;
        trial.target = "a";
        trial.count = 22;
        trial.reversals = 33;
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "11", at(headingLabels_, HeadingItem::snr_dB), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "a", at(headingLabels_, HeadingItem::target), line);
        assertNthCommaDelimitedEntryOfLine(writer, "22",
            at(headingLabels_, HeadingItem::correctKeywords), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "33", at(headingLabels_, HeadingItem::reversals), line);
    }

    void incorrect() override { trial.correct = false; }

    void correct() override { trial.correct = true; }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto evaluationEntryIndex() -> gsl::index override {
        return at(headingLabels_, HeadingItem::evaluation);
    }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

  private:
    CorrectKeywordsTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{{HeadingItem::snr_dB, 1},
        {HeadingItem::target, 2}, {HeadingItem::correctKeywords, 3},
        {HeadingItem::evaluation, 4}, {HeadingItem::reversals, 5}};
};

class WritingPassFailTrial : public WritingEvaluatedTrial {
  public:
    WritingPassFailTrial() { trial.target = "a.txt"; }

    void incorrect() override { trial.correct = false; }

    void correct() override { trial.correct = true; }

    auto evaluationEntryIndex() -> gsl::index override {
        return headingLabels_.at(HeadingItem::evaluation);
    }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "a.txt", at(headingLabels_, HeadingItem::target), line);
    }

    void run(OutputFileImpl &file) override { file.write(trial); }

    PassFailTrial trial;
    std::map<HeadingItem, gsl::index> headingLabels_{
        {HeadingItem::target, 1}, {HeadingItem::evaluation, 2}};
};

class WritingConsonantTrial : public WritingEvaluatedTrial {
  public:
    WritingConsonantTrial() {
        trial.correctConsonant = Consonant::bi;
        trial.subjectConsonant = Consonant::si;
        trial.target = "c";
        trial.rt.milliseconds = 2.34;
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(writer, "bi",
            at(headingLabels_, HeadingItem::correctConsonant), line);
        assertNthCommaDelimitedEntryOfLine(writer, "si",
            at(headingLabels_, HeadingItem::subjectConsonant), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "c", at(headingLabels_, HeadingItem::target), line);
        assertNthCommaDelimitedEntryOfLine(writer, "2.34",
            at(headingLabels_, HeadingItem::reactionTime), line);
    }

    void incorrect() override { trial.correct = false; }

    void correct() override { trial.correct = true; }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto evaluationEntryIndex() -> gsl::index override {
        return at(headingLabels_, HeadingItem::evaluation);
    }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

  private:
    ConsonantTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{
        {HeadingItem::correctConsonant, 1}, {HeadingItem::subjectConsonant, 2},
        {HeadingItem::evaluation, 3}, {HeadingItem::target, 4},
        {HeadingItem::reactionTime, 5}};
};

class WritingKeyPressTrial : public WritingTrial {
  public:
    WritingKeyPressTrial() {
        trial.target = "a";
        trial.key = KeyPressed::second;
        trial.rt.milliseconds = 3.4;
        trial.vibrotactileStimulus.vibrations.resize(2);
        trial.vibrotactileStimulus.vibrations.at(0).duration.seconds = 0.25;
        trial.vibrotactileStimulus.vibrations.at(1).duration.seconds = 0.1;
        trial.vibrotactileStimulus.targetStartRelativeDelay.seconds = 0.19;
    }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "a", at(headingLabels_, HeadingItem::target), line);
        assertNthCommaDelimitedEntryOfLine(writer, "second",
            at(headingLabels_, HeadingItem::keyPressed), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "3.4", at(headingLabels_, HeadingItem::reactionTime), line);
        assertNthCommaDelimitedEntryOfLine(writer, "[0.25; 0.1]",
            at(headingLabels_, HeadingItem::vibrotactileDuration), line);
        assertNthCommaDelimitedEntryOfLine(writer, "0.19",
            at(headingLabels_, HeadingItem::vibrotactileDelay), line);
    }

    void run(OutputFileImpl &file) override { file.write(trial); }

    KeyPressTrial trial;
    std::map<HeadingItem, gsl::index> headingLabels_{{HeadingItem::target, 1},
        {HeadingItem::keyPressed, 2}, {HeadingItem::reactionTime, 3},
        {HeadingItem::vibrotactileDuration, 4},
        {HeadingItem::vibrotactileDelay, 5}};
};

class WritingEmotionTrial : public WritingTrial {
  public:
    WritingEmotionTrial() {
        trial.target = "a";
        trial.emotion = Emotion::scared;
        trial.reactionTimeMilliseconds = 1.23;
    }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "a", at(headingLabels_, HeadingItem::target), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "scared", at(headingLabels_, HeadingItem::emotion), line);
        assertNthCommaDelimitedEntryOfLine(writer, "1.23",
            at(headingLabels_, HeadingItem::reactionTime), line);
    }

    void run(OutputFileImpl &file) override { file.write(trial); }

    EmotionTrial trial;
    std::map<HeadingItem, gsl::index> headingLabels_{{HeadingItem::target, 1},
        {HeadingItem::emotion, 2}, {HeadingItem::reactionTime, 3}};
};

class WritingFreeResponseTrial : public WritingFlaggableTrial {
  public:
    WritingFreeResponseTrial() {
        trial.target = "a";
        trial.response = "b";
        trial.time = "c";
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "a", at(headingLabels_, HeadingItem::target), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "b", at(headingLabels_, HeadingItem::freeResponse), line);
        assertNthCommaDelimitedEntryOfLine(
            writer, "c", at(headingLabels_, HeadingItem::time), line);
    }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

    void flag() override { trial.flagged = true; }

    void clearFlag() override { trial.flagged = false; }

  private:
    FreeResponseTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{{HeadingItem::time, 1},
        {HeadingItem::target, 2}, {HeadingItem::freeResponse, 3}};
};

class WritingThreeKeywordsTrial : public WritingFlaggableTrial {
  public:
    WritingThreeKeywordsTrial() {
        trial.target = "a";
        trial.firstCorrect = true;
        trial.secondCorrect = false;
        trial.thirdCorrect = true;
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "a", at(headingLabels_, HeadingItem::target), line);
        assertNthCommaDelimitedEntryOfLine(writer, "correct",
            at(headingLabels_, HeadingItem::firstKeywordEvaluation), line);
        assertNthCommaDelimitedEntryOfLine(writer, "incorrect",
            at(headingLabels_, HeadingItem::secondKeywordEvaluation), line);
        assertNthCommaDelimitedEntryOfLine(writer, "correct",
            at(headingLabels_, HeadingItem::thirdKeywordEvaluation), line);
    }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

    void flag() override { trial.flagged = true; }

    void clearFlag() override { trial.flagged = false; }

  private:
    ThreeKeywordsTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{{HeadingItem::target, 1},
        {HeadingItem::firstKeywordEvaluation, 2},
        {HeadingItem::secondKeywordEvaluation, 3},
        {HeadingItem::thirdKeywordEvaluation, 4}};
};

class WritingSyllableTrial : public WritingEvaluatedTrial,
                             public WritingFlaggableTrial {
  public:
    WritingSyllableTrial() {
        trial.target = "a";
        trial.subjectSyllable = Syllable::ki;
        trial.correctSyllable = Syllable::zi;
    }

    void assertContainsCommaDelimitedTrialOnLine(
        WriterStub &writer, gsl::index line) override {
        assertNthCommaDelimitedEntryOfLine(
            writer, "a", at(headingLabels_, HeadingItem::target), line);
        assertNthCommaDelimitedEntryOfLine(writer, "ki",
            at(headingLabels_, HeadingItem::subjectSyllable), line);
        assertNthCommaDelimitedEntryOfLine(writer, "zi",
            at(headingLabels_, HeadingItem::correctSyllable), line);
    }

    void run(OutputFileImpl &file) override { file.write(trial); }

    auto headingLabels() -> std::map<HeadingItem, gsl::index> override {
        return headingLabels_;
    }

    void incorrect() override { trial.correct = false; }

    void correct() override { trial.correct = true; }

    auto evaluationEntryIndex() -> gsl::index override {
        return at(headingLabels_, HeadingItem::evaluation);
    }

    void flag() override { trial.flagged = true; }

    void clearFlag() override { trial.flagged = false; }

  private:
    SyllableTrial trial{};
    std::map<HeadingItem, gsl::index> headingLabels_{
        {HeadingItem::correctSyllable, 1}, {HeadingItem::subjectSyllable, 2},
        {HeadingItem::evaluation, 3}, {HeadingItem::target, 4}};
};

void assertWritesFlaggedTrial(
    OutputFileImpl &file, WriterStub &writer, WritingFlaggableTrial &useCase) {
    useCase.flag();
    run(useCase, file);
    assertNthEntryOfSecondLine(
        writer, "FLAGGED", useCase.headingLabels().size() + 1);
}

void assertNonFlaggedTrialDoesNotWriteExtraEntry(
    OutputFileImpl &file, WriterStub &writer, WritingFlaggableTrial &useCase) {
    useCase.clearFlag();
    run(useCase, file);
    const auto precedingNewLine{
        find_nth_element(writtenString(writer), 2 - 1, '\n')};
    const auto line_{writtenString(writer).substr(precedingNewLine + 1)};
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        static_cast<
            std::iterator_traits<std::string::iterator>::difference_type>(
            useCase.headingLabels().size()) -
            1,
        std::count(line_.begin(), line_.end(), ','));
}

class OutputFileTests : public ::testing::Test {
  protected:
    WriterStub writer;
    OutputFilePathStub path;
    OutputFileImpl file{writer, path};
    WritingAdaptiveCoordinateResponseTrial
        writingAdaptiveCoordinateResponseTrial;
    WritingFixedLevelCoordinateResponseTrial
        writingFixedLevelCoordinateResponseTrial;
    WritingOpenSetAdaptiveTrial writingOpenSetAdaptiveTrial;
    WritingCorrectKeywordsTrial writingCorrectKeywordsTrial;
    WritingPassFailTrial writingPassFailTrial;
    WritingConsonantTrial writingConsonantTrial;
    WritingFreeResponseTrial writingFreeResponseTrial;
    WritingKeyPressTrial writingKeyPressTrial;
    WritingEmotionTrial writingEmotionTrial;
    WritingThreeKeywordsTrial writingThreeKeywordsTrial;
    WritingSyllableTrial writingSyllableTrial;
    FreeResponseTrial freeResponseTrial;
    BinocularGazeSamples eyeGazes;

    void assertHeadingAtLine(WritingTrial &useCase, gsl::index line) {
        for (auto [headingItem, index] : useCase.headingLabels())
            assertNthCommaDelimitedEntryOfLine(
                writer, headingItem, index, line);
    }

    void assertIncorrectTrialWritesEvaluation(WritingEvaluatedTrial &useCase) {
        useCase.incorrect();
        run(useCase, file);
        assertNthEntryOfSecondLine(
            writer, "incorrect", useCase.evaluationEntryIndex());
    }

    void assertCorrectTrialWritesEvaluation(WritingEvaluatedTrial &useCase) {
        useCase.correct();
        run(useCase, file);
        assertNthEntryOfSecondLine(
            writer, "correct", useCase.evaluationEntryIndex());
    }

    void assertWritesTrialOnLine(WritingTrial &useCase, gsl::index line) {
        run(useCase, file);
        useCase.assertContainsCommaDelimitedTrialOnLine(writer, line);
    }

    void assertWritesHeadingOnFirstLine(WritingTrial &useCase) {
        run(useCase, file);
        assertHeadingAtLine(useCase, 1);
    }

    void assertWritesHeadingTwiceWhenNewFileOpened(WritingTrial &useCase) {
        run(useCase, file);
        openNewFile(file);
        run(useCase, file);
        assertHeadingAtLine(useCase, 3);
    }

    void setGazePositionsRelativeScreenAndEyeTrackerTimes(
        std::vector<std::int_least64_t> t, std::vector<EyeGaze> left,
        std::vector<EyeGaze> right) {
        eyeGazes.resize(t.size());
        std::generate(eyeGazes.begin(), eyeGazes.end(), [&, n = 0]() mutable {
            BinocularGazeSample gazeSamples{{t.at(n)},
                Gaze{GazeOrigin{},
                    GazePosition{{}, Point2D{left.at(n).x, left.at(n).y}}},
                Gaze{GazeOrigin{},
                    GazePosition{{}, Point2D{right.at(n).x, right.at(n).y}}}};
            ++n;
            return gazeSamples;
        });
    }

    void assertWritesTrialOnLineAfterWritingTwice(
        WritingTrial &useCase, gsl::index line) {
        run(useCase, file);
        assertWritesTrialOnLine(useCase, line);
    }
};

#define OUTPUT_FILE_TEST(a) TEST_F(OutputFileTests, a)

OUTPUT_FILE_TEST(parentPathIsParentPath) {
    path.setOutputDirectory("/Users/user/data");
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL("/Users/user/data", file.parentPath());
}

OUTPUT_FILE_TEST(
    writingAdaptiveCoordinateResponseTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingAdaptiveCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(
    writingFixedLevelCoordinateResponseTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingFixedLevelCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(writingFreeResponseTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingFreeResponseTrial);
}

OUTPUT_FILE_TEST(writingKeyPressTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingKeyPressTrial);
}

OUTPUT_FILE_TEST(writingEmotionTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingEmotionTrial);
}

OUTPUT_FILE_TEST(writingThreeKeywordsTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingThreeKeywordsTrial);
}

OUTPUT_FILE_TEST(writingSyllableTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingSyllableTrial);
}

OUTPUT_FILE_TEST(writingCorrectKeywordsTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingCorrectKeywordsTrial);
}

OUTPUT_FILE_TEST(writingPassFailTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingPassFailTrial);
}

OUTPUT_FILE_TEST(writingConsonantTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingConsonantTrial);
}

OUTPUT_FILE_TEST(writingOpenSetAdaptiveTrialWritesHeadingOnFirstLine) {
    assertWritesHeadingOnFirstLine(writingOpenSetAdaptiveTrial);
}

OUTPUT_FILE_TEST(writeKeyPressTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingKeyPressTrial, 2);
}

OUTPUT_FILE_TEST(writeEmotionTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingEmotionTrial, 2);
}

OUTPUT_FILE_TEST(writeAdaptiveCoordinateResponseTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingAdaptiveCoordinateResponseTrial, 2);
}

OUTPUT_FILE_TEST(
    writeFixedLevelCoordinateResponseTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingFixedLevelCoordinateResponseTrial, 2);
}

OUTPUT_FILE_TEST(writingConsonantTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingConsonantTrial, 2);
}

OUTPUT_FILE_TEST(writeFreeResponseTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingFreeResponseTrial, 2);
}

OUTPUT_FILE_TEST(writeThreeKeywordsTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingThreeKeywordsTrial, 2);
}

OUTPUT_FILE_TEST(writeOpenSetAdaptiveTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingOpenSetAdaptiveTrial, 2);
}

OUTPUT_FILE_TEST(writeCorrectKeywordsTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingCorrectKeywordsTrial, 2);
}

OUTPUT_FILE_TEST(writePassFailTrialWritesTrialOnSecondLine) {
    assertWritesTrialOnLine(writingPassFailTrial, 2);
}

OUTPUT_FILE_TEST(
    writeAdaptiveCoordinateResponseTrialTwiceDoesNotWriteHeadingTwice) {
    assertWritesTrialOnLineAfterWritingTwice(
        writingAdaptiveCoordinateResponseTrial, 3);
}

OUTPUT_FILE_TEST(
    writeFixedLevelCoordinateResponseTrialTwiceDoesNotWriteHeadingTwice) {
    assertWritesTrialOnLineAfterWritingTwice(
        writingFixedLevelCoordinateResponseTrial, 3);
}

OUTPUT_FILE_TEST(writingConsonantTrialTwiceDoesNotWriteHeadingTwice) {
    assertWritesTrialOnLineAfterWritingTwice(writingConsonantTrial, 3);
}

OUTPUT_FILE_TEST(writeFreeResponseTrialTwiceDoesNotWriteHeadingTwice) {
    assertWritesTrialOnLineAfterWritingTwice(writingFreeResponseTrial, 3);
}

OUTPUT_FILE_TEST(writeThreeKeywordsTrialTwiceDoesNotWriteHeadingTwice) {
    assertWritesTrialOnLineAfterWritingTwice(writingThreeKeywordsTrial, 3);
}

OUTPUT_FILE_TEST(writeOpenSetAdaptiveTrialTwiceDoesNotWriteHeadingTwice) {
    assertWritesTrialOnLineAfterWritingTwice(writingOpenSetAdaptiveTrial, 3);
}

OUTPUT_FILE_TEST(writeCorrectKeywordsTrialTwiceDoesNotWriteHeadingTwice) {
    assertWritesTrialOnLineAfterWritingTwice(writingCorrectKeywordsTrial, 3);
}

OUTPUT_FILE_TEST(
    writingAdaptiveCoordinateResponseTrialTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    assertWritesHeadingTwiceWhenNewFileOpened(
        writingAdaptiveCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(
    writingConsonantTrialTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    assertWritesHeadingTwiceWhenNewFileOpened(writingConsonantTrial);
}

OUTPUT_FILE_TEST(
    writingFixedLevelCoordinateResponseTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    assertWritesHeadingTwiceWhenNewFileOpened(
        writingFixedLevelCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(
    writingFreeResponseTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    assertWritesHeadingTwiceWhenNewFileOpened(writingFreeResponseTrial);
}

OUTPUT_FILE_TEST(
    writingOpenSetAdaptiveTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    assertWritesHeadingTwiceWhenNewFileOpened(writingOpenSetAdaptiveTrial);
}

OUTPUT_FILE_TEST(
    writingCorrectKeywordsTrialTwiceWritesTrialHeadingTwiceWhenNewFileOpened) {
    assertWritesHeadingTwiceWhenNewFileOpened(writingCorrectKeywordsTrial);
}

OUTPUT_FILE_TEST(writeIncorrectAdaptiveCoordinateResponseTrial) {
    assertIncorrectTrialWritesEvaluation(
        writingAdaptiveCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(writingIncorrectConsonantTrial) {
    assertIncorrectTrialWritesEvaluation(writingConsonantTrial);
}

OUTPUT_FILE_TEST(writeIncorrectFixedLevelCoordinateResponseTrial) {
    assertIncorrectTrialWritesEvaluation(
        writingFixedLevelCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(writeIncorrectOpenSetAdaptiveTrial) {
    assertIncorrectTrialWritesEvaluation(writingOpenSetAdaptiveTrial);
}

OUTPUT_FILE_TEST(writeIncorrectKeywordsTrial) {
    assertIncorrectTrialWritesEvaluation(writingCorrectKeywordsTrial);
}

OUTPUT_FILE_TEST(writeIncorrectPassFailTrial) {
    assertIncorrectTrialWritesEvaluation(writingPassFailTrial);
}

OUTPUT_FILE_TEST(writeIncorrectSyllableTrial) {
    assertIncorrectTrialWritesEvaluation(writingSyllableTrial);
}

OUTPUT_FILE_TEST(writeCorrectAdaptiveCoordinateResponseTrial) {
    assertCorrectTrialWritesEvaluation(writingAdaptiveCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(writingCorrectConsonantTrial) {
    assertCorrectTrialWritesEvaluation(writingConsonantTrial);
}

OUTPUT_FILE_TEST(writeCorrectFixedLevelCoordinateResponseTrial) {
    assertCorrectTrialWritesEvaluation(
        writingFixedLevelCoordinateResponseTrial);
}

OUTPUT_FILE_TEST(writeCorrectOpenSetAdaptiveTrial) {
    assertCorrectTrialWritesEvaluation(writingOpenSetAdaptiveTrial);
}

OUTPUT_FILE_TEST(writeCorrectKeywordsTrialWritesCorrectEvaluation) {
    assertCorrectTrialWritesEvaluation(writingCorrectKeywordsTrial);
}

OUTPUT_FILE_TEST(writeCorrectSyllableTrial) {
    assertCorrectTrialWritesEvaluation(writingSyllableTrial);
}

OUTPUT_FILE_TEST(writeFlaggedFreeResponseTrial) {
    assertWritesFlaggedTrial(file, writer, writingFreeResponseTrial);
}

OUTPUT_FILE_TEST(writeNoFlagFreeResponseTrialOnlyTwoEntries) {
    assertNonFlaggedTrialDoesNotWriteExtraEntry(
        file, writer, writingFreeResponseTrial);
}

OUTPUT_FILE_TEST(writeFlaggedSyllablesTrial) {
    assertWritesFlaggedTrial(file, writer, writingSyllableTrial);
}

OUTPUT_FILE_TEST(writeNoFlagSyllablesTrialDoesNotHaveExtraEntry) {
    assertNonFlaggedTrialDoesNotWriteExtraEntry(
        file, writer, writingSyllableTrial);
}

OUTPUT_FILE_TEST(writeFlaggedThreeKeywordsTrial) {
    assertWritesFlaggedTrial(file, writer, writingThreeKeywordsTrial);
}

OUTPUT_FILE_TEST(writeNoFlagThreeKeywordsTrialDoesNotHaveExtraEntry) {
    assertNonFlaggedTrialDoesNotWriteExtraEntry(
        file, writer, writingThreeKeywordsTrial);
}

OUTPUT_FILE_TEST(uninitializedColorDoesNotBreak) {
    coordinate_response_measure::AdaptiveTrial uninitialized;
    file.write(uninitialized);
}

OUTPUT_FILE_TEST(writeAdaptiveTestResult) {
    AdaptiveTestResults results{};
    results.push_back({{"a"}, 1.});
    results.push_back({{"b"}, 2.});
    results.push_back({{"c"}, 3.});
    file.write(results);
    assertContainsColonDelimitedEntry(writer, "a", "1");
    assertContainsColonDelimitedEntry(writer, "b", "2");
    assertContainsColonDelimitedEntry(writer, "c", "3");
}

OUTPUT_FILE_TEST(writeUmlAdaptiveTestResult) {
    AdaptiveTestResults results{};
    results.push_back({{"a"}, Phi{1., 2., 3., 4.}});
    results.push_back({{"b"}, Phi{3., 1., 2., 4.}});
    file.write(results);
    assertContainsColonDelimitedEntry(writer, "a", "1 2 3 4");
    assertContainsColonDelimitedEntry(writer, "b", "3 1 2 4");
}

OUTPUT_FILE_TEST(writeGazePositionsRelativeScreenAndEyeTrackerTime) {
    setGazePositionsRelativeScreenAndEyeTrackerTimes({1, 2, 3},
        {{0.4F, 0.44F}, {0.5F, 0.55F}, {0.6F, 0.66F}},
        {{0.7F, 0.77F}, {0.8F, 0.88F}, {0.9F, 0.99F}});
    write(file, eyeGazes);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::eyeTrackerTime, 1, 1);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::leftGazePositionRelativeScreen, 2, 1);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::rightGazePositionRelativeScreen, 3, 1);
    assertNthCommaDelimitedEntryOfLine(writer, "1", 1, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "0.4 0.44", 2, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "0.7 0.77", 3, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "2", 1, 3);
    assertNthCommaDelimitedEntryOfLine(writer, "0.5 0.55", 2, 3);
    assertNthCommaDelimitedEntryOfLine(writer, "0.8 0.88", 3, 3);
    assertNthCommaDelimitedEntryOfLine(writer, "3", 1, 4);
    assertNthCommaDelimitedEntryOfLine(writer, "0.6 0.66", 2, 4);
    assertNthCommaDelimitedEntryOfLine(writer, "0.9 0.99", 3, 4);
}

OUTPUT_FILE_TEST(writeGazePositionsRelativeTracker) {
    std::vector<Point3D> left{
        {0.4F, 0.44F, 0.444F}, {0.5, 0.55F, 0.555F}, {0.6F, 0.66F, 0.666F}};
    std::vector<Point3D> right{
        {0.7F, 0.77F, 0.777F}, {0.8F, 0.88F, 0.888F}, {0.9F, 0.99F, 0.999F}};
    eyeGazes.resize(left.size());
    std::generate(eyeGazes.begin(), eyeGazes.end(), [&, n = 0]() mutable {
        BinocularGazeSample gazeSamples{{},
            Gaze{GazeOrigin{}, GazePosition{left.at(n), {}}},
            Gaze{GazeOrigin{}, GazePosition{right.at(n), {}}}};
        ++n;
        return gazeSamples;
    });
    write(file, eyeGazes);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::leftGazePositionRelativeTracker, 4, 1);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::rightGazePositionRelativeTracker, 5, 1);
    assertNthCommaDelimitedEntryOfLine(writer, "0.4 0.44 0.444", 4, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "0.7 0.77 0.777", 5, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "0.5 0.55 0.555", 4, 3);
    assertNthCommaDelimitedEntryOfLine(writer, "0.8 0.88 0.888", 5, 3);
    assertNthCommaDelimitedEntryOfLine(writer, "0.6 0.66 0.666", 4, 4);
    assertNthCommaDelimitedEntryOfLine(writer, "0.9 0.99 0.999", 5, 4);
}

OUTPUT_FILE_TEST(writeGazeOrigins) {
    std::vector<Point3D> left{
        {0.4F, 0.44F, 0.444F}, {0.5, 0.55F, 0.555F}, {0.6F, 0.66F, 0.666F}};
    std::vector<Point3D> right{
        {0.7F, 0.77F, 0.777F}, {0.8F, 0.88F, 0.888F}, {0.9F, 0.99F, 0.999F}};
    eyeGazes.resize(left.size());
    std::generate(eyeGazes.begin(), eyeGazes.end(), [&, n = 0]() mutable {
        BinocularGazeSample gazeSamples{{}, Gaze{GazeOrigin{left.at(n)}, {}},
            Gaze{GazeOrigin{right.at(n)}, {}}};
        ++n;
        return gazeSamples;
    });
    write(file, eyeGazes);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::leftGazeOriginRelativeTracker, 6, 1);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::rightGazeOriginRelativeTracker, 7, 1);
    assertNthCommaDelimitedEntryOfLine(writer, "0.4 0.44 0.444", 6, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "0.7 0.77 0.777", 7, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "0.5 0.55 0.555", 6, 3);
    assertNthCommaDelimitedEntryOfLine(writer, "0.8 0.88 0.888", 7, 3);
    assertNthCommaDelimitedEntryOfLine(writer, "0.6 0.66 0.666", 6, 4);
    assertNthCommaDelimitedEntryOfLine(writer, "0.9 0.99 0.999", 7, 4);
}

OUTPUT_FILE_TEST(writeGazeValidities) {
    BinocularGazeSample a;
    a.left.origin.valid = true;
    a.left.position.valid = false;
    a.right.origin.valid = false;
    a.right.position.valid = true;
    BinocularGazeSamples samples = {a};
    write(file, samples);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::leftGazePositionRelativeScreenIsValid, 8, 1);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::rightGazePositionRelativeScreenIsValid, 9, 1);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::leftGazePositionRelativeTrackerIsValid, 10, 1);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::rightGazePositionRelativeTrackerIsValid, 11, 1);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::leftGazeOriginRelativeTrackerIsValid, 12, 1);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::rightGazeOriginRelativeTrackerIsValid, 13, 1);
    assertNthCommaDelimitedEntryOfLine(writer, "n", 8, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "y", 9, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "n", 10, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "y", 11, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "y", 12, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "n", 13, 2);
}

OUTPUT_FILE_TEST(writeTargetStartTime) {
    writeTargetStartTimeNanoseconds(file, 1);
    assertContainsColonDelimitedEntry(writer, "target start time (ns)", "1");
}

OUTPUT_FILE_TEST(writeEyeTrackerTargetPlayerSynchronization) {
    EyeTrackerTargetPlayerSynchronization s{};
    s.eyeTrackerSystemTime.microseconds = 1;
    s.targetPlayerSystemTime.nanoseconds = 2;
    file.write(s);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::eyeTrackerTime, 1, 1);
    assertNthCommaDelimitedEntryOfLine(
        writer, HeadingItem::targetPlayerTime, 2, 1);
    assertNthCommaDelimitedEntryOfLine(writer, "1", 1, 2);
    assertNthCommaDelimitedEntryOfLine(writer, "2", 2, 2);
}

OUTPUT_FILE_TEST(openPassesFormattedFilePath) {
    path.setFileName("a");
    path.setOutputDirectory("b");
    openNewFile(file);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(std::string{"b/a.txt"}, writer.filePath());
}

OUTPUT_FILE_TEST(closeClosesWriter) {
    file.close();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(writer.closed());
}

OUTPUT_FILE_TEST(saveSavesWriter) {
    file.save();
    AV_SPEECH_IN_NOISE_EXPECT_TRUE(writer.saved());
}

OUTPUT_FILE_TEST(openPassesTestInformation) {
    TestIdentity testIdentity;
    openNewFile(file, testIdentity);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(testIdentity), path.testIdentity());
}

OUTPUT_FILE_TEST(writeWritablePassWritable) {
    WritableStub writable;
    file.write(writable);
    AV_SPEECH_IN_NOISE_EXPECT_EQUAL(
        &std::as_const(writable), writer.writable());
}

OUTPUT_FILE_TEST(writeWritableWritesNewLine) {
    WritableStub writable;
    file.write(writable);
    assertEndsWith(writer, "\n");
}

class FailingWriter : public Writer {
    bool failed_{};

  public:
    void open(const std::string &) override { failed_ = true; }

    auto failed() -> bool override { return failed_; }

    void close() override {}
    void write(const std::string &) override {}
    void write(Writable &) override {}
    void save() override {}
};

TEST(FailingOutputFileTests, openThrowsOpenFailureWhenWriterFails) {
    FailingWriter writer;
    OutputFilePathStub path;
    OutputFileImpl file{writer, path};
    try {
        openNewFile(file);
        FAIL() << "Expected OutputFileImpl::OpenFailure";
    } catch (const OutputFileImpl::OpenFailure &) {
    }
}
}
}
