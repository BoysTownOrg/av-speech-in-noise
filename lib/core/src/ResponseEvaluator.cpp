#include "ResponseEvaluator.hpp"
#include <gsl/gsl>
#include <algorithm>
#include <cctype>
#include <regex>
#include <filesystem>

namespace av_speech_in_noise {
static auto consonant(const std::string &match) -> char {
    if (match == "bi")
        return 'b';
    if (match == "si")
        return 'c';
    if (match == "di")
        return 'd';
    if (match == "hi")
        return 'h';
    if (match == "ki")
        return 'k';
    if (match == "mi")
        return 'm';
    if (match == "ni")
        return 'n';
    if (match == "pi")
        return 'p';
    if (match == "shi")
        return 's';
    if (match == "ti")
        return 't';
    if (match == "vi")
        return 'v';
    if (match == "zi")
        return 'z';
    return '\0';
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

static auto correctConsonant(const LocalUrl &url) -> char {
    const auto stem{av_speech_in_noise::stem(url)};
    std::regex pattern{"choose_(.*?)_.*"};
    std::smatch match;
    std::regex_search(stem, match, pattern);
    return match.size() > 1 ? consonant(match[1]) : '\0';
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

auto ResponseEvaluatorImpl::correctConsonant(const LocalUrl &url) -> char {
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
