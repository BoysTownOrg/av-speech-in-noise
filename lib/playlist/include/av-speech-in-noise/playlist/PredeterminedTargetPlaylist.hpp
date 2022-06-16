#ifndef AV_SPEECH_IN_NOISE_LIB_PLAYLIST_INCLUDE_AVSPEECHINNOISE_PLAYLIST_PREDETERMINEDTARGETPLAYLISTHPP_
#define AV_SPEECH_IN_NOISE_LIB_PLAYLIST_INCLUDE_AVSPEECHINNOISE_PLAYLIST_PREDETERMINEDTARGETPLAYLISTHPP_

#include "av-speech-in-noise/Model.hpp"
#include <av-speech-in-noise/core/TargetPlaylist.hpp>
#include <av-speech-in-noise/core/TextFileReader.hpp>

#include <sstream>
#include <vector>

namespace av_speech_in_noise {
class PredeterminedTargetPlaylist : public FiniteTargetPlaylistWithRepeatables {
  public:
    explicit PredeterminedTargetPlaylist(TextFileReader &fileReader)
        : fileReader{fileReader} {}
    void load(const LocalUrl &url) override {
        targets.clear();
        std::stringstream stream{fileReader.read(url)};
        for (std::string line; std::getline(stream, line);)
            targets.push_back(LocalUrl{line});
    }
    auto next() -> LocalUrl override {
        current_ = targets.front();
        targets.erase(targets.begin());
        return current_;
    }
    auto current() -> LocalUrl override { return current_; }
    auto directory() -> LocalUrl override { return {}; }
    auto empty() -> bool override { return targets.empty(); }
    void reinsertCurrent() override { targets.push_back(current_); }

  private:
    TextFileReader &fileReader;
    std::vector<LocalUrl> targets;
    LocalUrl current_;
};
}

#endif
