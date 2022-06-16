#include <av-speech-in-noise/core/TargetPlaylist.hpp>

#include <gtest/gtest.h>

#include <filesystem>

namespace av_speech_in_noise {
class PredeterminedTargetPlaylist : public TargetPlaylist {
  public:
    void load(const LocalUrl &) override {}
    auto next() -> LocalUrl override { return {}; }
    auto current() -> LocalUrl override { return {}; }
    auto directory() -> LocalUrl override { return {}; }
};

class PredeterminedTargetPlaylistTests : public ::testing::Test {};

TEST_F(PredeterminedTargetPlaylistTests, tbd) {
    PredeterminedTargetPlaylist playlist;
}
}