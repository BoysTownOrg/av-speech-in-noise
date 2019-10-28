#ifndef OutputFileStub_h
#define OutputFileStub_h

#include "LogString.h"
#include <recognition-test/RecognitionTestModel.hpp>

namespace av_speech_in_noise::tests {
class OutputFileStub : public OutputFile {
    coordinate_response_measure::AdaptiveTrial
        writtenAdaptiveCoordinateResponseTrial_{};
    coordinate_response_measure::FixedLevelTrial writtenFixedLevelTrial_{};
    FreeResponseTrial writtenFreeResponseTrial_{};
    open_set::AdaptiveTrial writtenOpenSetAdaptiveTrial_{};
    LogString log_{};
    const AdaptiveTest *adaptiveTest_{};
    const FixedLevelTest *fixedLevelTest_{};
    const TestIdentity *openNewFileParameters_{};
    bool throwOnOpen_{};

  public:
    void save() override { addToLog("save "); }

    void writeTrial(
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

    void writeTrial(const FreeResponseTrial &p) override {
        addToLog("writeTrial ");
        writtenFreeResponseTrial_ = p;
    }

    void writeTrial(const open_set::AdaptiveTrial &p) override {
        addToLog("writeTrial ");
        writtenOpenSetAdaptiveTrial_ = p;
    }

    void writeTrial(
        const coordinate_response_measure::FixedLevelTrial &trial) override {
        addToLog("writeTrial ");
        writtenFixedLevelTrial_ = trial;
    }

    auto &writtenFixedLevelTrial() const { return writtenFixedLevelTrial_; }

    void addToLog(std::string s) { log_.insert(std::move(s)); }

    void throwOnOpen() { throwOnOpen_ = true; }

    auto openNewFileParameters() const { return openNewFileParameters_; }

    auto &log() const { return log_; }

    auto adaptiveTest() const { return adaptiveTest_; }

    auto fixedLevelTest() const { return fixedLevelTest_; }

    auto &writtenAdaptiveCoordinateResponseTrial() const {
        return writtenAdaptiveCoordinateResponseTrial_;
    }

    auto &writtenFreeResponseTrial() const { return writtenFreeResponseTrial_; }

    auto &writtenOpenSetAdaptiveTrial() const {
        return writtenOpenSetAdaptiveTrial_;
    }
};
} // namespace av_speech_in_noise::tests
#endif
