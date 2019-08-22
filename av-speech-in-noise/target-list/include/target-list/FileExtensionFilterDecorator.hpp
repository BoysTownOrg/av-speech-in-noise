#ifndef target_list_FileFilterDecorator_hpp
#define target_list_FileFilterDecorator_hpp

#include <target-list/RandomizedTargetList.hpp>

namespace target_list {
    class FileExtensionFilterDecorator : public DirectoryReader {
        std::vector<std::string> filters;
        DirectoryReader *reader;
    public:
        FileExtensionFilterDecorator(DirectoryReader *, std::vector<std::string> filters);
        std::vector<std::string> filesIn(std::string directory) override;
        std::vector<std::string> subDirectories(std::string directory) override;
    private:
        bool endingMatchesFilter(
            const std::string &,
            const std::string &filter
        );
        std::vector<std::string> filtered(std::vector<std::string>);
    };

    class FileIdentifierFilterDecorator : public DirectoryReader {
        std::string identifier;
        DirectoryReader *reader;
    public:
        FileIdentifierFilterDecorator(DirectoryReader *, std::string identifier);
        std::vector<std::string> filesIn(std::string directory) override;
        std::vector<std::string> subDirectories(std::string directory) override;
    private:
        bool containsIdentifier(
            const std::string &
        );
        std::vector<std::string> filtered(std::vector<std::string>);
    };
}

#endif
