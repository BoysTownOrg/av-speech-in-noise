#include "ResponseEvaluator.hpp"

#include <gsl/gsl>

#include <regex>
#include <filesystem>

namespace av_speech_in_noise {
static auto consonant(const std::string &match) -> Consonant {
    if (match == "bi")
        return Consonant::bi;
    if (match == "si")
        return Consonant::si;
    if (match == "di")
        return Consonant::di;
    if (match == "hi")
        return Consonant::hi;
    if (match == "ki")
        return Consonant::ki;
    if (match == "mi")
        return Consonant::mi;
    if (match == "ni")
        return Consonant::ni;
    if (match == "pi")
        return Consonant::pi;
    if (match == "shi")
        return Consonant::shi;
    if (match == "ti")
        return Consonant::ti;
    if (match == "vi")
        return Consonant::vi;
    if (match == "zi")
        return Consonant::zi;
    if (match == "thi")
        return Consonant::thi;
    if (match == "fi")
        return Consonant::fi;
    return Consonant::unknown;
}

static auto color(const std::string &colorName)
    -> coordinate_response_measure::Color {
    using coordinate_response_measure::Color;
    if (colorName == "green")
        return Color::green;
    if (colorName == "blue")
        return Color::blue;
    if (colorName == "red")
        return Color::red;
    if (colorName == "white")
        return Color::white;

    return Color::unknown;
}

static auto fileName(const LocalUrl &url) -> std::string {
    return std::filesystem::path{url.path}.filename();
}

static auto stem(const LocalUrl &url) -> std::string {
    return std::filesystem::path{url.path}.stem();
}

static auto correctConsonant(const LocalUrl &url) -> Consonant {
    const auto stem{av_speech_in_noise::stem(url)};
    std::regex pattern{"choose_(.*?)_.*"};
    std::smatch match;
    std::regex_search(stem, match, pattern);
    return match.size() > 1 ? consonant(match[1]) : Consonant::unknown;
}

static auto correctNumber(const LocalUrl &url) -> int {
    const auto stem{av_speech_in_noise::stem(url)};
    std::regex pattern{"[A-Za-z]*(\\d).*"};
    std::smatch match;
    std::regex_search(stem, match, pattern);
    return match.size() > 1 ? std::stoi(match[1])
                            : ResponseEvaluatorImpl::invalidNumber;
}

static auto correctColor(const LocalUrl &url)
    -> coordinate_response_measure::Color {
    const auto stem{av_speech_in_noise::stem(url)};
    std::regex pattern{"([A-Za-z]*).*"};
    std::smatch match;
    std::regex_search(stem, match, pattern);
    return match.size() > 1 ? color(match[1])
                            : coordinate_response_measure::Color::unknown;
}

const int ResponseEvaluatorImpl::invalidNumber = -1;

auto ResponseEvaluatorImpl::correctColor(const LocalUrl &url)
    -> coordinate_response_measure::Color {
    return av_speech_in_noise::correctColor(url);
}

auto ResponseEvaluatorImpl::correctConsonant(const LocalUrl &url) -> Consonant {
    return av_speech_in_noise::correctConsonant(url);
}

auto ResponseEvaluatorImpl::correctNumber(const LocalUrl &url) -> int {
    return av_speech_in_noise::correctNumber(url);
}

auto ResponseEvaluatorImpl::fileName(const LocalUrl &url) -> std::string {
    return av_speech_in_noise::fileName(url);
}

auto ResponseEvaluatorImpl::correct(const LocalUrl &url,
    const coordinate_response_measure::Response &r) -> bool {
    return av_speech_in_noise::correctNumber(url) == r.number &&
        av_speech_in_noise::correctColor(url) == r.color &&
        r.color != coordinate_response_measure::Color::unknown &&
        r.number != invalidNumber;
}

auto ResponseEvaluatorImpl::correct(
    const LocalUrl &url, const ConsonantResponse &r) -> bool {
    return av_speech_in_noise::correctConsonant(url) == r.consonant;
}
}
