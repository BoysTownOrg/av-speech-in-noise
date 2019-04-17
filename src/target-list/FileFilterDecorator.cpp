#include "FileFilterDecorator.hpp"

namespace target_list {
    FileFilterDecorator::FileFilterDecorator(
        DirectoryReader *reader,
        std::vector<std::string> filters
    ) :
        filters{std::move(filters)},
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
        for (const auto &file : files)
            for (const auto &filter : filters)
                if (endingMatchesFilter(file, filter))
                    filtered_.push_back(file);
        return filtered_;
    }
    
    bool FileFilterDecorator::endingMatchesFilter(
        const std::string &file,
        const std::string &filter
    ) {
        return
            file.length() >= filter.length() &&
            0 == file.compare(
                file.length() - filter.length(),
                filter.length(),
                filter
            );
    }
}
