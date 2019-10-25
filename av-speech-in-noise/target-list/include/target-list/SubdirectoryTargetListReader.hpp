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
    virtual std::shared_ptr<av_speech_in_noise::TargetList> make() = 0;
};

class DirectoryReader {
public:
    virtual ~DirectoryReader() = default;
    virtual std::vector<std::string> subDirectories(std::string directory) = 0;
    virtual std::vector<std::string> filesIn(std::string directory) = 0;
};

class SubdirectoryTargetListReader : public av_speech_in_noise::TargetListReader {
    TargetListFactory *targetListFactory;
    DirectoryReader *directoryReader;
public:
    SubdirectoryTargetListReader(TargetListFactory *, DirectoryReader *);
    lists_type read(std::string directory) override;
private:
    std::vector<std::string> subDirectories(std::string directory);
};
}

#endif
