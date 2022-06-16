#include "PredeterminedTargetPlaylist.hpp"

#include <filesystem>
#include <sstream>

namespace av_speech_in_noise {
PredeterminedTargetPlaylist::PredeterminedTargetPlaylist(
    TextFileReader &fileReader)
    : fileReader{fileReader} {}

void PredeterminedTargetPlaylist::load(const LocalUrl &url) {
    targets.clear();
    std::stringstream stream{fileReader.read(url)};
    for (std::string line; std::getline(stream, line);)
        targets.push_back(LocalUrl{line});
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
}
