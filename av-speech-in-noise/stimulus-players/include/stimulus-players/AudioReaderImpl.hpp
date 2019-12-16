#ifndef AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_AUDIOREADERIMPL_HPP_
#define AV_SPEECH_IN_NOISE_STIMULUS_PLAYERS_INCLUDE_STIMULUS_PLAYERS_AUDIOREADERIMPL_HPP_

#include "AudioReader.hpp"
#include <string>
#include <vector>
#include <memory>

namespace stimulus_players {
class AudioBuffer {
  public:
    virtual ~AudioBuffer() = default;
    virtual auto channels() -> int = 0;
    virtual auto channel(int) -> std::vector<int> = 0;
    virtual auto empty() -> bool = 0;
};

class BufferedAudioReader {
  public:
    virtual ~BufferedAudioReader() = default;
    virtual void loadFile(std::string) = 0;
    virtual auto failed() -> bool = 0;
    virtual auto readNextBuffer() -> std::shared_ptr<AudioBuffer> = 0;
    virtual auto minimumPossibleSample() -> int = 0;
};

class AudioReaderImpl : public AudioReader {
  public:
    explicit AudioReaderImpl(BufferedAudioReader *);
    auto read(std::string filePath) -> audio_type override;

  private:
    void loadFile(std::string filePath);

    BufferedAudioReader *reader;
};
}

#endif
