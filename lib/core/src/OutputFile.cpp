#include "OutputFile.hpp"
#include "IOutputFile.hpp"

#include <av-speech-in-noise/Interface.hpp>

#include <sstream>
#include <ostream>
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
    KeyPress,
    Emotion,
    PassFail,
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

static auto operator<<(std::ostream &os, HeadingItem item) -> std::ostream & {
    return os << name(item);
}

static auto operator<<(std::ostream &os, Consonant item) -> std::ostream & {
    return os << name(item);
}

static auto operator<<(std::ostream &os, Syllable item) -> std::ostream & {
    return os << name(item);
}

static auto operator<<(std::ostream &os, KeyPressed item) -> std::ostream & {
    return os << name(item);
}

static auto operator<<(std::ostream &os, Emotion item) -> std::ostream & {
    return os << name(item);
}

static auto operator<<(std::ostream &os, Point3D point) -> std::ostream & {
    return os << point.x << ' ' << point.y << ' ' << point.z;
}

static auto operator<<(std::ostream &os, Point2D point) -> std::ostream & {
    return os << point.x << ' ' << point.y;
}

template <typename T>
auto insert(std::ostream &stream, T item) -> std::ostream & {
    return stream << item;
}

static auto insertCommaAndSpace(std::ostream &stream) -> std::ostream & {
    return insert(stream, ", ");
}

static auto insertNewLine(std::ostream &stream) -> std::ostream & {
    return insert(stream, '\n');
}

template <typename T>
auto insertLabeledLine(std::ostream &stream, const std::string &label, T thing)
    -> std::ostream & {
    return insertNewLine(insert(insert(insert(stream, label), ": "), thing));
}

static auto string(const std::stringstream &stream) -> std::string {
    return stream.str();
}

static auto insertSubjectId(std::ostream &stream, const TestIdentity &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "subject", p.subjectId);
}

static auto insertTester(std::ostream &stream, const TestIdentity &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "tester", p.testerId);
}

static auto insertSession(std::ostream &stream, const TestIdentity &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "session", p.session);
}

static auto insertMethod(std::ostream &stream, const TestIdentity &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "method", p.method);
}

static auto insertRmeSetting(std::ostream &stream, const TestIdentity &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "RME setting", p.rmeSetting);
}

static auto insertTransducer(std::ostream &stream, const TestIdentity &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "transducer", p.transducer);
}

static auto insertMasker(std::ostream &stream, const Test &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "masker", p.maskerFileUrl.path);
}

static auto insertTargetPlaylist(std::ostream &stream, const Test &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "targets", p.targetsUrl.path);
}

static auto insertMaskerLevel(std::ostream &stream, const Test &p)
    -> std::ostream & {
    return insertLabeledLine(
        stream, "masker level (dB SPL)", p.maskerLevel.dB_SPL);
}

static auto insertCondition(std::ostream &stream, const Test &p)
    -> std::ostream & {
    return insertLabeledLine(stream, "condition", name(p.condition));
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

static auto operator<<(std::ostream &stream, const TestIdentity &identity)
    -> std::ostream & {
    return insertTransducer(
        insertRmeSetting(
            insertMethod(
                insertSession(
                    insertTester(insertSubjectId(stream, identity), identity),
                    identity),
                identity),
            identity),
        identity);
}

static auto operator<<(std::ostream &stream, const AdaptiveTest &test)
    -> std::ostream & {
    stream << identity(test);
    insertMasker(stream, test);
    insertTargetPlaylist(stream, test);
    insertMaskerLevel(stream, test);
    insertLabeledLine(stream, "starting SNR (dB)", test.startingSnr.dB);
    insertCondition(stream, test);
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
    insertLabeledLine(stream, "up", up);
    insertLabeledLine(stream, "down", down);
    insertLabeledLine(stream, "reversals per step size", runCounts);
    insertLabeledLine(stream, "step sizes (dB)", stepSizes);
    insertLabeledLine(stream, "threshold reversals", test.thresholdReversals);
    return insertNewLine(stream);
}

static auto operator<<(std::ostream &stream, const FixedLevelTest &test)
    -> std::ostream & {
    stream << identity(test);
    insertMasker(stream, test);
    insertTargetPlaylist(stream, test);
    insertMaskerLevel(stream, test);
    insertLabeledLine(stream, "SNR (dB)", test.snr.dB);
    insertCondition(stream, test);
    return insertNewLine(stream);
}

static auto operator<<(std::ostream &stream,
    const BinocularGazeSamples &gazeSamples) -> std::ostream & {
    insert(stream, name(HeadingItem::eyeTrackerTime));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::leftGazePositionRelativeScreen));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::rightGazePositionRelativeScreen));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::leftGazePositionRelativeTracker));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::rightGazePositionRelativeTracker));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::leftGazeOriginRelativeTracker));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::rightGazeOriginRelativeTracker));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::leftGazePositionRelativeScreenIsValid));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::rightGazePositionRelativeScreenIsValid));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::leftGazePositionRelativeTrackerIsValid));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::rightGazePositionRelativeTrackerIsValid));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::leftGazeOriginRelativeTrackerIsValid));
    insertCommaAndSpace(stream);
    insert(stream, name(HeadingItem::rightGazeOriginRelativeTrackerIsValid));
    std::for_each(gazeSamples.begin(), gazeSamples.end(),
        [&](const BinocularGazeSample &g) {
            insertNewLine(stream);
            insert(stream, g.systemTime.microseconds);
            insertCommaAndSpace(stream);
            insert(stream, g.left.position.relativeScreen);
            insertCommaAndSpace(stream);
            insert(stream, g.right.position.relativeScreen);
            insertCommaAndSpace(stream);
            insert(stream, g.left.position.relativeTrackbox);
            insertCommaAndSpace(stream);
            insert(stream, g.right.position.relativeTrackbox);
            insertCommaAndSpace(stream);
            insert(stream, g.left.origin.relativeTrackbox);
            insertCommaAndSpace(stream);
            insert(stream, g.right.origin.relativeTrackbox);
            insertCommaAndSpace(stream);
            stream << (g.left.position.valid ? 'y' : 'n');
            insertCommaAndSpace(stream);
            stream << (g.right.position.valid ? 'y' : 'n');
            insertCommaAndSpace(stream);
            stream << (g.left.position.valid ? 'y' : 'n');
            insertCommaAndSpace(stream);
            stream << (g.right.position.valid ? 'y' : 'n');
            insertCommaAndSpace(stream);
            stream << (g.left.origin.valid ? 'y' : 'n');
            insertCommaAndSpace(stream);
            stream << (g.right.origin.valid ? 'y' : 'n');
        });
    return insertNewLine(stream);
}

static auto operator<<(std::ostream &stream, TargetStartTime t)
    -> std::ostream & {
    return insertLabeledLine(stream, "target start time (ns)", t.nanoseconds);
}

static auto operator<<(std::ostream &stream,
    const EyeTrackerTargetPlayerSynchronization &s) -> std::ostream & {
    insert(stream, HeadingItem::eyeTrackerTime);
    insertCommaAndSpace(stream);
    insert(stream, HeadingItem::targetPlayerTime);
    insertNewLine(stream);
    insert(stream, s.eyeTrackerSystemTime.microseconds);
    insertCommaAndSpace(stream);
    insert(stream, s.targetPlayerSystemTime.nanoseconds);
    return insertNewLine(stream);
}

static auto operator<<(std::ostream &stream, const AdaptiveTestResult &result)
    -> std::ostream & {
    return insertLabeledLine(
        stream, "threshold for " + result.targetsUrl.path, result.threshold);
}

static auto operator<<(std::ostream &stream, const Flaggable &flaggable)
    -> std::ostream & {
    if (flaggable.flagged) {
        insertCommaAndSpace(stream);
        insert(stream, "FLAGGED");
    }
    return stream;
}

namespace {
class TrialFormatter {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TrialFormatter);
    virtual auto insertHeading(std::ostream &s) -> std::ostream & = 0;
    virtual auto insertTrial(std::ostream &s) -> std::ostream & = 0;
};

class FixedLevelCoordinateResponseTrialFormatter : public TrialFormatter {
  public:
    explicit FixedLevelCoordinateResponseTrialFormatter(
        const coordinate_response_measure::FixedLevelTrial &trial_)
        : trial_{trial_} {}

    auto insertHeading(std::ostream &stream) -> std::ostream & override {
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
        return insertNewLine(stream);
    }

    auto insertTrial(std::ostream &stream) -> std::ostream & override {
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
        return insertNewLine(stream);
    }

  private:
    const coordinate_response_measure::FixedLevelTrial &trial_;
};

class AdaptiveCoordinateResponseTrialFormatter : public TrialFormatter {
  public:
    explicit AdaptiveCoordinateResponseTrialFormatter(
        const coordinate_response_measure::AdaptiveTrial &trial_)
        : trial_{trial_} {}

    auto insertHeading(std::ostream &stream) -> std::ostream & override {
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
        return insertNewLine(stream);
    }

    auto insertTrial(std::ostream &stream) -> std::ostream & override {
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
        return insertNewLine(stream);
    }

  private:
    const coordinate_response_measure::AdaptiveTrial &trial_;
};

class FreeResponseTrialFormatter : public TrialFormatter {
  public:
    explicit FreeResponseTrialFormatter(const FreeResponseTrial &trial_)
        : trial_{trial_} {}

    auto insertHeading(std::ostream &stream) -> std::ostream & override {
        insert(stream, HeadingItem::time);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::target);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::freeResponse);
        return insertNewLine(stream);
    }

    auto insertTrial(std::ostream &stream) -> std::ostream & override {
        insert(stream, trial_.time);
        insertCommaAndSpace(stream);
        insert(stream, trial_.target);
        insertCommaAndSpace(stream);
        insert(stream, trial_.response);
        stream << static_cast<const Flaggable &>(trial_);
        return insertNewLine(stream);
    }

  private:
    const FreeResponseTrial &trial_;
};

class OpenSetAdaptiveTrialFormatter : public TrialFormatter {
  public:
    explicit OpenSetAdaptiveTrialFormatter(
        const open_set::AdaptiveTrial &trial_)
        : trial_{trial_} {}

    auto insertHeading(std::ostream &stream) -> std::ostream & override {
        insert(stream, HeadingItem::snr_dB);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::target);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::evaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::reversals);
        return insertNewLine(stream);
    }

    auto insertTrial(std::ostream &stream) -> std::ostream & override {
        insert(stream, trial_.snr.dB);
        insertCommaAndSpace(stream);
        insert(stream, trial_.target);
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_));
        insertCommaAndSpace(stream);
        insert(stream, trial_.reversals);
        return insertNewLine(stream);
    }

  private:
    const open_set::AdaptiveTrial &trial_;
};

class CorrectKeywordsTrialFormatter : public TrialFormatter {
  public:
    explicit CorrectKeywordsTrialFormatter(const CorrectKeywordsTrial &trial_)
        : trial_{trial_} {}

    auto insertHeading(std::ostream &stream) -> std::ostream & override {
        insert(stream, HeadingItem::snr_dB);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::target);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::correctKeywords);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::evaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::reversals);
        return insertNewLine(stream);
    }

    auto insertTrial(std::ostream &stream) -> std::ostream & override {
        insert(stream, trial_.snr.dB);
        insertCommaAndSpace(stream);
        insert(stream, trial_.target);
        insertCommaAndSpace(stream);
        insert(stream, trial_.count);
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_));
        insertCommaAndSpace(stream);
        insert(stream, trial_.reversals);
        return insertNewLine(stream);
    }

  private:
    const CorrectKeywordsTrial &trial_;
};

class ConsonantTrialFormatter : public TrialFormatter {
  public:
    explicit ConsonantTrialFormatter(const ConsonantTrial &trial_)
        : trial_{trial_} {}

    auto insertHeading(std::ostream &stream) -> std::ostream & override {
        insert(stream, HeadingItem::correctConsonant);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::subjectConsonant);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::evaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::target);
        return insertNewLine(stream);
    }

    auto insertTrial(std::ostream &stream) -> std::ostream & override {
        insert(stream, trial_.correctConsonant);
        insertCommaAndSpace(stream);
        insert(stream, trial_.subjectConsonant);
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_));
        insertCommaAndSpace(stream);
        insert(stream, trial_.target);
        return insertNewLine(stream);
    }

  private:
    const ConsonantTrial &trial_;
};

class ThreeKeywordsTrialFormatter : public TrialFormatter {
  public:
    explicit ThreeKeywordsTrialFormatter(const ThreeKeywordsTrial &trial_)
        : trial_{trial_} {}

    auto insertHeading(std::ostream &stream) -> std::ostream & override {
        insert(stream, HeadingItem::target);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::firstKeywordEvaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::secondKeywordEvaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::thirdKeywordEvaluation);
        return insertNewLine(stream);
    }

    auto insertTrial(std::ostream &stream) -> std::ostream & override {
        insert(stream, trial_.target);
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_.firstCorrect));
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_.secondCorrect));
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_.thirdCorrect));
        stream << static_cast<const Flaggable &>(trial_);
        return insertNewLine(stream);
    }

  private:
    const ThreeKeywordsTrial &trial_;
};

class SyllableTrialFormatter : public TrialFormatter {
  public:
    explicit SyllableTrialFormatter(const SyllableTrial &trial_)
        : trial_{trial_} {}

    auto insertHeading(std::ostream &stream) -> std::ostream & override {
        insert(stream, HeadingItem::correctSyllable);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::subjectSyllable);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::evaluation);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::target);
        return insertNewLine(stream);
    }

    auto insertTrial(std::ostream &stream) -> std::ostream & override {
        insert(stream, trial_.correctSyllable);
        insertCommaAndSpace(stream);
        insert(stream, trial_.subjectSyllable);
        insertCommaAndSpace(stream);
        insert(stream, evaluation(trial_));
        insertCommaAndSpace(stream);
        insert(stream, trial_.target);
        stream << static_cast<const Flaggable &>(trial_);
        return insertNewLine(stream);
    }

  private:
    const SyllableTrial &trial_;
};

class EmotionTrialFormatter : public TrialFormatter {
  public:
    explicit EmotionTrialFormatter(const EmotionTrial &trial_)
        : trial_{trial_} {}

    auto insertHeading(std::ostream &stream) -> std::ostream & override {
        insert(stream, HeadingItem::target);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::emotion);
        return insertNewLine(stream);
    }

    auto insertTrial(std::ostream &stream) -> std::ostream & override {
        insert(stream, trial_.target);
        insertCommaAndSpace(stream);
        insert(stream, trial_.emotion);
        return insertNewLine(stream);
    }

  private:
    const EmotionTrial &trial_;
};

class PassFailTrialFormatter : public TrialFormatter {
  public:
    explicit PassFailTrialFormatter(const PassFailTrial &trial)
        : trial{trial} {}

    auto insertHeading(std::ostream &stream) -> std::ostream & override {
        insert(stream, HeadingItem::target);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::evaluation);
        return insertNewLine(stream);
    }

    auto insertTrial(std::ostream &s) -> std::ostream & override { return s; }

    const PassFailTrial &trial;
};

class KeyPressTrialFormatter : public TrialFormatter {
  public:
    explicit KeyPressTrialFormatter(const KeyPressTrial &trial_)
        : trial_{trial_} {}

    auto insertHeading(std::ostream &stream) -> std::ostream & override {
        insert(stream, HeadingItem::target);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::keyPressed);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::reactionTime);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::vibrotactileDuration);
        insertCommaAndSpace(stream);
        insert(stream, HeadingItem::vibrotactileDelay);
        return insertNewLine(stream);
    }

    auto insertTrial(std::ostream &stream) -> std::ostream & override {
        insert(stream, trial_.target);
        insertCommaAndSpace(stream);
        insert(stream, trial_.key);
        insertCommaAndSpace(stream);
        insert(stream, trial_.rt.milliseconds);
        insertCommaAndSpace(stream);
        insert(stream, trial_.vibrotactileStimulus.duration.seconds);
        insertCommaAndSpace(stream);
        insert(stream,
            trial_.vibrotactileStimulus.targetStartRelativeDelay.seconds);
        return insertNewLine(stream);
    }

  private:
    const KeyPressTrial &trial_;
};
}

static void write(Writer &writer, const std::string &s) { writer.write(s); }

static void write(Writer &writer, TrialFormatter &formatter,
    OutputFileImpl::Trial &currentTrial, OutputFileImpl::Trial trial) {
    std::stringstream stream;
    if (currentTrial != trial)
        formatter.insertHeading(stream);
    formatter.insertTrial(stream);
    write(writer, string(stream));
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

void OutputFileImpl::write(const KeyPressTrial &trial) {
    KeyPressTrialFormatter formatter{trial};
    av_speech_in_noise::write(writer, formatter, currentTrial, Trial::KeyPress);
}

void OutputFileImpl::write(const PassFailTrial &trial) {
    PassFailTrialFormatter formatter{trial};
    av_speech_in_noise::write(writer, formatter, currentTrial, Trial::PassFail);
}

void OutputFileImpl::write(const EmotionTrial &trial) {
    EmotionTrialFormatter formatter{trial};
    av_speech_in_noise::write(writer, formatter, currentTrial, Trial::Emotion);
}

void OutputFileImpl::write(const AdaptiveTest &test) {
    std::stringstream stream;
    stream << test;
    write(string(stream));
}

void OutputFileImpl::write(const FixedLevelTest &test) {
    std::stringstream stream;
    stream << test;
    write(string(stream));
}

void OutputFileImpl::write(const BinocularGazeSamples &gazeSamples) {
    std::stringstream stream;
    stream << gazeSamples;
    write(string(stream));
}

void OutputFileImpl::write(TargetStartTime t) {
    std::stringstream stream;
    stream << t;
    write(string(stream));
}

void OutputFileImpl::write(const EyeTrackerTargetPlayerSynchronization &s) {
    std::stringstream stream;
    stream << s;
    write(string(stream));
}

void OutputFileImpl::openNewFile(const TestIdentity &test) {
    writer.open(generateNewFilePath(test));
    if (writer.failed())
        throw OpenFailure{};
    currentTrial = Trial::none;
}

auto OutputFileImpl::generateNewFilePath(const TestIdentity &test)
    -> std::string {
    path.setRelativeOutputDirectory(test.relativeOutputUrl.path);
    return path.outputDirectory() + "/" + path.generateFileName(test) + ".txt";
}

void OutputFileImpl::close() { writer.close(); }

void OutputFileImpl::save() { writer.save(); }

void OutputFileImpl::write(const AdaptiveTestResults &results) {
    std::stringstream stream;
    for (const auto &result : results)
        stream << result;
    write(string(stream));
}

void OutputFileImpl::write(Writable &w) {
    writer.write(w);
    writer.write("\n");
}

auto OutputFileImpl::parentPath() -> std::filesystem::path {
    return path.outputDirectory();
}
}
