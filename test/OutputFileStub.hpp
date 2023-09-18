#ifndef TESTS_OUTPUTFILESTUB_HPP_
#define TESTS_OUTPUTFILESTUB_HPP_

#include "LogString.hpp"

#include <av-speech-in-noise/core/IOutputFile.hpp>

#include <filesystem>
#include <string>

namespace av_speech_in_noise {
class OutputFileStub : public OutputFile {
  public:
    auto parentPath() -> std::filesystem::path override { return parentPath_; }

    void setParentPath(std::filesystem::path path) {
        parentPath_ = std::move(path);
    }

    auto targetStartTime() -> TargetStartTime { return targetStartTime_; }

    void save() override { addToLog("save "); }

    void openNewFile(const TestIdentity &p) override {
        addToLog("openNewFile ");
        openNewFileParameters_ = &p;
        if (throwOnOpen_)
            throw OpenFailure{};
    }

    void close() override { addToLog("close "); }

    void write(
        const coordinate_response_measure::AdaptiveTrial &trial) override {
        addToLog("writeTrial ");
        adaptiveCoordinateResponseTrial_ = trial;
    }

    void write(const AdaptiveTest &test) override {
        addToLog("writeTest ");
        adaptiveTest_ = &test;
    }

    void write(const FixedLevelTest &p) override {
        addToLog("writeTest ");
        fixedLevelTest_ = &p;
    }

    void write(const FreeResponseTrial &p) override {
        addToLog("writeTrial ");
        freeResponseTrial_ = p;
    }

    void write(const ThreeKeywordsTrial &p) override {
        addToLog("writeTrial ");
        threeKeywordsTrial_ = p;
    }

    void write(const SyllableTrial &p) override {
        addToLog("writeTrial ");
        syllableTrial_ = p;
    }

    void write(const CorrectKeywordsTrial &p) override {
        addToLog("writeTrial ");
        correctKeywordsTrial_ = p;
    }

    void write(const ConsonantTrial &p) override {
        addToLog("writeTrial ");
        consonantTrial_ = p;
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

    void write(Writable &) override { addToLog("writeWritable "); }

    void write(const EyeTrackerTargetPlayerSynchronization &e) override {
        eyeTrackerTargetPlayerSynchronization_ = e;
    }

    void write(const BinocularGazeSamples &g) override { eyeGazes_ = g; }

    void write(TargetStartTime t) override {
        targetStartTimeNanoseconds_ = t.nanoseconds;
        targetStartTime_ = t;
    }

    auto writtenFixedLevelTrial() const
        -> const coordinate_response_measure::FixedLevelTrial & {
        return fixedLevelTrial_;
    }

    void addToLog(const std::string &s) { insert(log_, s); }

    void throwOnOpen() { throwOnOpen_ = true; }

    auto openNewFileParameters() const { return openNewFileParameters_; }

    auto log() const -> const std::stringstream & { return log_; }

    auto adaptiveTest() const -> const AdaptiveTest * { return adaptiveTest_; }

    auto fixedLevelTest() const -> const FixedLevelTest * {
        return fixedLevelTest_;
    }

    auto eyeGazes() const -> BinocularGazeSamples { return eyeGazes_; }

    auto targetStartTimeNanoseconds() const -> std::uintmax_t {
        return targetStartTimeNanoseconds_;
    }

    auto eyeTrackerTargetPlayerSynchronization() const
        -> const EyeTrackerTargetPlayerSynchronization & {
        return eyeTrackerTargetPlayerSynchronization_;
    }

    auto fadeInCompleteConvertedAudioSampleSystemTimeNanoseconds() const
        -> std::uintmax_t {
        return fadeInCompleteConvertedAudioSampleSystemTimeNanoseconds_;
    }

    auto fadeInCompleteAudioSampleOffset() const -> gsl::index {
        return fadeInCompleteAudioSampleOffset_;
    }

    auto adaptiveCoordinateResponseTrial() const
        -> const coordinate_response_measure::AdaptiveTrial & {
        return adaptiveCoordinateResponseTrial_;
    }

    auto freeResponseTrial() const -> const FreeResponseTrial & {
        return freeResponseTrial_;
    }

    auto correctKeywordsTrial() const -> const CorrectKeywordsTrial & {
        return correctKeywordsTrial_;
    }

    auto consonantTrial() const -> const ConsonantTrial & {
        return consonantTrial_;
    }

    auto openSetAdaptiveTrial() const -> const open_set::AdaptiveTrial & {
        return openSetAdaptiveTrial_;
    }

    auto threeKeywordsTrial() -> const ThreeKeywordsTrial & {
        return threeKeywordsTrial_;
    }

    auto adaptiveTestResult() const -> const AdaptiveTestResults & {
        return adaptiveTestResult_;
    }

    auto syllableTrial() -> const SyllableTrial & { return syllableTrial_; }

  private:
    coordinate_response_measure::AdaptiveTrial
        adaptiveCoordinateResponseTrial_{};
    coordinate_response_measure::FixedLevelTrial fixedLevelTrial_{};
    FreeResponseTrial freeResponseTrial_{};
    CorrectKeywordsTrial correctKeywordsTrial_{};
    ConsonantTrial consonantTrial_{};
    ThreeKeywordsTrial threeKeywordsTrial_{};
    SyllableTrial syllableTrial_{};
    open_set::AdaptiveTrial openSetAdaptiveTrial_{};
    BinocularGazeSamples eyeGazes_;
    AdaptiveTestResults adaptiveTestResult_{};
    std::stringstream log_{};
    std::filesystem::path parentPath_;
    EyeTrackerTargetPlayerSynchronization
        eyeTrackerTargetPlayerSynchronization_{};
    TargetStartTime targetStartTime_{};
    std::uintmax_t fadeInCompleteConvertedAudioSampleSystemTimeNanoseconds_{};
    std::uintmax_t targetStartTimeNanoseconds_{};
    gsl::index fadeInCompleteAudioSampleOffset_{};
    const AdaptiveTest *adaptiveTest_{};
    const FixedLevelTest *fixedLevelTest_{};
    const TestIdentity *openNewFileParameters_{};
    bool throwOnOpen_{};
};
}

#endif
