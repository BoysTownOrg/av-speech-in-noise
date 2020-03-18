#ifndef TESTS_OUTPUTFILESTUB_HPP_
#define TESTS_OUTPUTFILESTUB_HPP_

#include "LogString.hpp"
#include "av-speech-in-noise/Model.hpp"
#include <recognition-test/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class OutputFileStub : public OutputFile {
    coordinate_response_measure::AdaptiveTrial
        adaptiveCoordinateResponseTrial_{};
    coordinate_response_measure::FixedLevelTrial fixedLevelTrial_{};
    open_set::FreeResponseTrial freeResponseTrial_{};
    open_set::CorrectKeywordsTrial correctKeywords_{};
    open_set::AdaptiveTrial openSetAdaptiveTrial_{};
    std::vector<AdaptiveTestResult> adaptiveTestResult_{};
    LogString log_{};
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

    void writeTest(const AdaptiveTest &test) override {
        addToLog("writeTest ");
        adaptiveTest_ = &test;
    }

    void writeTest(const FixedLevelTest &p) override {
        addToLog("writeTest ");
        fixedLevelTest_ = &p;
    }

    void write(const open_set::FreeResponseTrial &p) override {
        addToLog("writeTrial ");
        freeResponseTrial_ = p;
    }

    void write(const open_set::CorrectKeywordsTrial &p) override {
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

    void write(const std::vector<AdaptiveTestResult> &result) override {
        adaptiveTestResult_ = result;
    }

    auto writtenFixedLevelTrial() const -> auto & { return fixedLevelTrial_; }

    void addToLog(const std::string &s) { log_.insert(s); }

    void throwOnOpen() { throwOnOpen_ = true; }

    auto openNewFileParameters() const { return openNewFileParameters_; }

    auto log() const -> auto & { return log_; }

    auto adaptiveTest() const { return adaptiveTest_; }

    auto fixedLevelTest() const { return fixedLevelTest_; }

    auto adaptiveCoordinateResponseTrial() const -> auto & {
        return adaptiveCoordinateResponseTrial_;
    }

    auto freeResponseTrial() const -> auto & {
        return freeResponseTrial_;
    }

    auto correctKeywordsTrial() const -> auto & { return correctKeywords_; }

    auto openSetAdaptiveTrial() const -> auto & {
        return openSetAdaptiveTrial_;
    }

    auto adaptiveTestResult() const -> std::vector<AdaptiveTestResult> {
        return adaptiveTestResult_;
    }
};
}

#endif
