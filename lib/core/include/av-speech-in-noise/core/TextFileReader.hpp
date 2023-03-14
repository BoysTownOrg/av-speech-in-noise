#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TEXTFILEREADER_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_TEXTFILEREADER_HPP_

#include "av-speech-in-noise/Interface.hpp"
#include "av-speech-in-noise/Model.hpp"

#include <string>

namespace av_speech_in_noise {
class TextFileReader {
  public:
    AV_SPEECH_IN_NOISE_INTERFACE_SPECIAL_MEMBER_FUNCTIONS(TextFileReader);
    class FileDoesNotExist {};
    virtual auto read(const LocalUrl &) -> std::string = 0;
};
}

#endif
