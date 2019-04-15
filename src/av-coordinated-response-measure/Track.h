#ifndef av_coordinated_response_measure_Track_h
#define av_coordinated_response_measure_Track_h

#include <vector>

namespace av_coordinated_response_measure {
    class Track {
    public:
        virtual ~Track() = default;
        struct Parameters {
            struct RunSequence {
                int runCount;
                int stepSize;
                int down;
                int up;
            };
            using Rule = typename std::vector<RunSequence>;
            Rule runSequences;
            int startingX;
        };
        virtual void reset(const Parameters &) = 0;
        virtual void pushDown() = 0;
        virtual void pushUp() = 0;
        virtual int x() = 0;
        virtual bool complete() = 0;
        virtual int reversals() = 0;
    };
}

#endif
