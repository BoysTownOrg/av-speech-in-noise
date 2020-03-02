#include "TestSettingsInterpreter.hpp"
#include <gsl/gsl>
#include <sstream>

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

void TestSettingsInterpreterImpl::apply(
    Model &model, const std::string &contents) {
    AdaptiveTest test;
    test.trackingRule.resize(1);
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
            if (test.trackingRule.size() < v.size())
                test.trackingRule.resize(v.size());
            for (gsl::index i{0}; i < v.size(); ++i)
                test.trackingRule.at(i).up = v.at(i);
        } else if (entryName == name(TestSetting::down)) {
            auto v{vectorOfInts(entry)};
            if (test.trackingRule.size() < v.size())
                test.trackingRule.resize(v.size());
            for (gsl::index i{0}; i < v.size(); ++i)
                test.trackingRule.at(i).down = v.at(i);
        } else if (entryName == name(TestSetting::reversalsPerStepSize)) {
            auto v{vectorOfInts(entry)};
            if (test.trackingRule.size() < v.size())
                test.trackingRule.resize(v.size());
            for (gsl::index i{0}; i < v.size(); ++i)
                test.trackingRule.at(i).runCount = v.at(i);
        } else if (entryName == name(TestSetting::stepSizes)) {
            auto v{vectorOfInts(entry)};
            if (test.trackingRule.size() < v.size())
                test.trackingRule.resize(v.size());
            for (gsl::index i{0}; i < v.size(); ++i)
                test.trackingRule.at(i).stepSize = v.at(i);
        } else if (entryName == name(TestSetting::condition))
            if (entry == conditionName(Condition::audioVisual))
                test.condition = Condition::audioVisual;
    }
    model.initializeTest(test);
}
}
