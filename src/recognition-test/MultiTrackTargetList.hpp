#ifndef av_coordinate_response_measure_MultiTrackTargetList_hpp
#define av_coordinate_response_measure_MultiTrackTargetList_hpp

#include <recognition-test/RecognitionTestModel.hpp>
#include <memory>

namespace av_coordinate_response_measure {
    class TargetListFactory {
    public:
        virtual ~TargetListFactory() = default;
        virtual std::shared_ptr<TargetList> make() = 0;
    };
    
    class DirectoryReader {
    public:
        virtual ~DirectoryReader() = default;
        virtual std::vector<std::string> subDirectories(std::string directory) = 0;
    };
    
    class MultiTrackTargetList : public TargetListReader {
        TargetListFactory *targetListFactory;
        DirectoryReader *directoryReader;
    public:
        MultiTrackTargetList(TargetListFactory *, DirectoryReader *);
        lists_type read(std::string directory) override;
    };
}

#endif
