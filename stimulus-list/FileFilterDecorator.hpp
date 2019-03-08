#ifndef stimulus_list_FileFilterDecorator_hpp
#define stimulus_list_FileFilterDecorator_hpp

#include <stimulus-list/RandomizedStimulusList.hpp>

namespace stimulus_list {
    class FileFilterDecorator : public DirectoryReader {
        std::string filter;
        DirectoryReader *reader;
    public:
        FileFilterDecorator(DirectoryReader *reader, std::string filter);
        std::vector<std::string> filesIn(std::string directory) override;
    private:
        std::vector<std::string> filtered(std::vector<std::string>);
    };
}

#endif
