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
    std::stringstream stream{s};
    std::vector<int> v;
    int x;
    while (stream >> x)
        v.push_back(x);
    return v;
}

static void resizeTrackingRuleEnough(
    AdaptiveTest &test, const std::vector<int> &v) {
    if (test.trackingRule.size() < v.size())
        test.trackingRule.resize(v.size());
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
    const std::vector<int> &v) {
    for (gsl::index i{0}; i < v.size(); ++i)
        f(test.trackingRule.at(i), v.at(i));
}

void TestSettingsInterpreterImpl::apply(
    Model &model, const std::string &contents) {
    AdaptiveTest test;
    std::stringstream stream{contents};
    for (auto line{nextLine(stream)}; !line.empty(); line = nextLine(stream)) {
        auto entryName{line.substr(0, entryDelimiter(line))};
        auto entry{line.substr(entryDelimiter(line) + 2)};
        if (entryName == name(TestSetting::targets))
            test.targetListDirectory = entry;
        else if (entryName == name(TestSetting::masker))
            test.maskerFilePath = entry;
        else if (entryName == name(TestSetting::maskerLevel))
            test.maskerLevel_dB_SPL = std::stoi(entry);
        else if (entryName == name(TestSetting::up)) {
            auto v{vectorOfInts(entry)};
            resizeTrackingRuleEnough(test, v);
            applyToEachTrackingRule(test, applyToUp, v);
        } else if (entryName == name(TestSetting::down)) {
            auto v{vectorOfInts(entry)};
            resizeTrackingRuleEnough(test, v);
            applyToEachTrackingRule(test, applyToDown, v);
        } else if (entryName == name(TestSetting::reversalsPerStepSize)) {
            auto v{vectorOfInts(entry)};
            resizeTrackingRuleEnough(test, v);
            applyToEachTrackingRule(test, applyToRunCount, v);
        } else if (entryName == name(TestSetting::stepSizes)) {
            auto v{vectorOfInts(entry)};
            resizeTrackingRuleEnough(test, v);
            applyToEachTrackingRule(test, applyToStepSize, v);
        } else if (entryName == name(TestSetting::condition))
            if (entry == conditionName(Condition::audioVisual))
                test.condition = Condition::audioVisual;
    }
    model.initializeTest(test);
}
}
