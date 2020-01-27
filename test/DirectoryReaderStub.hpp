#ifndef TESTS_DIRECTORYREADERSTUB_HPP_
#define TESTS_DIRECTORYREADERSTUB_HPP_

#include <target-list/SubdirectoryTargetListReader.hpp>
#include <utility>

namespace target_list {
class DirectoryReaderStub : public DirectoryReader {
    std::vector<std::string> subDirectories_{};
    std::vector<std::string> fileNames_;
    std::string directory_{};

  public:
    void setFileNames(std::vector<std::string> files) {
        fileNames_ = std::move(files);
    }

    auto filesIn(std::string directory) -> std::vector<std::string> override {
        directory_ = std::move(directory);
        return fileNames_;
    }

    auto subDirectories(std::string d) -> std::vector<std::string> override {
        directory_ = std::move(d);
        return subDirectories_;
    }

    [[nodiscard]] auto directory() const { return directory_; }

    void setSubDirectories(std::vector<std::string> v) {
        subDirectories_ = std::move(v);
    }
};
}

#endif
