#include "SubdirectoryTargetListReader.hpp"

namespace target_list {
SubdirectoryTargetListReader::SubdirectoryTargetListReader(
    TargetListFactory *targetListFactory, DirectoryReader *directoryReader)
    : targetListFactory{targetListFactory}, directoryReader{directoryReader} {}

auto SubdirectoryTargetListReader::read(std::string directory) -> lists_type {
    lists_type lists{};
    auto subDirectories_ = subDirectories(directory);
    for (const auto &subDirectory : subDirectories_) {
        lists.push_back(targetListFactory->make());
        auto fullPath = directory;
        fullPath.append("/" + subDirectory);
        lists.back()->loadFromDirectory(std::move(fullPath));
    }
    if (subDirectories_.empty()) {
        lists.push_back(targetListFactory->make());
        lists.back()->loadFromDirectory(directory);
    }
    return lists;
}

std::vector<std::string> SubdirectoryTargetListReader::subDirectories(
    std::string directory) {
    return directoryReader->subDirectories(std::move(directory));
}
}
