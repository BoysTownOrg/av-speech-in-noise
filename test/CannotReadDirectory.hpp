#ifndef AV_SPEECH_IN_NOISE_TEST_CANNOTREADDIRECTORY_HPP_
#define AV_SPEECH_IN_NOISE_TEST_CANNOTREADDIRECTORY_HPP_

#include <av-speech-in-noise/playlist/SubdirectoryTargetPlaylistReader.hpp>

namespace av_speech_in_noise {
class CannotReadDirectory : public DirectoryReader {
  public:
    auto subDirectories(const LocalUrl &directory) -> LocalUrls override {
        throw CannotRead{directory.path};
    }

    auto filesIn(const LocalUrl &directory) -> LocalUrls override {
        throw CannotRead{directory.path};
    }
};
}

#endif
