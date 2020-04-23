#ifndef TESTS_RESPONSEEVALUATORSTUB_HPP_
#define TESTS_RESPONSEEVALUATORSTUB_HPP_

#include <recognition-test/Model.hpp>
#include <utility>

namespace av_speech_in_noise {
class ResponseEvaluatorStub : public ResponseEvaluator {
  public:
    void setFileName(std::string s) { fileName_ = std::move(s); }

    void setCorrectNumber(int x) { correctNumber_ = x; }

    void setCorrectColor(coordinate_response_measure::Color c) {
        correctColor_ = c;
    }

    [[nodiscard]] auto correctNumberFilePath() const {
        return correctNumberFilePath_;
    }

    [[nodiscard]] auto correctColorFilePath() const {
        return correctColorFilePath_;
    }

    [[nodiscard]] auto correctFilePath() const { return correctTarget_; }

    [[nodiscard]] auto response() const { return response_; }

    void setCorrect() { correct_ = true; }

    void setIncorrect() { correct_ = false; }

    auto correct(const LocalUrl &target,
        const coordinate_response_measure::Response &p) -> bool override {
        correctTarget_ = target.path;
        response_ = &p;
        return correct_;
    }

    auto correctColor(const LocalUrl &filePath)
        -> coordinate_response_measure::Color override {
        correctColorFilePath_ = filePath.path;
        return correctColor_;
    }

    auto correctNumber(const LocalUrl &filePath) -> int override {
        correctNumberFilePath_ = filePath.path;
        return correctNumber_;
    }

    auto fileName(const LocalUrl &filePath) -> std::string override {
        filePathForFileName_ = filePath.path;
        return fileName_;
    }

    [[nodiscard]] auto filePathForFileName() const {
        return filePathForFileName_;
    }

  private:
    std::string correctTarget_;
    std::string correctNumberFilePath_;
    std::string correctColorFilePath_;
    std::string fileName_;
    std::string filePathForFileName_;
    const coordinate_response_measure::Response *response_{};
    int correctNumber_{};
    coordinate_response_measure::Color correctColor_{};
    bool correct_{};
};
}

#endif
