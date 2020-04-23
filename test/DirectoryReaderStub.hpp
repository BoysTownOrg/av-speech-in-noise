#ifndef TESTS_DIRECTORYREADERSTUB_HPP_
#define TESTS_DIRECTORYREADERSTUB_HPP_

#include <target-list/SubdirectoryTargetListReader.hpp>
#include <utility>

namespace target_list {
class DirectoryReaderStub : public DirectoryReader {
    std::vector<av_speech_in_noise::LocalUrl> subDirectories_{};
    std::vector<av_speech_in_noise::LocalUrl> fileNames_;
    std::string directory_{};

  public:
    void setFileNames(std::vector<av_speech_in_noise::LocalUrl> files) {
        fileNames_ = std::move(files);
    }

    auto filesIn(const av_speech_in_noise::LocalUrl &directory)
        -> std::vector<av_speech_in_noise::LocalUrl> override {
        directory_ = directory.path;
        return fileNames_;
    }

    auto subDirectories(const av_speech_in_noise::LocalUrl &d)
        -> std::vector<av_speech_in_noise::LocalUrl> override {
        directory_ = d.path;
        return subDirectories_;
    }

    [[nodiscard]] auto directory() const { return directory_; }

    void setSubDirectories(std::vector<av_speech_in_noise::LocalUrl> v) {
        subDirectories_ = std::move(v);
    }
};
}

#endif
