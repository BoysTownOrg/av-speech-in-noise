#ifndef av_coordinate_response_measure_MultiTrackTargetList_hpp
#define av_coordinate_response_measure_MultiTrackTargetList_hpp

#include <recognition-test/RecognitionTestModel.hpp>
#include <memory>

namespace av_coordinate_response_measure {
    class TrackFactory {
    public:
        virtual ~TrackFactory() = default;
        virtual std::shared_ptr<Track> make() = 0;
    };
    
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
    
    class MultiTrackTargetList : public Track, public TargetList {
        TargetListFactory *targetListFactory;
        DirectoryReader *directoryReader;
    public:
        MultiTrackTargetList(TargetListFactory *, DirectoryReader *);
        void loadFromDirectory(std::string directory) override;
        std::string next() override;
        std::string current() override;
        void reset(const Track::Settings &) override;
        void pushDown() override;
        void pushUp() override;
        int x() override;
        bool complete() override;
        int reversals() override;
    };
}

#endif
