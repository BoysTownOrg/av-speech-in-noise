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
    
    class MultiTrackTargetList : public TargetList {
        TargetListFactory *targetListFactory;
        DirectoryReader *directoryReader;
    public:
        class EventListener {
        public:
            virtual ~EventListener() = default;
            void listsLoaded(int);
            void listSelected(int);
        };
        MultiTrackTargetList(TargetListFactory *, DirectoryReader *);
        void loadFromDirectory(std::string directory) override;
        std::string next() override;
        std::string current() override;
    };
}

#endif
