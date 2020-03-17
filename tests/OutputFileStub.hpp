#ifndef TESTS_OUTPUTFILESTUB_HPP_
#define TESTS_OUTPUTFILESTUB_HPP_

#include "LogString.h"
#include "av-speech-in-noise/Model.hpp"
#include <recognition-test/Model.hpp>
#include <string>

namespace av_speech_in_noise {
class OutputFileStub : public OutputFile {
    coordinate_response_measure::AdaptiveTrial
        writtenAdaptiveCoordinateResponseTrial_{};
    coordinate_response_measure::FixedLevelTrial writtenFixedLevelTrial_{};
    open_set::FreeResponseTrial writtenFreeResponseTrial_{};
    open_set::CorrectKeywordsTrial writtenCorrectKeywords_{};
    open_set::AdaptiveTrial writtenOpenSetAdaptiveTrial_{};
    AdaptiveTestResult adaptiveTestResult_{};
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
        writtenAdaptiveCoordinateResponseTrial_ = trial;
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
        writtenFreeResponseTrial_ = p;
    }

    void write(const open_set::CorrectKeywordsTrial &p) override {
        addToLog("writeTrial ");
        writtenCorrectKeywords_ = p;
    }

    void write(const open_set::AdaptiveTrial &p) override {
        addToLog("writeTrial ");
        writtenOpenSetAdaptiveTrial_ = p;
    }

    void write(
        const coordinate_response_measure::FixedLevelTrial &trial) override {
        addToLog("writeTrial ");
        writtenFixedLevelTrial_ = trial;
    }

    void write(const AdaptiveTestResult &result) override {
        adaptiveTestResult_ = result;
    }

    auto writtenFixedLevelTrial() const -> auto & {
        return writtenFixedLevelTrial_;
    }

    void addToLog(const std::string &s) { log_.insert(s); }

    void throwOnOpen() { throwOnOpen_ = true; }

    auto openNewFileParameters() const { return openNewFileParameters_; }

    auto log() const -> auto & { return log_; }

    auto adaptiveTest() const { return adaptiveTest_; }

    auto fixedLevelTest() const { return fixedLevelTest_; }

    auto writtenAdaptiveCoordinateResponseTrial() const -> auto & {
        return writtenAdaptiveCoordinateResponseTrial_;
    }

    auto writtenFreeResponseTrial() const -> auto & {
        return writtenFreeResponseTrial_;
    }

    auto writtenCorrectKeywords() const -> auto & {
        return writtenCorrectKeywords_;
    }

    auto writtenOpenSetAdaptiveTrial() const -> auto & {
        return writtenOpenSetAdaptiveTrial_;
    }

    auto adaptiveTestResult() const -> AdaptiveTestResult {
        return adaptiveTestResult_;
    }
};
}

#endif
