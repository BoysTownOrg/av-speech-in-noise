#include "SubdirectoryTargetListReader.hpp"

namespace target_list {
SubdirectoryTargetListReader::SubdirectoryTargetListReader(
    TargetListFactory *targetListFactory, DirectoryReader *directoryReader)
    : targetListFactory{targetListFactory}, directoryReader{directoryReader} {}

auto SubdirectoryTargetListReader::read(
    const av_speech_in_noise::LocalUrl &directory) -> lists_type {
    lists_type lists{};
    auto subDirectories_ = subDirectories(directory);
    for (const auto &subDirectory : subDirectories_) {
        lists.push_back(targetListFactory->make());
        auto fullPath{directory.path};
        fullPath.append("/" + subDirectory.path);
        lists.back()->loadFromDirectory({fullPath});
    }
    if (subDirectories_.empty()) {
        lists.push_back(targetListFactory->make());
        lists.back()->loadFromDirectory(directory);
    }
    return lists;
}

auto SubdirectoryTargetListReader::subDirectories(
    const av_speech_in_noise::LocalUrl &directory)
    -> std::vector<av_speech_in_noise::LocalUrl> {
    return directoryReader->subDirectories(directory);
}
}
