#ifndef OutputFileStub_h
#define OutputFileStub_h

#include "LogString.h"
#include <recognition-test/RecognitionTestModel.hpp>

namespace av_speech_in_noise::tests {
    class OutputFileStub : public OutputFile {
        coordinate_response_measure::AdaptiveTrial writtenAdaptiveCoordinateResponseTrial_;
        coordinate_response_measure::Trial writtenAdaptiveCoordinateResponseTrial2_;
        coordinate_response_measure::Trial writtenFixedLevelTrial2_;
        FreeResponseTrial writtenFreeResponseTrial_;
        LogString log_;
        const AdaptiveTest *adaptiveTest_{};
        const FixedLevelTest *fixedLevelTest_{};
        const TestInformation *openNewFileParameters_{};
        bool throwOnOpen_{};
    public:
        void save() override {
            addToLog("save ");
        }

        void writeTrial(
            const coordinate_response_measure::AdaptiveTrial &trial
        ) override {
            addToLog("writeTrial ");
            writtenAdaptiveCoordinateResponseTrial_ = trial;
            writtenAdaptiveCoordinateResponseTrial2_ = trial.trial;
        }

        auto &writtenAdaptiveCoordinateResponseTrial2() const {
            return writtenAdaptiveCoordinateResponseTrial2_;
        }

        void openNewFile(const TestInformation &p) override {
            addToLog("openNewFile ");
            openNewFileParameters_ = &p;
            if (throwOnOpen_)
                throw OpenFailure{};
        }

        void close() override {
            addToLog("close ");
        }

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

        void writeTrial(const coordinate_response_measure::FixedLevelTrial &trial) override {
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
}
#endif
