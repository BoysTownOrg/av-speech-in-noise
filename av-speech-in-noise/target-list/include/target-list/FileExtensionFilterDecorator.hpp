#ifndef target_list_FileFilterDecorator_hpp
#define target_list_FileFilterDecorator_hpp

#include <target-list/RandomizedTargetList.hpp>
#include <vector>

namespace target_list {
    class FileFilter {
    public:
        virtual ~FileFilter() = default;
        virtual std::vector<std::string> filter(std::vector<std::string>) = 0;
    };

    class FileFilterDecorator : public DirectoryReader {
        DirectoryReader *reader;
        FileFilter *filter;
    public:
        FileFilterDecorator(DirectoryReader *, FileFilter *);
        std::vector<std::string> filesIn(std::string directory) override;
        std::vector<std::string> subDirectories(std::string directory) override;
    };

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

    class FileIdentifierExcluderFilterDecorator : public DirectoryReader {
        std::vector<std::string> identifiers;
        DirectoryReader *reader;
    public:
        FileIdentifierExcluderFilterDecorator(
            DirectoryReader *,
            std::vector<std::string> identifiers
        );
        std::vector<std::string> filesIn(std::string directory) override;
        std::vector<std::string> subDirectories(std::string directory) override;
    };

    class RandomSubsetFilesDecorator : public DirectoryReader {
        DirectoryReader *reader;
        Randomizer *randomizer;
        int N;
    public:
        RandomSubsetFilesDecorator(DirectoryReader *, Randomizer *, int);
        std::vector<std::string> filesIn(std::string directory) override;
        std::vector<std::string> subDirectories(std::string directory) override;
    };

    class DirectoryReaderComposite : public DirectoryReader {
        std::vector<DirectoryReader *> readers;
    public:
        explicit DirectoryReaderComposite(std::vector<DirectoryReader *>);
        std::vector<std::string> filesIn(std::string directory) override;
        std::vector<std::string> subDirectories(std::string directory) override;
    };
}

#endif
