#ifndef target_list_SubdirectoryTargetListReader_hpp
#define target_list_SubdirectoryTargetListReader_hpp

#include <recognition-test/RecognitionTestModel.hpp>
#include <memory>

namespace target_list {
    class TargetListFactory {
    public:
        virtual ~TargetListFactory() = default;
        virtual std::shared_ptr<av_speech_in_noise::TargetList> make() = 0;
    };
    
    class DirectoryReader {
    public:
        virtual ~DirectoryReader() = default;
        virtual std::vector<std::string> subDirectories(std::string directory) = 0;
        virtual std::vector<std::string> filesIn(std::string directory) = 0;
    };
    
    class SubdirectoryTargetListReader : public av_speech_in_noise::TargetListReader {
        TargetListFactory *targetListFactory;
        DirectoryReader *directoryReader;
    public:
        SubdirectoryTargetListReader(TargetListFactory *, DirectoryReader *);
        lists_type read(std::string directory) override;
    private:
        std::vector<std::string> subDirectories(std::string directory);

    };
}

#endif
