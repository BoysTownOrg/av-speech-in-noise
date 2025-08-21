#include "TestSettingsInterpreter.hpp"

#include <algorithm>
#include <gsl/gsl>

#include <sstream>
#include <functional>
#include <string>
#include <utility>

namespace av_speech_in_noise {
static auto entryDelimiter(const std::string &s) -> gsl::index {
    return s.find(':');
}

// https://stackoverflow.com/a/217605
static auto trim(std::string s) -> std::string {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return std::isspace(ch) == 0;
    }));
    s.erase(std::find_if(s.rbegin(), s.rend(),
                [](unsigned char ch) { return std::isspace(ch) == 0; })
                .base(),
        s.end());
    return s;
}

static auto key(const std::string &line) -> std::string {
    return trim(line.substr(0, entryDelimiter(line)));
}

static auto size(const std::string &s) -> gsl::index { return s.size(); }

static auto value(const std::string &line) -> std::string {
    return entryDelimiter(line) >= size(line) - 1
        ? ""
        : trim(line.substr(entryDelimiter(line) + 1));
}

static void applyToEachEntry(
    const std::function<void(const std::string &, const std::string &)> &f,
    const std::string &contents) {
    std::stringstream stream{contents};
    for (std::string line; std::getline(stream, line);)
        f(key(line), value(line));
}

static void broadcast(
    const std::map<std::string,
        std::vector<std::reference_wrapper<Configurable>>> &configurables,
    const std::string &key, const std::string &value) {
    if (const auto search{configurables.find(key)};
        search != configurables.end())
        for (const auto each : search->second)
            each.get().configure(key, value);
}

TestSettingsInterpreterImpl::TestSettingsInterpreterImpl(
    RunningATest &runningATest, SessionController &sessionController)
    : runningATest{runningATest}, sessionController{sessionController} {}

void TestSettingsInterpreterImpl::subscribe(
    Configurable &c, const std::string &key) {
    configurables[key].emplace_back(c);
}

void TestSettingsInterpreterImpl::apply(
    const std::string &contents, const std::vector<std::string> &matches) {
    applyToEachEntry(
        [&](const auto &key, const auto &value) {
            if (std::find(matches.begin(), matches.end(), key) != matches.end())
                broadcast(configurables, key, value);
        },
        contents);
}

void TestSettingsInterpreterImpl::initializeTest(const std::string &contents,
    const TestIdentity &testIdentity, const std::string &startingSnr) {
    broadcast(configurables, "relative output path",
        "Documents/AvSpeechInNoise Data");
    broadcast(configurables, "puzzle", "");
    broadcast(configurables, "subject ID", testIdentity.subjectId);
    broadcast(configurables, "tester ID", testIdentity.testerId);
    broadcast(configurables, "session", testIdentity.session);
    broadcast(configurables, "RME setting", testIdentity.rmeSetting);
    broadcast(configurables, "transducer", testIdentity.transducer);
    broadcast(configurables, "starting SNR (dB)", startingSnr);

    applyToEachEntry(
        [&](const auto &key, const auto &value) {
            broadcast(configurables, key, value);
        },
        contents);

    runningATest.initialize();
    if (!runningATest.testComplete())
        sessionController.prepare();
}

auto TestSettingsInterpreterImpl::meta(const std::string &contents)
    -> std::string {
    std::stringstream stream{contents};
    for (std::string line; std::getline(stream, line);)
        if (key(line) == "meta")
            return value(line);
    return "";
}
}
