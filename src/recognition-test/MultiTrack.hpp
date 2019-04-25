#ifndef av_coordinate_response_measure_MultiTrack_hpp
#define av_coordinate_response_measure_MultiTrack_hpp

#include <recognition-test/RecognitionTestModel.hpp>

namespace av_coordinate_response_measure {
    class MultiTrack : public Track {

        void reset(const Track::Settings &) override;
        
        void pushDown() override;
        
        void pushUp() override;
        
        int x() override;
        
        bool complete() override;
        
        int reversals() override;
        
    };
}
#endif
