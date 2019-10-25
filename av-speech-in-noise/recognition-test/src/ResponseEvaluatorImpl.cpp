#include "ResponseEvaluatorImpl.hpp"
#include <algorithm>
#include <cctype>

namespace av_speech_in_noise {
int ResponseEvaluatorImpl::invalidNumber = -1;

bool ResponseEvaluatorImpl::correct(
    const std::string &filePath,
    const coordinate_response_measure::SubjectResponse &r
) {
    return
        correctNumber(filePath) == r.number &&
        correctColor(filePath) == r.color &&
        r.color != coordinate_response_measure::Color::notAColor &&
        r.number != invalidNumber;
}

int ResponseEvaluatorImpl::correctNumber(const std::string &filePath) {
    try {
        return correctNumber_(filePath);
    }
    catch (const std::invalid_argument &) {
        return invalidNumber;
    }
}

int ResponseEvaluatorImpl::correctNumber_(const std::string &filePath) {
    auto leadingPathLength_ = leadingPathLength(filePath);
    auto number = filePath.substr(
        leadingPathLength_ + colorNameLength(filePath, leadingPathLength_),
        1
    );
    return std::stoi(number);
}

unsigned long ResponseEvaluatorImpl::leadingPathLength(const std::string &filePath) {
    return filePath.find_last_of("/") + 1;
}

long ResponseEvaluatorImpl::colorNameLength(const std::string &filePath, unsigned long leadingPathLength_) {
    auto fileNameBeginning = filePath.begin() + leadingPathLength_;
    auto notAlpha = std::find_if(
        fileNameBeginning,
        filePath.end(),
        [](unsigned char c) { return !std::isalpha(c); }
    );
    return std::distance(fileNameBeginning, notAlpha);
}

coordinate_response_measure::Color ResponseEvaluatorImpl::correctColor(const std::string &filePath) {
    auto leadingPathLength_ = leadingPathLength(filePath);
    auto colorName = filePath.substr(leadingPathLength_, colorNameLength(filePath, leadingPathLength_));
    return color(colorName);
}

coordinate_response_measure::Color ResponseEvaluatorImpl::color(const std::string &colorName) {
    using coordinate_response_measure::Color;
    if (colorName == "green")
        return Color::green;
    else if (colorName == "blue")
        return Color::blue;
    else if (colorName == "red")
        return Color::red;
    else if (colorName == "white")
        return Color::white;
    else
        return Color::notAColor;
}

std::string ResponseEvaluatorImpl::fileName(const std::string &filePath) {
    return filePath.substr(leadingPathLength(filePath));
}
}
