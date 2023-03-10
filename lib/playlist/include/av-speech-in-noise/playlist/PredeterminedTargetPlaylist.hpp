#ifndef AV_SPEECH_IN_NOISE_LIB_PLAYLIST_INCLUDE_AVSPEECHINNOISE_PLAYLIST_PREDETERMINEDTARGETPLAYLISTHPP_
#define AV_SPEECH_IN_NOISE_LIB_PLAYLIST_INCLUDE_AVSPEECHINNOISE_PLAYLIST_PREDETERMINEDTARGETPLAYLISTHPP_

#include "av-speech-in-noise/Model.hpp"
#include "av-speech-in-noise/playlist/SubdirectoryTargetPlaylistReader.hpp"
#include <av-speech-in-noise/core/TargetPlaylist.hpp>
#include <av-speech-in-noise/core/TextFileReader.hpp>

#include <vector>

namespace av_speech_in_noise {
class FileValidator {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(FileValidator);
    virtual auto check(const LocalUrl &) -> bool = 0;
};

class PredeterminedTargetPlaylist : public FiniteTargetPlaylistWithRepeatables {
  public:
    PredeterminedTargetPlaylist(TextFileReader &fileReader, FileValidator &);
    void load(const LocalUrl &url) override;
    auto next() -> LocalUrl override;
    auto current() -> LocalUrl override;
    auto directory() -> LocalUrl override;
    auto empty() -> bool override;
    void reinsertCurrent() override;

  private:
    TextFileReader &fileReader;
    FileValidator &fileValidator;
    std::vector<LocalUrl> targets;
    LocalUrl current_;
};
}

#endif
