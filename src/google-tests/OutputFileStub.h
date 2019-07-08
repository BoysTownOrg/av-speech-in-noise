#ifndef OutputFileStub_h
#define OutputFileStub_h

#include "LogString.h"
#include <recognition-test/RecognitionTestModel.hpp>

class OutputFileStub : public av_speech_in_noise::OutputFile {
    av_speech_in_noise::coordinate_response_measure::AdaptiveTrial writtenAdaptiveCoordinateResponseTrial_;
    av_speech_in_noise::coordinate_response_measure::Trial writtenAdaptiveCoordinateResponseTrial2_;
    av_speech_in_noise::coordinate_response_measure::Trial writtenFixedLevelTrial2_;
    av_speech_in_noise::FreeResponseTrial writtenFreeResponseTrial_;
    LogString log_;
    const av_speech_in_noise::AdaptiveTest *adaptiveTest_{};
    const av_speech_in_noise::FixedLevelTest *fixedLevelTest_{};
    const av_speech_in_noise::TestInformation *openNewFileParameters_{};
    bool throwOnOpen_{};
public:
    void save() override {
        addToLog("save ");
    }

    void writeTrial(
        const av_speech_in_noise::coordinate_response_measure::AdaptiveTrial &trial
    ) override {
        addToLog("writeTrial ");
        writtenAdaptiveCoordinateResponseTrial_ = trial;
        writtenAdaptiveCoordinateResponseTrial2_ = trial.trial;
    }

    auto &writtenAdaptiveCoordinateResponseTrial2() const {
        return writtenAdaptiveCoordinateResponseTrial2_;
    }

    void openNewFile(const av_speech_in_noise::TestInformation &p) override {
        addToLog("openNewFile ");
        openNewFileParameters_ = &p;
        if (throwOnOpen_)
            throw OpenFailure{};
    }

    void close() override {
        addToLog("close ");
    }

    void writeTest(const av_speech_in_noise::AdaptiveTest &test) override {
        addToLog("writeTest ");
        adaptiveTest_ = &test;
    }

    void writeTest(const av_speech_in_noise::FixedLevelTest &p) override {
        addToLog("writeTest ");
        fixedLevelTest_ = &p;
    }

    void writeTrial(const av_speech_in_noise::FreeResponseTrial &p) override {
        addToLog("writeTrial ");
        writtenFreeResponseTrial_ = p;
    }

    void writeTrial(const av_speech_in_noise::coordinate_response_measure::FixedLevelTrial &trial) override {
        addToLog("writeTrial ");
        writtenFixedLevelTrial2_ = trial.trial;
    }

    auto &writtenFixedLevelTrial2() const {
        return writtenFixedLevelTrial2_;
    }

    void addToLog(std::string s) {
        log_.insert(std::move(s));
    }

    void throwOnOpen() {
        throwOnOpen_ = true;
    }

    auto openNewFileParameters() const {
        return openNewFileParameters_;
    }

    auto &log() const {
        return log_;
    }

    auto adaptiveTest() const {
        return adaptiveTest_;
    }

    auto fixedLevelTest() const {
        return fixedLevelTest_;
    }

    auto &writtenAdaptiveCoordinateResponseTrial() const {
        return writtenAdaptiveCoordinateResponseTrial_;
    }

    auto &writtenFreeResponseTrial() const {
        return writtenFreeResponseTrial_;
    }
};

#endif
