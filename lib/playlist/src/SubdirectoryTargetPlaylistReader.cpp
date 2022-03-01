#include "SubdirectoryTargetPlaylistReader.hpp"
#include "av-speech-in-noise/Model.hpp"

#include <filesystem>

namespace av_speech_in_noise {
SubdirectoryTargetPlaylistReader::SubdirectoryTargetPlaylistReader(
    TargetPlaylistFactory *targetListFactory, DirectoryReader *directoryReader)
    : targetListFactory{targetListFactory}, directoryReader{directoryReader} {}

static void add(TargetPlaylistReader::lists_type &lists,
    TargetPlaylistFactory *factory, const LocalUrl &url) {
    auto list{factory->make()};
    list->loadFromDirectory(url);
    lists.push_back(std::move(list));
}

auto SubdirectoryTargetPlaylistReader::read(const LocalUrl &directory)
    -> lists_type {
    lists_type lists{};
    try {
        const auto subDirectories = directoryReader->subDirectories(directory);
        for (const auto &subDirectory : subDirectories)
            add(lists, targetListFactory,
                LocalUrl{
                    std::filesystem::path{directory.path} / subDirectory.path});
        if (subDirectories.empty())
            add(lists, targetListFactory, directory);

    } catch (const DirectoryReader::CannotRead &e) {
    }
    return lists;
}
}
