#ifndef AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_SUBDIRECTORYTARGETLISTREADER_HPP_
#define AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_SUBDIRECTORYTARGETLISTREADER_HPP_

#include <recognition-test/AdaptiveMethod.hpp>
#include <memory>
#include <vector>
#include <string>

namespace av_speech_in_noise {
using LocalUrls = typename std::vector<av_speech_in_noise::LocalUrl>;

class TargetListFactory {
  public:
    virtual ~TargetListFactory() = default;
    virtual auto make() -> std::shared_ptr<av_speech_in_noise::TargetList> = 0;
};

class DirectoryReader {
  public:
    virtual ~DirectoryReader() = default;
    DirectoryReader() = default;
    DirectoryReader(const DirectoryReader &) = default;
    virtual auto subDirectories(const av_speech_in_noise::LocalUrl &directory)
        -> LocalUrls = 0;
    virtual auto filesIn(const av_speech_in_noise::LocalUrl &directory)
        -> LocalUrls = 0;
};

class SubdirectoryTargetListReader
    : public av_speech_in_noise::TargetListReader {
    TargetListFactory *targetListFactory;
    DirectoryReader *directoryReader;

  public:
    SubdirectoryTargetListReader(TargetListFactory *, DirectoryReader *);
    auto read(const av_speech_in_noise::LocalUrl &) -> lists_type override;

  private:
    auto subDirectories(const av_speech_in_noise::LocalUrl &directory)
        -> LocalUrls;
};
}

#endif
