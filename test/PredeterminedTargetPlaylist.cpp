#include <av-speech-in-noise/core/TargetPlaylist.hpp>

#include <gtest/gtest.h>

#include <filesystem>

namespace av_speech_in_noise {
class PredeterminedTargetPlaylist : public TargetPlaylist {
  public:
    virtual void loadFromDirectory(const LocalUrl &directory) {}
    virtual auto next() -> LocalUrl { return {}; }
    virtual auto current() -> LocalUrl { return {}; }
    virtual auto directory() -> LocalUrl { return {}; }
};

class PredeterminedTargetPlaylistTests : public ::testing::Test {};

TEST_F(PredeterminedTargetPlaylistTests, tbd) {
    PredeterminedTargetPlaylist playlist;
}
}