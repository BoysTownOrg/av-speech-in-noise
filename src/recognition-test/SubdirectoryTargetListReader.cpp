#include "SubdirectoryTargetListReader.hpp"

namespace av_coordinate_response_measure {
    SubdirectoryTargetListReader::SubdirectoryTargetListReader(
        TargetListFactory *targetListFactory,
        DirectoryReader *directoryReader
    ) :
        targetListFactory{targetListFactory},
        directoryReader{directoryReader} {}
    
    auto SubdirectoryTargetListReader::read(std::string directory) -> lists_type {
        lists_type lists{};
        for (auto d : subDirectories(directory)) {
            lists.push_back(targetListFactory->make());
            lists.back()->loadFromDirectory(d);
        }
        return lists;
    }
    
    std::vector<std::string> SubdirectoryTargetListReader::subDirectories(std::string directory) {
        return directoryReader->subDirectories(std::move(directory));
    }
}
