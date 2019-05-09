#include "OutputFileImpl.hpp"
#include <sstream>

namespace av_speech_in_noise {
    namespace {
        class FormattedStream {
            std::stringstream stream;
        public:
            template<typename T>
            void writeLabeledLine(std::string label, T thing) {
                stream << std::move(label);
                stream << ": ";
                stream << thing;
                stream << '\n';
            }
            
            template<typename T>
            void insert(T item) {
                stream << item;
            }
            
            void insertCommaAndSpace() {
                stream << ", ";
            }
            
            void insertNewLine() {
                stream << '\n';
            }
            
            auto str() const {
                return stream.str();
            }
        };
    }
    
    OutputFileImpl::OutputFileImpl(Writer *writer, OutputFilePath *path) :
        writer{writer},
        path{path} {}
    
    void OutputFileImpl::writeTrial(const coordinate_response_measure::Trial &trial) {
        write(formatTrial(trial));
    }
    
    void OutputFileImpl::write(std::string s) {
        writer->write(std::move(s));
    }

    std::string OutputFileImpl::formatTrial(const coordinate_response_measure::Trial &trial) {
        FormattedStream stream;
        stream.insert(trial.SNR_dB);
        stream.insertCommaAndSpace();
        stream.insert(trial.correctNumber);
        stream.insertCommaAndSpace();
        stream.insert(trial.subjectNumber);
        stream.insertCommaAndSpace();
        stream.insert(colorName(trial.correctColor));
        stream.insertCommaAndSpace();
        stream.insert(colorName(trial.subjectColor));
        stream.insertCommaAndSpace();
        stream.insert(evaluation(trial));
        stream.insertCommaAndSpace();
        stream.insert(trial.reversals);
        stream.insertNewLine();
        return stream.str();
    }
    
    std::string OutputFileImpl::evaluation(const coordinate_response_measure::Trial &trial) {
        return trial.correct ? "correct" : "incorrect";
    }

    std::string OutputFileImpl::colorName(coordinate_response_measure::Color c) {
        switch (c) {
        case coordinate_response_measure::Color::green:
            return "green";
        case coordinate_response_measure::Color::red:
            return "red";
        case coordinate_response_measure::Color::blue:
            return "blue";
        case coordinate_response_measure::Color::white:
            return "white";
        case coordinate_response_measure::Color::notAColor:
            return "not a color";
        }
        return "unknown";
    }
    
    void OutputFileImpl::writeCoordinateResponseTrialHeading() {
        write(formatCoordinateResponseTrialHeading());
    }
    
    std::string OutputFileImpl::formatCoordinateResponseTrialHeading() {
        FormattedStream stream;
        stream.insert("SNR (dB)");
        stream.insertCommaAndSpace();
        stream.insert("correct number");
        stream.insertCommaAndSpace();
        stream.insert("subject number");
        stream.insertCommaAndSpace();
        stream.insert("correct color");
        stream.insertCommaAndSpace();
        stream.insert("subject color");
        stream.insertCommaAndSpace();
        stream.insert("evaluation");
        stream.insertCommaAndSpace();
        stream.insert("reversals");
        stream.insertNewLine();
        return stream.str();
    }
    
    void OutputFileImpl::writeFreeResponseTrialHeading() {
        write(formatOpenSetTrialHeading());
    }
    
    std::string OutputFileImpl::formatOpenSetTrialHeading() {
        FormattedStream stream;
        stream.insert("target");
        stream.insertCommaAndSpace();
        stream.insert("response");
        stream.insertNewLine();
        return stream.str();
    }
    
    void OutputFileImpl::openNewFile(const TestInformation &test) {
        writer->open(generateNewFilePath(test));
        if (writer->failed())
            throw OpenFailure{};
    }
    
    std::string OutputFileImpl::generateNewFilePath(const TestInformation &test) {
        return
            path->outputDirectory() + "/" +
            path->generateFileName(test) + ".txt";
    }
    
    void OutputFileImpl::writeTest(const AdaptiveTest &test) {
        write(formatTest(test));
    }
    
    void OutputFileImpl::writeTest(const FixedLevelTest &test) {
        write(formatTest(test));
    }
    
    std::string OutputFileImpl::formatTest(const AdaptiveTest &test) {
        FormattedStream stream;
        stream.writeLabeledLine("subject", test.information.subjectId);
        stream.writeLabeledLine("tester", test.information.testerId);
        stream.writeLabeledLine("session", test.information.session);
        stream.writeLabeledLine("masker", test.maskerFilePath);
        stream.writeLabeledLine("targets", test.targetListDirectory);
        stream.writeLabeledLine("masker level (dB SPL)", test.maskerLevel_dB_SPL);
        stream.writeLabeledLine("starting SNR (dB)", test.startingSnr_dB);
        stream.writeLabeledLine("condition", conditionName(test.condition));
        stream.insertNewLine();
        return stream.str();
    }
    
    std::string OutputFileImpl::formatTest(const FixedLevelTest &test) {
        FormattedStream stream;
        stream.writeLabeledLine("subject", test.information.subjectId);
        stream.writeLabeledLine("tester", test.information.testerId);
        stream.writeLabeledLine("session", test.information.session);
        stream.writeLabeledLine("masker", test.maskerFilePath);
        stream.writeLabeledLine("targets", test.targetListDirectory);
        stream.writeLabeledLine("masker level (dB SPL)", test.maskerLevel_dB_SPL);
        stream.writeLabeledLine("SNR (dB)", test.snr_dB);
        stream.writeLabeledLine("condition", conditionName(test.condition));
        stream.insertNewLine();
        return stream.str();
    }
    
    void OutputFileImpl::close() {
        writer->close();
    }
    
    void OutputFileImpl::writeTrial(const FreeResponseTrial &trial) {
        write(formatTrial(trial));
    }

    std::string OutputFileImpl::formatTrial(const FreeResponseTrial &trial) {
        FormattedStream stream;
        stream.insert(trial.target);
        stream.insertCommaAndSpace();
        stream.insert(trial.response);
        stream.insertNewLine();
        return stream.str();
    }
}
