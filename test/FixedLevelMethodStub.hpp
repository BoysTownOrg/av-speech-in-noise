#ifndef AV_SPEECH_IN_NOISE_TEST_FIXEDLEVELMETHODSTUB_HPP_
#define AV_SPEECH_IN_NOISE_TEST_FIXEDLEVELMETHODSTUB_HPP_

#include <av-speech-in-noise/core/IFixedLevelMethod.hpp>

namespace av_speech_in_noise {
class FixedLevelMethodStub : public FixedLevelMethod {
  public:
    void initialize(
        const FixedLevelFixedTrialsTest &t, TargetPlaylist *list) override {
        targetList = list;
        fixedLevelFixedTrialsTest = t;
    }

    void initialize(const FixedLevelTest &t,
        FiniteTargetPlaylistWithRepeatables *list) override {
        targetList = list;
        test = t;
    }

    void initialize(
        const FixedLevelTest &t, FiniteTargetPlaylist *list) override {
        targetList = list;
        test = t;
    }

    auto complete() -> bool override { return {}; }

    auto nextTarget() -> LocalUrl override { return {}; }

    auto currentTarget() -> LocalUrl override { return currentTarget_; }

    auto snr() -> SNR override { return SNR{}; }

    void submit(const FreeResponse &) override {
        submittedFreeResponse_ = true;
    }

    void submit(const ConsonantResponse &) override {
        submittedConsonant_ = true;
    }

    void writeTestingParameters(OutputFile &) override {}

    void writeLastCoordinateResponse(OutputFile &) override {}

    void writeLastConsonant(OutputFile &) override {}

    void writeTestResult(OutputFile &) override {}

    void submit(const coordinate_response_measure::Response &) override {}

    auto keywordsTestResults() -> KeywordsTestResults override {
        return keywordsTestResults_;
    }

    void submit(const ThreeKeywordsResponse &r) override {
        threeKeywords_ = &r;
    }

    void submit(const SyllableResponse &r) override { syllableResponse_ = &r; }

    KeywordsTestResults keywordsTestResults_{};
    LocalUrl currentTarget_;
    const ThreeKeywordsResponse *threeKeywords_{};
    const SyllableResponse *syllableResponse_{};
    FixedLevelTest test{};
    FixedLevelFixedTrialsTest fixedLevelFixedTrialsTest{};
    TargetPlaylist *targetList{};
    bool submittedConsonant_{};
    bool submittedFreeResponse_{};
};
}

#endif
