#ifndef TESTS_OUTPUTFILESTUB_HPP_
#define TESTS_OUTPUTFILESTUB_HPP_

#include "LogString.hpp"
#include <av-speech-in-noise/Model.hpp>
#include <recognition-test/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class OutputFileStub : public OutputFile {
    coordinate_response_measure::AdaptiveTrial
        adaptiveCoordinateResponseTrial_{};
    coordinate_response_measure::FixedLevelTrial fixedLevelTrial_{};
    FreeResponseTrial freeResponseTrial_{};
    CorrectKeywordsTrial correctKeywords_{};
    open_set::AdaptiveTrial openSetAdaptiveTrial_{};
    BinocularGazeSamples eyeGazes_;
    AdaptiveTestResults adaptiveTestResult_{};
    LogString log_{};
    std::uintmax_t fadeInCompleteConvertedAudioSampleSystemTimeNanoseconds_{};
    std::uintmax_t targetStartTimeNanoseconds_{};
    gsl::index fadeInCompleteAudioSampleOffset_{};
    const AdaptiveTest *adaptiveTest_{};
    const FixedLevelTest *fixedLevelTest_{};
    const TestIdentity *openNewFileParameters_{};
    bool throwOnOpen_{};

  public:
    void save() override { addToLog("save "); }

    void write(
        const coordinate_response_measure::AdaptiveTrial &trial) override {
        addToLog("writeTrial ");
        adaptiveCoordinateResponseTrial_ = trial;
    }

    void openNewFile(const TestIdentity &p) override {
        addToLog("openNewFile ");
        openNewFileParameters_ = &p;
        if (throwOnOpen_)
            throw OpenFailure{};
    }

    void close() override { addToLog("close "); }

    void write(const AdaptiveTest &test) override {
        addToLog("write ");
        adaptiveTest_ = &test;
    }

    void write(const FixedLevelTest &p) override {
        addToLog("write ");
        fixedLevelTest_ = &p;
    }

    void write(const FreeResponseTrial &p) override {
        addToLog("writeTrial ");
        freeResponseTrial_ = p;
    }

    void write(const CorrectKeywordsTrial &p) override {
        addToLog("writeTrial ");
        correctKeywords_ = p;
    }

    void write(const open_set::AdaptiveTrial &p) override {
        addToLog("writeTrial ");
        openSetAdaptiveTrial_ = p;
    }

    void write(
        const coordinate_response_measure::FixedLevelTrial &trial) override {
        addToLog("writeTrial ");
        fixedLevelTrial_ = trial;
    }

    void write(const AdaptiveTestResults &result) override {
        adaptiveTestResult_ = result;
    }

    auto writtenFixedLevelTrial() const -> auto & { return fixedLevelTrial_; }

    void addToLog(const std::string &s) { log_.insert(s); }

    void throwOnOpen() { throwOnOpen_ = true; }

    auto openNewFileParameters() const { return openNewFileParameters_; }

    auto log() const -> auto & { return log_; }

    auto adaptiveTest() const { return adaptiveTest_; }

    auto fixedLevelTest() const { return fixedLevelTest_; }

    auto eyeGazes() const -> BinocularGazeSamples { return eyeGazes_; }

    auto targetStartTimeNanoseconds() const -> std::uintmax_t {
        return targetStartTimeNanoseconds_;
    }

    auto fadeInCompleteConvertedAudioSampleSystemTimeNanoseconds() const
        -> std::uintmax_t {
        return fadeInCompleteConvertedAudioSampleSystemTimeNanoseconds_;
    }

    void write(const BinocularGazeSamples &g) override { eyeGazes_ = g; }

    void write(TargetStartTime t) override {
        targetStartTimeNanoseconds_ = t.nanoseconds;
    }

    auto fadeInCompleteAudioSampleOffset() const -> gsl::index {
        return fadeInCompleteAudioSampleOffset_;
    }

    auto adaptiveCoordinateResponseTrial() const -> auto & {
        return adaptiveCoordinateResponseTrial_;
    }

    auto freeResponseTrial() const -> auto & { return freeResponseTrial_; }

    auto correctKeywordsTrial() const -> auto & { return correctKeywords_; }

    auto openSetAdaptiveTrial() const -> auto & {
        return openSetAdaptiveTrial_;
    }

    auto adaptiveTestResult() const -> AdaptiveTestResults {
        return adaptiveTestResult_;
    }
};
}

#endif
