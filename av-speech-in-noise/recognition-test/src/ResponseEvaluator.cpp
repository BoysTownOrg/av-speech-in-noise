#include "ResponseEvaluator.hpp"
#include <algorithm>
#include <cctype>

namespace av_speech_in_noise {
int ResponseEvaluatorImpl::invalidNumber = -1;

auto ResponseEvaluatorImpl::correct(const std::string &filePath,
    const coordinate_response_measure::Response &r) -> bool {
    return correctNumber(filePath) == r.number &&
        correctColor(filePath) == r.color &&
        r.color != coordinate_response_measure::Color::notAColor &&
        r.number != invalidNumber;
}

auto ResponseEvaluatorImpl::correctNumber(const std::string &filePath) -> int {
    try {
        return correctNumber_(filePath);
    } catch (const std::invalid_argument &) {
        return invalidNumber;
    }
}

static auto leadingPathLength(const std::string &filePath) -> unsigned long {
    return filePath.find_last_of('/') + 1;
}

auto ResponseEvaluatorImpl::fileName(const std::string &filePath)
    -> std::string {
    return filePath.substr(leadingPathLength(filePath));
}

auto ResponseEvaluatorImpl::correctNumber_(const std::string &filePath) -> int {
    auto leadingPathLength_ = leadingPathLength(filePath);
    auto number = filePath.substr(
        leadingPathLength_ + colorNameLength(filePath, leadingPathLength_), 1);
    return std::stoi(number);
}

auto ResponseEvaluatorImpl::colorNameLength(
    const std::string &filePath, unsigned long leadingPathLength_) -> long {
    auto fileNameBeginning = filePath.begin() + leadingPathLength_;
    auto notAlpha = std::find_if(fileNameBeginning, filePath.end(),
        [](unsigned char c) { return std::isalpha(c) == 0; });
    return std::distance(fileNameBeginning, notAlpha);
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

    return Color::notAColor;
}

auto ResponseEvaluatorImpl::correctColor(const std::string &filePath)
    -> coordinate_response_measure::Color {
    auto leadingPathLength_ = leadingPathLength(filePath);
    auto colorName = filePath.substr(
        leadingPathLength_, colorNameLength(filePath, leadingPathLength_));
    return color(colorName);
}
}
