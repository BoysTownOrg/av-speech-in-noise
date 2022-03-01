#include "SubdirectoryTargetPlaylistReader.hpp"

#include <filesystem>

namespace av_speech_in_noise {
SubdirectoryTargetPlaylistReader::SubdirectoryTargetPlaylistReader(
    TargetPlaylistFactory *targetListFactory, DirectoryReader *directoryReader)
    : targetListFactory{targetListFactory}, directoryReader{directoryReader} {}

auto SubdirectoryTargetPlaylistReader::read(const LocalUrl &directory)
    -> lists_type {
    lists_type lists{};
    try {
        const auto subDirectories = directoryReader->subDirectories(directory);
        for (const auto &subDirectory : subDirectories) {
            lists.push_back(targetListFactory->make());
            std::filesystem::path fullPath{directory.path};
            lists.back()->loadFromDirectory({fullPath / subDirectory.path});
        }
        if (subDirectories.empty()) {
            lists.push_back(targetListFactory->make());
            lists.back()->loadFromDirectory(directory);
        }
    } catch (const DirectoryReader::CannotRead &e) {
    }
    return lists;
}
}
