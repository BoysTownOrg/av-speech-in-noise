#include "SubdirectoryTargetListReader.hpp"

namespace av_coordinate_response_measure {
    SubdirectoryTargetListReader::SubdirectoryTargetListReader(
        TargetListFactory *targetListFactory,
        DirectoryReader *directoryReader
    ) :
        targetListFactory{targetListFactory},
        directoryReader{directoryReader} {}
    
    auto SubdirectoryTargetListReader::read(std::string directory) -> lists_type {
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
