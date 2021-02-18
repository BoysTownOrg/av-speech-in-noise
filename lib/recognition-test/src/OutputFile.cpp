#include "OutputFile.hpp"
#include <av-speech-in-noise/name.hpp>
#include <av-speech-in-noise/Interface.hpp>
#include <sstream>
#include <algorithm>

namespace av_speech_in_noise {
enum class OutputFileImpl::Trial {
    FixedLevelCoordinateResponse,
    AdaptiveCoordinateResponse,
    FreeResponse,
    OpenSetAdaptive,
    CorrectKeywords,
    Consonant,
    ThreeKeywords,
    Syllable,
    none
};

static auto operator<<(std::ostream &os, const std::vector<int> &v)
    -> std::ostream & {
    if (!v.empty()) {
        auto first{true};
        os << v.front();
        for (auto x : v) {
            if (first) {
                first = false;
                continue;
            }
            os << " ";
            os << x;
        }
    }
    return os;
}

static auto operator<<(std::ostream &os, HeadingItem item) { os << name(item); }

static auto operator<<(std::ostream &os, Syllable item) { os << name(item); }

template <typename T> void insert(std::stringstream &stream, T item) {
    stream << item;
}

static void insertCommaAndSpace(std::stringstream &stream) {
    insert(stream, ", ");
}

static void insertNewLine(std::stringstream &stream) { insert(stream, '\n'); }

template <typename T>
void writeLabeledLine(
    std::stringstream &stream, const std::string &label, T thing) {
    insert(stream, label);
    insert(stream, ": ");
    insert(stream, thing);
    insertNewLine(stream);
}

static auto string(const std::stringstream &stream) -> std::string {
    return stream.str();
}

static void writeSubjectId(std::stringstream &stream, const TestIdentity &p) {
    writeLabeledLine(stream, "subject", p.subjectId);
}

static void writeTester(std::stringstream &stream, const TestIdentity &p) {
    writeLabeledLine(stream, "tester", p.testerId);
}

static void writeSession(std::stringstream &stream, const TestIdentity &p) {
    writeLabeledLine(stream, "session", p.session);
}

static void writeMethod(std::stringstream &stream, const TestIdentity &p) {
    writeLabeledLine(stream, "method", p.method);
}

static void writeRmeSetting(std::stringstream &stream, const TestIdentity &p) {
    writeLabeledLine(stream, "RME setting", p.rmeSetting);
}

static void writeTransducer(std::stringstream &stream, const TestIdentity &p) {
    writeLabeledLine(stream, "transducer", p.transducer);
}

static void writeMasker(std::stringstream &stream, const Test &p) {
    writeLabeledLine(stream, "masker", p.maskerFileUrl.path);
}

static void writeTargetPlaylist(std::stringstream &stream, const Test &p) {
    writeLabeledLine(stream, "targets", p.targetsUrl.path);
}

static void writeMaskerLevel(std::stringstream &stream, const Test &p) {
    writeLabeledLine(stream, "masker level (dB SPL)", p.maskerLevel.dB_SPL);
}

static void writeCondition(std::stringstream &stream, const Test &p) {
    writeLabeledLine(stream, "condition", name(p.condition));
}

constexpr auto correct{"correct"};
constexpr auto incorrect{"incorrect"};

static auto evaluation(bool b) -> std::string {
    return b ? correct : incorrect;
}

static auto evaluation(const open_set::AdaptiveTrial &trial) -> std::string {
    return evaluation(trial.correct);
}

static auto evaluation(const ConsonantTrial &trial) -> std::string {
    return evaluation(trial.correct);
}

static auto evaluation(const coordinate_response_measure::Trial &trial)
    -> std::string {
    return evaluation(trial.correct);
}

static auto evaluation(const SyllableTrial &trial) -> std::string {
    return evaluation(trial.correct);
}

static auto identity(const Test &test) -> TestIdentity { return test.identity; }

static void write(std::stringstream &stream, const TestIdentity &identity) {
    writeSubjectId(stream, identity);
    writeTester(stream, identity);
    writeSession(stream, identity);
    writeMethod(stream, identity);
    writeRmeSetting(stream, identity);
    writeTransducer(stream, identity);
}

static auto format(const AdaptiveTest &test) -> std::string {
    std::stringstream stream;
    write(stream, identity(test));
    writeMasker(stream, test);
    writeTargetPlaylist(stream, test);
    writeMaskerLevel(stream, test);
    writeLabeledLine(stream, "starting SNR (dB)", test.startingSnr.dB);
    writeCondition(stream, test);
    std::vector<int> up;
    std::vector<int> down;
    std::vector<int> runCounts;
    std::vector<int> stepSizes;
    for (auto sequence : test.trackingRule) {
        up.push_back(sequence.up);
        down.push_back(sequence.down);
        runCounts.push_back(sequence.runCount);
        stepSizes.push_back(sequence.stepSize);
    }
    writeLabeledLine(stream, "up", up);
    writeLabeledLine(stream, "down", down);
    writeLabeledLine(stream, "reversals per step size", runCounts);
    writeLabeledLine(stream, "step sizes (dB)", stepSizes);
    writeLabeledLine(stream, "threshold reversals", test.thresholdReversals);
    insertNewLine(stream);
    return string(stream);
}

static auto format(const FixedLevelTest &test) -> std::string {
    std::stringstream stream;
    write(stream, identity(test));
    writeMasker(stream, test);
    writeTargetPlaylist(stream, test);
    writeMaskerLevel(stream, test);
    writeLabeledLine(stream, "SNR (dB)", test.snr.dB);
    writeCondition(stream, test);
    insertNewLine(stream);
    return string(stream);
}

static auto format(const BinocularGazeSamples &gazeSamples) -> std::string {
    std::stringstream stream;
    insert(stream, name(HeadingItem::eyeTrackerTime));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::leftGaze));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::rightGaze));
    std::for_each(gazeSamples.begin(), gazeSamples.end(), [&](auto g) {
        insertNewLine(stream);
        insert(stream, g.systemTime.microseconds);
        insertCommaAndSpace(stream);
        insert(stream, g.left.x);
        insert(stream, " ");
        insert(stream, g.left.y);
        insertCommaAndSpace(stream);
        insert(stream, g.right.x);
        insert(stream, " ");
        insert(stream, g.right.y);
    });
    insertNewLine(stream);
    return stream.str();
}

static auto format(TargetStartTime t) -> std::string {
    std::stringstream stream;
    writeLabeledLine(stream, "target start time (ns)", t.nanoseconds);
    return stream.str();
}

static auto format(const EyeTrackerTargetPlayerSynchronization &s)
    -> std::string {
    std::stringstream stream;
    insert(stream, HeadingItem::eyeTrackerTime);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::targetPlayerTime);
    insertNewLine(stream);
    insert(stream, s.eyeTrackerSystemTime.microseconds);
    insertCommaAndSpace(stream);
    insert(stream, s.targetPlayerSystemTime.nanoseconds);
    insertNewLine(stream);
    return stream.str();
}

static auto format(const AdaptiveTestResult &result) -> std::string {
    std::stringstream stream;
    writeLabeledLine(
        stream, "threshold for " + result.targetsUrl.path, result.threshold);
    return string(stream);
}

namespace {
class TrialFormatter {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TrialFormatter);
    virtual auto heading() -> std::string = 0;
    virtual auto trial() -> std::string = 0;
};

class FixedLevelCoordinateResponseTrialFormatter : public TrialFormatter {
  public:
    explicit FixedLevelCoordinateResponseTrialFormatter(
        const coordinate_response_measure::FixedLevelTrial &trial_)
        : trial_{trial_} {}

    auto heading() -> std::string override {
        std::stringstream stream;
        insert(stream, HeadingItem::correctNumber);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::subjectNumber);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::correctColor);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::subjectColor);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::evaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::target);
        insertNewLine(stream);
        return string(stream);
    }

    auto trial() -> std::string override {
        std::stringstream stream;
        insert(stream, trial_.correctNumber);
        insertCommaAndSpace(stream);
        insert(stream, trial_.subjectNumber);
        insertCommaAndSpace(stream);
        insert(stream, name(trial_.correctColor));
        insertCommaAndSpace(stream);
        insert(stream, name(trial_.subjectColor));
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_));
        insertCommaAndSpace(stream);
        insert(stream, trial_.target);
        insertNewLine(stream);
        return string(stream);
    }

  private:
    const coordinate_response_measure::FixedLevelTrial &trial_;
};

class AdaptiveCoordinateResponseTrialFormatter : public TrialFormatter {
  public:
    explicit AdaptiveCoordinateResponseTrialFormatter(
        const coordinate_response_measure::AdaptiveTrial &trial_)
        : trial_{trial_} {}

    auto heading() -> std::string override {
        std::stringstream stream;
        insert(stream, HeadingItem::snr_dB);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::correctNumber);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::subjectNumber);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::correctColor);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::subjectColor);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::evaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::reversals);
        insertNewLine(stream);
        return string(stream);
    }

    auto trial() -> std::string override {
        std::stringstream stream;
        insert(stream, trial_.snr.dB);
        insertCommaAndSpace(stream);
        insert(stream, trial_.correctNumber);
        insertCommaAndSpace(stream);
        insert(stream, trial_.subjectNumber);
        insertCommaAndSpace(stream);
        insert(stream, name(trial_.correctColor));
        insertCommaAndSpace(stream);
        insert(stream, name(trial_.subjectColor));
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_));
        insertCommaAndSpace(stream);
        insert(stream, trial_.reversals);
        insertNewLine(stream);
        return string(stream);
    }

  private:
    const coordinate_response_measure::AdaptiveTrial &trial_;
};

class FreeResponseTrialFormatter : public TrialFormatter {
  public:
    explicit FreeResponseTrialFormatter(const FreeResponseTrial &trial_)
        : trial_{trial_} {}

    auto heading() -> std::string override {
        std::stringstream stream;
        insert(stream, HeadingItem::target);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::freeResponse);
        insertNewLine(stream);
        return string(stream);
    }

    auto trial() -> std::string override {
        std::stringstream stream;
        insert(stream, trial_.target);
        insertCommaAndSpace(stream);
        insert(stream, trial_.response);
        if (trial_.flagged) {
            insertCommaAndSpace(stream);
            insert(stream, "FLAGGED");
        }
        insertNewLine(stream);
        return string(stream);
    }

  private:
    const FreeResponseTrial &trial_;
};

class OpenSetAdaptiveTrialFormatter : public TrialFormatter {
  public:
    explicit OpenSetAdaptiveTrialFormatter(
        const open_set::AdaptiveTrial &trial_)
        : trial_{trial_} {}

    auto heading() -> std::string override {
        std::stringstream stream;
        insert(stream, HeadingItem::snr_dB);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::target);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::evaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::reversals);
        insertNewLine(stream);
        return string(stream);
    }

    auto trial() -> std::string override {
        std::stringstream stream;
        insert(stream, trial_.snr.dB);
        insertCommaAndSpace(stream);
        insert(stream, trial_.target);
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_));
        insertCommaAndSpace(stream);
        insert(stream, trial_.reversals);
        insertNewLine(stream);
        return string(stream);
    }

  private:
    const open_set::AdaptiveTrial &trial_;
};

class CorrectKeywordsTrialFormatter : public TrialFormatter {
  public:
    explicit CorrectKeywordsTrialFormatter(const CorrectKeywordsTrial &trial_)
        : trial_{trial_} {}

    auto heading() -> std::string override {
        std::stringstream stream;
        insert(stream, HeadingItem::snr_dB);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::target);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::correctKeywords);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::evaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::reversals);
        insertNewLine(stream);
        return string(stream);
    }

    auto trial() -> std::string override {
        std::stringstream stream;
        insert(stream, trial_.snr.dB);
        insertCommaAndSpace(stream);
        insert(stream, trial_.target);
        insertCommaAndSpace(stream);
        insert(stream, trial_.count);
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_));
        insertCommaAndSpace(stream);
        insert(stream, trial_.reversals);
        insertNewLine(stream);
        return string(stream);
    }

  private:
    const CorrectKeywordsTrial &trial_;
};

class ConsonantTrialFormatter : public TrialFormatter {
  public:
    explicit ConsonantTrialFormatter(const ConsonantTrial &trial_)
        : trial_{trial_} {}

    auto heading() -> std::string override {
        std::stringstream stream;
        insert(stream, HeadingItem::correctConsonant);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::subjectConsonant);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::evaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::target);
        insertNewLine(stream);
        return string(stream);
    }

    auto trial() -> std::string override {
        std::stringstream stream;
        insert(stream, trial_.correctConsonant);
        insertCommaAndSpace(stream);
        insert(stream, trial_.subjectConsonant);
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_));
        insertCommaAndSpace(stream);
        insert(stream, trial_.target);
        insertNewLine(stream);
        return string(stream);
    }

  private:
    const ConsonantTrial &trial_;
};

class ThreeKeywordsTrialFormatter : public TrialFormatter {
  public:
    explicit ThreeKeywordsTrialFormatter(const ThreeKeywordsTrial &trial_)
        : trial_{trial_} {}

    auto heading() -> std::string override {
        std::stringstream stream;
        insert(stream, HeadingItem::target);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::firstKeywordEvaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::secondKeywordEvaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::thirdKeywordEvaluation);
        insertNewLine(stream);
        return string(stream);
    }

    auto trial() -> std::string override {
        std::stringstream stream;
        insert(stream, trial_.target);
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_.firstCorrect));
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_.secondCorrect));
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_.thirdCorrect));
        insertNewLine(stream);
        return string(stream);
    }

  private:
    const ThreeKeywordsTrial &trial_;
};

class SyllableTrialFormatter : public TrialFormatter {
  public:
    explicit SyllableTrialFormatter(const SyllableTrial &trial_)
        : trial_{trial_} {}

    auto heading() -> std::string override {
        std::stringstream stream;
        insert(stream, HeadingItem::correctSyllable);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::subjectSyllable);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::evaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::target);
        insertNewLine(stream);
        return string(stream);
    }

    auto trial() -> std::string override {
        std::stringstream stream;
        insert(stream, trial_.correctSyllable);
        insertCommaAndSpace(stream);
        insert(stream, trial_.subjectSyllable);
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_));
        insertCommaAndSpace(stream);
        insert(stream, trial_.target);
        if (trial_.flagged) {
            insertCommaAndSpace(stream);
            insert(stream, "FLAGGED");
        }
        insertNewLine(stream);
        return string(stream);
    }

  private:
    const SyllableTrial &trial_;
};
}

static void write(Writer &writer, const std::string &s) { writer.write(s); }

static void write(Writer &writer, TrialFormatter &formatter,
    OutputFileImpl::Trial &currentTrial, OutputFileImpl::Trial trial) {
    if (currentTrial != trial)
        write(writer, formatter.heading());
    write(writer, formatter.trial());
    currentTrial = trial;
}

OutputFileImpl::OutputFileImpl(Writer &writer, OutputFilePath &path)
    : writer{writer}, path{path}, currentTrial{Trial::none} {}

void OutputFileImpl::write(const std::string &s) { writer.write(s); }

void OutputFileImpl::write(
    const coordinate_response_measure::AdaptiveTrial &trial) {
    AdaptiveCoordinateResponseTrialFormatter formatter{trial};
    av_speech_in_noise::write(
        writer, formatter, currentTrial, Trial::AdaptiveCoordinateResponse);
}

void OutputFileImpl::write(
    const coordinate_response_measure::FixedLevelTrial &trial) {
    FixedLevelCoordinateResponseTrialFormatter formatter{trial};
    av_speech_in_noise::write(
        writer, formatter, currentTrial, Trial::FixedLevelCoordinateResponse);
}

void OutputFileImpl::write(const FreeResponseTrial &trial) {
    FreeResponseTrialFormatter formatter{trial};
    av_speech_in_noise::write(
        writer, formatter, currentTrial, Trial::FreeResponse);
}

void OutputFileImpl::write(const CorrectKeywordsTrial &trial) {
    CorrectKeywordsTrialFormatter formatter{trial};
    av_speech_in_noise::write(
        writer, formatter, currentTrial, Trial::CorrectKeywords);
}

void OutputFileImpl::write(const ConsonantTrial &trial) {
    ConsonantTrialFormatter formatter{trial};
    av_speech_in_noise::write(
        writer, formatter, currentTrial, Trial::Consonant);
}

void OutputFileImpl::write(const open_set::AdaptiveTrial &trial) {
    OpenSetAdaptiveTrialFormatter formatter{trial};
    av_speech_in_noise::write(
        writer, formatter, currentTrial, Trial::OpenSetAdaptive);
}

void OutputFileImpl::write(const ThreeKeywordsTrial &trial) {
    ThreeKeywordsTrialFormatter formatter{trial};
    av_speech_in_noise::write(
        writer, formatter, currentTrial, Trial::ThreeKeywords);
}

void OutputFileImpl::write(const SyllableTrial &trial) {
    SyllableTrialFormatter formatter{trial};
    av_speech_in_noise::write(writer, formatter, currentTrial, Trial::Syllable);
}

void OutputFileImpl::write(const AdaptiveTest &test) { write(format(test)); }

void OutputFileImpl::write(const FixedLevelTest &test) { write(format(test)); }

void OutputFileImpl::write(const BinocularGazeSamples &gazeSamples) {
    write(format(gazeSamples));
}

void OutputFileImpl::write(TargetStartTime t) { write(format(t)); }

void OutputFileImpl::write(const EyeTrackerTargetPlayerSynchronization &s) {
    write(format(s));
}

void OutputFileImpl::openNewFile(const TestIdentity &test) {
    writer.open(generateNewFilePath(test));
    if (writer.failed())
        throw OpenFailure{};
    currentTrial = Trial::none;
}

auto OutputFileImpl::generateNewFilePath(const TestIdentity &test)
    -> std::string {
    return path.outputDirectory() + "/" + path.generateFileName(test) + ".txt";
}

void OutputFileImpl::close() { writer.close(); }

void OutputFileImpl::save() { writer.save(); }

void OutputFileImpl::write(const AdaptiveTestResults &results) {
    for (const auto &result : results)
        write(format(result));
}
}
