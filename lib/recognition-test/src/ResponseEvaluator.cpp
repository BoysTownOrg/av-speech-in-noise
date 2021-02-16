#include "ResponseEvaluator.hpp"
#include <gsl/gsl>
#include <algorithm>
#include <cctype>
#include <regex>

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

const int ResponseEvaluatorImpl::invalidNumber = -1;

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

auto ResponseEvaluatorImpl::correct(
    const LocalUrl &file, const ConsonantResponse &r) -> bool {
    auto stem{av_speech_in_noise::stem(file)};
    std::regex pattern{"choose_(.*?)_.*"};
    std::smatch match;
    std::regex_search(stem, match, pattern);
    return match.size() > 1 && consonant(match[1]) == r.consonant;
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
    auto stem{av_speech_in_noise::stem(file)};
    std::regex pattern{"choose_(.*?)_.*"};
    std::smatch match;
    std::regex_search(stem, match, pattern);
    if (match.size() > 1)
        return consonant(match[1]);
    return '\0';
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

static auto syllable(const std::string &match) -> Syllable {
    if (match == "bi")
        return Syllable::bi;
    if (match == "di")
        return Syllable::di;
    if (match == "dji")
        return Syllable::dji;
    if (match == "fi")
        return Syllable::fi;
    if (match == "gi")
        return Syllable::gi;
    if (match == "hi")
        return Syllable::hi;
    if (match == "ji")
        return Syllable::ji;
    if (match == "ki")
        return Syllable::ki;
    if (match == "li")
        return Syllable::li;
    if (match == "mi")
        return Syllable::mi;
    if (match == "ni")
        return Syllable::ni;
    if (match == "pi")
        return Syllable::pi;
    if (match == "ri")
        return Syllable::ri;
    if (match == "shi")
        return Syllable::shi;
    if (match == "si")
        return Syllable::si;
    if (match == "thi")
        return Syllable::thi;
    if (match == "ti")
        return Syllable::ti;
    if (match == "tsi")
        return Syllable::tsi;
    if (match == "vi")
        return Syllable::vi;
    if (match == "wi")
        return Syllable::wi;
    if (match == "zi")
        return Syllable::zi;
    return Syllable::unknown;
}

auto correctSyllable(const LocalUrl &file) -> Syllable {
    auto stem{av_speech_in_noise::stem(file)};
    std::regex pattern{"say_(.*?)_.*"};
    std::smatch match;
    std::regex_search(stem, match, pattern);
    if (match.size() > 1)
        return syllable(match[1]);
    return Syllable::unknown;
}

auto ResponseEvaluatorImpl::correct(
    const LocalUrl &url, const SyllableResponse &response) -> bool {
    return correctSyllable(url) == response.syllable;
}
}
