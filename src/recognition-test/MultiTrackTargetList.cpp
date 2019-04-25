//
//  MultiTrackTargetList.cpp
//  recognition-test
//
//  Created by Bashford, Seth on 4/25/19.
//

#include "MultiTrackTargetList.hpp"

namespace av_coordinate_response_measure {
    MultiTrackTargetList::MultiTrackTargetList(
        TargetListFactory *targetListFactory,
        DirectoryReader *directoryReader
    ) :
        targetListFactory{targetListFactory},
        directoryReader{directoryReader} {}
    
    void MultiTrackTargetList::loadFromDirectory(std::string directory) {
        auto subDirectories = directoryReader->subDirectories({});
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
    
    void MultiTrackTargetList::reset(const Track::Settings &) { 
        ;
    }
    
    void MultiTrackTargetList::pushDown() { 
        ;
    }
    
    void MultiTrackTargetList::pushUp() { 
        ;
    }
    
    int MultiTrackTargetList::x() { 
        return {};
    }
    
    bool MultiTrackTargetList::complete() { 
        return {};
    }
    
    int MultiTrackTargetList::reversals() { 
        return {};
    }
    
    
}
