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

static auto leadingPathLength(const LocalUrl &url) -> gsl::index {
    return url.path.find_last_of('/') + 1;
}

static auto subString(
    const std::string &s, gsl::index position, gsl::index size) -> std::string {
    return s.substr(position, size);
}

static auto fileName(const LocalUrl &url) -> std::string {
    return std::filesystem::path{url.path}.filename();
}

static auto stem(const LocalUrl &url) -> std::string {
    return std::filesystem::path{url.path}.stem();
}

static auto correctConsonant(const LocalUrl &url) -> char {
    auto stem{av_speech_in_noise::stem(url)};
    std::regex pattern{"choose_(.*?)_.*"};
    std::smatch match;
    std::regex_search(stem, match, pattern);
    if (match.size() > 1)
        return consonant(match[1]);
    return '\0';
}

static auto correctSyllable(const LocalUrl &file) -> Syllable {
    auto stem{av_speech_in_noise::stem(file)};
    std::regex pattern{"say_(.*?)_.*"};
    std::smatch match;
    std::regex_search(stem, match, pattern);
    if (match.size() > 1)
        return syllable(match[1]);
    return Syllable::unknown;
}

static auto colorNameLength(const LocalUrl &url, gsl::index leadingPathLength_)
    -> gsl::index {
    const auto fileNameBeginning{url.path.begin() + leadingPathLength_};
    return std::distance(fileNameBeginning,
        std::find_if(fileNameBeginning, url.path.end(),
            [](unsigned char c) { return std::isalpha(c) == 0; }));
}

static auto correctNumber_(const LocalUrl &url) -> int {
    const auto leadingPathLength_{leadingPathLength(url)};
    return std::stoi(subString(url.path,
        leadingPathLength_ + colorNameLength(url, leadingPathLength_), 1));
}

const int ResponseEvaluatorImpl::invalidNumber = -1;

auto ResponseEvaluatorImpl::correctConsonant(const LocalUrl &url) -> char {
    return av_speech_in_noise::correctConsonant(url);
}

auto ResponseEvaluatorImpl::correctNumber(const LocalUrl &url) -> int {
    try {
        return correctNumber_(url);
    } catch (const std::invalid_argument &) {
        return invalidNumber;
    }
}

auto ResponseEvaluatorImpl::correctColor(const LocalUrl &url)
    -> coordinate_response_measure::Color {
    const auto leadingPathLength_{leadingPathLength(url)};
    return color(subString(url.path, leadingPathLength_,
        colorNameLength(url, leadingPathLength_)));
}

auto ResponseEvaluatorImpl::fileName(const LocalUrl &url) -> std::string {
    return av_speech_in_noise::fileName(url);
}

auto ResponseEvaluatorImpl::correct(const LocalUrl &url,
    const coordinate_response_measure::Response &r) -> bool {
    return correctNumber(url) == r.number && correctColor(url) == r.color &&
        r.color != coordinate_response_measure::Color::unknown &&
        r.number != invalidNumber;
}

auto ResponseEvaluatorImpl::correct(
    const LocalUrl &url, const ConsonantResponse &r) -> bool {
    return av_speech_in_noise::correctConsonant(url) == r.consonant;
}

auto ResponseEvaluatorImpl::correct(
    const LocalUrl &url, const SyllableResponse &response) -> bool {
    return correctSyllable(url) == response.syllable;
}
}
