#ifndef AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_FILEFILTERDECORATOR_HPP_
#define AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_FILEFILTERDECORATOR_HPP_

#include <target-list/RandomizedTargetList.hpp>
#include <vector>
#include <string>

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
};

class FileIdentifierFilter : public FileFilter {
    std::string identifier;

  public:
    explicit FileIdentifierFilter(std::string identifier);
    std::vector<std::string> filter(std::vector<std::string>) override;

  private:
    bool containsIdentifier(const std::string &);
};

class FileIdentifierExcluderFilter : public FileFilter {
    std::vector<std::string> identifiers;

  public:
    explicit FileIdentifierExcluderFilter(std::vector<std::string> identifiers);
    std::vector<std::string> filter(std::vector<std::string>) override;
};

class RandomSubsetFiles : public FileFilter {
    Randomizer *randomizer;
    int N;

  public:
    RandomSubsetFiles(Randomizer *, int);
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
