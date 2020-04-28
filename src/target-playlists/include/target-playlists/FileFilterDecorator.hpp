#ifndef AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_FILEFILTERDECORATOR_HPP_
#define AV_SPEECH_IN_NOISE_TARGET_LIST_INCLUDE_TARGET_LIST_FILEFILTERDECORATOR_HPP_

#include "RandomizedTargetPlaylist.hpp"
#include <vector>
#include <string>

namespace av_speech_in_noise {
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
    auto filesIn(const LocalUrl &directory) -> LocalUrls override;
    auto subDirectories(const LocalUrl &) -> LocalUrls override;
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
    target_list::Randomizer *randomizer;
    int N;

  public:
    RandomSubsetFiles(target_list::Randomizer *, int);
    auto filter(LocalUrls) -> LocalUrls override;
};

class DirectoryReaderComposite : public DirectoryReader {
    std::vector<DirectoryReader *> readers;

  public:
    explicit DirectoryReaderComposite(std::vector<DirectoryReader *>);
    auto filesIn(const LocalUrl &) -> LocalUrls override;
    auto subDirectories(const LocalUrl &) -> LocalUrls override;
};
}

#endif
