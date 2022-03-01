#include "SubdirectoryTargetPlaylistReader.hpp"

namespace av_speech_in_noise {
SubdirectoryTargetPlaylistReader::SubdirectoryTargetPlaylistReader(
    TargetPlaylistFactory *targetListFactory, DirectoryReader *directoryReader)
    : targetListFactory{targetListFactory}, directoryReader{directoryReader} {}

auto SubdirectoryTargetPlaylistReader::read(const LocalUrl &directory)
    -> lists_type {
    lists_type lists{};
    try {
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
    } catch (const DirectoryReader::CannotRead &e) {
    }
    return lists;
}

auto SubdirectoryTargetPlaylistReader::subDirectories(const LocalUrl &directory)
    -> LocalUrls {
    return directoryReader->subDirectories(directory);
}
}
