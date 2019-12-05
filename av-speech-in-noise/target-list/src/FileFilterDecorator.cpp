#include "FileFilterDecorator.hpp"
#include <gsl/gsl>
#include <numeric>

namespace target_list {
FileFilterDecorator::FileFilterDecorator(
    DirectoryReader *reader, FileFilter *filter)
    : reader{reader}, filter{filter} {}

std::vector<std::string> FileFilterDecorator::filesIn(std::string directory) {
    return filter->filter(reader->filesIn(std::move(directory)));
}

std::vector<std::string> FileFilterDecorator::subDirectories(
    std::string directory) {
    return reader->subDirectories(std::move(directory));
}

FileExtensionFilter::FileExtensionFilter(std::vector<std::string> filters)
    : filters{std::move(filters)} {}

static bool endingMatchesFilter(
    const std::string &file, const std::string &filter) {
    return file.length() >= filter.length() &&
        0 ==
        file.compare(file.length() - filter.length(), filter.length(), filter);
}

std::vector<std::string> FileExtensionFilter::filter(
    std::vector<std::string> files) {
    std::vector<std::string> filtered_{};
    for (const auto &file : files)
        for (const auto &filter : filters)
            if (endingMatchesFilter(file, filter))
                filtered_.push_back(file);
    return filtered_;
}

FileIdentifierExcluderFilter::FileIdentifierExcluderFilter(
    std::vector<std::string> identifiers)
    : identifiers{std::move(identifiers)} {}

static bool endsWith(const std::string &s, const std::string &what) {
    auto withoutExtension = s.substr(0, s.find('.'));
    return endingMatchesFilter(withoutExtension, what);
}

std::vector<std::string> FileIdentifierExcluderFilter::filter(
    std::vector<std::string> files) {
    std::vector<std::string> filtered_{};
    for (const auto &file : files) {
        bool exclude = false;
        for (const auto &identifier : identifiers)
            if (endsWith(file, identifier)) {
                exclude = true;
                break;
            }
        if (!exclude)
            filtered_.push_back(file);
    }
    return filtered_;
}

FileIdentifierFilter::FileIdentifierFilter(std::string identifier)
    : identifier{std::move(identifier)} {}

std::vector<std::string> FileIdentifierFilter::filter(
    std::vector<std::string> files) {
    std::vector<std::string> filtered_{};
    for (const auto &file : files)
        if (containsIdentifier(file))
            filtered_.push_back(file);
    return filtered_;
}

bool FileIdentifierFilter::containsIdentifier(const std::string &file) {
    return file.find(identifier) != std::string::npos;
}

RandomSubsetFiles::RandomSubsetFiles(Randomizer *randomizer, int N)
    : randomizer{randomizer}, N{N} {}

std::vector<std::string> RandomSubsetFiles::filter(
    std::vector<std::string> files) {
    if (files.size() < gsl::narrow<size_t>(N))
        return files;
    std::vector<int> indices(files.size());
    std::iota(indices.begin(), indices.end(), 0);
    randomizer->shuffle(indices.begin(), indices.end());
    std::vector<std::string> subset;
    for (int i = 0; i < N; ++i)
        subset.push_back(files.at(indices.at(i)));
    return subset;
}

DirectoryReaderComposite::DirectoryReaderComposite(
    std::vector<DirectoryReader *> readers)
    : readers{std::move(readers)} {}

std::vector<std::string> DirectoryReaderComposite::subDirectories(
    std::string directory) {
    return readers.front()->subDirectories(std::move(directory));
}

std::vector<std::string> DirectoryReaderComposite::filesIn(
    std::string directory) {
    std::vector<std::string> files;
    for (auto r : readers) {
        auto next = r->filesIn(directory);
        files.insert(files.end(), next.begin(), next.end());
    }
    return files;
}
}
