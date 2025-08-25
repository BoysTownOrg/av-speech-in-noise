#include "PredeterminedTargetPlaylist.hpp"

#include <filesystem>
#include <sstream>

namespace av_speech_in_noise {
PredeterminedTargetPlaylist::PredeterminedTargetPlaylist(
    ConfigurationRegistry &registry, TextFileReader &fileReader,
    TargetValidator &fileValidator, FixedLevelMethod &fixedLevelMethod)
    : fileReader{fileReader}, targetValidator{fileValidator},
      fixedLevelMethod{fixedLevelMethod} {
    registry.subscribe(*this, "method");
}

// https://stackoverflow.com/a/25829178
static auto trim(std::string s) -> std::string {
    const auto first = s.find_first_not_of(' ');
    if (std::string::npos == first)
        return "";
    const auto last = s.find_last_not_of(' ');
    return s.substr(first, (last - first + 1));
}

void PredeterminedTargetPlaylist::load(const LocalUrl &url) {
    targets.clear();
    try {
        std::stringstream stream{fileReader.read(url)};
        for (std::string line; std::getline(stream, line);) {
            const auto trimmed = trim(line);
            if (!trimmed.empty())
                targets.push_back(LocalUrl{trimmed});
        }
    } catch (const TextFileReader::FileDoesNotExist &) {
        throw LoadFailure{};
    }
    for (const auto &target : targets)
        if (!targetValidator.isValid(target))
            throw LoadFailure{};
}

auto PredeterminedTargetPlaylist::next() -> LocalUrl {
    current_ = targets.front();
    targets.erase(targets.begin());
    return current_;
}

auto PredeterminedTargetPlaylist::current() -> LocalUrl { return current_; }

auto PredeterminedTargetPlaylist::directory() -> LocalUrl {
    return LocalUrl{std::filesystem::path{current_.path}.parent_path()};
}

auto PredeterminedTargetPlaylist::empty() -> bool { return targets.empty(); }

void PredeterminedTargetPlaylist::reinsertCurrent() {
    targets.push_back(current_);
}

void PredeterminedTargetPlaylist::configure(
    const std::string &key, const std::string &value) {
    if (key == "method")
        if (contains(value, "predetermined stimuli"))
            fixedLevelMethod.attach(this);
}
}
