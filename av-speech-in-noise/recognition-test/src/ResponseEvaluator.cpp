#include "ResponseEvaluator.hpp"
#include <gsl/gsl>
#include <algorithm>
#include <cctype>

namespace av_speech_in_noise {
static auto leadingPathLength(const std::string &filePath) -> gsl::index {
    return filePath.find_last_of('/') + 1;
}

static auto subString(
    const std::string &s, gsl::index position, gsl::index size) -> std::string {
    return s.substr(position, size);
}

static auto subString(const std::string &s, gsl::index position)
    -> std::string {
    return s.substr(position);
}

int ResponseEvaluatorImpl::invalidNumber = -1;

auto ResponseEvaluatorImpl::correct(const std::string &filePath,
    const coordinate_response_measure::Response &r) -> bool {
    return correctNumber(filePath) == r.number &&
        correctColor(filePath) == r.color &&
        r.color != coordinate_response_measure::Color::notAColor &&
        r.number != invalidNumber;
}

static auto colorNameLength(
    const std::string &filePath, gsl::index leadingPathLength_) -> gsl::index {
    auto fileNameBeginning = filePath.begin() + leadingPathLength_;
    auto notAlpha = std::find_if(fileNameBeginning, filePath.end(),
        [](unsigned char c) { return std::isalpha(c) == 0; });
    return std::distance(fileNameBeginning, notAlpha);
}

static auto correctNumber_(const std::string &filePath) -> int {
    auto leadingPathLength_ = leadingPathLength(filePath);
    auto number = subString(filePath,
        leadingPathLength_ + colorNameLength(filePath, leadingPathLength_), 1);
    return std::stoi(number);
}

auto ResponseEvaluatorImpl::correctNumber(const std::string &filePath) -> int {
    try {
        return correctNumber_(filePath);
    } catch (const std::invalid_argument &) {
        return invalidNumber;
    }
}

auto ResponseEvaluatorImpl::fileName(const std::string &filePath)
    -> std::string {
    return subString(filePath, leadingPathLength(filePath));
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
    auto colorName = subString(filePath, leadingPathLength_,
        colorNameLength(filePath, leadingPathLength_));
    return color(colorName);
}
}
