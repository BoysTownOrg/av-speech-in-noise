#include "ResponseEvaluator.hpp"

#include <gsl/gsl>

#include <regex>
#include <filesystem>

namespace av_speech_in_noise {
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
}
