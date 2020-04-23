#ifndef AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_FILEFILTERDECORATOR_HPP_
#define AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_FILEFILTERDECORATOR_HPP_

#include "RandomizedTargetList.hpp"
#include <vector>
#include <string>

namespace target_list {
class FileFilter {
  public:
    virtual ~FileFilter() = default;
    virtual auto filter(LocalUrls) -> LocalUrls = 0;
};

class FileFilterDecorator : public DirectoryReader {
    DirectoryReader *reader;
    FileFilter *filter;

  public:
    FileFilterDecorator(DirectoryReader *, FileFilter *);
    auto filesIn(const av_speech_in_noise::LocalUrl &directory)
        -> LocalUrls override;
    auto subDirectories(const av_speech_in_noise::LocalUrl &)
        -> LocalUrls override;
};

class FileExtensionFilter : public FileFilter {
    std::vector<std::string> filters;

  public:
    explicit FileExtensionFilter(std::vector<std::string> filters);
    auto filter(LocalUrls) -> LocalUrls override;
};

class FileIdentifierFilter : public FileFilter {
    std::string identifier;

  public:
    explicit FileIdentifierFilter(std::string identifier);
    auto filter(LocalUrls) -> LocalUrls override;

  private:
    auto containsIdentifier(const std::string &) -> bool;
};

class FileIdentifierExcluderFilter : public FileFilter {
    std::vector<std::string> identifiers;

  public:
    explicit FileIdentifierExcluderFilter(std::vector<std::string> identifiers);
    auto filter(LocalUrls) -> LocalUrls override;
};

class RandomSubsetFiles : public FileFilter {
    Randomizer *randomizer;
    int N;

  public:
    RandomSubsetFiles(Randomizer *, int);
    auto filter(LocalUrls) -> LocalUrls override;
};

class DirectoryReaderComposite : public DirectoryReader {
    std::vector<DirectoryReader *> readers;

  public:
    explicit DirectoryReaderComposite(std::vector<DirectoryReader *>);
    auto filesIn(const av_speech_in_noise::LocalUrl &) -> LocalUrls override;
    auto subDirectories(const av_speech_in_noise::LocalUrl &)
        -> LocalUrls override;
};
}

#endif
