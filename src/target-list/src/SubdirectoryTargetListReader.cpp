#include "SubdirectoryTargetListReader.hpp"

namespace target_list {
SubdirectoryTargetListReader::SubdirectoryTargetListReader(
    TargetListFactory *targetListFactory, DirectoryReader *directoryReader)
    : targetListFactory{targetListFactory}, directoryReader{directoryReader} {}

auto SubdirectoryTargetListReader::read(
    const av_speech_in_noise::LocalUrl &directory) -> lists_type {
    lists_type lists{};
    auto subDirectories_ = subDirectories(directory.path);
    for (const auto &subDirectory : subDirectories_) {
        lists.push_back(targetListFactory->make());
        auto fullPath{directory.path};
        fullPath.append("/" + subDirectory);
        lists.back()->loadFromDirectory(fullPath);
    }
    if (subDirectories_.empty()) {
        lists.push_back(targetListFactory->make());
        lists.back()->loadFromDirectory(directory.path);
    }
    return lists;
}

auto SubdirectoryTargetListReader::subDirectories(std::string directory)
    -> std::vector<std::string> {
    return directoryReader->subDirectories(std::move(directory));
}
}
