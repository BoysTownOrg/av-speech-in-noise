#ifndef AV_SPEECH_IN_NOISE_LIB_PLAYLIST_INCLUDE_AVSPEECHINNOISE_PLAYLIST_SUBDIRECTORYTARGETPLAYLISTREADERHPP_
#define AV_SPEECH_IN_NOISE_LIB_PLAYLIST_INCLUDE_AVSPEECHINNOISE_PLAYLIST_SUBDIRECTORYTARGETPLAYLISTREADERHPP_

#include <av-speech-in-noise/core/AdaptiveMethod.hpp>
#include <memory>
#include <stdexcept>
#include <vector>
#include <string>

namespace av_speech_in_noise {
using LocalUrls = typename std::vector<av_speech_in_noise::LocalUrl>;

class TargetPlaylistFactory {
  public:
    virtual ~TargetPlaylistFactory() = default;
    virtual auto make()
        -> std::shared_ptr<av_speech_in_noise::TargetPlaylist> = 0;
};

class DirectoryReader {
  public:
    virtual ~DirectoryReader() = default;
    DirectoryReader() = default;
    DirectoryReader(const DirectoryReader &) = default;
    class CannotRead : public std::runtime_error {
      public:
        explicit CannotRead(const std::string &s) : std::runtime_error{s} {}
    };
    virtual auto subDirectories(const LocalUrl &directory) -> LocalUrls = 0;
    virtual auto filesIn(const LocalUrl &directory) -> LocalUrls = 0;
};

class SubdirectoryTargetPlaylistReader : public TargetPlaylistReader {
    TargetPlaylistFactory *targetListFactory;
    DirectoryReader *directoryReader;

  public:
    SubdirectoryTargetPlaylistReader(
        TargetPlaylistFactory *, DirectoryReader *);
    auto read(const LocalUrl &) -> lists_type override;

  private:
    auto subDirectories(const LocalUrl &directory) -> LocalUrls;
};
}

#endif
