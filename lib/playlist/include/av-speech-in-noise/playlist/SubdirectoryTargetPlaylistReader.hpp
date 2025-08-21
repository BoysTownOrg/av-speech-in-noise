#ifndef AV_SPEECH_IN_NOISE_LIB_PLAYLIST_INCLUDE_AVSPEECHINNOISE_PLAYLIST_SUBDIRECTORYTARGETPLAYLISTREADERHPP_
#define AV_SPEECH_IN_NOISE_LIB_PLAYLIST_INCLUDE_AVSPEECHINNOISE_PLAYLIST_SUBDIRECTORYTARGETPLAYLISTREADERHPP_

#include <av-speech-in-noise/Interface.hpp>
#include <av-speech-in-noise/core/AdaptiveMethod.hpp>

#include <stdexcept>
#include <vector>
#include <string>

namespace av_speech_in_noise {
using LocalUrls = typename std::vector<av_speech_in_noise::LocalUrl>;

class DirectoryReader {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(DirectoryReader);
    class CannotRead : public std::runtime_error {
      public:
        explicit CannotRead(const std::string &s) : std::runtime_error{s} {}
    };
    virtual auto subDirectories(const LocalUrl &directory) -> LocalUrls = 0;
    virtual auto filesIn(const LocalUrl &directory) -> LocalUrls = 0;
};

class SubdirectoryTargetPlaylistReader : public TargetPlaylistReader {
    TargetPlaylistFactory *targetListFactory{};
    DirectoryReader *directoryReader;

  public:
    explicit SubdirectoryTargetPlaylistReader(DirectoryReader *);
    auto read(const LocalUrl &) -> lists_type override;
    void attach(TargetPlaylistFactory *) override;
};
}

#endif
