#ifndef av_coordinate_response_measure_MultiTrack_hpp
#define av_coordinate_response_measure_MultiTrack_hpp

#include <recognition-test/RecognitionTestModel.hpp>

namespace av_coordinate_response_measure {
    class TrackFactory {
    public:
        virtual ~TrackFactory() = default;
        virtual std::shared_ptr<Track> make(const Track::Settings &) = 0;
    };
    
    class MultiTrack : public Track {
    public:
        void reset(const Track::Settings &) override;
        void pushDown() override;
        void pushUp() override;
        int x() override;
        bool complete() override;
        int reversals() override;
    };
}

#endif
