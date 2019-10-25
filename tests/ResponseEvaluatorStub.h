#ifndef ResponseEvaluatorStub_h
#define ResponseEvaluatorStub_h

#include <recognition-test/RecognitionTestModel.hpp>

namespace av_speech_in_noise::tests {
class ResponseEvaluatorStub : public ResponseEvaluator {
  std::string correctTarget_;
  std::string correctNumberFilePath_;
  std::string correctColorFilePath_;
  std::string fileName_;
  std::string filePathForFileName_;
  const coordinate_response_measure::Response *response_{};
  int correctNumber_{};
  coordinate_response_measure::Color correctColor_{};
  bool correct_{};

public:
  void setFileName(std::string s) { fileName_ = std::move(s); }

  void setCorrectNumber(int x) { correctNumber_ = x; }

  void setCorrectColor(coordinate_response_measure::Color c) {
    correctColor_ = c;
  }

  auto correctNumberFilePath() const { return correctNumberFilePath_; }

  auto correctColorFilePath() const { return correctColorFilePath_; }

  auto correctFilePath() const { return correctTarget_; }

  auto response() const { return response_; }

  void setCorrect() { correct_ = true; }

  void setIncorrect() { correct_ = false; }

  bool correct(const std::string &target,
               const coordinate_response_measure::Response &p) override {
    correctTarget_ = target;
    response_ = &p;
    return correct_;
  }

  coordinate_response_measure::Color
  correctColor(const std::string &filePath) override {
    correctColorFilePath_ = filePath;
    return correctColor_;
  }

  int correctNumber(const std::string &filePath) override {
    correctNumberFilePath_ = filePath;
    return correctNumber_;
  }

  std::string fileName(const std::string &filePath) override {
    filePathForFileName_ = filePath;
    return fileName_;
  }

  auto filePathForFileName() const { return filePathForFileName_; }
};
} // namespace av_speech_in_noise::tests
#endif
