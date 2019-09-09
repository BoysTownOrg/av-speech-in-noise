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

class FileExtensionFilter : public FileFilter {
    std::vector<std::string> filters;
public:
    explicit FileExtensionFilter(std::vector<std::string> filters);
    std::vector<std::string> filter(std::vector<std::string>) override;
private:
    bool endingMatchesFilter(
        const std::string &,
        const std::string &filter
    );
};

class FileIdentifierFilter : public FileFilter {
    std::string identifier;
public:
    explicit FileIdentifierFilter(std::string identifier);
    std::vector<std::string> filter(std::vector<std::string>) override;
private:
    bool containsIdentifier(
        const std::string &
    );
};

class FileIdentifierExcluderFilterDecorator : public FileFilter {
    std::vector<std::string> identifiers;
public:
    explicit FileIdentifierExcluderFilterDecorator(
        std::vector<std::string> identifiers
    );
    std::vector<std::string> filter(std::vector<std::string>) override;
};

class RandomSubsetFilesDecorator : public FileFilter {
    Randomizer *randomizer;
    int N;
public:
    RandomSubsetFilesDecorator(Randomizer *, int);
    std::vector<std::string> filter(std::vector<std::string>) override;
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
