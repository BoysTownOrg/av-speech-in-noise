#include "MultiTrackTargetList.hpp"

namespace av_coordinate_response_measure {
    MultiTrackTargetList::MultiTrackTargetList(
        TargetListFactory *targetListFactory,
        DirectoryReader *directoryReader
    ) :
        targetListFactory{targetListFactory},
        directoryReader{directoryReader} {}
    
    auto MultiTrackTargetList::read(std::string directory) -> lists_type {
        auto subDirectories = directoryReader->subDirectories(std::move(directory));
        lists_type lists{};
        for (auto d : subDirectories) {
            auto targetList = targetListFactory->make();
            targetList->loadFromDirectory(d);
            lists.push_back(targetList);
        }
        return lists;
    }
}
