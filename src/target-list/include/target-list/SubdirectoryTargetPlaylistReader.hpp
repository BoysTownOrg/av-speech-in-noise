#ifndef AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_SUBDIRECTORYTARGETLISTREADER_HPP_
#define AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_SUBDIRECTORYTARGETLISTREADER_HPP_

#include <recognition-test/AdaptiveMethod.hpp>
#include <memory>
#include <vector>
#include <string>

namespace av_speech_in_noise {
using LocalUrls = typename std::vector<av_speech_in_noise::LocalUrl>;

class TargetPlaylistFactory {
  public:
    virtual ~TargetPlaylistFactory() = default;
    virtual auto make() -> std::shared_ptr<av_speech_in_noise::TargetPlaylist> = 0;
};

class DirectoryReader {
  public:
    virtual ~DirectoryReader() = default;
    DirectoryReader() = default;
    DirectoryReader(const DirectoryReader &) = default;
    virtual auto subDirectories(const LocalUrl &directory) -> LocalUrls = 0;
    virtual auto filesIn(const LocalUrl &directory) -> LocalUrls = 0;
};

class SubdirectoryTargetPlaylistReader : public TargetPlaylistReader {
    TargetPlaylistFactory *targetListFactory;
    DirectoryReader *directoryReader;

  public:
    SubdirectoryTargetPlaylistReader(TargetPlaylistFactory *, DirectoryReader *);
    auto read(const LocalUrl &) -> lists_type override;

  private:
    auto subDirectories(const LocalUrl &directory) -> LocalUrls;
};
}

#endif
