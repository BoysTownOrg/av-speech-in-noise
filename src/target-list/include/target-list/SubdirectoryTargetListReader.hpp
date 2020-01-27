#ifndef AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_SUBDIRECTORYTARGETLISTREADER_HPP_
#define AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_SUBDIRECTORYTARGETLISTREADER_HPP_

#include <recognition-test/AdaptiveMethod.hpp>
#include <memory>
#include <vector>
#include <string>

namespace target_list {
class TargetListFactory {
  public:
    virtual ~TargetListFactory() = default;
    virtual auto make() -> std::shared_ptr<av_speech_in_noise::TargetList> = 0;
};

class DirectoryReader {
  public:
    virtual ~DirectoryReader() = default;
    virtual auto subDirectories(std::string directory)
        -> std::vector<std::string> = 0;
    virtual auto filesIn(std::string directory) -> std::vector<std::string> = 0;
};

class SubdirectoryTargetListReader
    : public av_speech_in_noise::TargetListReader {
    TargetListFactory *targetListFactory;
    DirectoryReader *directoryReader;

  public:
    SubdirectoryTargetListReader(TargetListFactory *, DirectoryReader *);
    auto read(std::string directory) -> lists_type override;

  private:
    auto subDirectories(std::string directory) -> std::vector<std::string>;
};
}

#endif
