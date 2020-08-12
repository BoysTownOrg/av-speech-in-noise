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

auto ResponseEvaluatorImpl::correct(const LocalUrl &filePath,
    const coordinate_response_measure::Response &r) -> bool {
    return correctNumber(filePath) == r.number &&
        correctColor(filePath) == r.color &&
        r.color != coordinate_response_measure::Color::unknown &&
        r.number != invalidNumber;
}

static auto fileName(const LocalUrl &filePath) -> std::string {
    return subString(filePath.path, leadingPathLength(filePath.path));
}

static auto stem(const LocalUrl &file) -> std::string {
    const auto fileName{av_speech_in_noise::fileName(file)};
    auto dot{fileName.find('.')};
    return fileName.substr(0, dot);
}

auto ResponseEvaluatorImpl::correct(
    const LocalUrl &file, const ConsonantResponse &r) -> bool {
    auto stem{av_speech_in_noise::stem(file)};
    return stem.size() == 1 && stem.front() == r.consonant;
}

static auto colorNameLength(
    const LocalUrl &filePath, gsl::index leadingPathLength_) -> gsl::index {
    auto fileNameBeginning = filePath.path.begin() + leadingPathLength_;
    auto notAlpha = std::find_if(fileNameBeginning, filePath.path.end(),
        [](unsigned char c) { return std::isalpha(c) == 0; });
    return std::distance(fileNameBeginning, notAlpha);
}

static auto correctNumber_(const LocalUrl &filePath) -> int {
    auto leadingPathLength_ = leadingPathLength(filePath.path);
    auto number = subString(filePath.path,
        leadingPathLength_ + colorNameLength(filePath, leadingPathLength_), 1);
    return std::stoi(number);
}

auto ResponseEvaluatorImpl::correctNumber(const LocalUrl &filePath) -> int {
    try {
        return correctNumber_(filePath);
    } catch (const std::invalid_argument &) {
        return invalidNumber;
    }
}

auto ResponseEvaluatorImpl::correctConsonant(const LocalUrl &file) -> char {
    return av_speech_in_noise::stem(file).front();
}

auto ResponseEvaluatorImpl::fileName(const LocalUrl &file) -> std::string {
    return av_speech_in_noise::fileName(file);
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

auto ResponseEvaluatorImpl::correctColor(const LocalUrl &filePath)
    -> coordinate_response_measure::Color {
    auto leadingPathLength_ = leadingPathLength(filePath.path);
    auto colorName = subString(filePath.path, leadingPathLength_,
        colorNameLength(filePath, leadingPathLength_));
    return color(colorName);
}
}
