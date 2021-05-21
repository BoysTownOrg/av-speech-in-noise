#ifndef TESTS_DIRECTORYREADERSTUB_HPP_
#define TESTS_DIRECTORYREADERSTUB_HPP_

#include <av-speech-in-noise/playlist/SubdirectoryTargetPlaylistReader.hpp>
#include <utility>

namespace av_speech_in_noise {
class DirectoryReaderStub : public DirectoryReader {
    std::vector<LocalUrl> subDirectories_{};
    std::vector<LocalUrl> fileNames_;
    std::string directory_{};

  public:
    void setFileNames(std::vector<LocalUrl> files) {
        fileNames_ = std::move(files);
    }

    auto filesIn(const LocalUrl &directory) -> std::vector<LocalUrl> override {
        directory_ = directory.path;
        return fileNames_;
    }

    auto subDirectories(const LocalUrl &d) -> std::vector<LocalUrl> override {
        directory_ = d.path;
        return subDirectories_;
    }

    [[nodiscard]] auto directory() const { return directory_; }

    void setSubDirectories(std::vector<LocalUrl> v) {
        subDirectories_ = std::move(v);
    }
};
}

#endif
