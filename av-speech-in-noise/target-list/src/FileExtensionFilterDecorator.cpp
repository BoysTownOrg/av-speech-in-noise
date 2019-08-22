#include "FileExtensionFilterDecorator.hpp"
#include <numeric>

namespace target_list {
    FileExtensionFilterDecorator::FileExtensionFilterDecorator(
        DirectoryReader *reader,
        std::vector<std::string> filters
    ) :
        filters{std::move(filters)},
        reader{reader} {}

    std::vector<std::string> FileExtensionFilterDecorator::filesIn(
        std::string directory
    ) {
        return filtered(reader->filesIn(std::move(directory)));
    }
    
    std::vector<std::string> FileExtensionFilterDecorator::filtered(
        std::vector<std::string> files
    ) {
        std::vector<std::string> filtered_{};
        for (const auto &file : files)
            for (const auto &filter : filters)
                if (endingMatchesFilter(file, filter))
                    filtered_.push_back(file);
        return filtered_;
    }
    
    bool FileExtensionFilterDecorator::endingMatchesFilter(
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
    
    std::vector<std::string> FileExtensionFilterDecorator::subDirectories(
        std::string directory
    ) {
        return reader->subDirectories(std::move(directory));
    }

    FileIdentifierFilterDecorator::FileIdentifierFilterDecorator(
        DirectoryReader *reader,
        std::string identifier
    ) :
        identifier{std::move(identifier)},
        reader{reader} {}

    std::vector<std::string> FileIdentifierFilterDecorator::filesIn(
        std::string directory
    ) {
        return filtered(reader->filesIn(std::move(directory)));
    }
    
    std::vector<std::string> FileIdentifierFilterDecorator::filtered(
        std::vector<std::string> files
    ) {
        std::vector<std::string> filtered_{};
        for (const auto &file : files)
            if (containsIdentifier(file))
                filtered_.push_back(file);
        return filtered_;
    }

    bool FileIdentifierFilterDecorator::containsIdentifier(const std::string &file) {
        return file.find(identifier) != std::string::npos;
    }
    
    std::vector<std::string> FileIdentifierFilterDecorator::subDirectories(
        std::string directory
    ) {
        return reader->subDirectories(std::move(directory));
    }
    
    RandomSubsetFilesDecorator::RandomSubsetFilesDecorator(
        DirectoryReader *reader, 
        Randomizer *randomizer, 
        int
    ) :
        reader{reader},
        randomizer{randomizer} {}
    
    std::vector<std::string> RandomSubsetFilesDecorator::filesIn(
        std::string directory
    ) { 
        auto files = reader->filesIn(std::move(directory));
        std::vector<int> indices(files.size());
        std::iota(indices.begin(), indices.end(), 0);
        randomizer->shuffle(indices.begin(), indices.end());
        return {directory};
    }
    
    std::vector<std::string> RandomSubsetFilesDecorator::subDirectories(
        std::string directory
    ) {
        return reader->subDirectories(std::move(directory));
    }
}
