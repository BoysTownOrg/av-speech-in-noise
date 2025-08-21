#ifndef AV_SPEECH_IN_NOISE_TEST_TARGETLISTSETREADERSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_TARGETLISTSETREADERSTUB_HPP_

#include <av-speech-in-noise/core/AdaptiveMethod.hpp>

#include <utility>

namespace av_speech_in_noise {
class TargetPlaylistSetReaderStub : public TargetPlaylistReader {
    lists_type targetLists_;
    std::string directory_;

  public:
    void setTargetPlaylists(lists_type lists) {
        targetLists_ = std::move(lists);
    }

    auto read(const LocalUrl &d) -> lists_type override {
        directory_ = d.path;
        return targetLists_;
    }

    void attach(TargetPlaylistFactory *f) override { playlistFactory = f; }

    [[nodiscard]] auto directory() const { return directory_; }

    TargetPlaylistFactory *playlistFactory;
};
}

#endif
