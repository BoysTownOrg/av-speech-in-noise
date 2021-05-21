#ifndef AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_INPUT_HPP_
#define AV_SPEECH_IN_NOISE_PRESENTATION_INCLUDE_PRESENTATION_INPUT_HPP_

#include <string>
#include <sstream>

namespace av_speech_in_noise {
class BadInput : public std::runtime_error {
  public:
    explicit BadInput(const std::string &s) : std::runtime_error{s} {}
};

inline auto readInteger(const std::string &x, const std::string &identifier)
    -> int {
    try {
        return std::stoi(x);
    } catch (const std::invalid_argument &) {
        std::stringstream stream;
        stream << '"' << x << '"';
        stream << " is not a valid ";
        stream << identifier;
        stream << '.';
        throw BadInput{stream.str()};
    }
}
}

#endif
