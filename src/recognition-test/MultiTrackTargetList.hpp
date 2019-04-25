#ifndef av_coordinate_response_measure_MultiTrackTargetList_hpp
#define av_coordinate_response_measure_MultiTrackTargetList_hpp

#include <recognition-test/RecognitionTestModel.hpp>

namespace av_coordinate_response_measure {
    class MultiTrackTargetList : public TargetList {
        void loadFromDirectory(std::string directory) override;
        std::string next() override;
        std::string current() override;
    };
}

#endif
