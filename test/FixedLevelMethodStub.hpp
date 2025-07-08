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

    auto snr() -> FloatSNR override { return FloatSNR{}; }

    void submit(const Flaggable &) override { submittedFlaggable = true; }

    void writeTestingParameters(OutputFile &) override {}

    void writeLastCoordinateResponse(OutputFile &) override {}

    void writeTestResult(OutputFile &) override {}

    void submit(const coordinate_response_measure::Response &) override {}

    auto keywordsTestResults() -> KeywordsTestResults override {
        return keywordsTestResults_;
    }

    void submit(const ThreeKeywordsResponse &r) override {
        threeKeywords_ = &r;
    }

    KeywordsTestResults keywordsTestResults_{};
    LocalUrl currentTarget_;
    const ThreeKeywordsResponse *threeKeywords_{};
    FixedLevelTest test{};
    FixedLevelFixedTrialsTest fixedLevelFixedTrialsTest{};
    TargetPlaylist *targetList{};
    bool submittedFlaggable{};
};
}

#endif
