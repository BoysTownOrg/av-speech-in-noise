#include "FileFilterDecorator.hpp"

namespace stimulus_list {
    FileFilterDecorator::FileFilterDecorator(
        DirectoryReader *reader,
        std::string filter
    ) :
        filter{filter},
        reader{reader} {}

    std::vector<std::string> FileFilterDecorator::filesIn(
        std::string directory
    ) {
        return filtered(reader->filesIn(std::move(directory)));
    }
    
    std::vector<std::string> FileFilterDecorator::filtered(
        std::vector<std::string> files
    ) {
        std::vector<std::string> filtered_{};
        for (auto &file : files)
            if (endMatchesFilter(file))
                filtered_.push_back(file);
        return filtered_;
    }
    
    bool FileFilterDecorator::endMatchesFilter(const std::string &file) {
        return
            file.length() >= filter.length() &&
            0 == file.compare(
                file.length() - filter.length(),
                filter.length(),
                filter
            );
    }
}
