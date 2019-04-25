#ifndef av_coordinate_response_measure_MultiTrackTargetList_hpp
#define av_coordinate_response_measure_MultiTrackTargetList_hpp

#include <recognition-test/RecognitionTestModel.hpp>

namespace av_coordinate_response_measure {
    class TrackFactory {
    public:
        virtual ~TrackFactory() = default;
        virtual std::shared_ptr<Track> make(const Track::Settings &) = 0;
    };
    
    class TargetListFactory {
    public:
        virtual ~TargetListFactory() = default;
        virtual std::shared_ptr<TargetList> make(std::string directory) = 0;
    };
    
    class MultiTrackTargetList : public Track, public TargetList {
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
