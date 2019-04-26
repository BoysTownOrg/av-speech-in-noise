#include "MultiTrackTargetList.hpp"

namespace av_coordinate_response_measure {
    MultiTrackTargetList::MultiTrackTargetList(
        TargetListFactory *targetListFactory,
        DirectoryReader *directoryReader
    ) :
        targetListFactory{targetListFactory},
        directoryReader{directoryReader} {}
    
    void MultiTrackTargetList::loadFromDirectory(std::string directory) {
        auto subDirectories = directoryReader->subDirectories(std::move(directory));
        for (auto d : subDirectories) {
            auto targetList = targetListFactory->make();
            targetList->loadFromDirectory(d);
        }
    }

    std::string MultiTrackTargetList::next() {
        return {};
    }

    std::string MultiTrackTargetList::current() {
        return {};
    }
}
