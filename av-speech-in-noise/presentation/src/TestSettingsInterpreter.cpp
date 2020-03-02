#include "TestSettingsInterpreter.hpp"
#include <gsl/gsl>
#include <sstream>
#include <functional>

namespace av_speech_in_noise {
static auto entryDelimiter(const std::string &s) -> gsl::index {
    return s.find(':');
}

static auto nextLine(std::stringstream &stream) -> std::string {
    std::string line;
    std::getline(stream, line);
    return line;
}

static auto vectorOfInts(const std::string &s) -> std::vector<int> {
    std::vector<int> v;
    std::stringstream stream{s};
    int x;
    while (stream >> x)
        v.push_back(x);
    return v;
}

static auto trackingRule(AdaptiveTest &test) -> TrackingRule & {
    return test.trackingRule;
}

static auto trackingRule(const AdaptiveTest &test) -> const TrackingRule & {
    return test.trackingRule;
}

static void resizeTrackingRuleEnough(
    AdaptiveTest &test, const std::vector<int> &v) {
    if (trackingRule(test).size() < v.size())
        trackingRule(test).resize(v.size());
}

static void applyToUp(TrackingSequence &sequence, int x) { sequence.up = x; }

static void applyToDown(TrackingSequence &sequence, int x) {
    sequence.down = x;
}

static void applyToRunCount(TrackingSequence &sequence, int x) {
    sequence.runCount = x;
}

static void applyToStepSize(TrackingSequence &sequence, int x) {
    sequence.stepSize = x;
}

static void applyToEachTrackingRule(AdaptiveTest &test,
    const std::function<void(TrackingSequence &, int)> &f,
    const std::string &entry) {
    auto v{vectorOfInts(entry)};
    resizeTrackingRuleEnough(test, v);
    for (gsl::index i{0}; i < v.size(); ++i)
        f(trackingRule(test).at(i), v.at(i));
}

static void applyToEachEntry(
    const std::function<void(const std::string &, const std::string &)> &f,
    const std::string &contents) {
    std::stringstream stream{contents};
    for (auto line{nextLine(stream)}; !line.empty(); line = nextLine(stream)) {
        auto entryName{line.substr(0, entryDelimiter(line))};
        auto entry{line.substr(entryDelimiter(line) + 2)};
        f(entryName, entry);
    }
}

static void assign(
    Test &test, const std::string &entryName, const std::string &entry) {
    if (entryName == name(TestSetting::targets))
        test.targetListDirectory = entry;
    else if (entryName == name(TestSetting::masker))
        test.maskerFilePath = entry;
    else if (entryName == name(TestSetting::maskerLevel))
        test.maskerLevel_dB_SPL = std::stoi(entry);
    else if (entryName == name(TestSetting::condition))
        if (entry == conditionName(Condition::audioVisual))
            test.condition = Condition::audioVisual;
}

static void assignAdaptive(AdaptiveTest &test, const std::string &entryName,
    const std::string &entry) {
    if (entryName == name(TestSetting::up))
        applyToEachTrackingRule(test, applyToUp, entry);
    else if (entryName == name(TestSetting::down))
        applyToEachTrackingRule(test, applyToDown, entry);
    else if (entryName == name(TestSetting::reversalsPerStepSize))
        applyToEachTrackingRule(test, applyToRunCount, entry);
    else if (entryName == name(TestSetting::stepSizes))
        applyToEachTrackingRule(test, applyToStepSize, entry);
    else if (entryName == name(TestSetting::startingSnr))
        test.startingSnr_dB = std::stoi(entry);
}

static void assignFixedLevel(FixedLevelTest &test, const std::string &entryName,
    const std::string &entry) {
    if (entryName == name(TestSetting::startingSnr))
        test.snr_dB = std::stoi(entry);
}

static auto methodName(const std::string &contents) -> std::string {
    std::stringstream stream{contents};
    for (auto line{nextLine(stream)}; !line.empty(); line = nextLine(stream)) {
        auto entryName{line.substr(0, entryDelimiter(line))};
        auto entry{line.substr(entryDelimiter(line) + 2)};
        if (entryName == name(TestSetting::method))
            return entry;
    }
    return {};
}

static auto adaptive(const std::string &contents) -> bool {
    auto entry{methodName(contents)};
    return entry == methodName(Method::adaptivePassFail) ||
        entry == methodName(Method::adaptiveCorrectKeywords) ||
        entry ==
        methodName(
            Method::adaptiveCoordinateResponseMeasureWithDelayedMasker) ||
        entry ==
        methodName(
            Method::adaptiveCoordinateResponseMeasureWithSingleSpeaker) ||
        entry == methodName(Method::defaultAdaptiveCoordinateResponseMeasure);
}

void TestSettingsInterpreterImpl::apply(
    Model &model, const std::string &contents) {
    if (adaptive(contents)) {
        AdaptiveTest test;
        applyToEachEntry(
            [&](auto entryName, auto entry) {
                assignAdaptive(test, entryName, entry);
            },
            contents);
        applyToEachEntry(
            [&](auto entryName, auto entry) { assign(test, entryName, entry); },
            contents);
        test.ceilingSnr_dB = Presenter::ceilingSnr_dB;
        test.floorSnr_dB = Presenter::floorSnr_dB;
        test.trackBumpLimit = Presenter::trackBumpLimit;
        test.fullScaleLevel_dB_SPL = Presenter::fullScaleLevel_dB_SPL;
        if (methodName(contents) ==
            methodName(
                Method::adaptiveCoordinateResponseMeasureWithDelayedMasker))
            model.initializeTestWithDelayedMasker(test);
        else
            model.initializeTest(test);
    } else {
        FixedLevelTest test;
        applyToEachEntry(
            [&](auto entryName, auto entry) {
                assignFixedLevel(test, entryName, entry);
            },
            contents);
        applyToEachEntry(
            [&](auto entryName, auto entry) { assign(test, entryName, entry); },
            contents);
        test.fullScaleLevel_dB_SPL = Presenter::fullScaleLevel_dB_SPL;
        model.initializeTest(test);
    }
}
}
